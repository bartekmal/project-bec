#!/usr/bin/env python

'''
Script to calculate the study systematics.

Needs to run `lb-conda default` for a correct python verion with PyROOT.
'''

# Futures
# from __future__ import print_function

# Built-in/Generic Imports
import math
import os
import sys

from statistics import mean
from typing import List
from dataclasses import dataclass, field

# Libs
import ROOT
ROOT.gROOT.SetBatch(True)

# Own modules
# from {path} import {class}

# Own C++ includes
ROOT.gInterpreter.ProcessLine(
    '#include "' + os.getenv('BEC_BASE_CODE_HBT') + '/SelectionClass.hpp"')
ROOT.gInterpreter.ProcessLine(
    '#include "' + os.getenv('BEC_BASE_CODE_HBT') + '/Utils.hpp"')
ROOT.gInterpreter.ProcessLine(
    '#include "' + os.getenv('BEC_BASE_CODE') + '/utils/Styles.hpp"')
ROOT.gInterpreter.ProcessLine(
    '#include "' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/fitModel.C"')
ROOT.gInterpreter.ProcessLine(
    '#include "' + os.getenv('BEC_BASE_CODE_HBT') + '/Units.hpp"')

__author__ = 'Bartosz Malecki'
__email__ = 'bartosz.piotr.malecki@cern.ch'

# helper classes


@dataclass
class ErrorEntry:
    ''''Class for keeping info on an error (statistical / systematical).'''
    valueAbsolute: float
    valueRelative: float

    def getAbs(self) -> float:
        return self.valueAbsolute

    def getRel(self) -> float:
        return self.valueRelative


@dataclass
class FitResultEntry:
    ''''Class for keeping info on the fit results (value and error).'''
    value: float
    error: float

    def getValue(self) -> float:
        return self.value

    def getError(self) -> float:
        return self.error


@dataclass
class ContributionConfig:
    ''''Class for keeping configuration of the given contribution to systematics.
            - strategy flags:
                0 - take max diff wrt central result
                1 - take average diff wrt central result
                2 - take RMS of a sample from jobs and the central result
    '''
    jobs: List[str] = field(default_factory=List[str])
    strategy: int = 0

    def getJobs(self) -> List[str]:
        return self.jobs

    def getStrategy(self) -> int:
        return self.strategy

# global config

listOfParams = ['radius', 'lambda', 'norm', 'delta','MinFcn']
paramUnits = {'radius': 'fm', 'lambda': '-', 'norm': '-', 'delta': 'GeV-1', 'MinFcn': '-'}

keyMainResult = 'main'
keyTotalSystError = 'syst'
keyStatError = 'stat'

basePath = os.getenv('BEC_BASE')

listOfJobs = {
    # main result
    keyMainResult: os.path.join(basePath, 'analysis/main'),
    # fit range max
    'fitRangeMax_0': os.path.join(basePath, 'analysis/systematics/fitConfig/rangeMax/1c8'),
    'fitRangeMax_1': os.path.join(basePath, 'analysis/systematics/fitConfig/rangeMax/2c2'),
    # fit range min
    'fitRangeMin_0': os.path.join(basePath, 'analysis/systematics/fitConfig/rangeMin/0c04'),
    'fitRangeMin_1': os.path.join(basePath, 'analysis/systematics/fitConfig/rangeMin/0c06'),
    # coulomb
    'coulomb_0': os.path.join(basePath, 'analysis/systematics/coulomb/rEff_0c5'),
    'coulomb_1': os.path.join(basePath, 'analysis/systematics/coulomb/rEff_8c0'),
    # ref sample
    'refSample_0': os.path.join(basePath, 'analysis/systematics/refSample/nrEventsToMix_50'),
    'refSample_1': os.path.join(basePath, 'analysis/systematics/refSample/nrEventsToMix_100'),
    # selection
    'selection_zPv_0': os.path.join(basePath, 'analysis/systematics/selection/zPv/looser'),
    'selection_zPv_1': os.path.join(basePath, 'analysis/systematics/selection/zPv/stricter'),
    'selection_ghosts': os.path.join(basePath, 'analysis/systematics/selection/ghosts'),
    'selection_pid_pion_0': os.path.join(basePath, 'analysis/systematics/selection/pid/loosePion'),
    # bkg
    'bkg_scaling_0': os.path.join(basePath, 'analysis/systematics/bkg/scaling/low'),
    'bkg_scaling_1': os.path.join(basePath, 'analysis/systematics/bkg/scaling/high'),

    'bkg_minRange_0': os.path.join(basePath, 'analysis/systematics/bkg/minRange/0c225'),
    'bkg_minRange_1': os.path.join(basePath, 'analysis/systematics/bkg/minRange/0c275'),

    'bkg_fixedN0_0': os.path.join(basePath, 'analysis/systematics/bkg/fixedN0/N0_10c0'),
    'bkg_fixedN0_1': os.path.join(basePath, 'analysis/systematics/bkg/fixedN0/N0_20c0'),

    'bkg_resonances_0': os.path.join(basePath, 'analysis/systematics/bkg/resonances/narrower'),
    'bkg_resonances_1': os.path.join(basePath, 'analysis/systematics/bkg/resonances/wider')
}

# group individual jobs to calculate the given contribution to the systematics
listOfContributions = {
    # fit range
    'fitRangeMax': ContributionConfig(['fitRangeMax_0', 'fitRangeMax_1'], 2),
    'fitRangeMin': ContributionConfig(['fitRangeMin_0', 'fitRangeMin_1'], 2),
    # Coulomb
    'coulomb': ContributionConfig(['coulomb_0', 'coulomb_1'], 2),
    # ref sample
    'refSample': ContributionConfig(['refSample_0', 'refSample_1'], 2),
    # selection
    'selection_zPv': ContributionConfig(['selection_zPv_0', 'selection_zPv_1'], 2),
    'selection_ghosts': ContributionConfig(['selection_ghosts'], 0),
    'selection_pid': ContributionConfig(['selection_pid_pion_0'], 0),
    # bkg
    'bkg_scaling': ContributionConfig(['bkg_scaling_0', 'bkg_scaling_1'], 1),
    'bkg_minRange': ContributionConfig(['bkg_minRange_0', 'bkg_minRange_1'], 2),
    'bkg_fixedN0': ContributionConfig(['bkg_fixedN0_0', 'bkg_fixedN0_1'], 2),
    'bkg_resonances': ContributionConfig(['bkg_resonances_0', 'bkg_resonances_1'], 2)
}

# helpers


def getRms(listOfNumbers):

    sum = 0
    # loop over pairs in the sample
    for val1, val2 in zip(listOfNumbers, listOfNumbers[1:]):
        sum += (val1-val2)**2

    return math.sqrt(1 / len(listOfNumbers) * sum)


def getSumInQuadrature(listOfNumbers):
    '''Sum up the elements in the list in quadrature.'''
    sum = 0
    for el in listOfNumbers:
        sum += el**2

    return math.sqrt(sum)


def getRelativeDiff(diff, refNumber):
    '''Return a relative difference wrt the reference value [%].'''

    return diff/refNumber * 100.0


def setStyleLocal(flagStyle):
    '''Set local style (based on the general ones)'''
    ROOT.HBT.Styles.setStyle(flagStyle)

    # local style settings
    ROOT.gStyle.SetPadLeftMargin(0.15)
    ROOT.HBT.Styles.setColorPalette()
    ROOT.gROOT.ForceStyle()


# functions


def getFitResults(inputPath, listOfJobs, nrOfBins, hNameBase, hCommonEndName, funcName):
    '''Provide a list of fit results for a configurable list of jobs and expected #bins'''

    # basic loop over individual jobs
    fitResults = {}
    for key, el in listOfJobs.items():

        curFile = ROOT.TFile(os.path.join(el, inputPath), "READ")

        # get result in each mult bin
        fitResultsBins = {}
        for curBin in range(nrOfBins):
            curHistName = ROOT.HBT.Utils.getHistogramName(
                hNameBase, hCommonEndName, True, False, curBin, 1)
            fitResult = curFile.Get(str(ROOT.HBT.Utils.getFitResultName(
                curHistName, funcName)))

            # check if the fit is valid (and exists)
            if not ((fitResult != None) and (fitResult.IsValid())):
                # print(f'Fit result is not valid: {curBin}')
                continue
            else:
                # store fit result
                fitResultsBins[curBin] = fitResult

        fitResults[key] = fitResultsBins

        curFile.Close()

    return fitResults


def sanitiseFitResults(fitResults):
    '''Check validity of the loaded results. Returns a list of valid bins (or None).'''

    if not keyMainResult in fitResults:
        return None

    # check if each contribution has the same number of valid fits (in bins) as the main result
    expectedNrOfFits = len(fitResults[keyMainResult])
    for key, el in fitResults.items():
        if len(el) != expectedNrOfFits:
            print(f'Hello there! (invalid fits): {key}\n')
            return None

    return fitResults[keyMainResult].keys()


def getTransformedFitResults(fitResults, fitParams, listOfBins):
    '''Transform a list of TFitResults into a convenient local data structure.'''

    results = {}
    for bin in listOfBins:
        resultsInBin = {}

        for paramKey, paramId in fitParams.items():
            curResults = {}

            for resultKey, result in fitResults.items():
                if paramKey == 'MinFcn':
                    curResults[resultKey] = FitResultEntry(result[bin].MinFcnValue(), result[bin].Ndf())
                else:    
                    curResults[resultKey] = FitResultEntry(result[bin].Parameter(paramId), result[bin].ParError(paramId))

            resultsInBin[paramKey] = curResults

        results[bin] = resultsInBin
    return results


def calculateContribution(contribution, results):

    listOfJobs = contribution.getJobs()
    flagStrategy = contribution.getStrategy()

    mainResult = results[keyMainResult].getValue()
    otherResults = [results[keyJob].getValue() for keyJob in listOfJobs]

    resultsDiffs = [math.fabs(curResult - mainResult)
                    for curResult in otherResults]

    if flagStrategy == 0:
        # max diff from main
        errorValue = max(resultsDiffs)
    elif flagStrategy == 1:
        # average diff
        errorValue = mean(resultsDiffs)
    elif flagStrategy == 2:
        # RMS of the full sample
        resultsSample = otherResults.copy()
        resultsSample.append(mainResult)
        errorValue = getRms(resultsSample)
    else:
        return None

    return ErrorEntry(errorValue, getRelativeDiff(errorValue, mainResult))


def getContributions(listOfContributions, inputs):
    '''Get contributions to the systematic uncertainty based on the stored results (inputs)'''

    outputs = {}
    for binNr, inputsInBin in inputs.items():
        outputsInBin = {}

        for keyParam, inputsForParam in inputsInBin.items():
            outputsForParam = {}

            for keyContribution, valContribution in listOfContributions.items():
                outputsForParam[keyContribution] = calculateContribution(
                    valContribution, inputsForParam)

            outputsInBin[keyParam] = outputsForParam

        outputs[binNr] = outputsInBin
    return outputs


def getTotalErrors(inputs):
    '''Get total systematic uncertainty by summing up in quadrature the stored contributions (inputs).'''

    outputs = {}
    for binNr, inputsInBin in inputs.items():
        outputsInBin = {}

        for keyParam, inputsForParam in inputsInBin.items():
            outputsForParam = {}

            outputsForParam[keyTotalSystError] = ErrorEntry(getSumInQuadrature([el.getAbs() for el in inputsForParam.values(
            )]), getSumInQuadrature([el.getRel() for el in inputsForParam.values()]))

            outputsInBin[keyParam] = outputsForParam

        outputs[binNr] = outputsInBin
    return outputs


def getStatErrors(inputs):
    '''Get statistical errors from the fit results.'''

    outputs = {}
    for binNr, inputsInBin in inputs.items():
        outputsInBin = {}

        for keyParam, inputsForParam in inputsInBin.items():
            outputsForParam = {}

            outputsForParam[keyStatError] = ErrorEntry(inputsForParam[keyMainResult].getError(), getRelativeDiff(
                math.fabs(inputsForParam[keyMainResult].getError()), inputsForParam[keyMainResult].getValue()))

            outputsInBin[keyParam] = outputsForParam

        outputs[binNr] = outputsInBin
    return outputs


def printResults(results, description):
    '''Print values given in format values[bin][param][job].'''
    for binNr, valuesInBin in results.items():
        print(f'{description} in bin {binNr:2d} :\n')
        for keyParam, valuesForParam in valuesInBin.items():
            print(f'\t{keyParam:15s} :\n')
            for keyValue, value in valuesForParam.items():
                if isinstance(value, ErrorEntry):
                    print(
                        f'\t\t{keyValue:25s} : {value.getAbs():6.4f} ({value.getRel():4.2f}%)')
                elif isinstance(value, FitResultEntry):
                    print(
                        f'\t\t{keyValue:25s} : {value.getValue():6.4f} +- {value.getError():6.4f}')
                else:
                    print(f'\t\t{keyValue:25s} : {value:6.4f}')
            print('')
        print('')


def saveAsFile(fileName, header, binCentres, fitResults, errors, paramKey):

    with open(fileName, 'w', encoding='utf8') as outputFile:
        outputFile.write(header)
        # list of (common) valid bins (results only contain valid fits)
        bins = fitResults.keys()
        for bin in bins:
            if paramKey == 'radius':
                outputFile.write(f'{binCentres[bin]:6.2f} \pm {ROOT.HBT.Units.GevInvToFm(fitResults[bin][paramKey][keyMainResult].getValue()):7.3f} \pm {ROOT.HBT.Units.GevInvToFm(errors[bin][paramKey][keyStatError].getAbs()):7.3f} \pm {ROOT.HBT.Units.GevInvToFm(errors[bin][paramKey][keyTotalSystError].getAbs()):7.3f}\n')
            else:
                outputFile.write(f'{binCentres[bin]:6.2f} \pm {fitResults[bin][paramKey][keyMainResult].getValue():7.3f} \pm {errors[bin][paramKey][keyStatError].getAbs():7.3f} \pm {errors[bin][paramKey][keyTotalSystError].getAbs():7.3f}\n')

def plotErrors(inputs, binCentres, fitParams, dataLabel):
    '''Plot uncertainties in bins.'''

    # list of contributions (inputs should only contain valid data of uniform sizes in each dimension - get any (first) bin info)
    contributions = inputs[next(iter(inputs))][next(iter(fitParams))].keys()

    for paramKey in fitParams:

        # prepare global canvas
        canvas = ROOT.TCanvas(f'{paramKey}', f'{paramKey}',
                              ROOT.HBT.Styles.defaultCanvasSizeX,
                              ROOT.HBT.Styles.defaultCanvasSizeY)
        canvas.SaveAs('.pdf[')

        paramName = '#font[12]{R}' if paramKey == 'radius' else (
            '#font[12]{#lambda}'
            if paramKey == 'lambda' else fitParams[paramKey])

        for contributionKey in contributions:

            # prepare canvas & graph
            canvas.SetGrid()
            title = f'{contributionKey}' + ';#font[12]{N}_{VELO};#font[12]{#sigma}_{syst}(' + paramName + ') [%]'
            # TODO (manual) range setting (improve config)
            frame = canvas.DrawFrame(0.0, 0.0, 180., 45.0, title)
            graph = ROOT.TGraphErrors()

            # add points
            for binKey, bin in inputs.items():
                graph.SetPoint(
                    int(binKey), binCentres[binKey],
                    bin[paramKey][contributionKey].getRel())

            # draw
            graph.Draw('P PMC PLC')

            # add LHCb label (& description)
            lhcbLabel = ROOT.HBT.Styles.makeLhcbLabel(0.05, 0.35, 0.80, 0.95)
            ROOT.HBT.Utils.addMultilineText(f'LHCb preliminary', lhcbLabel)
            # TODO custom name only for interesting plots (improve config)
            contributionName = 'total uncertainty' if contributionKey == 'syst' else (
                'bkg scaling' if contributionKey == 'bkg_scaling' else
                ('PID optimisation'
                 if contributionKey == 'selection_pid' else contributionKey))
            ROOT.HBT.Utils.addMultilineText(f'{contributionName}', lhcbLabel)
            lhcbLabel.Draw()
            canvas.Update()

            # add legend
            legend = ROOT.TLegend(0.20, 0.55, 0.55, 0.725)
            legend.AddEntry(graph, dataLabel, 'pe')
            legend.Draw('SAME')

            # save plot
            canvas.SaveAs(f'.pdf')

        canvas.SaveAs('.pdf]')


# main part


def main():
    # get args
    args = sys.argv[1:]
    if len(args) != 5:
        print('Wrong number of arguments!\n')
        return 1
    inputPath = args[0]
    hNameBase = args[1]
    hCommonEndName = args[2]
    funcName = args[3]
    dataLabel = args[4]
    print(
        f'Input args: {inputPath} {hNameBase} {hCommonEndName} {funcName} {dataLabel} \n')

    # do systematics
    print(
        f'Number of jobs used/listed in the systematics study: {len(listOfJobs)}\n')

    # get params info
    paramsListFull = ROOT.prepareParamsFull(True)  # True for mult bins only
    fitParams = {key: paramsListFull[key].id() for key in listOfParams}
    print(f'The following fit params are studied: {fitParams}\n')

    # load info on the mult bins
    selection = ROOT.SelectionClass()
    nrOfBinsMult = selection.getNrOfBinsMult()
    binCentres = selection.getBinsOfMultiplicityCentres()
    print(f'Number of mult bins: {nrOfBinsMult}')
    print(f'Bin centres: {binCentres}\n')

    # get individual fit results and sanitise
    fitResults = getFitResults(
        inputPath, listOfJobs, nrOfBinsMult, hNameBase, hCommonEndName, funcName)
    validBinKeys = sanitiseFitResults(fitResults)
    if validBinKeys is None:
        print('Input (fit results) not valid.\n')
        return
    print(f'Valid bins: {*validBinKeys,}\n')

    # transform results into a convenient data structure
    results = getTransformedFitResults(fitResults, fitParams, validBinKeys)
    # printResults(results, 'Fit results')

    # calculate the contributions from the different systematics sources in each bin
    contributions = getContributions(listOfContributions, results)
    # printResults(contributions, 'Contributions to the systematic uncertainty')

    # get total errors
    totalErrors = getTotalErrors(contributions)
    # printResults(totalErrors, 'Total systematic uncertainty')

    # get stat errors
    statErrors = getStatErrors(results)
    # printResults(statErrors, 'Statistical uncertainty')

    # save results
    summary = contributions
    for bin, summaryInBin in summary.items():
        for keyParam, summaryForParam in summaryInBin.items():
            summaryForParam.update(totalErrors[bin][keyParam])
            # store also statistical error in summary (temporary)
            summaryForParam.update(statErrors[bin][keyParam])
    printResults(summary, 'Summary on systematic uncertainty')

    # save total systematic and statisctical uncertainty
    for param in fitParams.keys():
        saveAsFile(f'{param}_multReconstructed.csv',
                   f'N_rec(bin centre)\t{param}[{paramUnits[param]}]\tstat[{paramUnits[param]}]\tsyst[{paramUnits[param]}]\n', binCentres, results, summary, param)

    # plot uncertainties in bins
    setStyleLocal(ROOT.HBT.Styles.defaultStyleFlag)
    plotErrors(summary, binCentres, fitParams, dataLabel)


if __name__ == '__main__':
    main()
