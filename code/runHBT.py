#!/usr/bin/env python

import os

#consts
os.environ['MYROOTPATH'] = '/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt'

#define a structure for BEC jobs
class jobBEC:
    def __init__(self, jobName, outputPath, inputData, dataType, outputFileName = 'HBTAnalysis.root', nrOfEventsToProcess = ''):
        self.jobName = jobName
        self.outputPath = outputPath
        self.inputData = inputData
        self.dataType = dataType
        self.outputFileName = outputFileName
        self.nrOfEventsToProcess = nrOfEventsToProcess

#helper functions

def mkdirIfNotExists( myPath ):
    if not os.path.exists( myPath ):
        os.mkdir( myPath )

def recreateAndChangeDir( folderName ):
    os.system( 'rm -r ' + folderName )
    os.mkdir( folderName )
    os.chdir( folderName )

def prepareOutputFolder( outputFolderName ):
    outputDirPath = os.getenv('MYDIR') + "/output/" + outputFolderName
    mkdirIfNotExists( outputDirPath )

    return outputDirPath

#set the BEC environment
def setEnvironmentBEC():
    currentUser = os.getenv( 'USER' )

    if currentUser == "bmalecki":
        os.environ['BEC_SETUP_SCRIPT'] = '/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb/setupBEC.sh'
        os.environ['BEC_QUEUE'] = '1nd'
        os.environ['BEC_BATCH_SYSTEM_COMMAND'] = 'bsub -q ' + os.getenv('BEC_QUEUE')
        os.system( "source " + os.getenv('BEC_SETUP_SCRIPT') )
        print "BEC environment set for user: bmalecki (lxplus)\n"
        return True
    elif currentUser == "plgbmalecki":
        os.environ['BEC_SETUP_SCRIPT'] = '/mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh'
        os.environ['BEC_QUEUE'] = 'plgrid'
	os.environ['BEC_BATCH_MAXTIME'] = '24:00:00'
        os.environ['BEC_BATCH_SYSTEM_COMMAND'] = 'sbatch -p ' + os.getenv('BEC_QUEUE') + ' -t ' + os.getenv('BEC_BATCH_MAXTIME')
        os.system( "source " + os.getenv('BEC_SETUP_SCRIPT') )
        print "BEC environment set for user: plgbmalecki (PLGrid)\n"
        return True
    else:
        return False   

def createRunScriptForJob( aJob ):
    myFile = open('runHBT_singleJobOnBatch.sh','w')

    myFile.write('#!/bin/sh\n')
    myFile.write('\n')
    myFile.write('source ' + os.getenv('BEC_SETUP_SCRIPT') + '\n')
    myFile.write('OUTPUT_DIR=${SLURM_SUBMIT_DIR}\n')
    myFile.write('\n')
    myFile.write('cd ${OUTPUT_DIR}\n')
    myFile.write( os.getenv('BEC_BASE_DAVINCI') + '/run ' + os.getenv('CODE_EXEC') + ' ' + aJob.inputData + ' ' + aJob.dataType + ' ' + aJob.outputFileName + ' ' + aJob.nrOfEventsToProcess + ' > HBTAnalysis.log' + '\n' )

    myFile.close()

    return os.path.realpath( myFile.name )

#################################################### analysis `rules`

def runHBTJobs( outputFolderName = "HBTAnalysis" ):

    #prepare output directory
    outputDirPath = prepareOutputFolder( outputFolderName )
    
    print "Running HBT jobs with output in directory:\n{0}\n".format( outputDirPath )

    #define jobs
    jobsToRun = [
        jobBEC( jobName = 'RD_pPb_MU', outputPath = 'RD_pPb_MU', inputData = os.getenv('BEC_DATA') + '/ntuple/RD_pPb_MU/hbtNTuple.root', dataType = 'RD' ),
        jobBEC( jobName = 'RD_pPb_MD', outputPath = 'RD_pPb_MD', inputData = os.getenv('BEC_DATA') + '/ntuple/RD_pPb_MD/hbtNTuple.root', dataType = 'RD' ),
        jobBEC( jobName = 'RD_Pbp_MU', outputPath = 'RD_Pbp_MU', inputData = os.getenv('BEC_DATA') + '/ntuple/RD_Pbp_MU/hbtNTuple.root', dataType = 'RD' ),
        jobBEC( jobName = 'RD_Pbp_MD', outputPath = 'RD_Pbp_MD', inputData = os.getenv('BEC_DATA') + '/ntuple/RD_Pbp_MD/hbtNTuple.root', dataType = 'RD' ),
        jobBEC( jobName = 'MC_pPb_MU', outputPath = 'MC_pPb_MU', inputData = os.getenv('BEC_DATA') + '/ntuple/MC_pPb_MU/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'MC_pPb_MD', outputPath = 'MC_pPb_MD', inputData = os.getenv('BEC_DATA') + '/ntuple/MC_pPb_MD/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'MC_Pbp_MU', outputPath = 'MC_Pbp_MU', inputData = os.getenv('BEC_DATA') + '/ntuple/MC_Pbp_MU/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'MC_Pbp_MD', outputPath = 'MC_Pbp_MD', inputData = os.getenv('BEC_DATA') + '/ntuple/MC_Pbp_MD/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'EPOS_local_pPb_MU', outputPath = 'EPOS_local_pPb_MU', inputData = os.getenv('BEC_DATA') + '/ntuple/MC_pPb_MU/EPOS_local/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'EPOS_local_pPb_MD', outputPath = 'EPOS_local_pPb_MD', inputData = os.getenv('BEC_DATA') + '/ntuple/MC_pPb_MD/EPOS_local/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'EPOS_local_Pbp_MU', outputPath = 'EPOS_local_Pbp_MU', inputData = os.getenv('BEC_DATA') + '/ntuple/MC_Pbp_MU/EPOS_local/hbtNTuple_corrected.root', dataType = 'MC' ),
        jobBEC( jobName = 'EPOS_local_Pbp_MD', outputPath = 'EPOS_local_Pbp_MD', inputData = os.getenv('BEC_DATA') + '/ntuple/MC_Pbp_MD/EPOS_local/hbtNTuple_corrected.root', dataType = 'MC' ),   
    ]        

    #compile
    os.environ['CODE_NAME'] = 'HBTAnalysis'
    os.environ['CODE_INPUT'] = os.getenv('BEC_BASE_CODE_HBT')
    os.environ['CODE_EXEC'] = os.getenv('MYDIR')+'/'+os.getenv('CODE_NAME')+'.exe'
    os.system('rm ${CODE_EXEC}')
    os.system('${BEC_BASE_DAVINCI}/run g++ -g -std=c++11 -O2  -pthread -m64 -I${MYROOTPATH}/include ${CODE_INPUT}/*.cpp -o ${CODE_EXEC} -L${MYROOTPATH}/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore  -pthread -lm -ldl -rdynamic -lTreePlayer -lMinuit -lTMVA -lXMLIO -lMLP')

    #run job
    for aJob in jobsToRun:
        os.chdir( outputDirPath ) 
        recreateAndChangeDir( aJob.outputPath )
               
        runFilePath = createRunScriptForJob( aJob )
        os.system( os.getenv('BEC_BATCH_SYSTEM_COMMAND') + ' -J ' + aJob.jobName + ' ' + runFilePath )
        #os.system( '${BEC_BASE_DAVINCI}/run ${CODE_EXEC} ' + aJob.inputData + ' ' + aJob.dataType + ' ' + aJob.outputFileName + ' ' + aJob.nrOfEventsToProcess + ' HBTAnalysis.log')

def runMerge( outputFolderName = "merged" ):

    #prepare output directory
    outputDirPath = prepareOutputFolder( outputFolderName )
    
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
                
        os.system( 'lb-run ROOT hadd -ff merged.root' + ' ' + ' '.join( aJob['filesToMerge'] ) )

def runDivide( outputFolderName = "correlations" ):

    #prepare output directory
    outputDirPath = prepareOutputFolder( outputFolderName )
    
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

        os.system( 'lb-run ROOT root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}")\''.format(
            aJob['file1Path'], aJob['h1NameLike'], aJob['file2Path'], aJob['h2NameLike'], outputFile, aJob['hOutNameLike'], int( aJob['isDR'] ), int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd']  
        ) )
        os.system( 'lb-run ROOT root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}")\''.format(
            aJob['file1Path'], aJob['h1NameLike'], aJob['file2Path'], aJob['h2NameLike'], outputFile, aJob['hOutNameLike'], int( aJob['isDR'] ), int( aJob['nrBinsMultForKt'] ),  int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd']  
        ) )
        os.system( 'lb-run ROOT root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}")\''.format(
            aJob['file1Path'], aJob['h1NameUnlike'], aJob['file2Path'], aJob['h2NameUnlike'], outputFile, aJob['hOutNameUnlike'], int( aJob['isDR'] ), int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd']  
        ) )
        os.system( 'lb-run ROOT root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/divideHistogramsInBins.C("{0}","{1}","{2}","{3}","{4}","{5}",{6},{7},{8},"{9}")\''.format(
            aJob['file1Path'], aJob['h1NameUnlike'], aJob['file2Path'], aJob['h2NameUnlike'], outputFile, aJob['hOutNameUnlike'], int( aJob['isDR'] ), int( aJob['nrBinsMultForKt'] ),  int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd']  
        ) )

def runDraw( outputFolderName = "plots" ):

    #prepare output directory
    outputDirPath = prepareOutputFolder( outputFolderName )
    
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

        os.system( 'lb-run ROOT root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawForBins.C("{0}","{1}","{2}","{3}",{4},{5},"{6}")\''.format(
            aJob['fileName'], aJob['hBaseNameLike'], aJob['hBaseNameUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultOnly'] ), 0, aJob['hNameCommonEnd']  
        ) )
        os.system( 'lb-run ROOT root -l -b -q \'' + os.getenv('BEC_BASE_CODE_SCRIPTS') + '/drawForBins.C("{0}","{1}","{2}","{3}",{4},{5},"{6}")\''.format(
            aJob['fileName'], aJob['hBaseNameLike'], aJob['hBaseNameUnlike'], aJob['outputFolder'], int( aJob['nrBinsMultForKt'] ), int( aJob['nrBinsKt'] ), aJob['hNameCommonEnd']  
        ) )

####################################################

#setup local job environment

if not setEnvironmentBEC():
    print "BEC environemnt not set -> exiting.\n"
exit

os.environ['MYDIR'] = os.getenv('PWD')
mkdirIfNotExists( os.getenv('MYDIR') + "/output" )
print "Setup local BEC job environment in directory:\n{0}\n".format( os.getenv('MYDIR') )