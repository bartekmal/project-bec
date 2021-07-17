#!/usr/bin/env python

import os

######### configuration ############

# ignore bins
ignoreBinMultLower = {'pPb': 0, 'Pbp': 0}
ignoreBinMultUpper = {'pPb': 15, 'Pbp': 100}
ignoreBinMultForKtLower = {'pPb': 0, 'Pbp': 0}
ignoreBinMultForKtUpper = {'pPb': 6, 'Pbp': 100}
ignoreBinKtLower = {'pPb': 1, 'Pbp': 1}
ignoreBinKtUpper = {'pPb': 100, 'Pbp': 100}

# histograms to draw
# histogram types: std / normalised / integrated
# flagBins: 0 (no bins) / 1 (only mult) / 2 (mult & kT)
# flagRefInput : same / custom
histogramsToDraw = [
    # 1D, selection
    {'name': 'h100', 'refName': 'h100', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h101', 'refName': 'h101', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h102', 'refName': 'h101', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h200', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': True},

    {'name': 'h1005', 'refName': 'h1005', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1006', 'refName': 'h1006', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1007', 'refName': 'h1007', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1008', 'refName': 'h1008', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1009', 'refName': 'h1009', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1010', 'refName': 'h1010', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1011', 'refName': 'h1011', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1017', 'refName': 'h1017', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1018', 'refName': 'h1018', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1019', 'refName': 'h1019', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1020', 'refName': 'h1020', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1021', 'refName': 'h1021', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1022', 'refName': 'h1022', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1023', 'refName': 'h1023', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1024', 'refName': 'h1024', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1025', 'refName': 'h1025', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1035', 'refName': 'h1035', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},

    # 1D - mult, kT, Q distributions (LIKE & EVMIX)
    {'name': 'h3002', 'refName': 'h3002', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'std', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3003', 'refName': 'h3003', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'std', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3002', 'refName': 'h3002', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3003', 'refName': 'h3003', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3002', 'refName': 'h3002', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'integrated', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3003', 'refName': 'h3003', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'integrated', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3010', 'refName': 'h3510', 'flagRefInput': 'same', 'nameEnd': '_400',
     'dim': '1D', 'type': 'normalised', 'flagBins': 2, 'isHistFullName': False},
    {'name': 'h3510', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '_400',
     'dim': '1D', 'type': 'normalised', 'flagBins': 2, 'isHistFullName': False},
    {'name': 'h3110', 'refName': 'h3610', 'flagRefInput': 'same', 'nameEnd': '_400',
     'dim': '1D', 'type': 'normalised', 'flagBins': 2, 'isHistFullName': False},
    {'name': 'h3610', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '_400',
     'dim': '1D', 'type': 'normalised', 'flagBins': 2, 'isHistFullName': False},

    # 2D - mult / kT VS Q (LIKE & EVMIX)
    {'name': 'h2000', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2500', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2100', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2600', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2001', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2501', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2101', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2601', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},

    # zPv plots
    {'name': 'h1011', 'refName': 'h1011', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': True},
    {'name': 'h1050_2D', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': True},

    {'name': 'h2002', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2003', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2502', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2503', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},

    {'name': 'h3002', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3002', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3002', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'standard', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3002', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},

    # angular plots
    {'name': 'h2004', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2005', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2006', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2007', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2008', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2104', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2105', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2106', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2107', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2108', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2504', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2505', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2506', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2507', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2508', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2604', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2605', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2606', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2607', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h2608', 'refName': '', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '2D', 'type': 'standard', 'flagBins': 0, 'isHistFullName': False},

    {'name': 'h3020', 'refName': 'h3020', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3020', 'refName': 'h3020', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3021', 'refName': 'h3021', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3021', 'refName': 'h3021', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3022', 'refName': 'h3022', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3022', 'refName': 'h3022', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3120', 'refName': 'h3120', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3120', 'refName': 'h3120', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3121', 'refName': 'h3121', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3121', 'refName': 'h3121', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3122', 'refName': 'h3122', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3122', 'refName': 'h3122', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3520', 'refName': 'h3520', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3520', 'refName': 'h3520', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3521', 'refName': 'h3521', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3521', 'refName': 'h3521', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3522', 'refName': 'h3522', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3522', 'refName': 'h3522', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3620', 'refName': 'h3620', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3620', 'refName': 'h3620', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3621', 'refName': 'h3621', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3621', 'refName': 'h3621', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},
    {'name': 'h3622', 'refName': 'h3622', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 0, 'isHistFullName': False},
    {'name': 'h3622', 'refName': 'h3622', 'flagRefInput': 'custom', 'nameEnd': '',
     'dim': '1D', 'type': 'normalised', 'flagBins': 1, 'isHistFullName': False},

]

######### end of configuration ############

# define a structure for BEC jobs


class jobBEC:
    def __init__(self, jobName, outputPath, inputData, dataType, outputFileName='HBTAnalysis.root', nrOfEventsToProcess=''):
        self.jobName = jobName
        self.outputPath = outputPath
        self.inputData = inputData
        self.dataType = dataType
        self.outputFileName = outputFileName
        self.nrOfEventsToProcess = str(nrOfEventsToProcess)

# helper functions


def mkdirIfNotExists(myPath):
    if not os.path.exists(myPath):
        os.mkdir(myPath)


def recreateDir(folderName):
    os.system('rm -r ' + folderName)
    os.mkdir(folderName)


def recreateAndChangeDir(folderName):
    os.system('rm -r ' + folderName)
    os.mkdir(folderName)
    os.chdir(folderName)


def prepareFolder(outputFolderPath, outputFolderName):
    tmpFolder = outputFolderPath + "/" + outputFolderName
    mkdirIfNotExists(tmpFolder)

    return tmpFolder

# set the BEC environment


def setEnvironmentBEC():
    currentHost = os.getenv('BEC_HOST')

    if currentHost == "lxplus":
        # job flavours in condor
        # espresso     = 20 mins
        # microcentury = 1 hour
        # longlunch    = 2 hours
        # workday      = 8 hours
        # tomorrow     = 1 day
        # testmatch    = 3 days
        # nextweek     = 1 week
        os.environ['BEC_SETUP_SCRIPT'] = os.environ['BEC_BASE']+'/setupBEC.sh'
        os.environ['BEC_BATCH_QUEUE'] = 'tomorrow'
        os.environ['BEC_BATCH_MAXTIME'] = '86000'  # in secs
        os.environ['BEC_BATCH_TMPDIR'] = 'TMPDIR'
        os.environ['BEC_BATCH_SYSTEM_COMMAND'] = 'condor_submit'
        print("BEC environment set for user: bmalecki (lxplus)\n")
        return True
    elif currentHost == "plgrid":
        os.environ['BEC_SETUP_SCRIPT'] = os.environ['BEC_BASE']+'/setupBEC.sh'
        os.environ['BEC_BATCH_QUEUE'] = 'plgrid'
        os.environ['BEC_BATCH_MAXTIME'] = '24:00:00'
        os.environ['BEC_BATCH_TMPDIR'] = 'SLURM_MYDIR'
        os.environ['BEC_BATCH_SYSTEM_COMMAND'] = 'sbatch -p ' + \
            os.getenv('BEC_BATCH_QUEUE') + ' -t ' + \
            os.getenv('BEC_BATCH_MAXTIME')
        print("BEC environment set for user: plgbmalecki (PLGrid)\n")
        return True
    else:
        return False


def createRunScriptForJob(aJob, outputDir):

    singleJobOnBatchFileName = 'start_job.sh'
    batchSubmitFileName = 'condor.sub'

    myFile = open(singleJobOnBatchFileName, 'w')

    myFile.write('#!/bin/sh\n')
    myFile.write('\n')
    myFile.write('#setup environment\n')
    myFile.write('source /cvmfs/lhcb.cern.ch/lib/LbEnv.sh -c ' +
                 os.getenv("MY_CMTCONFIG") + '\n')
    myFile.write('source ' + os.getenv("MY_ROOT_SETUP") + '\n')
    myFile.write('\n')
    myFile.write('cd ${' + os.getenv("BEC_BATCH_TMPDIR") + '}\n')
    myFile.write(os.getenv('CODE_EXEC') + ' ' + aJob.inputData + ' ' + aJob.dataType + ' ' +
                 aJob.outputFileName + ' ' + aJob.nrOfEventsToProcess + ' > HBTAnalysis.log' + '\n')
    myFile.write('\n')
    myFile.write('#get output\n')
    myFile.write('sleep ' + os.getenv('SLEEP_TIME') + '\n')
    myFile.write('mv *.log ' + outputDir + '/HBTAnalysis.log\n')
    myFile.write('mv *.root ' + outputDir + '/HBTAnalysis.root\n')

    myFile.close()

    submitFile = open(batchSubmitFileName, 'w')

    submitFile.write('executable = ' + singleJobOnBatchFileName + '\n')
    submitFile.write('arguments = $(ProcId)\n')
    submitFile.write('\n')
    submitFile.write('log = condor.log\n')
    submitFile.write('output = condor_$(ProcId).out\n')
    submitFile.write('error = condor_$(ProcId).err\n')
    submitFile.write('+JobFlavour = ' + os.getenv('BEC_BATCH_QUEUE') + '\n')
    submitFile.write('+MaxRuntime = ' + os.getenv('BEC_BATCH_MAXTIME') + '\n')
    submitFile.write('\n')
    submitFile.write('queue 1\n')

    submitFile.close()

    return os.path.realpath(submitFile.name)

# analysis `rules`


def runHBTJobs(outputFolderName="HBTAnalysis"):

    # prepare empty job directories
    os.environ['TEMP_DIR'] = os.getenv('MYDIR') + '/temp'
    recreateDir(os.getenv('TEMP_DIR'))
    recreateDir(os.getenv('MYDIR') + '/output')
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print("Running HBT jobs with output in directory:\n{0}\n".format(
        outputDirPath))

    # define jobs
    jobsToRun = [
        jobBEC(jobName='RD_pPb_MU', outputPath='RD_pPb_MU', inputData=os.getenv(
            'BEC_STORAGE') + '/ntuple/data/pPb_MU/hbtNTuple.root', dataType='RD'),
        jobBEC(jobName='RD_pPb_MD', outputPath='RD_pPb_MD', inputData=os.getenv(
            'BEC_STORAGE') + '/ntuple/data/pPb_MD/hbtNTuple.root', dataType='RD'),
        jobBEC(jobName='RD_Pbp_MU', outputPath='RD_Pbp_MU', inputData=os.getenv(
            'BEC_STORAGE') + '/ntuple/data/Pbp_MU/hbtNTuple.root', dataType='RD'),
        jobBEC(jobName='RD_Pbp_MD', outputPath='RD_Pbp_MD', inputData=os.getenv(
            'BEC_STORAGE') + '/ntuple/data/Pbp_MD/hbtNTuple.root', dataType='RD'),
        jobBEC(jobName='MC_pPb_MU', outputPath='MC_pPb_MU', inputData=os.getenv('BEC_STORAGE') +
               '/ntuple/sim/epos/central/Sim09c/pPb_MU/hbtNTuple_corrected.root', dataType='MC'),
        jobBEC(jobName='MC_pPb_MD', outputPath='MC_pPb_MD', inputData=os.getenv('BEC_STORAGE') +
               '/ntuple/sim/epos/central/Sim09c/pPb_MD/hbtNTuple_corrected.root', dataType='MC'),
        jobBEC(jobName='MC_Pbp_MU', outputPath='MC_Pbp_MU', inputData=os.getenv('BEC_STORAGE') +
               '/ntuple/sim/epos/central/Sim09c/Pbp_MU/hbtNTuple_corrected.root', dataType='MC'),
        jobBEC(jobName='MC_Pbp_MD', outputPath='MC_Pbp_MD', inputData=os.getenv('BEC_STORAGE') +
               '/ntuple/sim/epos/central/Sim09c/Pbp_MD/hbtNTuple_corrected.root', dataType='MC'),
        jobBEC(jobName='EPOS_local_pPb_MU', outputPath='EPOS_local_pPb_MU', inputData=os.getenv(
            'BEC_STORAGE') + '/ntuple/sim/epos/local/Sim09c/pPb_MU/hbtNTuple_corrected.root', dataType='MC'),
        jobBEC(jobName='EPOS_local_pPb_MD', outputPath='EPOS_local_pPb_MD', inputData=os.getenv(
            'BEC_STORAGE') + '/ntuple/sim/epos/local/Sim09c/pPb_MD/hbtNTuple_corrected.root', dataType='MC'),
        jobBEC(jobName='EPOS_local_Pbp_MU', outputPath='EPOS_local_Pbp_MU', inputData=os.getenv(
            'BEC_STORAGE') + '/ntuple/sim/epos/local/Sim09c/Pbp_MU/hbtNTuple_corrected.root', dataType='MC'),
        jobBEC(jobName='EPOS_local_Pbp_MD', outputPath='EPOS_local_Pbp_MD', inputData=os.getenv(
            'BEC_STORAGE') + '/ntuple/sim/epos/local/Sim09c/Pbp_MD/hbtNTuple_corrected.root', dataType='MC'),
    ]

    # prepare temporary job scripts/files
    os.chdir(os.getenv('TEMP_DIR'))

    # compile
    os.environ['CODE_NAME'] = 'HBTAnalysis'
    os.environ['CODE_INPUT'] = os.getenv('BEC_BASE_CODE_HBT')
    # os.environ['CODE_INPUT'] = os.getenv('MYDIR') + "/HBTAnalysis"
    os.environ['CODE_EXEC'] = os.getenv(
        'TEMP_DIR')+'/'+os.getenv('CODE_NAME')+'.exe'
    os.system(
        'COMPILER=`root-config --cxx`;' +
        'FLAGS=`root-config --cflags --libs`;' +
        '$COMPILER -g -O3 -Wall -Wextra -Wpedantic ${CODE_INPUT}/*.cpp -o ${CODE_EXEC} $FLAGS'
    )

    # run job
    for aJob in jobsToRun:

        # prepare folder for output files
        os.chdir(outputDirPath)
        recreateDir(aJob.outputPath)
        outputDirForCurrentJob = outputDirPath + "/" + aJob.outputPath

        # temporary job files
        os.chdir(os.getenv('TEMP_DIR'))
        recreateAndChangeDir(aJob.outputPath)
        runFilePath = createRunScriptForJob(aJob, outputDirForCurrentJob)

        os.system(os.getenv('BEC_BATCH_SYSTEM_COMMAND') + " " + runFilePath)
        # os.system( '${CODE_EXEC} ' + aJob.inputData + ' ' + aJob.dataType + ' ' + aJob.outputFileName + ' ' + aJob.nrOfEventsToProcess + ' > HBTAnalysis.log')


def runMerge(outputFolderName="merged"):

    # prepare output directory
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print("Running merge in directory:\n{0}\n".format(outputDirPath))

    # define jobs
    jobsToRun = [
        {
            'outputFolder': "RD_pPb",
            'filesToMerge': [
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/RD_pPb_MU/HBTAnalysis.root",
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/RD_pPb_MD/HBTAnalysis.root"
            ]
        },
        {
            'outputFolder': "RD_Pbp",
            'filesToMerge': [
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/RD_Pbp_MU/HBTAnalysis.root",
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/RD_Pbp_MD/HBTAnalysis.root",
            ]
        },
        {
            'outputFolder': "MC_pPb",
            'filesToMerge': [
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/MC_pPb_MU/HBTAnalysis.root",
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/MC_pPb_MD/HBTAnalysis.root",
                os.getenv(
                    'MYDIR') + "/output/HBTAnalysis/EPOS_local_pPb_MU/HBTAnalysis.root",
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/EPOS_local_pPb_MD/HBTAnalysis.root"
            ]
        },
        {
            'outputFolder': "MC_Pbp",
            'filesToMerge': [
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/MC_Pbp_MU/HBTAnalysis.root",
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/MC_Pbp_MD/HBTAnalysis.root",
                os.getenv(
                    'MYDIR') + "/output/HBTAnalysis/EPOS_local_Pbp_MU/HBTAnalysis.root",
                os.getenv('MYDIR') +
                "/output/HBTAnalysis/EPOS_local_Pbp_MD/HBTAnalysis.root"
            ]
        }
    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        recreateAndChangeDir(aJob['outputFolder'])

        os.system('hadd -ff merged.root' + ' ' +
                  ' '.join(aJob['filesToMerge']) + ' >> merge.log')


def runHistograms(outputFolderName="histograms"):

    # prepare output directory
    os.chdir(os.getenv('MYDIR') + '/output')
    recreateAndChangeDir(outputFolderName)
    outputDirPath = os.getenv('MYDIR') + '/output/' + outputFolderName

    print("Running histograms in directory:\n{0}\n".format(outputDirPath))

    # define jobs
    for hist in histogramsToDraw:

        isHist1D = (hist['dim'] == '1D')
        flagNormalise = (hist['type'] == 'normalised')
        flagIntegrated = (hist['type'] == 'integrated')
        flagRefInputSame = (hist['flagRefInput'] == 'same')

        jobsToRun = [
            {
                'fileMainPath': os.getenv('MYDIR') + "/output/merged/RD_pPb/merged.root",
                'hMainName': hist['name'],
                'fileRefPath': (os.getenv('MYDIR') + "/output/merged/RD_pPb/merged.root") if flagRefInputSame else (os.getenv('MYDIR') + "/output/merged/RD_Pbp/merged.root"),
                'hRefName': hist['refName'],
                'dataTypeRef': "RD_pPb" if flagRefInputSame else "RD_Pbp",
                'hMainNameEnd': hist['nameEnd'],
                'outputFolder': "RD_pPb"
            },
            {
                'fileMainPath': os.getenv('MYDIR') + "/output/merged/RD_Pbp/merged.root",
                'hMainName': hist['name'],
                'fileRefPath': (os.getenv('MYDIR') + "/output/merged/RD_Pbp/merged.root") if flagRefInputSame else (os.getenv('MYDIR') + "/output/merged/RD_pPb/merged.root"),
                'hRefName': hist['refName'],
                'dataTypeRef': "RD_Pbp" if flagRefInputSame else "RD_pPb",
                'hMainNameEnd': hist['nameEnd'],
                'outputFolder': "RD_Pbp"
            }
        ]

        # run job
        for aJob in jobsToRun:
            os.chdir(outputDirPath)
            mkdirIfNotExists(aJob['outputFolder'])
            os.chdir(aJob['outputFolder'])

            os.system('root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawHistograms.C("{}","{}","{}","{}","{}","{}","{}",{},{},{},{},{})\' >> histograms.log'.format(
                aJob['fileMainPath'], aJob['hMainName'], aJob['fileRefPath'], aJob['hRefName'], aJob['hMainNameEnd'], aJob['outputFolder'], aJob['dataTypeRef'], int(
                    isHist1D), int(flagNormalise), int(flagIntegrated), hist['flagBins'], int(hist['isHistFullName'])
            ))


def runDivide(outputFolderName="correlations"):

    # prepare output directory
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print("Running divide in directory:\n{0}\n".format(outputDirPath))

    # define jobs
    jobsToRun = [
        {
            'file1Path': os.getenv('MYDIR') + "/output/merged/RD_pPb/merged.root",
            'h1NameLike': "h3010",
            'h1NameUnlike': "h3110",
            'file2Path': os.getenv('MYDIR') + "/output/merged/RD_pPb/merged.root",
            'h2NameLike': "h3510",
            'h2NameUnlike': "h3610",
            'outputFolder': "RD_pPb",
            'hOutNameLike': "h4010",
            'hOutNameUnlike': "h4110",
            'isDR': False,
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400",
            'flagCorrectCoulomb': False
        },
        {
            'file1Path': os.getenv('MYDIR') + "/output/merged/RD_Pbp/merged.root",
            'h1NameLike': "h3010",
            'h1NameUnlike': "h3110",
            'file2Path': os.getenv('MYDIR') + "/output/merged/RD_Pbp/merged.root",
            'h2NameLike': "h3510",
            'h2NameUnlike': "h3610",
            'outputFolder': "RD_Pbp",
            'hOutNameLike': "h4010",
            'hOutNameUnlike': "h4110",
            'isDR': False,
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400",
            'flagCorrectCoulomb': False
        },
        {
            'file1Path': os.getenv('MYDIR') + "/output/merged/MC_pPb/merged.root",
            'h1NameLike': "h3010",
            'h1NameUnlike': "h3110",
            'file2Path': os.getenv('MYDIR') + "/output/merged/MC_pPb/merged.root",
            'h2NameLike': "h3510",
            'h2NameUnlike': "h3610",
            'outputFolder': "MC_pPb",
            'hOutNameLike': "h4010",
            'hOutNameUnlike': "h4110",
            'isDR': False,
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400",
            'flagCorrectCoulomb': False
        },
        {
            'file1Path': os.getenv('MYDIR') + "/output/merged/MC_Pbp/merged.root",
            'h1NameLike': "h3010",
            'h1NameUnlike': "h3110",
            'file2Path': os.getenv('MYDIR') + "/output/merged/MC_Pbp/merged.root",
            'h2NameLike': "h3510",
            'h2NameUnlike': "h3610",
            'outputFolder': "MC_Pbp",
            'hOutNameLike': "h4010",
            'hOutNameUnlike': "h4110",
            'isDR': False,
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400",
            'flagCorrectCoulomb': False
        },
        {
            'file1Path': os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'h1NameLike': "h4010",
            'h1NameUnlike': "h4110",
            'file2Path': os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
            'h2NameLike': "h4010",
            'h2NameUnlike': "h4110",
            'outputFolder': "DR_pPb",
            'hOutNameLike': "h5010",
            'hOutNameUnlike': "h5110",
            'isDR': True,
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400",
            'flagCorrectCoulomb': False
        },
        {
            'file1Path': os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'h1NameLike': "h4010",
            'h1NameUnlike': "h4110",
            'file2Path': os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
            'h2NameLike': "h4010",
            'h2NameUnlike': "h4110",
            'outputFolder': "DR_Pbp",
            'hOutNameLike': "h5010",
            'hOutNameUnlike': "h5110",
            'isDR': True,
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400",
            'flagCorrectCoulomb': False
        },

    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        recreateAndChangeDir(aJob['outputFolder'])
        outputFile = os.getenv('MYDIR') + "/output/correlations/" + \
            aJob['outputFolder'] + "/correlations.root"

        os.system('root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}","{6}","{7}",{8},{9},{10})\' >> divide.log'.format(
            aJob['file1Path'], aJob['h1NameLike'], aJob['file2Path'], aJob['h2NameLike'], outputFile, aJob['hOutNameLike'], aJob[
                'hNameCommonEndForMult'], aJob['hNameCommonEndForKt'], int(aJob['isDR']), int(aJob['flagCorrectCoulomb']), int(True)
        ))
        os.system('root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}","{6}","{7}",{8},{9},{10})\' >> divide.log'.format(
            aJob['file1Path'], aJob['h1NameUnlike'], aJob['file2Path'], aJob['h2NameUnlike'], outputFile, aJob['hOutNameUnlike'], aJob[
                'hNameCommonEndForMult'], aJob['hNameCommonEndForKt'], int(aJob['isDR']), int(aJob['flagCorrectCoulomb']), int(False)
        ))


def runFits(outputFolderName="fits"):

    # prepare output directory
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print("Running plots in directory:\n{0}\n".format(outputDirPath))

    # define jobs
    jobsToRun = [
        # MC UNLIKE
        # {
        #     'inputFile' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
        #     'hMainNameBase' : "h4110",
        #     'isMC' : "true",
        #     'isUnlike' : "true",
        #     'outputFolder' : "MC_pPb",
        #     'hCommonEndNameForMult' : "_400",
        #     'hCommonEndNameForKt' : "_400",
        #     'flagDoFit' : 1,
        #     'inputFileRef' : "",
        #     'hRefNameBase' : "",
        #     'refType' : "",
        #     'flagDrawRef' : "false",
        #     'flagUseBkgFromRef' : "false",
        # },
        # {
        #     'inputFile' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
        #     'hMainNameBase' : "h4110",
        #     'isMC' : "true",
        #     'isUnlike' : "true",
        #     'outputFolder' : "MC_Pbp",
        #     'hCommonEndNameForMult' : "_400",
        #     'hCommonEndNameForKt' : "_400",
        #     'flagDoFit' : 1,
        #     'inputFileRef' : "",
        #     'hRefNameBase' : "",
        #     'refType' : "",
        #     'flagDrawRef' : "false",
        #     'flagUseBkgFromRef' : "false",
        # },

        # MC LIKE
        # {
        #     'inputFile' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
        #     'hMainNameBase' : "h4010",
        #     'isMC' : "true",
        #     'isUnlike' : "false",
        #     'outputFolder' : "MC_pPb",
        #     'hCommonEndNameForMult' : "_400",
        #     'hCommonEndNameForKt' : "_400",
        #     'flagDoFit' : 1,
        #     'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
        #     'hRefNameBase' : "h4110",
        #     'refType' : "MC_pPb - UNLIKE",
        #     'flagDrawRef' : "true",
        #     'flagUseBkgFromRef' : "false",
        # },
        # {
        #     'inputFile' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
        #     'hMainNameBase' : "h4010",
        #     'isMC' : "true",
        #     'isUnlike' : "false",
        #     'outputFolder' : "MC_Pbp",
        #     'hCommonEndNameForMult' : "_400",
        #     'hCommonEndNameForKt' : "_400",
        #     'flagDoFit' : 1,
        #     'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
        #     'hRefNameBase' : "h4110",
        #     'refType' : "MC_Pbp - UNLIKE",
        #     'flagDrawRef' : "true",
        #     'flagUseBkgFromRef' : "false",
        # },

        # DATA UNLIKE
        {
            'inputFile': os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hMainNameBase': "h4110",
            'hMainNameBase_1': "h3110",
            'hMainNameBase_2': "h3610",
            'isMC': "false",
            'isUnlike': "true",
            'outputFolder': "RD_pPb",
            'hCommonEndNameForMult': "_400",
            'hCommonEndNameForKt': "_400",
            'flagDoFit': 2,
            'inputFileRef': os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
            'hRefNameBase': "h4110",
            'refType': "MC_pPb - UNLIKE",
            'flagDrawRef': "false",
            'flagUseBkgFromRef': "false",
            'flagUseBkgScaling': "false",
            'ignoreBinMultLower': ignoreBinMultLower['pPb'],
            'ignoreBinMultUpper': ignoreBinMultUpper['pPb'],
            'ignoreBinMultForKtLower': ignoreBinMultForKtLower['pPb'],
            'ignoreBinMultForKtUpper': ignoreBinMultForKtUpper['pPb'],
            'ignoreBinKtLower': ignoreBinKtLower['pPb'],
            'ignoreBinKtUpper': ignoreBinKtUpper['pPb']
        },
        {
            'inputFile': os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hMainNameBase': "h4110",
            'hMainNameBase_1': "h3110",
            'hMainNameBase_2': "h3610",
            'isMC': "false",
            'isUnlike': "true",
            'outputFolder': "RD_Pbp",
            'hCommonEndNameForMult': "_400",
            'hCommonEndNameForKt': "_400",
            'flagDoFit': 2,
            'inputFileRef': os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
            'hRefNameBase': "h4110",
            'refType': "MC_Pbp - UNLIKE",
            'flagDrawRef': "false",
            'flagUseBkgFromRef': "false",
            'flagUseBkgScaling': "false",
            'ignoreBinMultLower': ignoreBinMultLower['Pbp'],
            'ignoreBinMultUpper': ignoreBinMultUpper['Pbp'],
            'ignoreBinMultForKtLower': ignoreBinMultForKtLower['Pbp'],
            'ignoreBinMultForKtUpper': ignoreBinMultForKtUpper['Pbp'],
            'ignoreBinKtLower': ignoreBinKtLower['Pbp'],
            'ignoreBinKtUpper': ignoreBinKtUpper['Pbp']

        },

        # DATA LIKE
        {
            'inputFile': os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hMainNameBase': "h4010",
            'hMainNameBase_1': "h3010",
            'hMainNameBase_2': "h3510",
            'isMC': "false",
            'isUnlike': "false",
            'outputFolder': "RD_pPb",
            'hCommonEndNameForMult': "_400",
            'hCommonEndNameForKt': "_400",
            'flagDoFit': 1,
            'inputFileRef': os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hRefNameBase': "h4110",
            'refType': "RD_pPb - UNLIKE",
            'flagDrawRef': "true",
            'flagUseBkgFromRef': "true",
            'flagUseBkgScaling': "false",
            'ignoreBinMultLower': ignoreBinMultLower['pPb'],
            'ignoreBinMultUpper': ignoreBinMultUpper['pPb'],
            'ignoreBinMultForKtLower': ignoreBinMultForKtLower['pPb'],
            'ignoreBinMultForKtUpper': ignoreBinMultForKtUpper['pPb'],
            'ignoreBinKtLower': ignoreBinKtLower['pPb'],
            'ignoreBinKtUpper': ignoreBinKtUpper['pPb']
        },
        {
            'inputFile': os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hMainNameBase': "h4010",
            'hMainNameBase_1': "h3010",
            'hMainNameBase_2': "h3510",
            'isMC': "false",
            'isUnlike': "false",
            'outputFolder': "RD_Pbp",
            'hCommonEndNameForMult': "_400",
            'hCommonEndNameForKt': "_400",
            'flagDoFit': 1,
            'inputFileRef': os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hRefNameBase': "h4110",
            'refType': "RD_Pbp - UNLIKE",
            'flagDrawRef': "true",
            'flagUseBkgFromRef': "true",
            'flagUseBkgScaling': "false",
            'ignoreBinMultLower': ignoreBinMultLower['Pbp'],
            'ignoreBinMultUpper': ignoreBinMultUpper['Pbp'],
            'ignoreBinMultForKtLower': ignoreBinMultForKtLower['Pbp'],
            'ignoreBinMultForKtUpper': ignoreBinMultForKtUpper['Pbp'],
            'ignoreBinKtLower': ignoreBinKtLower['Pbp'],
            'ignoreBinKtUpper': ignoreBinKtUpper['Pbp']
        }
    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        mkdirIfNotExists(aJob['outputFolder'])
        os.chdir(aJob['outputFolder'])

        os.system('root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/fitInBins.C("{}","{}","{}","{}",{},{},"{}","{}","{}",{},"{}","{}","{}",{},{},{},{},{},{},{},{},{})\' >> fit.log'.format(
            aJob['inputFile'], aJob['hMainNameBase'], aJob['hMainNameBase_1'], aJob['hMainNameBase_2'], aJob['isMC'], aJob['isUnlike'], aJob['outputFolder'], aJob['hCommonEndNameForMult'],  aJob['hCommonEndNameForKt'], int(
                aJob['flagDoFit']), aJob['inputFileRef'], aJob['hRefNameBase'], aJob['refType'], aJob['flagDrawRef'], aJob['flagUseBkgFromRef'], aJob['flagUseBkgScaling'], int(aJob['ignoreBinMultLower']), int(aJob['ignoreBinMultUpper']), int(aJob['ignoreBinMultForKtLower']), int(aJob['ignoreBinMultForKtUpper']), int(aJob['ignoreBinKtLower']), int(aJob['ignoreBinKtUpper'])
        ))


def runTrends(outputFolderName="trends"):

    # prepare output directory
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print("Running trends in directory:\n{0}\n".format(outputDirPath))

    # define jobs
    jobsToRun = [
        # MC UNLIKE
        # {
        #     'fileName' : os.getenv('MYDIR') + "/output/fits/MC_pPb/fitResults.root",
        #     'hBaseName' : "h4110",
        #     'flagIsUnlike' : "true",
        #     'outputFolder' : "MC_pPb",
        #     'hNameCommonEndForMult' : "_400",
        #     'hNameCommonEndForKt' : "_400"
        # },
        # {
        #     'fileName' : os.getenv('MYDIR') + "/output/fits/MC_Pbp/fitResults.root",
        #     'hBaseName' : "h4110",
        #     'flagIsUnlike' : "true",
        #     'outputFolder' : "MC_Pbp",
        #     'hNameCommonEndForMult' : "_400",
        #     'hNameCommonEndForKt' : "_400"
        # },

        # MC LIKE
        # {
        #     'fileName' : os.getenv('MYDIR') + "/output/fits/MC_pPb/fitResults.root",
        #     'hBaseName' : "h4010",
        #     'flagIsUnlike' : "false",
        #     'outputFolder' : "MC_pPb",
        #     'hNameCommonEndForMult' : "_400",
        #     'hNameCommonEndForKt' : "_400"
        # },
        # {
        #     'fileName' : os.getenv('MYDIR') + "/output/fits/MC_Pbp/fitResults.root",
        #     'hBaseName' : "h4010",
        #     'flagIsUnlike' : "false",
        #     'outputFolder' : "MC_Pbp",
        #     'hNameCommonEndForMult' : "_400",
        #     'hNameCommonEndForKt' : "_400"
        # },

        # DATA UNLIKE
        # {
        #     'fileName': os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
        #     'hBaseName': "h4110",
        #     'fName': "funcMain",
        #     'flagIsUnlike': "true",
        #     'outputFolder': "RD_pPb",
        #     'hNameCommonEndForMult': "_400",
        #     'hNameCommonEndForKt': "_400"
        # },
        # {
        #     'fileName': os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
        #     'hBaseName': "h4110",
        #     'fName': "funcMain",
        #     'flagIsUnlike': "true",
        #     'outputFolder': "RD_Pbp",
        #     'hNameCommonEndForMult': "_400",
        #     'hNameCommonEndForKt': "_400"
        # },

        # DATA LIKE
        {
            'fileName': os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseName': "h4010",
            'fName': "funcMain",
            'flagIsUnlike': "false",
            'outputFolder': "RD_pPb",
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400"
        },
        {
            'fileName': os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseName': "h4010",
            'fName': "funcMain",
            'flagIsUnlike': "false",
            'outputFolder': "RD_Pbp",
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400"
        }
    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        mkdirIfNotExists(aJob['outputFolder'])
        os.chdir(aJob['outputFolder'])

        os.system('root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawTrends.C("{}","{}","{}",{},"{}","{}","{}")\' >> trends.log'.format(
            aJob['fileName'], aJob['hBaseName'], aJob['fName'], aJob['flagIsUnlike'], aJob[
                'outputFolder'], aJob['hNameCommonEndForMult'], aJob['hNameCommonEndForKt']
        ))


def runDiffs(referencePath=os.getenv('BEC_BASE_ANALYSIS') + "/main/output/fits", outputFolderName="diffs"):

    # prepare output directory
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print("Running trends in directory:\n{0}\n".format(outputDirPath))

    # define jobs
    jobsToRun = [
        # # DATA UNLIKE
        # {
        #     'fileNameMain': os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
        #     'hBaseNameMain': "h4110",
        #     'fNameMain': "funcMain",
        #     'fileNameRef': referencePath + "/RD_pPb/fitResults.root",
        #     'hBaseNameRef': "h4110",
        #     'fNameRef': "funcMain",
        #     'flagIsUnlike': "true",
        #     'outputFolder': "RD_pPb",
        #     'hNameCommonEndForMult': "_400",
        #     'hNameCommonEndForKt': "_400"
        # },
        # {
        #     'fileNameMain': os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
        #     'hBaseNameMain': "h4110",
        #     'fNameMain': "funcMain",
        #     'fileNameRef': referencePath + "/RD_Pbp/fitResults.root",
        #     'hBaseNameRef': "h4110",
        #     'fNameRef': "funcMain",
        #     'flagIsUnlike': "true",
        #     'outputFolder': "RD_Pbp",
        #     'hNameCommonEndForMult': "_400",
        #     'hNameCommonEndForKt': "_400"
        # },

        # DATA LIKE
        {
            'fileNameMain': os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseNameMain': "h4010",
            'fNameMain': "funcMain",
            'fileNameRef': referencePath + "/RD_pPb/fitResults.root",
            'hBaseNameRef': "h4010",
            'fNameRef': "funcMain",
            'flagIsUnlike': "false",
            'outputFolder': "RD_pPb",
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400"
        },
        {
            'fileNameMain': os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseNameMain': "h4010",
            'fNameMain': "funcMain",
            'fileNameRef': referencePath + "/RD_Pbp/fitResults.root",
            'hBaseNameRef': "h4010",
            'fNameRef': "funcMain",
            'flagIsUnlike': "false",
            'outputFolder': "RD_Pbp",
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400"
        }
    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        mkdirIfNotExists(aJob['outputFolder'])
        os.chdir(aJob['outputFolder'])

        os.system('root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawDiffs.C("{}","{}","{}","{}","{}","{}",{},"{}","{}","{}")\' >> diffs.log'.format(
            aJob['fileNameMain'], aJob['hBaseNameMain'], aJob['fNameMain'], aJob['fileNameRef'], aJob['hBaseNameRef'], aJob[
                'fNameRef'], aJob['flagIsUnlike'], aJob['outputFolder'], aJob['hNameCommonEndForMult'], aJob['hNameCommonEndForKt']
        ))


def runFitsScaledBkg(outputFolderName="fits_scaledBkg"):

    # prepare output directory
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print("Running plots in directory:\n{0}\n".format(outputDirPath))

    # define jobs
    jobsToRun = [

        # DATA LIKE
        {
            'inputFile': os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hMainNameBase': "h4010",
            'hMainNameBase_1': "h3010",
            'hMainNameBase_2': "h3510",
            'isMC': "false",
            'isUnlike': "false",
            'outputFolder': "RD_pPb",
            'hCommonEndNameForMult': "_400",
            'hCommonEndNameForKt': "_400",
            'flagDoFit': 1,
            'inputFileRef': os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hRefNameBase': "h4110",
            'refType': "RD_pPb - UNLIKE",
            'flagDrawRef': "true",
            'flagUseBkgFromRef': "true",
            'flagUseBkgScaling': "true",
            'ignoreBinMultLower': ignoreBinMultLower['pPb'],
            'ignoreBinMultUpper': ignoreBinMultUpper['pPb'],
            'ignoreBinMultForKtLower': ignoreBinMultForKtLower['pPb'],
            'ignoreBinMultForKtUpper': ignoreBinMultForKtUpper['pPb'],
            'ignoreBinKtLower': ignoreBinKtLower['pPb'],
            'ignoreBinKtUpper': ignoreBinKtUpper['pPb']
        },
        {
            'inputFile': os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hMainNameBase': "h4010",
            'hMainNameBase_1': "h3010",
            'hMainNameBase_2': "h3510",
            'isMC': "false",
            'isUnlike': "false",
            'outputFolder': "RD_Pbp",
            'hCommonEndNameForMult': "_400",
            'hCommonEndNameForKt': "_400",
            'flagDoFit': 1,
            'inputFileRef': os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hRefNameBase': "h4110",
            'refType': "RD_Pbp - UNLIKE",
            'flagDrawRef': "true",
            'flagUseBkgFromRef': "true",
            'flagUseBkgScaling': "true",
            'ignoreBinMultLower': ignoreBinMultLower['Pbp'],
            'ignoreBinMultUpper': ignoreBinMultUpper['Pbp'],
            'ignoreBinMultForKtLower': ignoreBinMultForKtLower['Pbp'],
            'ignoreBinMultForKtUpper': ignoreBinMultForKtUpper['Pbp'],
            'ignoreBinKtLower': ignoreBinKtLower['Pbp'],
            'ignoreBinKtUpper': ignoreBinKtUpper['Pbp']
        }
    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        mkdirIfNotExists(aJob['outputFolder'])
        os.chdir(aJob['outputFolder'])

        os.system('root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/fitInBins.C("{}","{}","{}","{}",{},{},"{}","{}","{}",{},"{}","{}","{}",{},{},{},{},{},{},{},{},{})\' >> fit.log'.format(
            aJob['inputFile'], aJob['hMainNameBase'], aJob['hMainNameBase_1'], aJob['hMainNameBase_2'], aJob['isMC'], aJob['isUnlike'], aJob['outputFolder'], aJob['hCommonEndNameForMult'],  aJob['hCommonEndNameForKt'], int(
                aJob['flagDoFit']), aJob['inputFileRef'], aJob['hRefNameBase'], aJob['refType'], aJob['flagDrawRef'], aJob['flagUseBkgFromRef'], aJob['flagUseBkgScaling'], int(aJob['ignoreBinMultLower']), int(aJob['ignoreBinMultUpper']), int(aJob['ignoreBinMultForKtLower']), int(aJob['ignoreBinMultForKtUpper']), int(aJob['ignoreBinKtLower']), int(aJob['ignoreBinKtUpper'])
        ))


def runTrendsScaledBkg(outputFolderName="trends_scaledBkg"):

    # prepare output directory
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print("Running trends in directory:\n{0}\n".format(outputDirPath))

    # define jobs
    jobsToRun = [
        # DATA LIKE
        {
            'fileName': os.getenv('MYDIR') + "/output/fits_scaledBkg/RD_pPb/fitResults.root",
            'hBaseName': "h4010",
            'fName': "funcMain",
            'flagIsUnlike': "false",
            'outputFolder': "RD_pPb",
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400"
        },
        {
            'fileName': os.getenv('MYDIR') + "/output/fits_scaledBkg/RD_Pbp/fitResults.root",
            'hBaseName': "h4010",
            'fName': "funcMain",
            'flagIsUnlike': "false",
            'outputFolder': "RD_Pbp",
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400"
        }
    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        mkdirIfNotExists(aJob['outputFolder'])
        os.chdir(aJob['outputFolder'])

        os.system('root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawTrends.C("{}","{}","{}",{},"{}","{}","{}")\' >> trends.log'.format(
            aJob['fileName'], aJob['hBaseName'], aJob['fName'], aJob['flagIsUnlike'], aJob[
                'outputFolder'], aJob['hNameCommonEndForMult'], aJob['hNameCommonEndForKt']
        ))


def runDiffsScaledBkg(referencePath=os.getenv('BEC_BASE_ANALYSIS') + "/main/output/fits_scaledBkg", outputFolderName="diffs_scaledBkg"):

    # prepare output directory
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print("Running trends in directory:\n{0}\n".format(outputDirPath))

    # define jobs
    jobsToRun = [
        # DATA LIKE
        {
            'fileNameMain': os.getenv('MYDIR') + "/output/fits_scaledBkg/RD_pPb/fitResults.root",
            'hBaseNameMain': "h4010",
            'fNameMain': "funcMain",
            'fileNameRef': referencePath + "/RD_pPb/fitResults.root",
            'hBaseNameRef': "h4010",
            'fNameRef': "funcMain",
            'flagIsUnlike': "false",
            'outputFolder': "RD_pPb",
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400"
        },
        {
            'fileNameMain': os.getenv('MYDIR') + "/output/fits_scaledBkg/RD_Pbp/fitResults.root",
            'hBaseNameMain': "h4010",
            'fNameMain': "funcMain",
            'fileNameRef': referencePath + "/RD_Pbp/fitResults.root",
            'hBaseNameRef': "h4010",
            'fNameRef': "funcMain",
            'flagIsUnlike': "false",
            'outputFolder': "RD_Pbp",
            'hNameCommonEndForMult': "_400",
            'hNameCommonEndForKt': "_400"
        }
    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        mkdirIfNotExists(aJob['outputFolder'])
        os.chdir(aJob['outputFolder'])

        os.system('root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawDiffs.C("{}","{}","{}","{}","{}","{}",{},"{}","{}","{}")\' >> diffs.log'.format(
            aJob['fileNameMain'], aJob['hBaseNameMain'], aJob['fNameMain'], aJob['fileNameRef'], aJob['hBaseNameRef'], aJob[
                'fNameRef'], aJob['flagIsUnlike'], aJob['outputFolder'], aJob['hNameCommonEndForMult'], aJob['hNameCommonEndForKt']
        ))


def runSystematics(outputFolderName="systematics"):

    # prepare output directory
    outputDirPath = prepareFolder(
        os.getenv('MYDIR') + '/output', outputFolderName)

    print(f'Running {outputFolderName} in directory:\n{outputDirPath}\n')

    # define jobs
    jobsToRun = [
        # DATA LIKE
        {
            'fileName': "output/fits_scaledBkg/RD_pPb/fitResults.root",
            'hBaseName': "h4010",
            'hNameCommonEndForMult': "_400",
            'fName': "funcMain",
            'outputFolder': "RD_pPb"
        },
        {
            'fileName': "output/fits_scaledBkg/RD_Pbp/fitResults.root",
            'hBaseName': "h4010",
            'hNameCommonEndForMult': "_400",
            'fName': "funcMain",
            'outputFolder': "RD_Pbp"
        }
    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        recreateAndChangeDir(aJob['outputFolder'])

        os.system('lb-conda default python ' +
                  f'{os.getenv("BEC_BASE_CODE_SCRIPTS")}/results/systematics.py "{aJob["fileName"]}" "{aJob["hBaseName"]}" "{aJob["hNameCommonEndForMult"]}" "{aJob["fName"]}" >> {outputFolderName}.log')


def runDiscussion(outputFolderName="discussion"):

    # prepare output directory
    outputDirPath = os.getenv('MYDIR') + '/output'

    print(f'Running {outputFolderName} in directory:\n{outputDirPath}\n')

    # define jobs
    jobsToRun = [
        # DATA LIKE
        {
            'basePath': os.getenv("BEC_BASE_ANALYSIS"),
        }
    ]

    # run job
    for aJob in jobsToRun:
        os.chdir(outputDirPath)
        recreateAndChangeDir(outputFolderName)

        os.system('root -l -b -q \'' +
                  f'{os.getenv("BEC_BASE_CODE_SCRIPTS")}/results/discussion.C("{aJob["basePath"]}")\' >> {outputFolderName}.log')

####################################################
# poor make init


def runTrendsScaledBkgDownstream():
    runTrendsScaledBkg()
    runDiffsScaledBkg()


def runFitsScaledBkgDownstream():
    runFitsScaledBkg()
    runTrendsScaledBkgDownstream()


def runTrendsDownstream():
    runTrends()
    runDiffs()
    runFitsScaledBkgDownstream()


def runFitsDownstream():
    runFits()
    runTrendsDownstream()


def runDivideDownstream():
    runDivide()
    runFitsDownstream()


def runMergeDownstream():
    runMerge()
    runDivideDownstream()


def runSystematicsDownstream():
    runSystematics()
    runDiscussion()


def runAll():
    runMergeDownstream()
    runHistograms()

####################################################

# setup local job environment


if not setEnvironmentBEC():
    print("BEC environment not set -> exiting.\n")
    exit

os.environ['MYDIR'] = os.getenv('PWD')
mkdirIfNotExists(os.getenv('MYDIR') + "/output")
print("Setup local BEC job environment in directory:\n{0}\n".format(
    os.getenv('MYDIR')))
