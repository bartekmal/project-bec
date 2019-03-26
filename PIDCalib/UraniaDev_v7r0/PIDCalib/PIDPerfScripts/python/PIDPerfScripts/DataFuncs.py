import ROOT
import sys
import warnings
import time
import os
from os import path
from PIDPerfScripts.Definitions import *
from PIDPerfScripts.Exceptions import *
from PIDPerfScripts.RunDictFuncs import *
from PIDPerfScripts.TupleDataset import *
from  PIDPerfScripts import OverrideCalibDataStore
from DIRAC.Resources.Storage.StorageElement     import StorageElement
#Added BKK functionality for retrieving Run 2 WGP nTuples
from DIRAC.Core.Base import Script
Script.initialize()

import DIRAC
from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient   import BookkeepingClient
from DIRAC.DataManagementSystem.Client.DataManager import DataManager

def GetDataSetNameDictionary(PartName):
    #======================================================================
    # Define Mother and Workspace name given Particle name
    #======================================================================
    MotherName = GetMotherName(PartName)
    WSName = GetWorkspaceName(PartName)

    ret = {'MotherName'    : GetMotherName(PartName),
           'WorkspaceName' : GetWorkspaceName(PartName)
           }

    return ret


def GetDataSets(StripVer, MagPolarity, PartName, TrackCuts, runMin=None, runMax=None,
                verbose=False, allowMissingDataSets=False, minEntries=0, maxFiles=-1):

    CheckStripVer(StripVer)
    if None not in (runMin, runMax):
        if TrackCuts!='':
            TrackCuts+=' && '
        TrackCuts+='runNumber>='+runMin+' && runNumber<='+runMax
    if verbose:
        print 'Track Cuts: ', TrackCuts

    if verbose:
       print "Attemptng to get URLS: ({0},{1}) ".format(time.time(),time.clock())
    files = GetFiles(StripVer,MagPolarity,PartName,runMin,runMax,maxFiles,verbose)

    if verbose:
       print "Obtain URLS: ({0},{1}) ".format(time.time(),time.clock())
    DataSets = []

    for file in files:
        DataSet = GetDataSet(StripVer, MagPolarity,PartName,TrackCuts,file, verbose,
                             allowMissingDataSets, minEntries)
        if DataSet is not None:
            DataSets.append(DataSet)
    return DataSets

def GetDataSet(StripVer, MagPolarity,PartName,TrackCuts,PIDCutString,xBin,yBin,zBin,file, verbose=False,
               allowMissingDataSets=False, minEntries=1):
               
    #If Run I data has been requested and MC12TuneV4 or MC15TuneV1 ProbNN cut used, return error
    if 'Turbo' not in StripVer:
    	if 'MC12TuneV4' in PIDCutString or 'MC15TuneV1' in PIDCutString:
    		raise RooWorkspaceError("Cannot use MC12TuneV4 or MC15TuneV1 ProbNN for Run 1 data. Please use MC12TuneV2 or MC12TuneV3")

    RecoVer = GetRecoVer(StripVer)

    if verbose:
      print "Attempting to open file {0} for reading: ({1},{2})".format(file,time.time(),time.clock())


    f = ROOT.TFile.Open(file)

    if not f:
        if allowMissingDataSets:
            warnings.warn("File %s does not exist. Skipping this subsample" %file)
            return None
        else:
            raise IOError("Failed to open file %s for reading" %file)

    DataSetNameDict = GetDataSetNameDictionary(PartName)
    wsname=DataSetNameDict['WorkspaceName']
    if verbose:
      print "Attempting to get workspace {0}".format(wsname)
    ws = f.Get(wsname)

    #In the case of Run II data, read the WGP nTuple and convert to RooDataSet
    CheckPartType(PartName)
    PartType = GetRealPartType(PartName)
    DataSetNameDict = GetDataSetNameDictionary(PartName)

    if verbose: print "Attempting to create dataset: ({0},{1})".format(time.time(),time.clock())
    if ws:
      Data = ws.data('data')
    else:
      Data = getDataSetFromTuple ( file       = file
                                  , mother    = DataSetNameDict['MotherName']
                                  , part      = PartType
                                  , trackcuts = TrackCuts
                                  , pidcuts   = PIDCutString
                                  , xvar      = xBin
                                  , yvar      = yBin
                                  , zvar      = zBin
            )
      print "Automatic conversion from TTree to RooDataSet"
    if Data is None:
		print "No entires in RooDataSet - skipping this input file"
		return None
		
#        raise TFileError("Failed to retrieve workspace {wsname} from file {fname}".format(
#            wsname=DataSetNameDict['WorkspaceName'], fname=fname))
    if not Data:
        raise RooWorkspaceError("RooDataSet not found in workspace %s" %wsname)
    if verbose:
      print "Retrieved data: ({0},{1})".format(time.time(),time.clock())
	
    Data.Print("v")

    if verbose:
      print "Applying any cuts to the dataset."

    #======================================================================
    # Declare Instance of RICHTrackDataSet for Calibration tracks
    #======================================================================
    ROOT.gSystem.Load('libRooStats.so')
    #ROOT.gSystem.Load('libCintex.so')
    #cintex=ROOT.Cintex
    #cintex.Enable()
    ROOT.gSystem.Load('libPIDPerfToolsLib.so')
    ROOT.gSystem.Load('libPIDPerfToolsDict.so')

    DataSet = None
    dsType = None
    if (RecoVer>=14):
        dsType = 'GenericDataSet'

        VariableVector = ROOT.std.vector(ROOT.std.pair("string,string"))()
        for VarName, DataSetVarName in DataSetVariables().iteritems():
          #VariableAlias = ROOT.std.pair("string,string")(VarName, DataSetVarName.format(particle=PartType))
          VariableAlias = ROOT.std.pair("string,string")(VarName, DataSetVarName.dsname.format(particle=PartType))
          VariableVector.push_back(VariableAlias)


        DataSet = ROOT.GenericDataSet('Calib_Data'
                                       , Data
                                       , Data.get()
                                       , VariableVector
                                       , TrackCuts
                                       , 'nsig_sw'
                                       )

    else:
        print 'Reco Version < 14! Please use an earlier version of PIDPerfTools.'


    if ws:
      ws.Delete()

    f.Close()
    if verbose:
        print "DataSet is",str(DataSet)+", which uses the internal store:", str(DataSet.store())

    #======================================================================
    # Sanity test: do we have a dataset, and is it empty?
    #======================================================================
    if DataSet is None:
        raise RooDataSetError("Failed to create {0} from RooDataSet".format(
            dsType))

    if DataSet.sumEntries()==0:
        raise RooDataSetError("{0} contains no entries".format(
            dsType))

    if verbose:
        DataSet.Print('v')

    #======================================================================
    # Reduce dataset to only those events within binning limits
    #======================================================================
    #DataSet = None
    #if BinningScheme is not None:
    #    DataSet = AllDataSet.SetInBinSchema(BinningScheme)
    #    AllDataSet.Delete()
    #else:
    #    DataSet = AllDataSet

    #======================================================================
    # Veto ranges with insufficient statistics
    #======================================================================
    if DataSet.sumEntries()<minEntries:
        msg=('Insufficient events in {dsType} ({nEvt}). '
             'Requested at least {minEvts} entries. '
             'Skipping this sumsample').format(
            dsType=dsType, nEvt=DataSet.sumEntries(), minEvts=minEntries)
        warnings.warn( msg )
        return None

    return DataSet

##############################################
### RUN II FILE ACCESS WITH THIS FUNCTION ####
##############################################
	
def GetWGPFiles(StripVer,MagPolarity,verbose=False):
    rm = DataManager()
    bkClient = BookkeepingClient()
    bkDict = {} 
    #args = Script.getPositionalArgs()
    #print args
    
    bkDict[ 'ConfigName' ] = 'LHCb'
    bkDict[ 'EventTypeId' ] = '95100000'
    bkDict['FileType'] = 'PIDCALIB.ROOT'
    bkDict['DataTakingConditions'] = 'Beam6500GeV-VeloClosed-'+MagPolarity
    #Proton-Proton collision data
    if StripVer=="Turbo15" or StripVer=="Turbo16":
    	year = StripVer.replace('Turbo','')
    	bkDict[ 'ConfigVersion' ] = 'Collision'+year
    elif StripVer=="pATurbo15" or StripVer=="pATurbo16":
        year = StripVer.replace('pATurbo','')
    	bkDict[ 'ConfigVersion' ] = 'Protonion'+year
    elif StripVer=="ApTurbo15" or StripVer=="ApTurbo16":
    	year = StripVer.replace('ApTurbo','')
    	bkDict[ 'ConfigVersion' ] = 'Ionproton'+year
    
    reco = ''
    turbo = ''
    version = ''
    if StripVer=="Turbo15" or StripVer=="Turbo16":
    	############
    	### 2015 ###
    	############
    	if year == '15':
    		reco = '15a'
    		turbo = '02'
    		version = '4r1'
    	############
    	### 2016 ###
    	############
    	elif year == '16':
    		reco = '16'
    		turbo = '02a'
    		version = '4r1'
    	
    elif StripVer=="pATurbo15" or StripVer=="pATurbo16" or StripVer=="ApTurbo15" or StripVer=="ApTurbo16":	
    	############
    	### 2015 ###
    	############
    	if year == '15':
    		reco = '15pLead'
    		turbo = '03pLead'
    		version = '5r0'
    	############
    	### 2016 ###
    	############
    	elif year == '16':
    		reco = '16pLead'
    		turbo = '03pLead'
    		version = '5r0'

    bkDict[ 'ProcessingPass' ] = '/Real Data/Reco'+reco+'/Turbo'+turbo+'/PIDCalibTuples'+version+'/PIDMerge01'
    
    file = bkClient.getFilesWithGivenDataSets(bkDict)
	#file = bkClient.getVisibleFilesWithMetadata(bkDict)
    #print file
    files  = file['Value'] 
    
    print "There are " + str(len(files)) + " WGP nTuples in this dataset"
    #print files
    #print "============================================="
    newFileList = []
    for fileName in files:
        newFileList += [fileName.replace ( '/lhcb/LHCb/', "root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/" )]
        #print " ".join(newFileList)  
	
    return newFileList


#############################################
### RUN I FILE ACCESS WITH THIS FUNCTION ####
#############################################

def GetFiles(StripVer,MagPolarity,PartName,runMin=None,runMax=None,maxFiles = -1,verbose=False):
    #files = OverrideCalibDataStore.GetDictFiles(runMin,runMax,maxFiles,verbose)
    #if len(files) > 0:
    #   print "Using files from:" + os.getenv('OVERRIDECALIBDATASTORE')
    #   return files
       
    #else:
    #======================================================================
    # Create dictionary holding:
    # - Reconstruction version    ['RecoVer']
    # - np.array of:
    #        - MagUp run limits   ['UpRuns']
    #        - MagDown run limits ['DownRuns']
    #======================================================================
    files = []
    
    CheckStripVer(StripVer)
    
    CheckPartType(PartName)
    DataDict = GetRunDictionary(StripVer, PartName , verbose )
    
    #======================================================================
    # Determine min and max file indicies
    #======================================================================
    CheckMagPol(MagPolarity)
    IndexDict = GetMinMaxFileDictionary(DataDict, MagPolarity,
                                        runMin, runMax, maxFiles , verbose )
    
    from os import getenv
    
    RecoVer = GetRecoVer(StripVer)
    
    stv = StripVer
    if StripVer == '21_MCTuneV4':
        stv = '21'
    if StripVer == '21r1_MCTuneV4':
        stv = '21r1'
    if StripVer == '23_MCTuneV1':
        stv = '23'
                
                
    DataSetNameDict = GetDataSetNameDictionary(PartName)
                
    fname_protocol = ""
    fname_query = ""
    fname_extra = ""
                
    CalibDataProtocol=os.getenv("CALIBDATAURLPROTOCOL")
    CalibDataExtra=os.getenv("CALIBDATAEXTRA")

    # set the URL protocol (if applicable)
    if CalibDataProtocol is not None and CalibDataProtocol!="":
        fname_protocol = "{0}".format(CalibDataProtocol)


    if CalibDataExtra is not None and CalibDataExtra!="":
        fname_extra = "{0}".format(CalibDataExtra)

    vname_head = "CALIBDATASTORE"
    fname_head = os.getenv(vname_head)
    if fname_head is None:
        raise GetEnvError("Cannot retrieve dataset, environmental variable %s has not been set." %vname_head)

    PartType = GetRealPartType(PartName)
    for i in xrange(IndexDict['minIndex'], IndexDict['maxIndex']+1):
        fname = ("{prtcol}//{extra}//{topdir}/Reco{reco}_DATA/{pol}/"
                 "{mother}_{part}_{pol}_Strip{strp}_{idx}.root").format(
            prtcol=fname_protocol, extra=fname_extra,topdir=fname_head, reco=RecoVer,
            pol=MagPolarity, mother=DataSetNameDict['MotherName'],
            part=PartType, strp=stv, idx=i)
        files += [fname];
        print files[-1]
    return files
