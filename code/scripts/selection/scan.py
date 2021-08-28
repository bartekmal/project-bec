#!/usr/bin/env python

'''
Script to make PID scans.

Needs to run `lb-conda default` for a correct python verion with PyROOT.
'''

# Futures
# from __future__ import print_function

# Built-in/Generic Imports
import math
import os
import sys

from typing import List
from dataclasses import dataclass, field

# Libs
import ROOT
ROOT.gROOT.SetBatch(True)

# Own modules
# from {path} import {class}

# Own C++ includes
ROOT.gInterpreter.ProcessLine(
    '#include "/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb/code/HBTAnalysis/SelectionClass.hpp"')
ROOT.gInterpreter.ProcessLine(
    '#include "/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb/code/HBTAnalysis/Utils.hpp"')
ROOT.gInterpreter.ProcessLine(
    '#include "/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb/code/utils/Styles.hpp"')

__author__ = 'Bartosz Malecki'
__email__ = 'bartosz.piotr.malecki@cern.ch'

# helper classes


@dataclass
class ConfigEntry:
    ''''Class for keeping configuration of the given study.
            - strategy flags:
                0 - boolean-like histograms (hasClone / hasGhost)
                1 - MCID-like histograms
                2 - MCID-like histograms (get others)
                3 - isGhost info on particle-level
                4 - draw histograms
    '''
    id: str = ''
    jobs: List[str] = field(default_factory=List[str])
    strategy: int = 0
    hNameBase: str = ''
    hNameEnd: str = ''

# global config


tabForFString = '\t'  # ! ugly hack

keyMisreconstructed = 'misRec'
keyIsClone = 'isClone'

pidNumbers = {
    'pions': 211,
    'kaons': 321,
    'protons': 2212,
    'electrons': 11,
    'muons': 13,
    keyMisreconstructed: 0,  # ghosts / clones
    'mixed': -1,
}
binZeroInMCIDHists = 10001

# full list of available jobs
basePath = '/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb/analysis/studies/pionIdScan'
listOfCuts = ['0.30', '0.35', '0.40', '0.45', '0.50',
              '0.55', '0.60', '0.65', '0.70', '0.75', '0.80']
listOfJobs = {cutValue: os.path.join(
    basePath, cutValue.replace('.', 'c')) for cutValue in listOfCuts}

# config for individual studies
studiesParticles = {
    'pions': ConfigEntry('pions', listOfJobs.keys(), 1, 'h1237', ''),
    'kaons': ConfigEntry('kaons', listOfJobs.keys(), 1, 'h1237', ''),
    'protons': ConfigEntry('protons', listOfJobs.keys(), 1, 'h1237', ''),
    'electrons': ConfigEntry('electrons', listOfJobs.keys(), 1, 'h1237', ''),
    'muons': ConfigEntry('muons', listOfJobs.keys(), 1, 'h1237', ''),
    keyMisreconstructed: ConfigEntry(keyMisreconstructed, listOfJobs.keys(), 1, 'h1237', ''),
    'others': ConfigEntry('others', listOfJobs.keys(), 2, 'h1237', ''),
    keyIsClone: ConfigEntry(keyIsClone, listOfJobs.keys(), 0, 'h1230', ''),
    'isGhost': ConfigEntry('isGhost', listOfJobs.keys(), 3, 'h1237', ''),
}

keysOfIdentifiedParticles = pidNumbers.keys() & studiesParticles.keys()

studiesPairs = {
    'pions': ConfigEntry('pions', listOfJobs.keys(), 1, 'h3050_restrictedQ', ''),
    'kaons': ConfigEntry('kaons', listOfJobs.keys(), 1, 'h3050_restrictedQ', ''),
    'protons': ConfigEntry('protons', listOfJobs.keys(), 1, 'h3050_restrictedQ', ''),
    'mixed': ConfigEntry('mixed', listOfJobs.keys(), 1, 'h3050_restrictedQ', ''),
    keyMisreconstructed: ConfigEntry(keyMisreconstructed, listOfJobs.keys(), 1, 'h3050_restrictedQ', ''),
    'others': ConfigEntry('others', listOfJobs.keys(), 2, 'h3050_restrictedQ', ''),
    'hasClone': ConfigEntry('hasClone', listOfJobs.keys(), 0, 'h3051_restrictedQ', ''),
    'hasGhost': ConfigEntry('hasGhost', listOfJobs.keys(), 0, 'h3052_restrictedQ', ''),
}

# list to take into account in `others` calculation
keysOfIdentifiedPairs = pidNumbers.keys() & studiesPairs.keys()
maxForOthersCalculation = 100.0

# plots
listOfCutsPlots = ['0.50', '0.60', '0.65', '0.70', '0.80']
listOfJobsPlots = {cutValue: os.path.join(
    basePath, cutValue.replace('.', 'c')) for cutValue in listOfCutsPlots}


studiesPlots = {
    'corrFunc': ConfigEntry('corrFunc', listOfJobsPlots.keys(), 4, 'h4010', '_400'),
}

# helper functions


def getBoolHistInfo(hist):
    '''Get fraction [%] from histogram with 2 bins (true/false).'''
    nrFalse = hist.GetBinContent(1)
    nrTrue = hist.GetBinContent(2)
    nrEntries = hist.GetEntries()

    if not (nrTrue + nrFalse == nrEntries):
        return None
    else:
        return float(nrTrue) / nrEntries * 100


def getMCIDHistInfo(hist, pidNumber, binZero=binZeroInMCIDHists):
    '''Get fraction [%] of particles of the given type from MCID histogram.'''

    # add both charges / special case for zero
    if pidNumber != 0:
        nrCurrent = hist.GetBinContent(
            binZero + pidNumber) + hist.GetBinContent(binZero - pidNumber)
    else:
        nrCurrent = hist.GetBinContent(binZero)

    return float(nrCurrent) / hist.GetEntries() * 100


def setStyleLocal(flagStyle):
    '''Set local style (based on the general ones)'''
    ROOT.HBT.Styles.setStyle(flagStyle)

    # local style settings
    ROOT.gStyle.SetMarkerSize(ROOT.gStyle.GetMarkerSize() * 0.625); # ! a lot of points - improve visibility
    ROOT.HBT.Styles.setColorPalette()
    ROOT.gROOT.ForceStyle()


def printDescription(currentMultBin):

    # config / info
    posX = 0.075
    posY = 0.30
    selection = ROOT.SelectionClass()
    binRangesMult = selection.getBinsOfMultiplicityRangesAsStrings()

    # make description
    plotDescription = ROOT.HBT.Styles.makeLhcbLabel(posX, posX + 0.25, posY, posY + 0.10)
    plotDescription.SetTextSize(plotDescription.GetTextSize() * 0.825) # ! make slighlty smaller
    ROOT.HBT.Utils.addMultilineText("#font[12]{N}_{VELO} : " + binRangesMult[currentMultBin], plotDescription)
    plotDescription.Draw()

# functions


def getIndividualInputs(inputPathInJob, listOfJobs, nrOfBins, studies):
    '''Provide a list of individual inputs/results for a configurable list of jobs and expected #bins'''

    # basic loop over individual jobs
    inputs = {}
    for key, el in listOfJobs.items():

        curFile = ROOT.TFile(os.path.join(el, inputPathInJob), "READ")

        # get input in each mult bin
        inputsBins = {}
        for curBin in range(nrOfBins):
            inputsStudies = {}
            for studyKey, studyConfig in studies.items():
                curHistName = ROOT.HBT.Utils.getHistogramName(
                    studyConfig.hNameBase, studyConfig.hNameEnd, True, False, curBin, 1)
                input = curFile.Get(str(curHistName))

                # check if the input is valid
                if input is None:
                    print(f'Input is not valid: {curBin}')
                    continue
                else:
                    # store input
                    inputsStudies[studyKey] = input
                    # needed for histograms (?complex objects)
                    inputsStudies[studyKey].SetDirectory(0)
            inputsBins[curBin] = inputsStudies

        inputs[key] = inputsBins

        curFile.Close()

    return inputs


def sanitiseInputs(inputs):
    '''Check validity of the loaded results. Returns a list of valid bins (or None).'''

    if len(inputs) == 0:
        return None

    # get any (first) key in the dictionary
    referenceInputKey = next(iter(inputs))

    # check if each input (job) has the same number of valid entries (bins)
    expectedNrOfEntries = len(inputs[referenceInputKey])
    for key, el in inputs.items():
        if len(el) != expectedNrOfEntries:
            print(f'Hello there! (invalid inputs): {key}\n')
            return None

    # check if there are no empty histograms
    for binNr, inputsInBin in inputs.items():
        for contributionKey, inputsForContribution in inputsInBin.items():
            for jobKey, jobInput in inputsForContribution.items():
                if jobInput.GetEntries() == 0:
                    print(
                        f'Empty histograms found: {binNr} {contributionKey} {jobKey}\n')
                    return None

    return inputs[referenceInputKey].keys()


def getTransformedInputs(inputs, studies, listOfBins):
    '''Transform a list of inputs (histograms) into a convenient local data structure.'''

    transformedInputs = {}
    for bin in listOfBins:
        transformedInputsInBin = {}

        for studyKey in studies:
            curInputs = {}

            for inputKey, input in inputs.items():
                curInputs[inputKey] = input[bin][studyKey]

            transformedInputsInBin[studyKey] = curInputs

        transformedInputs[bin] = transformedInputsInBin
    return transformedInputs


def getContributions(studies, inputs, keysOfIdentified):
    '''Get actual results (contributions) based on the stored inputs.'''

    outputs = {}
    for binNr, inputsInBin in inputs.items():
        outputsInBin = {}

        for contributionKey, inputsForContribution in inputsInBin.items():
            outputsForContribution = {}

            for jobKey, jobInput in inputsForContribution.items():
                # process only info for jobs confgured in the given study
                if jobKey in studies[contributionKey].jobs:
                    # choose processing strategy
                    output = None
                    if studies[contributionKey].strategy == 0:
                        output = getBoolHistInfo(jobInput)
                    elif studies[contributionKey].strategy == 1:
                        output = getMCIDHistInfo(
                            jobInput, pidNumbers[contributionKey])
                    elif studies[contributionKey].strategy == 2:
                        output = maxForOthersCalculation
                        for identifiedKey in keysOfIdentified:
                            output -= getMCIDHistInfo(jobInput,
                                                      pidNumbers[identifiedKey])
                    elif studies[contributionKey].strategy == 3:
                        output = getMCIDHistInfo(
                            jobInput, pidNumbers[keyMisreconstructed]) - getBoolHistInfo(inputsInBin[keyIsClone][jobKey])

                    # save if valid
                    if output is None:
                        print(
                            f'Result not valid: {binNr:3d} \t {contributionKey:20s} \t {jobKey:10s}\n')
                        continue
                    else:
                        outputsForContribution[jobKey] = output

            outputsInBin[contributionKey] = outputsForContribution

        outputs[binNr] = outputsInBin
    return outputs


def makePlots(studies, inputs):
    '''Make plots based on the stored inputs.'''

    for studyKey, study in studies.items():

        # prepare global canvas
        canvas = ROOT.TCanvas(
            studyKey, study.id, ROOT.HBT.Styles.defaultCanvasSizeX, ROOT.HBT.Styles.defaultCanvasSizeY)
        canvas.SaveAs('.pdf[')

        for inputsInBinKey, inputsInBin in inputs.items():

            iSame = 0  # ! hack

            # prepare individual canvas
            canvas.Clear()
            legend = ROOT.TLegend(0.70, 0.20, 0.925, 0.475)
            legend.SetHeader('#font[12]{ProbNN}(pion)')

            for jobKey, jobInput in inputsInBin[studyKey].items():
                # process only info for jobs confgured in the given study
                if jobKey in studies[studyKey].jobs:
                    # check processing strategy
                    if studies[studyKey].strategy == 4:
                        # ! histograms style / config
                        jobInput.GetYaxis().SetRangeUser(0.5, 1.6)
                        jobInput.SetTitle(';#font[12]{Q} [GeV];#font[12]{C_{2}}(#font[12]{Q})')
                        jobInput.SetLineWidth(int(ROOT.gStyle.GetLineWidth() / 2.0)); # ! a lot of points - improve visibility
 
                        jobInput.Draw('SAME PMC PLC' if (iSame != 0) else 'PMC PLC')
                        legend.AddEntry(jobInput, '> ' + str(jobKey), 'pe')
                        iSame += 1

            # add LHCb label
            lhcbLabel = ROOT.HBT.Styles.makeLhcbLabel()
            ROOT.HBT.Utils.addMultilineText("LHCb preliminary;#font[12]{#sqrt{s_{#font[122]{NN}}}} = 5.02 TeV", lhcbLabel)
            lhcbLabel.Draw()
            canvas.Update()

            # add legend
            legend.Draw('SAME')

            # add description
            printDescription(inputsInBinKey)

            # plot individual canvas
            canvas.SaveAs('.pdf')

        # save global canvas
        canvas.SaveAs('.pdf]')


def printResults(results, description):
    '''Print values given in format values[bin][param][job].'''
    for binNr, valuesInBin in results.items():
        print(f'{description} in bin {binNr:2d} :\n')
        for keyParam, valuesForParam in valuesInBin.items():
            print(f'\t{keyParam:15s} :\n')
            for keyValue, value in valuesForParam.items():

                if isinstance(value, ROOT.TH1):
                    print(f'\t\t{keyValue:25s} : {value}')
                else:
                    print(f'\t\t{keyValue:10s} : {value:6.2f}')
            print('')
        print('')


def saveAsFile(fileName, header, fractions, studies, validBinKeys, keysOfIdentified):

    with open(fileName, 'w', encoding='utf8') as outputFile:
        # get (any/common) list of cuts for the identified particles
        cuts = fractions[next(iter(validBinKeys))][next(
            iter(keysOfIdentified))].keys()

        for bin in validBinKeys:

            outputFile.write(f'Bin nr: {bin+1:3d}\n')
            outputFile.write(f'{header}\n')

            fractionsInBin = fractions[bin]

            for cut in cuts:

                outputFile.write(f'{(cut):6s}')
                for study in studies:
                    outputFile.write(f'\t{fractionsInBin[study][cut]:6.2f}')
                outputFile.write(f'\n')

            outputFile.write(f'\n')


# main part


def main():
    # get args
    args = sys.argv[1:]
    if len(args) != 4:
        print('Wrong number of arguments!\n')
        return 1
    inputPath = args[0]
    inputPathPlots = args[1]
    ignoreBinsLower = int(args[2])
    ignoreBinsUpper = int(args[3])

    print(
        f'Input args: {inputPath} {inputPathPlots} {ignoreBinsLower} {ignoreBinsUpper}\n')

    # load info on the mult bins
    # ? make special case for nu mult bins
    selection = ROOT.SelectionClass()
    nrOfBinsMult = min(ignoreBinsUpper, selection.getNrOfBinsMult())
    binCentres = selection.getBinsOfMultiplicityCentres()
    binCentres = binCentres[ignoreBinsLower:ignoreBinsUpper]  # ! limit #bins
    print(f'Number of mult bins: {nrOfBinsMult}')
    print(f'Bin centres: {binCentres}\n')

    # do scans
    # TODO improve config (? common among studies)
    print(
        f'Number of jobs used/listed in the scan: {len(listOfJobs)}\n')

    # particle-level fractions
    inputsRawParticles = getIndividualInputs(
        inputPath, listOfJobs, nrOfBinsMult, studiesParticles)
    validBinKeysParticles = sanitiseInputs(inputsRawParticles)
    if validBinKeysParticles is None:
        print('Input not valid.\n')
        return
    # ! limit #bins
    validBinKeysParticles = [el for el in validBinKeysParticles if (
        el >= ignoreBinsLower and el < ignoreBinsUpper)]
    print(f'Valid bins: {*validBinKeysParticles,}\n')

    # transform inputs into a convenient data structure
    inputsParticles = getTransformedInputs(
        inputsRawParticles, studiesParticles, validBinKeysParticles)
    # printResults(inputsParticles, 'Inputs particle-level')

    # get the actual results/contributions
    fractionsParticles = getContributions(
        studiesParticles, inputsParticles, keysOfIdentifiedParticles)
    # printResults(fractionsParticles, 'Fractions particle-level')

    # save results
    saveAsFile(f'pidParticles.csv', f'cut \t {tabForFString.join(study for study in studiesParticles)}',
               fractionsParticles, studiesParticles, validBinKeysParticles, keysOfIdentifiedParticles)

    # get individual inputs and sanitise
    inputsRaw = getIndividualInputs(
        inputPath, listOfJobs, nrOfBinsMult, studiesPairs)
    validBinKeys = sanitiseInputs(inputsRaw)
    if validBinKeys is None:
        print('Input not valid.\n')
        return
    # ! limit #bins
    validBinKeys = [el for el in validBinKeys if (
        el >= ignoreBinsLower and el < ignoreBinsUpper)]
    print(f'Valid bins: {*validBinKeys,}\n')

    # transform inputs into a convenient data structure
    inputs = getTransformedInputs(
        inputsRaw, studiesPairs, validBinKeys)
    # printResults(inputs, 'Inputs pair-level')

    # get the actual results/contributions
    fractions = getContributions(studiesPairs, inputs, keysOfIdentifiedPairs)
    # printResults(fractions, 'Fractions pair-level')

    # save results
    saveAsFile(f'pidPairs.csv', f'cut \t {tabForFString.join(study for study in studiesPairs)}',
               fractions, studiesPairs, validBinKeys, keysOfIdentifiedPairs)

    # plots part
    print('Plots part\n')

    # set style
    setStyleLocal(ROOT.HBT.Styles.defaultStyleFlag)

    inputsRawPlots = getIndividualInputs(
        inputPathPlots, listOfJobs, nrOfBinsMult, studiesPlots)
    validBinKeysPlots = sanitiseInputs(inputsRawPlots)
    if validBinKeysPlots is None:
        print('Input not valid.\n')
        return
    validBinKeysPlots = [el for el in validBinKeysPlots if (
        el >= ignoreBinsLower and el < ignoreBinsUpper)]
    print(f'Valid bins: {*validBinKeysPlots,}\n')

    inputsPlots = getTransformedInputs(
        inputsRawPlots, studiesPlots, validBinKeysPlots)
    # printResults(inputsPlots, 'Inputs plots')

    makePlots(studiesPlots, inputsPlots)


if __name__ == '__main__':
    main()
