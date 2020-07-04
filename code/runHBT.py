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
            'h1NameLike' : "h3012",
            'h1NameUnlike' : "h3212",
            'file2Path' : os.getenv('MYDIR') + "/output/merged/RD_pPb/merged.root",
            'h2NameLike' : "h3510",
            'h2NameUnlike' : "h3610",
            'outputFolder' : "RD_pPb",
            'hOutNameLike' : "h4012",
            'hOutNameUnlike' : "h4212",
            'isDR' : False,
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        },
        { 
            'file1Path' : os.getenv('MYDIR') + "/output/merged/RD_Pbp/merged.root",
            'h1NameLike' : "h3012",
            'h1NameUnlike' : "h3212",
            'file2Path' : os.getenv('MYDIR') + "/output/merged/RD_Pbp/merged.root",
            'h2NameLike' : "h3510",
            'h2NameUnlike' : "h3610",
            'outputFolder' : "RD_Pbp",
            'hOutNameLike' : "h4012",
            'hOutNameUnlike' : "h4212",
            'isDR' : False,
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
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
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
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
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        },
        { 
            'file1Path' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'h1NameLike' : "h4012",
            'h1NameUnlike' : "h4212",
            'file2Path' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
            'h2NameLike' : "h4010",
            'h2NameUnlike' : "h4210",
            'outputFolder' : "DR_pPb",
            'hOutNameLike' : "h5012",
            'hOutNameUnlike' : "h5212",
            'isDR' : True,
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        },
        { 
            'file1Path' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'h1NameLike' : "h4012",
            'h1NameUnlike' : "h4212",
            'file2Path' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
            'h2NameLike' : "h4010",
            'h2NameUnlike' : "h4210",
            'outputFolder' : "DR_Pbp",
            'hOutNameLike' : "h5012",
            'hOutNameUnlike' : "h5212",
            'isDR' : True,
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        },
        
    ]        

    #run job
    for aJob in jobsToRun:
        os.chdir( outputDirPath ) 
        recreateAndChangeDir( aJob['outputFolder'] )
        outputFile = os.getenv('MYDIR') + "/output/correlations/" + aJob['outputFolder'] + "/correlations.root"     

        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}")\''.format(
            aJob['file1Path'], aJob['h1NameLike'], aJob['file2Path'], aJob['h2NameLike'], outputFile, aJob['hOutNameLike'], int( aJob['isDR'] ), int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd']  
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}")\''.format(
            aJob['file1Path'], aJob['h1NameLike'], aJob['file2Path'], aJob['h2NameLike'], outputFile, aJob['hOutNameLike'], int( aJob['isDR'] ), int( aJob['nrBinsMultForKt'] ),  int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd']  
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}")\''.format(
            aJob['file1Path'], aJob['h1NameUnlike'], aJob['file2Path'], aJob['h2NameUnlike'], outputFile, aJob['hOutNameUnlike'], int( aJob['isDR'] ), int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd']  
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}")\''.format(
            aJob['file1Path'], aJob['h1NameUnlike'], aJob['file2Path'], aJob['h2NameUnlike'], outputFile, aJob['hOutNameUnlike'], int( aJob['isDR'] ), int( aJob['nrBinsMultForKt'] ),  int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd']  
        ) )

def runDraw( outputFolderName = "plots" ):

    #prepare output directory
    outputDirPath = prepareFolder( os.getenv('MYDIR') + '/output', outputFolderName )
    
    print "Running plots in directory:\n{0}\n".format( outputDirPath )

    #define jobs
    jobsToRun = [
        { 
            'fileName' : os.getenv('MYDIR') + "/output/correlations/RD_pPb/correlations.root",
            'hBaseNameLike' : "h4012",
            'hBaseNameUnlike' : "h4212",
            'outputFolder' : "RD_pPb",
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        },
        { 
            'fileName' : os.getenv('MYDIR') + "/output/correlations/RD_Pbp/correlations.root",
            'hBaseNameLike' : "h4012",
            'hBaseNameUnlike' : "h4212",
            'outputFolder' : "RD_Pbp",
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        },
        { 
            'fileName' : os.getenv('MYDIR') + "/output/correlations/MC_pPb/correlations.root",
            'hBaseNameLike' : "h4010",
            'hBaseNameUnlike' : "h4210",
            'outputFolder' : "MC_pPb",
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        },
        { 
            'fileName' : os.getenv('MYDIR') + "/output/correlations/MC_Pbp/correlations.root",
            'hBaseNameLike' : "h4010",
            'hBaseNameUnlike' : "h4210",
            'outputFolder' : "MC_Pbp",
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        },
        { 
            'fileName' : os.getenv('MYDIR') + "/output/correlations/DR_pPb/correlations.root",
            'hBaseNameLike' : "h5012",
            'hBaseNameUnlike' : "h5212",
            'outputFolder' : "DR_pPb",
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        },
        { 
            'fileName' : os.getenv('MYDIR') + "/output/correlations/DR_Pbp/correlations.root",
            'hBaseNameLike' : "h5012",
            'hBaseNameUnlike' : "h5212",
            'outputFolder' : "DR_Pbp",
            'nrBinsMultOnly' : 6,
            'nrBinsMultForKt' : 3,
            'nrBinsKt' : 3,
            'hNameCommonEnd' : "_400"
        }
        
    ]        

    #run job
    for aJob in jobsToRun:
        os.chdir( outputDirPath ) 
        recreateAndChangeDir( aJob['outputFolder'] )    

        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawForBins.C("{0}","{1}","{2}","{3}",{4},{5},"{6}")\''.format(
            aJob['fileName'], aJob['hBaseNameLike'], aJob['hBaseNameUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd']  
        ) )
        os.system( 'root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawForBins.C("{0}","{1}","{2}","{3}",{4},{5},"{6}")\''.format(
            aJob['fileName'], aJob['hBaseNameLike'], aJob['hBaseNameUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultForKt'] ), int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd']  
        ) )

####################################################

#setup local job environment

if not setEnvironmentBEC():
    print "BEC environment not set -> exiting.\n"
exit

os.environ['MYDIR'] = os.getenv('PWD')
mkdirIfNotExists( os.getenv('MYDIR') + "/output" )
print "Setup local BEC job environment in directory:\n{0}\n".format( os.getenv('MYDIR') )
