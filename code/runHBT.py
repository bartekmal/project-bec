#!/usr/bin/env python

import os

#define a structure for BEC jobs
class jobBEC:
    def __init__(self, jobName, outputPath, inputData, dataType, outputFileName = 'HBTAnalysis.root', nrOfEventsToProcess = ''):
        self.jobName = jobName
        self.outputPath = outputPath
        self.inputData = inputData
        self.dataType = dataType
        self.outputFileName = outputFileName
        self.nrOfEventsToProcess = str(nrOfEventsToProcess)

#helper functions

def mkdirIfNotExists( myPath ):
    if not os.path.exists( myPath ):
        os.mkdir( myPath )

def recreateDir( folderName ):
    os.system( 'rm -r ' + folderName )
    os.mkdir( folderName )

def recreateAndChangeDir( folderName ):
    os.system( 'rm -r ' + folderName )
    os.mkdir( folderName )
    os.chdir( folderName )

def prepareFolder( outputFolderPath,outputFolderName ):
    tmpFolder = outputFolderPath + "/" + outputFolderName
    mkdirIfNotExists( tmpFolder )

    return tmpFolder

#set the BEC environment
def setEnvironmentBEC():
    currentHost = os.getenv( 'BEC_HOST' )

    if currentHost == "lxplus":
        #job flavours in condor
        #espresso     = 20 mins
        #microcentury = 1 hour
        #longlunch    = 2 hours
        #workday      = 8 hours
        #tomorrow     = 1 day
        #testmatch    = 3 days
        #nextweek     = 1 week
        os.environ['BEC_SETUP_SCRIPT'] = os.environ['BEC_BASE']+'/setupBEC.sh'
        os.environ['BEC_BATCH_QUEUE'] = 'tomorrow'
        os.environ['BEC_BATCH_MAXTIME'] = '50400' # in secs
        os.environ['BEC_BATCH_TMPDIR'] = 'TMPDIR'
        os.environ['BEC_BATCH_SYSTEM_COMMAND'] = 'condor_submit'
        print "BEC environment set for user: bmalecki (lxplus)\n"
        return True
    elif currentHost == "plgrid":
        os.environ['BEC_SETUP_SCRIPT'] = os.environ['BEC_BASE']+'/setupBEC.sh'
        os.environ['BEC_BATCH_QUEUE'] = 'plgrid'
        os.environ['BEC_BATCH_MAXTIME'] = '24:00:00'
        os.environ['BEC_BATCH_TMPDIR'] = 'SLURM_MYDIR'
        os.environ['BEC_BATCH_SYSTEM_COMMAND'] = 'sbatch -p ' + os.getenv('BEC_BATCH_QUEUE') + ' -t ' + os.getenv('BEC_BATCH_MAXTIME')
        print "BEC environment set for user: plgbmalecki (PLGrid)\n"
        return True
    else:
        return False   

def createRunScriptForJob( aJob, outputDir ):

    singleJobOnBatchFileName = 'start_job.sh'
    batchSubmitFileName = 'condor.sub'

    myFile = open(singleJobOnBatchFileName,'w')

    myFile.write('#!/bin/sh\n')
    myFile.write('\n')
    myFile.write('#setup environment\n')
    myFile.write('source /cvmfs/lhcb.cern.ch/lib/LbEnv.sh -c ' + os.getenv("MY_CMTCONFIG") + '\n')
    myFile.write('source ' + os.getenv("MY_ROOT_SETUP") + '\n')
    myFile.write('\n')
    myFile.write('cd ${'+ os.getenv("BEC_BATCH_TMPDIR") +'}\n')
    myFile.write( os.getenv('CODE_EXEC') + ' ' + aJob.inputData + ' ' + aJob.dataType + ' ' + aJob.outputFileName + ' ' + aJob.nrOfEventsToProcess + ' > HBTAnalysis.log' + '\n' )
    myFile.write('\n')
    myFile.write('#get output\n')
    myFile.write('sleep ' + os.getenv('SLEEP_TIME') + '\n')
    myFile.write('mv *.log ' + outputDir + '/HBTAnalysis.log\n')
    myFile.write('mv *.root ' + outputDir+ '/HBTAnalysis.root\n')

    myFile.close()

    submitFile = open(batchSubmitFileName,'w')

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

    return os.path.realpath( submitFile.name )

#################################################### analysis `rules`

def runHBTJobs( outputFolderName = "HBTAnalysis" ):

    #prepare empty job directories
    os.environ['TEMP_DIR'] = os.getenv('MYDIR') + '/temp'
    recreateDir(os.getenv('TEMP_DIR'))
    recreateDir(os.getenv('MYDIR') + '/output')
    outputDirPath = prepareFolder( os.getenv('MYDIR') + '/output', outputFolderName )
    
    print "Running HBT jobs with output in directory:\n{0}\n".format( outputDirPath )

    #define jobs
    jobsToRun = [
        jobBEC( jobName = 'RD_pPb_MU', outputPath = 'RD_pPb_MU', inputData = os.getenv('BEC_STORAGE') + '/ntuple/data/pPb_MU/hbtNTuple.root', dataType = 'RD' ),
        jobBEC( jobName = 'RD_pPb_MD', outputPath = 'RD_pPb_MD', inputData = os.getenv('BEC_STORAGE') + '/ntuple/data/pPb_MD/hbtNTuple.root', dataType = 'RD' ),
        jobBEC( jobName = 'RD_Pbp_MU', outputPath = 'RD_Pbp_MU', inputData = os.getenv('BEC_STORAGE') + '/ntuple/data/Pbp_MU/hbtNTuple.root', dataType = 'RD' ),
        jobBEC( jobName = 'RD_Pbp_MD', outputPath = 'RD_Pbp_MD', inputData = os.getenv('BEC_STORAGE') + '/ntuple/data/Pbp_MD/hbtNTuple.root', dataType = 'RD' ),
        jobBEC( jobName = 'MC_pPb_MU', outputPath = 'MC_pPb_MU', inputData = os.getenv('BEC_STORAGE') + '/ntuple/sim/epos/central/Sim09c/pPb_MU/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'MC_pPb_MD', outputPath = 'MC_pPb_MD', inputData = os.getenv('BEC_STORAGE') + '/ntuple/sim/epos/central/Sim09c/pPb_MD/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'MC_Pbp_MU', outputPath = 'MC_Pbp_MU', inputData = os.getenv('BEC_STORAGE') + '/ntuple/sim/epos/central/Sim09c/Pbp_MU/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'MC_Pbp_MD', outputPath = 'MC_Pbp_MD', inputData = os.getenv('BEC_STORAGE') + '/ntuple/sim/epos/central/Sim09c/Pbp_MD/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'EPOS_local_pPb_MU', outputPath = 'EPOS_local_pPb_MU', inputData = os.getenv('BEC_STORAGE') + '/ntuple/sim/epos/local/Sim09c/pPb_MU/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'EPOS_local_pPb_MD', outputPath = 'EPOS_local_pPb_MD', inputData = os.getenv('BEC_STORAGE') + '/ntuple/sim/epos/local/Sim09c/pPb_MD/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'EPOS_local_Pbp_MU', outputPath = 'EPOS_local_Pbp_MU', inputData = os.getenv('BEC_STORAGE') + '/ntuple/sim/epos/local/Sim09c/Pbp_MU/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'EPOS_local_Pbp_MD', outputPath = 'EPOS_local_Pbp_MD', inputData = os.getenv('BEC_STORAGE') + '/ntuple/sim/epos/local/Sim09c/Pbp_MD/hbtNTuple_corrected.root', dataType = 'MC' ),   
    ]        

    # prepare temporary job scripts/files
    os.chdir(os.getenv('TEMP_DIR'))

    #compile
    os.environ['CODE_NAME'] = 'HBTAnalysis'
    os.environ['CODE_INPUT'] = os.getenv('BEC_BASE_CODE_HBT')
    # os.environ['CODE_INPUT'] = os.getenv('MYDIR') + "/HBTAnalysis"
    os.environ['CODE_EXEC'] = os.getenv('TEMP_DIR')+'/'+os.getenv('CODE_NAME')+'.exe'
    os.system(
        'COMPILER=`root-config --cxx`;' +
        'FLAGS=`root-config --cflags --libs`;' +
        '$COMPILER -g -O3 -Wall -Wextra -Wpedantic ${CODE_INPUT}/*.cpp -o ${CODE_EXEC} $FLAGS'
    )
    
    #run job
    for aJob in jobsToRun:

        #prepare folder for output files
        os.chdir(outputDirPath)
        recreateDir(aJob.outputPath)
        outputDirForCurrentJob = outputDirPath + "/" + aJob.outputPath

        #temporary job files
        os.chdir( os.getenv('TEMP_DIR') )
        recreateAndChangeDir( aJob.outputPath )
        runFilePath = createRunScriptForJob( aJob, outputDirForCurrentJob )
               
        os.system( os.getenv('BEC_BATCH_SYSTEM_COMMAND') + " " + runFilePath )
        #os.system( '${CODE_EXEC} ' + aJob.inputData + ' ' + aJob.dataType + ' ' + aJob.outputFileName + ' ' + aJob.nrOfEventsToProcess + ' > HBTAnalysis.log')

def runMerge( outputFolderName = "merged" ):

    #prepare output directory
    outputDirPath = prepareFolder( os.getenv('MYDIR') + '/output', outputFolderName )
    
    print "Running merge in directory:\n{0}\n".format( outputDirPath )

    #define jobs
    jobsToRun = [
        { 
            'outputFolder' : "RD_pPb",
            'filesToMerge' : [ 
                os.getenv('MYDIR') + "/output/HBTAnalysis/RD_pPb_MU/HBTAnalysis.root", 
                os.getenv('MYDIR') + "/output/HBTAnalysis/RD_pPb_MD/HBTAnalysis.root"
            ]
        },
        { 
            'outputFolder' : "RD_Pbp",
            'filesToMerge' : [ 
                os.getenv('MYDIR') + "/output/HBTAnalysis/RD_Pbp_MU/HBTAnalysis.root", 
                os.getenv('MYDIR') + "/output/HBTAnalysis/RD_Pbp_MD/HBTAnalysis.root",
            ]
        },
        { 
            'outputFolder' : "MC_pPb",
            'filesToMerge' : [ 
                os.getenv('MYDIR') + "/output/HBTAnalysis/MC_pPb_MU/HBTAnalysis.root", 
                os.getenv('MYDIR') + "/output/HBTAnalysis/MC_pPb_MD/HBTAnalysis.root",
                os.getenv('MYDIR') + "/output/HBTAnalysis/EPOS_local_pPb_MU/HBTAnalysis.root", 
                os.getenv('MYDIR') + "/output/HBTAnalysis/EPOS_local_pPb_MD/HBTAnalysis.root"
            ]
        },
        { 
            'outputFolder' : "MC_Pbp",
            'filesToMerge' : [ 
                os.getenv('MYDIR') + "/output/HBTAnalysis/MC_Pbp_MU/HBTAnalysis.root", 
                os.getenv('MYDIR') + "/output/HBTAnalysis/MC_Pbp_MD/HBTAnalysis.root",
                os.getenv('MYDIR') + "/output/HBTAnalysis/EPOS_local_Pbp_MU/HBTAnalysis.root", 
                os.getenv('MYDIR') + "/output/HBTAnalysis/EPOS_local_Pbp_MD/HBTAnalysis.root"
            ]
        }
    ]        

    #run job
    for aJob in jobsToRun:
        os.chdir( outputDirPath ) 
        recreateAndChangeDir( aJob['outputFolder'] )
                
        os.system( 'hadd -ff merged.root' + ' ' + ' '.join( aJob['filesToMerge'] ) )

def runDivide( outputFolderName = "correlations" ):

    #prepare output directory
    outputDirPath = prepareFolder( os.getenv('MYDIR') + '/output', outputFolderName )
    
    print "Running divide in directory:\n{0}\n".format( outputDirPath )

    #define jobs
    jobsToRun = [
        { 
            'file1Path' : os.getenv('MYDIR') + "/output/merged/RD_pPb/merged.root",
            'h1NameLike' : "h3010",
            'h1NameUnlike' : "h3210",
            'file2Path' : os.getenv('MYDIR') + "/output/merged/RD_pPb/merged.root",
            'h2NameLike' : "h3510",
            'h2NameUnlike' : "h3610",
            'outputFolder' : "RD_pPb",
            'hOutNameLike' : "h4010",
            'hOutNameUnlike' : "h4210",
            'isDR' : False,
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200",
            'flagCorrectCoulomb' : False
        },
        { 
            'file1Path' : os.getenv('MYDIR') + "/output/merged/RD_Pbp/merged.root",
            'h1NameLike' : "h3010",
            'h1NameUnlike' : "h3210",
            'file2Path' : os.getenv('MYDIR') + "/output/merged/RD_Pbp/merged.root",
            'h2NameLike' : "h3510",
            'h2NameUnlike' : "h3610",
            'outputFolder' : "RD_Pbp",
            'hOutNameLike' : "h4010",
            'hOutNameUnlike' : "h4210",
            'isDR' : False,
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200",
            'flagCorrectCoulomb' : False
        },
        { 
            'file1Path' : os.getenv('MYDIR') + "/output/merged/MC_pPb/merged.root",
            'h1NameLike' : "h3010",
            'h1NameUnlike' : "h3210",
            'file2Path' : os.getenv('MYDIR') + "/output/merged/MC_pPb/merged.root",
            'h2NameLike' : "h3510",
            'h2NameUnlike' : "h3610",
            'outputFolder' : "MC_pPb",
            'hOutNameLike' : "h4010",
            'hOutNameUnlike' : "h4210",
            'isDR' : False,
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200",
            'flagCorrectCoulomb' : False
        },
        { 
            'file1Path' : os.getenv('MYDIR') + "/output/merged/MC_Pbp/merged.root",
            'h1NameLike' : "h3010",
            'h1NameUnlike' : "h3210",
            'file2Path' : os.getenv('MYDIR') + "/output/merged/MC_Pbp/merged.root",
            'h2NameLike' : "h3510",
            'h2NameUnlike' : "h3610",
            'outputFolder' : "MC_Pbp",
            'hOutNameLike' : "h4010",
            'hOutNameUnlike' : "h4210",
            'isDR' : False,
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200",
            'flagCorrectCoulomb' : False
        },
        { 
            'file1Path' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'h1NameLike' : "h4010",
            'h1NameUnlike' : "h4210",
            'file2Path' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
            'h2NameLike' : "h4010",
            'h2NameUnlike' : "h4210",
            'outputFolder' : "DR_pPb",
            'hOutNameLike' : "h5010",
            'hOutNameUnlike' : "h5210",
            'isDR' : True,
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200",
            'flagCorrectCoulomb' : False
        },
        { 
            'file1Path' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'h1NameLike' : "h4010",
            'h1NameUnlike' : "h4210",
            'file2Path' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
            'h2NameLike' : "h4010",
            'h2NameUnlike' : "h4210",
            'outputFolder' : "DR_Pbp",
            'hOutNameLike' : "h5010",
            'hOutNameUnlike' : "h5210",
            'isDR' : True,
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200",
            'flagCorrectCoulomb' : False
        },
        
    ]        

    #run job
    for aJob in jobsToRun:
        os.chdir( outputDirPath ) 
        recreateAndChangeDir( aJob['outputFolder'] )
        outputFile = os.getenv('MYDIR') + "/output/correlations/" + aJob['outputFolder'] + "/correlations.root"     

        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}",{10},{11})\''.format(
            aJob['file1Path'], aJob['h1NameLike'], aJob['file2Path'], aJob['h2NameLike'], outputFile, aJob['hOutNameLike'], int( aJob['isDR'] ), int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd'], int( aJob['flagCorrectCoulomb'] ), int( True )  
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}",{10},{11})\''.format(
            aJob['file1Path'], aJob['h1NameLike'], aJob['file2Path'], aJob['h2NameLike'], outputFile, aJob['hOutNameLike'], int( aJob['isDR'] ), int( aJob['nrBinsMultForKt'] ),  int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd'], int( aJob['flagCorrectCoulomb'] ), int( True )  
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}",{10},{11})\''.format(
            aJob['file1Path'], aJob['h1NameUnlike'], aJob['file2Path'], aJob['h2NameUnlike'], outputFile, aJob['hOutNameUnlike'], int( aJob['isDR'] ), int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd'], int( aJob['flagCorrectCoulomb'] ), int( False )  
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}",{10},{11})\''.format(
            aJob['file1Path'], aJob['h1NameUnlike'], aJob['file2Path'], aJob['h2NameUnlike'], outputFile, aJob['hOutNameUnlike'], int( aJob['isDR'] ), int( aJob['nrBinsMultForKt'] ),  int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd'], int( aJob['flagCorrectCoulomb'] ), int( False )    
        ) )

def runFits( outputFolderName = "fits" ):

    #prepare output directory
    outputDirPath = prepareFolder( os.getenv('MYDIR') + '/output', outputFolderName )
    
    print "Running plots in directory:\n{0}\n".format( outputDirPath )

    #define jobs
    jobsToRun = [
        # MC UNLIKE
        # { 
        #     'inputFile' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
        #     'hMainNameBase' : "h4210",
        #     'isMC' : "true",
        #     'isUnlike' : "true",
        #     'outputFolder' : "MC_pPb",
        #     'nrBinsMultOnly' : 11,
        #     'nrBinsMultForKt' : 7,
        #     'nrBinsKt' : 3,
        #     'hCommonEndName' : "_200",
        #     'flagDoFit' : "true",
        #     'inputFileRef' : "",
        #     'hRefNameBase' : "",
        #     'refType' : "",
        #     'flagDrawRef' : "false",
        #     'flagUseBkgFromRef' : "false",
        #     'flagIsBkgScaling' : "false"        
        # },
        # { 
        #     'inputFile' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
        #     'hMainNameBase' : "h4210",
        #     'isMC' : "true",
        #     'isUnlike' : "true",
        #     'outputFolder' : "MC_Pbp",
        #     'nrBinsMultOnly' : 11,
        #     'nrBinsMultForKt' : 7,
        #     'nrBinsKt' : 3,
        #     'hCommonEndName' : "_200",
        #     'flagDoFit' : "true",
        #     'inputFileRef' : "",
        #     'hRefNameBase' : "",
        #     'refType' : "",
        #     'flagDrawRef' : "false",
        #     'flagUseBkgFromRef' : "false",
        #     'flagIsBkgScaling' : "false"        
        # },

        # MC LIKE
        # { 
        #     'inputFile' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
        #     'hMainNameBase' : "h4010",
        #     'isMC' : "true",
        #     'isUnlike' : "false",
        #     'outputFolder' : "MC_pPb",
        #     'nrBinsMultOnly' : 11,
        #     'nrBinsMultForKt' : 7,
        #     'nrBinsKt' : 3,
        #     'hCommonEndName' : "_200",
        #     'flagDoFit' : "true",
        #     'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
        #     'hRefNameBase' : "h4210",
        #     'refType' : "MC_pPb - UNLIKE",
        #     'flagDrawRef' : "true",
        #     'flagUseBkgFromRef' : "false",
        #     'flagIsBkgScaling' : "false"        
        # },
        # { 
        #     'inputFile' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
        #     'hMainNameBase' : "h4010",
        #     'isMC' : "true",
        #     'isUnlike' : "false",
        #     'outputFolder' : "MC_Pbp",
        #     'nrBinsMultOnly' : 11,
        #     'nrBinsMultForKt' : 7,
        #     'nrBinsKt' : 3,
        #     'hCommonEndName' : "_200",
        #     'flagDoFit' : "true",
        #     'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
        #     'hRefNameBase' : "h4210",
        #     'refType' : "MC_Pbp - UNLIKE",
        #     'flagDrawRef' : "true",
        #     'flagUseBkgFromRef' : "false",
        #     'flagIsBkgScaling' : "false"        
        # },

        # DATA UNLIKE
        { 
            'inputFile' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hMainNameBase' : "h4210",
            'isMC' : "false",
            'isUnlike' : "true",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hCommonEndName' : "_200",
            'flagDoFit' : "true",
            'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
            'hRefNameBase' : "h4210",
            'refType' : "MC_pPb - UNLIKE",
            'flagDrawRef' : "false",
            'flagUseBkgFromRef' : "false",
            'flagIsBkgScaling' : "false",
            'flagUseBkgScaling' : "false",
            'fBkgScalingNameMain' : "",
            'fBkgScalingNameRef' : ""
        },
        { 
            'inputFile' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hMainNameBase' : "h4210",
            'isMC' : "false",
            'isUnlike' : "true",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hCommonEndName' : "_200",
            'flagDoFit' : "true",
            'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
            'hRefNameBase' : "h4210",
            'refType' : "MC_Pbp - UNLIKE",
            'flagDrawRef' : "false",
            'flagUseBkgFromRef' : "false",
            'flagIsBkgScaling' : "false",
            'flagUseBkgScaling' : "false",
            'fBkgScalingNameMain' : "",
            'fBkgScalingNameRef' : ""
        },

        # bkg scaling: DATA UNLIKE
        { 
            'inputFile' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hMainNameBase' : "h4210",
            'isMC' : "false",
            'isUnlike' : "true",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hCommonEndName' : "_200",
            'flagDoFit' : "true",
            'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hRefNameBase' : "h4210",
            'refType' : "MC_pPb - UNLIKE",
            'flagDrawRef' : "false",
            'flagUseBkgFromRef' : "true",
            'flagIsBkgScaling' : "true",
            'flagUseBkgScaling' : "false",
            'fBkgScalingNameMain' : "",
            'fBkgScalingNameRef' : ""
        },
        { 
            'inputFile' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hMainNameBase' : "h4210",
            'isMC' : "false",
            'isUnlike' : "true",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hCommonEndName' : "_200",
            'flagDoFit' : "true",
            'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hRefNameBase' : "h4210",
            'refType' : "MC_Pbp - UNLIKE",
            'flagDrawRef' : "false",
            'flagUseBkgFromRef' : "true",
            'flagIsBkgScaling' : "true",
            'flagUseBkgScaling' : "false",
            'fBkgScalingNameMain' : "",
            'fBkgScalingNameRef' : ""
        },

        # bkg scaling: DATA LIKE
        { 
            'inputFile' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hMainNameBase' : "h4010",
            'isMC' : "false",
            'isUnlike' : "false",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hCommonEndName' : "_200",
            'flagDoFit' : "true",
            'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hRefNameBase' : "h4210",
            'refType' : "RD_pPb - UNLIKE",
            'flagDrawRef' : "true",
            'flagUseBkgFromRef' : "true",
            'flagIsBkgScaling' : "true",
            'flagUseBkgScaling' : "false",
            'fBkgScalingNameMain' : "",
            'fBkgScalingNameRef' : ""
        },
        { 
            'inputFile' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hMainNameBase' : "h4010",
            'isMC' : "false",
            'isUnlike' : "false",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hCommonEndName' : "_200",
            'flagDoFit' : "true",
            'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hRefNameBase' : "h4210",
            'refType' : "RD_Pbp - UNLIKE",
            'flagDrawRef' : "true",
            'flagUseBkgFromRef' : "true",
            'flagIsBkgScaling' : "true",
            'flagUseBkgScaling' : "false",
            'fBkgScalingNameMain' : "",
            'fBkgScalingNameRef' : ""
        },
        
        # DATA LIKE
        { 
            'inputFile' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hMainNameBase' : "h4010",
            'isMC' : "false",
            'isUnlike' : "false",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hCommonEndName' : "_200",
            'flagDoFit' : "true",
            'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hRefNameBase' : "h4210",
            'refType' : "RD_pPb - UNLIKE",
            'flagDrawRef' : "true",
            'flagUseBkgFromRef' : "true",
            'flagIsBkgScaling' : "false",
            'flagUseBkgScaling' : "true",
            'fBkgScalingNameMain' : "funcBkgScaling",
            'fBkgScalingNameRef' : "funcMain" 
        },
        { 
            'inputFile' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hMainNameBase' : "h4010",
            'isMC' : "false",
            'isUnlike' : "false",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hCommonEndName' : "_200",
            'flagDoFit' : "true",
            'inputFileRef' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hRefNameBase' : "h4210",
            'refType' : "RD_Pbp - UNLIKE",
            'flagDrawRef' : "true",
            'flagUseBkgFromRef' : "true",
            'flagIsBkgScaling' : "false",
            'flagUseBkgScaling' : "true",
            'fBkgScalingNameMain' : "funcBkgScaling",
            'fBkgScalingNameRef' : "funcMain" 
        }
    ]        

    #run job
    for aJob in jobsToRun:
        os.chdir( outputDirPath )
        mkdirIfNotExists( aJob['outputFolder'] )
        os.chdir( aJob['outputFolder'] )    

        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/fitInBins.C("{}","{}",{},{},"{}",{},{},"{}",{},"{}","{}","{}",{},{},{},{},"{}","{}")\''.format(
            aJob['inputFile'], aJob['hMainNameBase'], aJob['isMC'], aJob['isUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultOnly'] ), 0, aJob['hCommonEndName'], aJob['flagDoFit'], aJob['inputFileRef'], aJob['hRefNameBase'], aJob['refType'], aJob['flagDrawRef'], aJob['flagUseBkgFromRef'], aJob['flagIsBkgScaling'], aJob['flagUseBkgScaling'], aJob['fBkgScalingNameMain'], aJob['fBkgScalingNameRef']    
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/fitInBins.C("{}","{}",{},{},"{}",{},{},"{}",{},"{}","{}","{}",{},{},{},{},"{}","{}")\''.format(
            aJob['inputFile'], aJob['hMainNameBase'], aJob['isMC'], aJob['isUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultForKt'] ), int( aJob['nrBinsKt'] ), aJob['hCommonEndName'], aJob['flagDoFit'], aJob['inputFileRef'], aJob['hRefNameBase'], aJob['refType'], aJob['flagDrawRef'], aJob['flagUseBkgFromRef'], aJob['flagIsBkgScaling'], aJob['flagUseBkgScaling'], aJob['fBkgScalingNameMain'], aJob['fBkgScalingNameRef']
        ) )

def runTrends( outputFolderName = "trends" ):

    #prepare output directory
    outputDirPath = prepareFolder( os.getenv('MYDIR') + '/output', outputFolderName )
    
    print "Running trends in directory:\n{0}\n".format( outputDirPath )

    #define jobs
    jobsToRun = [
        # MC UNLIKE
        # { 
        #     'fileName' : os.getenv('MYDIR') + "/output/fits/MC_pPb/fitResults.root",
        #     'hBaseName' : "h4210",
        #     'flagIsUnlike' : "true",
        #     'outputFolder' : "MC_pPb",
        #     'nrBinsMultOnly' : 11,
        #     'nrBinsMultForKt' : 7,
        #     'nrBinsKt' : 3,
        #     'hNameCommonEnd' : "_200"
        # },
        # { 
        #     'fileName' : os.getenv('MYDIR') + "/output/fits/MC_Pbp/fitResults.root",
        #     'hBaseName' : "h4210",
        #     'flagIsUnlike' : "true",
        #     'outputFolder' : "MC_Pbp",
        #     'nrBinsMultOnly' : 11,
        #     'nrBinsMultForKt' : 7,
        #     'nrBinsKt' : 3,
        #     'hNameCommonEnd' : "_200"
        # },

        # MC LIKE
        # { 
        #     'fileName' : os.getenv('MYDIR') + "/output/fits/MC_pPb/fitResults.root",
        #     'hBaseName' : "h4010",
        #     'flagIsUnlike' : "false",
        #     'outputFolder' : "MC_pPb",
        #     'nrBinsMultOnly' : 11,
        #     'nrBinsMultForKt' : 7,
        #     'nrBinsKt' : 3,
        #     'hNameCommonEnd' : "_200"
        # },
        # { 
        #     'fileName' : os.getenv('MYDIR') + "/output/fits/MC_Pbp/fitResults.root",
        #     'hBaseName' : "h4010",
        #     'flagIsUnlike' : "false",
        #     'outputFolder' : "MC_Pbp",
        #     'nrBinsMultOnly' : 11,
        #     'nrBinsMultForKt' : 7,
        #     'nrBinsKt' : 3,
        #     'hNameCommonEnd' : "_200"
        # },

        # DATA UNLIKE
        { 
            'fileName' : os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseName' : "h4210",
            'fName' : "funcMain",          
            'flagIsUnlike' : "true",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        { 
            'fileName' : os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseName' : "h4210",
            'fName' : "funcMain",          
            'flagIsUnlike' : "true",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },

        # bkg scaling: DATA UNLIKE
        { 
            'fileName' : os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseName' : "h4210",
            'fName' : "funcBkgScaling",          
            'flagIsUnlike' : "true",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        { 
            'fileName' : os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseName' : "h4210",
            'fName' : "funcBkgScaling",          
            'flagIsUnlike' : "true",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },  

        # bkg scaling: DATA LIKE
        { 
            'fileName' : os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseName' : "h4010",
            'fName' : "funcBkgScaling",          
            'flagIsUnlike' : "false",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        { 
            'fileName' : os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseName' : "h4010",
            'fName' : "funcBkgScaling",          
            'flagIsUnlike' : "false",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        
        # DATA LIKE
        { 
            'fileName' : os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseName' : "h4010",
            'fName' : "funcMain",          
            'flagIsUnlike' : "false",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        { 
            'fileName' : os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseName' : "h4010",
            'fName' : "funcMain",          
            'flagIsUnlike' : "false",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        }  
    ]        

    #run job
    for aJob in jobsToRun:
        os.chdir( outputDirPath )
        mkdirIfNotExists( aJob['outputFolder'] )
        os.chdir( aJob['outputFolder'] )    
        
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawTrends.C("{}","{}","{}",{},"{}",{},{},"{}")\''.format(
            aJob['fileName'], aJob['hBaseName'], aJob['fName'], aJob['flagIsUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd']  
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawTrends.C("{}","{}","{}",{},"{}",{},{},"{}")\''.format(
            aJob['fileName'], aJob['hBaseName'], aJob['fName'], aJob['flagIsUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultForKt'] ), int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd']  
        ) )

def runDiffs( referencePath = os.getenv('BEC_BASE_ANALYSIS') + "/evmix/0_reference/output/fits", outputFolderName = "diffs" ):

    #prepare output directory
    outputDirPath = prepareFolder( os.getenv('MYDIR') + '/output', outputFolderName )
    
    print "Running trends in directory:\n{0}\n".format( outputDirPath )

    #define jobs
    jobsToRun = [
        # DATA UNLIKE
        { 
            'fileNameMain' : os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseNameMain' : "h4210",
            'fNameMain' : "funcMain",
            'fileNameRef' : referencePath + "/RD_pPb/fitResults.root",
            'hBaseNameRef' : "h4210",
            'fNameRef' : "funcMain",                    
            'flagIsUnlike' : "true",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        { 
            'fileNameMain' : os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseNameMain' : "h4210",
            'fNameMain' : "funcMain",  
            'fileNameRef' : referencePath + "/RD_Pbp/fitResults.root",
            'hBaseNameRef' : "h4210",
            'fNameRef' : "funcMain",             
            'flagIsUnlike' : "true",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },

        # bkg scaling: DATA UNLIKE
        { 
            'fileNameMain' : os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseNameMain' : "h4210",
            'fNameMain' : "funcBkgScaling", 
            'fileNameRef' : referencePath + "/RD_pPb/fitResults.root",
            'hBaseNameRef' : "h4210",
            'fNameRef' : "funcBkgScaling",                             
            'flagIsUnlike' : "true",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        { 
            'fileNameMain' : os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseNameMain' : "h4210",
            'fNameMain' : "funcBkgScaling",
            'fileNameRef' : referencePath + "/RD_Pbp/fitResults.root",
            'hBaseNameRef' : "h4210",
            'fNameRef' : "funcBkgScaling",                                     
            'flagIsUnlike' : "true",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },  

        # bkg scaling: DATA LIKE
        { 
            'fileNameMain' : os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseNameMain' : "h4010",
            'fNameMain' : "funcBkgScaling",
            'fileNameRef' : referencePath + "/RD_pPb/fitResults.root",
            'hBaseNameRef' : "h4010",
            'fNameRef' : "funcBkgScaling",                                                   
            'flagIsUnlike' : "false",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        { 
            'fileNameMain' : os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseNameMain' : "h4010",
            'fNameMain' : "funcBkgScaling",  
            'fileNameRef' : referencePath + "/RD_Pbp/fitResults.root",
            'hBaseNameRef' : "h4010",
            'fNameRef' : "funcBkgScaling",                                                           
            'flagIsUnlike' : "false",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        
        # DATA LIKE
        { 
            'fileNameMain' : os.getenv('MYDIR') + "/output/fits/RD_pPb/fitResults.root",
            'hBaseNameMain' : "h4010",
            'fNameMain' : "funcMain",
            'fileNameRef' : referencePath + "/RD_pPb/fitResults.root",
            'hBaseNameRef' : "h4010",
            'fNameRef' : "funcMain",                                                                     
            'flagIsUnlike' : "false",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        },
        { 
            'fileNameMain' : os.getenv('MYDIR') + "/output/fits/RD_Pbp/fitResults.root",
            'hBaseNameMain' : "h4010",
            'fNameMain' : "funcMain",
            'fileNameRef' : referencePath + "/RD_Pbp/fitResults.root",
            'hBaseNameRef' : "h4010",
            'fNameRef' : "funcMain",                                                                            
            'flagIsUnlike' : "false",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 11,
            'nrBinsMultForKt' : 7,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_200"
        }  
    ]        

    #run job
    for aJob in jobsToRun:
        os.chdir( outputDirPath )
        mkdirIfNotExists( aJob['outputFolder'] )
        os.chdir( aJob['outputFolder'] )    
        
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawDiffs.C("{}","{}","{}","{}","{}","{}",{},"{}",{},{},"{}")\''.format(
            aJob['fileNameMain'], aJob['hBaseNameMain'], aJob['fNameMain'],aJob['fileNameRef'], aJob['hBaseNameRef'], aJob['fNameRef'], aJob['flagIsUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd']  
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawDiffs.C("{}","{}","{}","{}","{}","{}",{},"{}",{},{},"{}")\''.format(
            aJob['fileNameMain'], aJob['hBaseNameMain'], aJob['fNameMain'],aJob['fileNameRef'], aJob['hBaseNameRef'], aJob['fNameRef'], aJob['flagIsUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultForKt'] ), int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd']  
        ) )

####################################################

#setup local job environment

if not setEnvironmentBEC():
    print "BEC environment not set -> exiting.\n"
    exit

os.environ['MYDIR'] = os.getenv('PWD')
mkdirIfNotExists( os.getenv('MYDIR') + "/output" )
print "Setup local BEC job environment in directory:\n{0}\n".format( os.getenv('MYDIR') )
