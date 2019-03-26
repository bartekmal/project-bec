#!/bin/env python
from PIDPerfScripts.StartScreen import *

import ROOT

import sys
import os.path
from array import *
import argparse

#make projections
def makeprojections(hs,projlist,axes=[]):
    dim=hs.GetNdimensions()
    axs={}
    for a in range(0,dim):
        #print hs.GetAxis(a).GetName()
        if str(hs.GetAxis(a).GetName()) in axes:
            axs[a]=hs.GetAxis(a).GetName()
    
    projdim=[]
    for d in range(0,dim):
        if d not in axs.keys():
            projdim.append(d)
    
    if len(axs)==1:
        for xbin in range(0,hs.GetAxis(axs.keys()[0]).GetNbins()+2):
            hs.GetAxis(axs.keys()[0]).SetRange(xbin,xbin)
            tmpname=str(hs.GetName())+"_proj_"+axs[axs.keys()[0]]+"bin"+str(xbin)
            projlist[str(hs.GetName())+tmpname]=hs.Projection(len(projdim),array("i",projdim))
            projlist[str(hs.GetName())+tmpname].SetName(str(hs.GetName())+tmpname)
            projlist[str(hs.GetName())+tmpname].SetTitle(str(hs.GetName())+tmpname)
    
    if len(axs)==2:
        for xbin in range(0,hs.GetAxis(axs.keys()[0]).GetNbins()+2):
            hs.GetAxis(axs.keys()[0]).SetRange(xbin,xbin)
            for ybin in range(0,hs.GetAxis(axs.keys()[1]).GetNbins()+2):
                hs.GetAxis(axs.keys()[1]).SetRange(ybin,ybin)
                tmpname=str(hs.GetName())+"_proj_"+axs[axs.keys()[0]]+"bin"+str(xbin)+"_"+axs[axs.keys()[1]]+"bin"+str(ybin)
                projlist[str(hs.GetName())+tmpname]=hs.Projection(len(projdim),array("i",projdim))
                projlist[str(hs.GetName())+tmpname].SetName(str(hs.GetName())+tmpname)
                projlist[str(hs.GetName())+tmpname].SetTitle(str(hs.GetName())+tmpname)
          
    if len(axs)==3:
        for xbin in range(0,hs.GetAxis(axs.keys()[0]).GetNbins()+2):
            hs.GetAxis(axs.keys()[0]).SetRange(xbin,xbin)
            for ybin in range(0,hs.GetAxis(axs.keys()[1]).GetNbins()+2):
                hs.GetAxis(axs.keys()[1]).SetRange(ybin,ybin)
                for zbin in range(0,hs.GetAxis(axs.keys()[2]).GetNbins()+2):
                    hs.GetAxis(axs.keys()[2]).SetRange(zbin,zbin)
                    tmpname=str(hs.GetName())+"_proj_"+axs[axs.keys()[0]]+"bin"+str(xbin)+"_"+axs[axs.keys()[1]]+"bin"+str(ybin)+"_"+axs[axs.keys()[2]]+"bin"+str(zbin)
                    projlist[str(hs.GetName())+tmpname]=hs.Projection(len(projdim),array("i",projdim))
                    projlist[str(hs.GetName())+tmpname].SetName(str(hs.GetName())+tmpname)
                    projlist[str(hs.GetName())+tmpname].SetTitle(str(hs.GetName())+tmpname)

#function to fill THnSparse with dataset
def mcreweight_datasettothsparse(ds,Part,hsparse,varnames,weight,h1d_list=[]):
    if len(h1d_list)==len(varnames):
      for i in range(0,ds.numEntries()):
          Dset = ds.get(i)
          
          tmparray=[]
          
          tmpwgt=ds.weight()
          #print tmpwgt
          for k in range(len(varnames)):
              tmparray.append(Dset.getRealValue(varnames[k]))
              h1d_list[k].Fill(float(Dset.getRealValue(varnames[k])),tmpwgt)

          hsparse.Fill(array("d",tmparray),tmpwgt)
    else:
      for i in range(0,ds.numEntries()):
          Dset = ds.get(i)
          
          tmparray=[]
          
          tmpwgt=ds.weight()
          #print tmpwgt
          for k in range(len(varnames)):
              tmparray.append(Dset.getRealValue(varnames[k]))

          hsparse.Fill(array("d",tmparray),tmpwgt)

#function to fill THnSpare with dataset list
def mcreweight_dslisttothsparse(StripVersion, MagPolarity, RunMin, RunMax, verbose, allowMissing,Part,nm,totalbinning,weight,cuts="",kinbinning=[]):
    dummynbins=[]
    dummymins=[]
    dummymaxs=[]
    
    for b in range(len(totalbinning)):
        dummynbins.append(len((totalbinning[b])[2])-1)
        dummymins.append(-100.)
        dummymaxs.append(100.)
    
    #defining histograms
    from ROOT import THnSparseD, TH1D
    varnames={}
    kinvarnames=[]
    hs = THnSparseD(nm,nm,len(totalbinning),array("i",dummynbins),array("d",dummymins),array("d",dummymaxs))
    h1d_list = {}
    hprojections_list={}
    for i in range(0,len(totalbinning)):
        h1d_list[i]= TH1D("H1D_"+(totalbinning[i])[0],(totalbinning[i])[0],len((totalbinning[i])[2])-1,array("d",(totalbinning[i])[2]))
        hs.GetAxis(i).Set(len((totalbinning[i])[2])-1,array("d",(totalbinning[i])[2]))
        hs.GetAxis(i).SetTitle((totalbinning[i])[0])
        hs.GetAxis(i).SetName((totalbinning[i])[0])
        varnames[i]=(totalbinning[i])[1]
    
    for i in range(0,len(kinbinning)):
        kinvarnames.append((kinbinning[i])[0])
    
    #DataDict = GetRunDictionary(StripVersion, Part)
    #IndexDict = GetMinMaxFileDictionary(DataDict, MagPolarity, RunMin, RunMax, -1)
    
    print "Filling Histogram"
    
    #for i in xrange(IndexDict['minIndex'], IndexDict['maxIndex']+1):
        
    #    #ds = GetDataSet(StripVersion, MagPolarity, Part, 'runNumber>='+str(RunMin)+' && runNumber<='+str(RunMax), i, verbose, allowMissing)
    #    ds = GetDataSet(StripVersion, MagPolarity, Part, 'runNumber>='+str(RunMin)+' && runNumber<='+str(RunMax), opts.pidVars, opts.xVarName, opts.yVarName, opts.zVarName, i, verbose, allowMissing)
    #    if ds is not None:
    #        if cuts!="":
    #            cutds=ds.reduce(cuts)
    #            mcreweight_datasettothsparse(cutds,Part,hs,varnames,weight,h1d_list)
    #            cutds.IsA().Destructor(cutds) #free memory from cut dataset
    #        else:
    #            mcreweight_datasettothsparse(ds,Part,hs,varnames,weight,h1d_list)
    #    ds.Delete() #needed to free memory
    
    if "Turbo" not in StripVersion:
    	files = GetFiles(StripVersion,MagPolarity,Part,RunMin,RunMax,100000,opts.verbose)
    elif "Turbo" in StripVersion:
    	files = GetWGPFiles(StripVersion,MagPolarity,opts.verbose)
    
    for file in files:
    	
    	#ds = GetDataSet(StripVersion, MagPolarity, Part, 'runNumber>='+str(RunMin)+' && runNumber<='+str(RunMax), i, verbose, allowMissing)
        #ds = GetDataSet(StripVersion, MagPolarity, Part, 'runNumber>='+str(RunMin)+' && runNumber<='+str(RunMax), opts.pidVars, opts.xVarName, opts.yVarName, opts.zVarName, file, verbose, allowMissing)
        ds = GetDataSet(StripVersion, MagPolarity, Part, cuts, opts.pidVars, opts.xVarName, opts.yVarName, opts.zVarName, file, verbose, allowMissing)
    
        if ds is not None:
            if cuts!="":
            	print "Cuts are the following : " + cuts
                cutds=ds.reduce(cuts)
                mcreweight_datasettothsparse(cutds,Part,hs,varnames,weight,h1d_list)
                cutds.IsA().Destructor(cutds) #free memory from cut dataset
            else:
                mcreweight_datasettothsparse(ds,Part,hs,varnames,weight,h1d_list)
        	ds.Delete() #needed to free memory
    

    #making projections
    print "Filling projections for kinematic variables"
    #print kinvarnames
    makeprojections(hs,hprojections_list,kinvarnames)
    
    h1d_list[len(totalbinning)]=hs
    h1d_list.update(hprojections_list)
    return h1d_list
                


class ShowArgumentsParserMCreweight(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n\n' %message)
        parser.print_usage(sys.stderr)
        sys.stderr.write('\n'+self.description)
        sys.exit(2)
        
if '__main__' == __name__:
    start()
    print ""
    
    parser = ShowArgumentsParserMCreweight(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        prog=os.path.basename(sys.argv[0]),
        description=("""Make pid distributions for a given:
        a) Sample version <sampleVersion> (e.g. \"20\" for Run 1 Stripping 20, \"Turbo16\" for Run 2 WGP)
        b) magnet polarity <magPol> (\"MagUp\" or \"MagDown\")
        c) particle type <partName> (\"K\", \"P\", \"Pi\", \"e\" or \"Mu\")
        d) PID variable, <pidVar>
Multiple PID variables can be specified if necessary, e.g. \"[DLLK,DLLp]\".
Note that there is no space after the comma.

For a full list of arguments, do: 'python {0} -h'
""").format(os.path.basename(sys.argv[0])),
        epilog ="""To use the 'MuonUnBiased' hadron samples for muon misID studies, one of the
following tracks types should be used instead: \"K_MuonUnBiased\", \"Pi_MuonUnBiased\"
or \"P_MuonUnBiased\"."""
        )
    ## add the positional arguments
    parser.add_argument('sampleVersion', metavar='<sampleVersion>',  help="Sets the stripping version for Run I data, or the Turbo WGP production version for Run II")
    parser.add_argument('magPol', metavar='<magPol>',  help="Sets the magnet polarity")
    parser.add_argument('partName', metavar='<partName>', help="Sets the particle type")
    parser.add_argument('pidVars', metavar='<pidVars>', help="Sets the PID variables to store")
    
    
    ## add the optional arguments
    parser.add_argument('-x', '--minRun', dest="runMin", metavar="NUM",default=0, help="Sets the minimum run number to process (if applicable)")
    parser.add_argument('-y', '--maxRun', dest="runMax", metavar="NUM",default=9999999, help="Sets the maximum run number to process (if applicable)")
    parser.add_argument('-c', '--cuts', dest='cuts', metavar='CUTS', default='DEFAULT:ALL', help=("Sets the list of cuts to apply to the calibration "
                             "sample(s) prior to determine the PID efficiencies "
                             "(default: (default)s). "
                             "NB. It is up to the user to ensure that their reference "
                             "sample has the same cuts applied."
                             ))
    parser.add_argument("-o", "--outputDir", dest="outputDir", metavar="DIR",
                        help="Save the performance histograms to directory DIR "
                        "(default: current directory)")

    binGroup = parser.add_argument_group("binning options")
    binGroup.add_argument("-X", "--xVarName", dest="xVarName", metavar="NAME", default="P", help="Sets the NAME of the 1st (x) bin variable "
                          "(default: %(default)s)")
    binGroup.add_argument("-Y", "--yVarName", dest="yVarName", metavar="NAME", default="ETA", help="Sets the NAME of the 2nd (y) bin variable "
                          "(default: %(default)s). "
                          "If 1D binning is required, then this option should "
                          "be set to an empty string")
    binGroup.add_argument("-Z", "--zVarName", dest="zVarName", metavar="NAME", default="nTracks", help="Sets the NAME of the 3rd (z) bin variable "
                          "(default: %(default)s). "
                          "If 2D binning is required, then this option should "
                          "be set to an empty string")
    binGroup.add_argument("-s", "--schemeName", dest="schemeName", metavar="NAME",  default=None, help="Sets the NAME of the binning scheme, "
                          "as defined in the module 'PIDPerfScripts.binning'. "
                          "If this option is not set, the default "
                          "binning scheme is used.")

    binGroup.add_argument("-b", "--binSchemeFile", dest="binSchemeFile",
                          metavar="NAME", default=None,
                          help="Sets the NAME of the python file containing "
                          "user-defined binning schema. Without this option, "
                          "the script will only look for binning schema in "
                          "the 'PIDPerfScripts.binning' module")
    
    addGroup = parser.add_argument_group("further options")
    addGroup.add_argument("-q", "--quiet", dest="verbose", action="store_false",
                          default=True,
                          help="Suppresses the printing of verbose information")
    
    addGroup.add_argument("-M", "--allow-missing", dest="allowMissing",
                          action="store_true", default=False,
                          help="Allow missing calibration subsamples. "
                          "N.B. You should only use this option if requested to "
                          "do so by the PIDCalib authors") 

    opts = parser.parse_args()      
    
    
    from PIDPerfScripts.Definitions import *
    from PIDPerfScripts.DataFuncs import *
    from PIDPerfScripts.PerfResults import *
    from PIDPerfScripts.PerfCalcFuncs import *
    
    
    print "checking options"
#    print opts.cuts
    StripVersion = None
    MagPolarity = None
    PartName = None
    YourPart = None
    RunMin = 0
    RunMax = 9999999

    # set the stripping version
    StripVersion=opts.sampleVersion
    CheckStripVer(StripVersion)
    
    # set the magnet polarity
    MagPolarity=opts.magPol
    CheckMagPol(MagPolarity)
        
    # set the particle name
    PartName=opts.partName
    # CheckPartType(PartName)
    YourPart = opts.cuts
    YourPart_dict={"DEFAULT":"ALL"}
    # set run range
   
    RunMin = opts.runMin
    RunMax = opts.runMax

    if RunMin is not None:
        try:
            int(RunMin)
        except ValueError:
            parser.error(
                "Argument to --minRun ('%s') is not an integer'." %RunMin)
            
        if RunMax is None:
            parser.error(
                "Min run was specified as %s, but no max run was given." %RunMin)

    if RunMax is not None:
        try:
            int(RunMax)
        except ValueError:
            parser.error(
                "Argument to --maxRun ('%s') is not an integer'." %RunMax)
        if RunMin is None:
            parser.error(
                "Max run was specified as %s, but no min run was given." %RunMax)
    # set particle types
    
    if PartName.startswith("["):
        if not PartName.endswith("]"):
            parser.error("Invalid Particles string %s" %PartName)
        PartName = PartName[1:-1].split(',')
    elif PartName.startswith("("):
        if not PartName.endswith(")"):
            parser.error("Invalid Particles string %s" %PartName)
        PartName = PartName[1:-1].split(',')
    else:
        PartName = (PartName,)

    for tmpp in PartName:
        CheckPartType(tmpp)
    

    # set cuts of particles you want to get the weighted pid

    if YourPart.startswith("["):
        if not YourPart.endswith("]"):
            parser.error("Invalid Particles string %s" %YourPart)
        YourPart = YourPart[1:-1].split(',')
    elif YourPart.startswith("("):
        if not YourPart.endswith(")"):
            parser.error("Invalid Particles string %s" %YourPart)
        YourPart = YourPart[1:-1].split(',')
    else:
        YourPart = (YourPart,)
    if len(YourPart)>0:
        YourPart_dict={}
    for ypt in YourPart:
        tmps=ypt.split(':')
        YourPart_dict[tmps[0]]=tmps[1]
    print "setting cuts as"
    print YourPart_dict

    
    # set pid variables to store

    pidVars = opts.pidVars

    if pidVars.startswith("["):
        if not pidVars.endswith("]"):
            parser.error("Invalid pid variables string %s" %pidVars)
        pidVars = pidVars[1:-1].split(',')
    elif pidVars.startswith("("):
        if not pidVars.endswith(")"):
            parser.error("Invalid pid variables string %s" %pidVars)
        pidVars = pidVars[1:-1].split(',')
    else:
        pidVars = (pidVars,)
    ct=0
    
    pidVars_dict={}
    for v in pidVars:
        pidVars_dict[ct]=v
        ct=ct+1
    
    
    XVarName = opts.xVarName
    if XVarName=='':
        parser.error("Argument to --xBinVarName is an empty string.")
        
    YVarName = opts.yVarName
    ZVarName = opts.zVarName

    if ZVarName!='' and YVarName=='':
        parser.error("Argument to --yVarName is an empty string,"
                     "but argument to --zVarName. If you planned to"
                     "include only two binning variables. Did you"
                     "mean to do --yVarName='{0}' --zVarName='{1}'?".format(
            ZVarName, YVarName))

    SchemeName=opts.schemeName
    
    #=============================================================================
    # Declare Binning Schema (RooBinnings)
    #=============================================================================
    from PIDPerfScripts.Binning import GetBinScheme
    if opts.binSchemeFile is not None:
        import imp
        try:
            imp.load_source('userbinning', opts.binSchemeFile)
        except IOError:
            msg="Failed to load binning scheme file '{0}'".format(opts.binSchemeFile)
            raise IOError(msg)
    ROOT.gSystem.Load('libRooStats.so')
#    ROOT.gSystem.Load('libCintex.so')
#    cintex=ROOT.Cintex
#    cintex.Enable()
    ROOT.gSystem.Load('libPIDPerfToolsLib.so')
    ROOT.gSystem.Load('libPIDPerfToolsDict.so')

#    help(ROOT.PerfCalculator)

    #======================================================================
    # Open file to write TH1Fs to
    #======================================================================
    fnameSuffix=''
    if SchemeName is not None:
        fnameSuffix+='_{0}'.format(SchemeName)
    if "Turbo" not in StripVersion:
    	fname = "PIDHists_Strip{strp}_{pol}{suf}.root".format(
        	strp=StripVersion, pol=MagPolarity,
        	suf=fnameSuffix)
    elif "Turbo" in StripVersion:
    	fname = "PIDHists_{strp}_{pol}{suf}.root".format(
        	strp=StripVersion, pol=MagPolarity,
        	suf=fnameSuffix)
    if opts.outputDir is not None:
        fname = "%s/%s" %(opts.outputDir, fname)
        
    print "Saving histograms to %s" %fname
    f_Out = ROOT.TFile.Open(fname, "RECREATE")
    if not f_Out:
        raise IOError("Failed to open file %s for writing" %fname)
    
    #======================================================================
    # Getting Datasets
    #======================================================================
    print "Getting datasets and filling histograms"
    
    
    #create directory structure
    for k,yp in YourPart_dict.iteritems():
        f_Out.cd()
        labdir=f_Out.mkdir(k)
        labdir.cd()
        for Part in PartName:
            if labdir:
                part_dir=labdir.mkdir(Part)
            else:
                part_dir=f_Out.mkdir(Part)
    #f_Out.ls()            
    for Part in PartName:
        f_Out.cd()
        _tmpcuts=opts.cuts
    
    	if "Turbo" not in StripVersion:
    		files = GetFiles(StripVersion,MagPolarity,Part,RunMin,RunMax,100000,opts.verbose)
    	elif "Turbo" in StripVersion:
    		files = GetWGPFiles(StripVersion,MagPolarity,opts.verbose)
    
        _tmpDataSet = GetDataSet(StripVersion, MagPolarity, Part,"", opts.pidVars, opts.xVarName, opts.yVarName, opts.zVarName, files[0], opts.verbose, opts.allowMissing)
         
        #dsl=GetDataSets(StripVersion, MagPolarity, Part, "", RunMin, RunMax,opts.verbose,opts.allowMissing)


        X_Bin = GetBinScheme(Part, XVarName, SchemeName)
        Y_Bin = None
        if YVarName!='':
            Y_Bin = GetBinScheme(Part, YVarName, SchemeName)
        Z_Bin = None
        if ZVarName!='':
            Z_Bin = GetBinScheme(Part, ZVarName, SchemeName)


        if X_Bin:
            XVarNamec=_tmpDataSet.Get_ParamName(XVarName)
        if Y_Bin:
            YVarNamec=_tmpDataSet.Get_ParamName(YVarName)
        if Z_Bin:
            ZVarNamec=_tmpDataSet.Get_ParamName(ZVarName)
        XBinning=[]
        YBinning=[]
        ZBinning=[]
        for i in range(X_Bin.numBins()):
            XBinning.append(X_Bin.binLow(i))
        XBinning.append(X_Bin.binHigh(X_Bin.numBins()-1))
        if Y_Bin:
            for i in range(Y_Bin.numBins()):
                YBinning.append(Y_Bin.binLow(i))
            YBinning.append(Y_Bin.binHigh(Y_Bin.numBins()-1))
        if Z_Bin:
            for i in range(Z_Bin.numBins()):
                ZBinning.append(Z_Bin.binLow(i))
            ZBinning.append(Z_Bin.binHigh(Z_Bin.numBins()-1))

    #=============================================================================
    # Declare Binning Schema (RooBinnings)
    #=============================================================================

         
        if opts.verbose:
            print('========== Binning Schema ==========')
            if SchemeName is None:
                print "Default binning schema"
            else:
                print "Using binning schema '{0}'".format(SchemeName)
            print('====================================')

            
        _nprobnn=1000
        _ndlls=300
        totalbinning={}
        kinbinning={}
        _nvars=0
        totalbinning[_nvars]=(XVarName,XVarNamec,XBinning)
        kinbinning[_nvars]=(XVarName,XVarNamec,XBinning)
        _nvars=1
        if Y_Bin:
            totalbinning[_nvars]=(YVarName,YVarNamec,YBinning)
            kinbinning[_nvars]=(YVarName,YVarNamec,YBinning)
            _nvars= _nvars+1
            if Z_Bin:
                totalbinning[_nvars]=(ZVarName,ZVarNamec,ZBinning)
                kinbinning[_nvars]=(ZVarName,ZVarNamec,ZBinning)
                _nvars= _nvars+1


        
        for vpid in range(len(pidVars_dict)):
            _tmpbinning=[]
            #add underflow bin
            _tmpbinning.append(-1001.)
            _tmpbinning.append(-999.)
            if "DLL" not in pidVars_dict[vpid]:
                for nb in range(_nprobnn+1):
                    _tmpbinning.append(0.+float(nb)/float(_nprobnn))
            else:
                for nb in range(_ndlls+1):
                    _tmpbinning.append(-150.+float(nb))
            
            print vpid
            #print _tmpbinning
            totalbinning[_nvars]=(pidVars_dict[vpid],_tmpDataSet.Get_ParamName(pidVars_dict[vpid]),_tmpbinning)
            
            _nvars= _nvars+1
            

        for k,yp in YourPart_dict.iteritems():
            
            _tmpcuts=""
            print "Filling histograms for " +k
            if "ALL" not in yp:
                _tmpcuts=yp
                print  "Applying cut "+_tmpcuts
            
            
            hs=mcreweight_dslisttothsparse(StripVersion, MagPolarity, RunMin, RunMax,opts.verbose,opts.allowMissing,Part,"histo_PID",totalbinning,"nsig_sw",_tmpcuts,kinbinning)
            f_Out.cd()
            
            if len(YourPart_dict)>0:
                f_Out.cd(k+"/"+Part)
            else:
                f_Out.cd(Part)
            for th in hs.itervalues():
                th.Write()

       
        

    #======================================================================
    # Close file
    #======================================================================
    f_Out.Close()
    print "Done filling PID distributions"
            
