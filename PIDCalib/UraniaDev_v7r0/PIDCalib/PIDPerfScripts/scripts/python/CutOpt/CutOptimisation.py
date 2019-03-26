#!/bin/env python
from PIDPerfScripts.StartScreen import *
from PIDPerfScripts.Definitions import *
from PIDPerfScripts.DataFuncs import *
from PIDPerfScripts.PerfCalcFuncs import *
from PIDPerfScripts.PerfResults import *

import ROOT

import sys
import os.path
#import getopt
#import optparse
#import warnings
import argparse

class ShowArgumentsParser(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n\n' %message)
        parser.print_usage(sys.stderr)
        sys.stderr.write('\n'+self.description)
        sys.exit(2)

if '__main__' == __name__:
    start()
    print ""
    #TODO: update the help
    parser = ShowArgumentsParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        prog=os.path.basename(sys.argv[0]),
        description=("""Make punzi FoM vs cut histograms for a given:
        a) stripping version <stripVersion> (e.g. \"20\")
        b) magnet polarity  <magPol> (\"MagUp\" or \"MagDown\")
        c) particle type <partName> (\"K\", \"P\", \"Pi\", \"e\" or \"Mu\")
        d) PID variable, <pidVar>

For a full list of arguments, do: 'python {0} -h'

e.g. python {0} 20r1 MagDown Pi ProbNNpi ">" 10 0.8 1.0 output.root signal_input.root \
            subTree pi background_input.root subTree pi -f 1
""").format(os.path.basename(sys.argv[0])),
        epilog =
"""To use the 'MuonUnBiased' hadron samples for muon misID studies, one of the
following tracks types should be used instead: \"K_MuonUnBiased\", \"Pi_MuonUnBiased\"
or \"P_MuonUnBiased\".
""" )
    ## add the positional arguments
    parser.add_argument('stripVersion', metavar='<stripVersion>',
                        help="Sets the stripping version")
    parser.add_argument('magPol', metavar='<magPol>',
                        help="Sets the magnet polarity")
    parser.add_argument('partName', metavar='<partName>',
                        help="Sets the particle type")
    parser.add_argument('pidVar', metavar='<pidVar>',
                        help="Sets the PID variable to optimise")
    parser.add_argument('pidSign', metavar='<greater/less>',
                        help="Sets direction of the cut (> or <)")
    parser.add_argument('pidSteps', metavar='<pidSteps>',
                        help="Number of steps in PID variable")
    parser.add_argument('pidStart', metavar='<pidStart>',
                        help="Lower limit of PID variable")
    parser.add_argument('pidStop', metavar='<pidStop>',
                        help="Upper limit of PID variable")

    parser.add_argument('outFilename', metavar='<outFileName>',
                        help="Sets the name of the output file")

    parser.add_argument('refFilename', metavar='<refFileName>',
                        help="Sets the name of the file containing the reference TTree")
    parser.add_argument('refTree', metavar='<refTree>',
                        help="Sets the name of the reference TTree")
    parser.add_argument('refBranch', metavar='<refBranch>',
                        help="Sets the name of the particle in the reference TTree")

    parser.add_argument('bkgFilename', metavar='<bkgFileName>',
                        help="Sets the name of the file containing the background TTree")
    parser.add_argument('bkgTree', metavar='<bkgTree>',
                        help="Sets the name of the background TTree")
    parser.add_argument('bkgBranch', metavar='<bkgBranch>',
                        help="Sets the name of the particle in the background TTree")

    parser.add_argument('-F', '--fom', dest="fom", metavar="FOM", default="punzi",
                        help="Sets the figure of merit to optimise. "
                             "Can be one of: punzi, sqrtsoverb or soversqrtsplusb.")
    parser.add_argument('-S', '--fomScale', dest="fomScale", metavar="SCALE",
                        help="Set this to an estimate of the numer signal events, "
                             "it's needed to calculate the figure of merit.")

    ## add the optional arguments
    parser.add_argument('-m', '--minRun', dest="runMin", metavar="NUM",
                        help="Sets the minimum run number to process (if applicable)")
    parser.add_argument('-n', '--maxRun', dest="runMax", metavar="NUM",
                        help="Sets the maximum run number to process (if applicable)")
    parser.add_argument('-f', '--maxFiles', dest="maxFiles", metavar="NUM",
                        help="Sets the maximum number of calibration files to run over")
    parser.add_argument('-c', '--cuts', dest='cuts', metavar='CUTS', default='',
                        help=("Sets the list of cuts to apply to the calibration "
                             "sample(s) prior to determine the PID efficiencies "
                             "(default: (default)s). "
                             "NB. It is up to the user to ensure that their reference "
                             "sample has the same cuts applied."
                             ))
    parser.add_argument('-d', '--referenceCuts', dest='refCuts', metavar='CUTS', default='',
                        help=("Sets the list of cuts to apply to the reference "
                             "sample prior to reweighting PID efficiency."
                             ))
    parser.add_argument('-e', '--backgroundCuts', dest='bkgCuts', metavar='CUTS', default='',
                        help=("Sets the list of cuts to apply to the background "
                             "sample prior to determining the number of events "
                             "passing PID cuts."
                             ))
    parser.add_argument("-o", "--outputDir", dest="outputDir", metavar="DIR",
                        help="Save the performance histograms to directory DIR "
                        "(default: current directory)")

    binGroup = parser.add_argument_group("binning options")
    binGroup.add_argument("-X", "--xVarName", dest="xVarName", metavar="NAME",
                          default="P",
                          help="Sets the NAME of the 1st (x) bin variable "
                          "(default: %(default)s)")
    binGroup.add_argument("-Y", "--yVarName", dest="yVarName", metavar="NAME",
                          default="ETA",
                          help="Sets the NAME of the 2nd (y) bin variable "
                          "(default: %(default)s). "
                          "If 1D binning is required, then this option should "
                          "be set to an empty string")
    binGroup.add_argument("-Z", "--zVarName", dest="zVarName", metavar="NAME",
                          default="nTracks",
                          help="Sets the NAME of the 3rd (z) bin variable "
                          "(default: %(default)s). "
                          "If 2D binning is required, then this option should "
                          "be set to an empty string")
    binGroup.add_argument("-s", "--schemeName", dest="schemeName", metavar="NAME",
                          default=None,
                          help="Sets the NAME of the binning scheme, "
                          "as defined in the module 'PIDPerfScripts.binning'. "
                          "If this option is not set, the default "
                          "binning scheme is used.")

    binGroup.add_argument("-b", "--binSchemeFile", dest="binSchemeFile",
                          metavar="NAME", default=None,
                          help="Sets the NAME of the python file containing "
                          "user-defined binning schema. Without this option, "
                          "the script will only look for binning schema in "
                          "the 'PIDPerfScripts.binning' module")
    binGroup.add_argument("-N", "--noBinLimitCheck", dest="setRefLimits",
                          action="store_false", default=True,
                          help=("Don't remove events in the reference sample "
                          "that are outside the bin range of the performance "
                          "histograms (will produce an exception if there are "
                          "events outside the bin range)"))

    binGroup.add_argument("-x", "--xRefVarName", dest="xRefVarName", metavar="NAME",
                          default="P",
                          help=("Sets the NAME of the 1st (x) bin variable "
                          "in the reference sample (default: %(default)s)"))
    binGroup.add_argument("-y", "--yRefVarName", dest="yRefVarName", metavar="NAME",
                          default="TRACK_Eta",
                          help=("Sets the NAME of the 2nd (y) bin variable "
                          "in the reference sample (default: %(default)s)"))
    binGroup.add_argument("-z", "--zRefVarName", dest="zRefVarName",
                          metavar="NAME", default="nTracks",
                          help=("Sets the NAME of the 3rd (z) bin variable "
                          "in the reference sample (default: %(default)s)"))

    addGroup = parser.add_argument_group("further options")
    addGroup.add_argument("-q", "--quiet", dest="verbose", action="store_false",
                          default=True,
                          help="Suppresses the printing of verbose information")
    addGroup.add_argument("-T", "--noTables", dest="printTables",
                          action="store_false", default=True,
                          help="Disables the printing of performance tables "
                          "(they will still be written to a TFile)")

    addGroup.add_argument("-l", "--latexTables", dest="latexTables",
                          action="store_true", default=False,
                          help="print LaTeX-format tables (rather than ASCII)")
    addGroup.add_argument("-t", "--tableFilename", dest="tabFile", metavar="FILE",
                          help="if this option is used, the tables will printed "
                          "to the specified FILE rather than being printed to the screen")
    addGroup.add_argument("-M", "--allow-missing", dest="allowMissing",
                          action="store_true", default=False,
                          help="Allow missing calibration subsamples. "
                          "N.B. You should only use this option if requested to "
                          "do so by the PIDCalib authors")
    addGroup.add_argument("-P", "--printFreq", dest="printFreq", metavar="NUM",
                          type=int, default=500,
                          help=("Frequency with which to print information about "
                          "reference sample entries (only used when printing "
                          "verbose information, default: %(default)s)"))

    expGroup = parser.add_argument_group("EXPERIMENTAL options",
                                         "The options are experimental, and "
                                         "are still being tested."
                                         "Use at your own risk!")
    expGroup.add_argument("-w", "--use-sWeights", dest="use_sWeights",
                          action="store_true", default=False,
                          help=("this option should be included if the "
                                "reference sample contains sWeights. "
                                "WARNING: This option is experimental, and still being tested. "
                                "Use at you own risk!"))
    expGroup.add_argument("-W", "--sWeightVarName", dest="sWeightVarName",
                          metavar="NAME", default="nsig_sw",
                          help=("Name of the 'sWeights' variable in the reference "
                                "sample (default: %(default)s)"))

    opts = parser.parse_args()

    StripVersion = None
    MagPolarity = None
    PartName = None
    DLLCuts = []
    RunMin = None
    RunMax = None

    # set the stripping version
    StripVersion=opts.stripVersion
    CheckStripVer(StripVersion)

    # set the magnet polarity
    MagPolarity=opts.magPol
    CheckMagPol(MagPolarity)

    # set the particle name
    PartName=opts.partName
    CheckPartType(PartName)

    FoM = opts.fom.lower()
    FoMs = ["punzi","sqrtsoverb","soversqrtsplusb"]
    if FoM not in FoMs:
        parser.error("Figure of merit ('%s') is not a known figure of merit. "
                     "Known FoMs are: %s."%(FoM,str(FoMs)))

    FoM_Scale = opts.fomScale
    if FoM_Scale is None and FoM not in ["punzi"]:
        parser.error(
            "Figure of merit scale is not set. "
            "Set this to an estimate of the numer signal events, "
            "it's needed to calculate the figure of merit." )
    elif FoM not in ["punzi"]:
        try:
            FoM_Scale = float(FoM_Scale)
        except ValueError:
            parser.error(
                "Figure of merit scale ('%s') is not a number." %CutSteps)

    CutSteps = opts.pidSteps
    CutStart = opts.pidStart
    CutStop = opts.pidStop

    if CutSteps is not None:
        try:
            CutSteps = int(CutSteps)
        except ValueError:
            parser.error(
                "Number of steps ('%s') is not an integer." %CutSteps)

    if CutStart is not None:
        try:
            CutStart = float(CutStart)
        except ValueError:
            parser.error(
                "Lower limit of PID cut ('%s') is not numeric." %CutStart)

    if CutStop is not None:
        try:
            CutStop = float(CutStop)
        except ValueError:
            parser.error(
                "Upper limit of PID cut ('%s') is not numeric." %CutStop)

    if opts.pidSign not in ["<",">"]:
        parser.error("Cut direction ('%s') is not > or <." %opts.pidSign)

    ## set the PID cuts
    CutVals = [CutStart + (step)*(CutStop-CutStart)/(float(CutSteps)-1.) for step in xrange(CutSteps)]
    DLLCuts = []
    for val in CutVals:
        cut = "{var}{sign}{val}".format(var=opts.pidVar, sign=opts.pidSign, val=val)
        DLLCuts.append(cut)

    RunMin = opts.runMin
    RunMax = opts.runMax
    MaxFiles = opts.maxFiles

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

    if MaxFiles is not None:
        try:
            int(MaxFiles)
        except ValueError:
            parser.error(
                "Argument to --maxFiles ('%s') is not an integer'." %MaxFiles)
        if MaxFiles is None:
            parser.error(
                "Max files was specified as %s, but no min run was given." %MaxFiles)


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
            msg="Failed to load binning scheme file '{0}'".format(
                opts.binSchemeFile)
            raise IOError(msg)
    ROOT.gSystem.Load('libRooStats.so')
    ROOT.gSystem.Load('libCintex.so')
    cintex=ROOT.Cintex
    cintex.Enable()
    X_Bin = GetBinScheme(PartName, XVarName, SchemeName)
    Y_Bin = None
    if YVarName!='':
        Y_Bin = GetBinScheme(PartName, YVarName, SchemeName)
    Z_Bin = None
    if ZVarName!='':
        Z_Bin = GetBinScheme(PartName, ZVarName, SchemeName)

    BinSchema = ROOT.std.vector('RooBinning*')()
    BinSchema.push_back(X_Bin)
    if Y_Bin is not None:
        BinSchema.push_back(Y_Bin)
    if Z_Bin is not None:
        BinSchema.push_back(Z_Bin)

    #======================================================================
    # Check DLL Cuts etc.
    #======================================================================
    if opts.verbose: 
        print '========== Requested data samples =========='
        print "Stripping version: %s" %StripVersion
        print "Magnet polarity: %s" %MagPolarity
        print "Particle name: %s" %PartName
        if len(opts.cuts)>0:
            print "Initial cuts: %s" %opts.cuts
        for icut, cut in enumerate(DLLCuts):
            print "PID cut %d: %s" %(icut,cut)
        print "Using reference branch %s from tree %s:%s" %(opts.refBranch,opts.refFilename,opts.refTree)
        print "Using background branch %s from tree %s:%s" %(opts.refBranch,opts.refFilename,opts.refTree)
        print '===================================='
        
    #=============================================================================
    # Declare Binning Schema (RooBinnings)
    #=============================================================================

    BinSchema = ROOT.vector('RooBinning*')()
    BinVarNames = []

    BinSchema.push_back(X_Bin)
    BinVarNames.append(XVarName)

    if Y_Bin is not None:
      BinSchema.push_back(Y_Bin)
      BinVarNames.append(YVarName)

    if Z_Bin is not None:
      BinSchema.push_back(Z_Bin)
      BinVarNames.append(ZVarName)

    if opts.verbose:
        print('========== Binning Schema ==========')
        if SchemeName is None:
            print "Default binning schema"
        else:
            print "Using binning schema '{0}'".format(SchemeName)
        for ibinning in range(len(BinSchema)):
            msg="------ Var name '{0}'".format(BinVarNames[ibinning])
            msg+= '------'
            print msg
            BinSchema[ibinning].Print()
        print('====================================')

    #=============================================================================
    # Source backround TTree
    #=============================================================================
    fname_Bkg = opts.bkgFilename
    tname_Bkg = opts.bkgTree

    f_Bkg = ROOT.TFile.Open(fname_Bkg)
    if not f_Bkg:
        raise IOError("Failed to open background file {0} for reading".format(fname_Bkg))
    t_Bkg_orig = f_Bkg.Get(tname_Bkg)
    if not t_Bkg_orig:
        msg="Failed to retrieve background TTree {tname} from file {fname}".format(
            tname=tname_Bkg, fname=fname_Bkg)
        raise ValueError(msg)

    #=============================================================================
    # Source reference sample TTree
    #=============================================================================
    fname_Ref = opts.refFilename
    tname_Ref = opts.refTree

    f_Ref = ROOT.TFile.Open(fname_Ref)
    if not f_Ref:
        raise IOError("Failed to open reference file {0} for reading".format(fname_Ref))
    t_Ref_orig = f_Ref.Get(tname_Ref)
    if not t_Ref_orig:
        msg="Failed to retrieve reference TTree {tname} from file {fname}".format(
            tname=tname_Ref, fname=fname_Ref)
        raise ValueError(msg)

    #=============================================================================
    # Apply cuts to background sample TTree
    #=============================================================================
    tmp_file = ROOT.TFile.Open("tmp_file.root","RECREATE")
    t_Bkg = None
    if opts.bkgCuts not in [None, ""]:
        if not CheckCutVarsInTree(opts.bkgCuts,t_Bkg_orig):
            msg="Background cuts \"{cuts}\" contain a variable not in the TTree {fname}:{tname}.".format(
                cuts=opts.bkgCuts, tname=tname_Bkg, fname=fname_Bkg)
            raise ValueError(msg)
        t_Bkg = t_Bkg_orig.CopyTree(opts.bkgCuts)
    else:
        t_Bkg = t_Bkg_orig

    if not t_Bkg:
        msg="Failed to reduce reference TTree {tname} from file {fname}".format(
            tname=tname_Bkg, fname=fname_Bkg)
        raise ValueError(msg)

    print "Background TTree {fname}:{tname} opened. {nbefore} entries before cuts \"{cuts}\" and {nafter} afterwards.".format(tname=tname_Bkg, fname=fname_Bkg, nbefore=t_Bkg_orig.GetEntries(), nafter=t_Bkg.GetEntries(),cuts=opts.bkgCuts)

    #=============================================================================
    # Apply cuts to reference sample TTree
    #=============================================================================
    t_Ref = None
    if opts.refCuts not in [None, ""]:
        if not CheckCutVarsInTree(opts.refCuts,t_Ref_orig):
            msg="Reference cuts \"{cuts}\" contain a variable not in the TTree {fname}:{tname}.".format(
                cuts=opts.refCuts, tname=tname_Bkg, fname=fname_Bkg)
            raise ValueError(msg)
        t_Ref = t_Ref_orig.CopyTree(opts.refCuts)
    else:
        t_Ref = t_Ref_orig

    if not t_Ref:
        msg="Failed to reduce reference TTree {tname} from file {fname}".format(
            tname=tname_Ref, fname=fname_Ref)
        raise ValueError(msg)

    print "Reference TTree {fname}:{tname} opened. {nbefore} entries before cuts \"{cuts}\" and {nafter} afterwards.".format(tname=tname_Ref, fname=fname_Ref, nbefore=t_Ref_orig.GetEntries(), nafter=t_Ref.GetEntries(),cuts=opts.refCuts)


    #======================================================================
    # Final list of plots
    #======================================================================
    Plots = GetPerfPlotList(MakePerfPlotsList,
                            StripVersion,
                            MagPolarity,
                            PartName,
                            DLLCuts,
                            opts.cuts,
                            BinSchema,
                            RunMin,
                            RunMax,
                            opts.verbose,
                            opts.allowMissing,
                            MaxFiles)

    #======================================================================
    # Make Weighted Average
    #======================================================================
    MakeAveragePlotList(Plots)
    if opts.verbose:
        print 'Plots:', Plots

    #=============================================================================
    # Create instance of MultiTrackCalibTool
    #=============================================================================
    ROOT.gSystem.Load('libCintex.so')
    cintex=ROOT.Cintex
    cintex.Enable()
    ROOT.gSystem.Load('libPIDPerfToolsLib.so')
    ROOT.gSystem.Load('libPIDPerfToolsDict.so')

    BkgGraph = ROOT.TGraphErrors(CutSteps)
    EffGraph = ROOT.TGraphErrors(CutSteps)
    FoMGraph = ROOT.TGraphErrors(CutSteps)
    CalibTools = []

    for CutNumber,(CutVal) in enumerate(CutVals):

        PidCut = "{var}{sign}{val}".format(var=opts.pidVar, sign=opts.pidSign, val=CutVal)

        #=============================================================================
        # Check PID cut is possible on TTrees
        #=============================================================================
        if not CheckCutVarsInTree(opts.bkgBranch + "_" + PidCut,t_Bkg):
            msg="PID cut \"{cuts}\" contains a variable not in the background TTree {fname}:{tname}.".format(
                cuts=PidCut, tname=tname_Bkg, fname=fname_Bkg)
            raise ValueError(msg)

        if not CheckCutVarsInTree(opts.refBranch + "_" + PidCut,t_Ref):
            msg="PID cut \"{cuts}\" contains a variable not in the reference TTree {fname}:{tname}.".format(
                cuts=PidCut, tname=tname_Ref, fname=fname_Ref)
            raise ValueError(msg)

        print "Looping:",CutNumber,CutVal,PidCut

        #=============================================================================
        # Define the calibration tool
        #=============================================================================
        
        PidCutNameClean = PidCut
        for char in "/*+()[]{}\\'\"@;:?<>,.|!%^&_=": 
          PidCutNameClean = PidCutNameClean.replace(char,"_")
          
        #RefTreeClone = t_Ref.Clone("RefTree_%s"%PidCutNameClean)
        CalibTool =  ROOT.MultiTrackCalibTool("CalibTool_%s"%PidCutNameClean, 
                                              t_Ref, 
                                              "tmp_tree_file.root",
                                              "RECREATE", opts.verbose, opts.printFreq)
        #CalibTool = CalibTools[-1]

        #=============================================================================
        # Declare the name in the reference TTree for the various binning variables
        #=============================================================================
        for vname_calib, vname_ref in zip( (XVarName, YVarName, ZVarName),
                                          (opts.xRefVarName, opts.yRefVarName,
                                            opts.zRefVarName) ):
            if vname_calib=='': continue

            if vname_calib=="P":
                CalibTool.SetTrackMomVarName(vname_ref)

            elif vname_calib=="ETA":
                CalibTool.SetTrackEtaVarName(vname_ref)

            elif vname_calib=="nTracks":
                CalibTool.SetNTracksVarName(vname_ref)

            else:
                CalibTool.SetTrackPtVarName(vname_ref)

        #=============================================================================
        # Declare the name in the reference TTree for the sWeight variable
        #=============================================================================
        if opts.use_sWeights:
            CalibTool.SetSWeightVarName(opts.sWeightVarName)#'nsig_sw')

        #=============================================================================
        # Obtain Performance histogram for each track under consideration,
        # declare the name in the reference TTree of each track to be considered and
        # the PID performance histogram to be associated with it
        #=============================================================================

        #=============================================================================
        # For each reference track, retrieve the performance histogram from last step
        #=============================================================================
        PerfHist = None
        for plotArr in Plots:
            if (PartName+"_"+PidCut+"_All") == plotArr[-1].GetName():
                PerfHist = plotArr[-1]
        if not PerfHist:
            msg="Failed to find the histogram"
            raise ValueError(msg)
        CalibTool.DeclareSignalTrackAndPerfHist(opts.refBranch, PerfHist)
        if opts.verbose:
            msg=("Track name {tname}: Got performance histogram {hname}."
                ).format(tname=opts.refBranch, hname=PerfHist.GetName())
            print msg
            #TODO: what does this ^^ achieve?

        #===========================================================================
        # Ensure the reference sample tracks reside with the bin ranges of the
        # declared PID performance histograms
        #===========================================================================
        print "Checking ref data is in hist perf limits..."
        if opts.setRefLimits:
            CalibTool.SetRefDataInPerfHistLimits()
        print "Ref data is in hist perf limits"

        #===========================================================================
        # Calculate the per track event efficiencies
        #===========================================================================
        print "Using MultiTrackCalibTool to calculate weighted efficiency..."
        CalibTool.Calculate()

        #===========================================================================
        # Calculate the naive average event efficiency and associated error
        #===========================================================================
        NaiveResult = CalibTool.CalculateNaiveAverage();
        print 'Naive event efficiency and error: ({eff:.3f} +/- {err:.3f})%'.format(
            eff=100*NaiveResult.first, err=100*NaiveResult.second)

        if opts.use_sWeights:
          NaiveResult = CalibTool.CalculateNaiveWeightAverage();
          print 'Naive event efficiency and error (weighted): ({eff:.3f} +/- {err:.3f})%'.format(
              eff=100*NaiveResult.first, err=100*NaiveResult.second)

        eff = NaiveResult.first
        eff_err = NaiveResult.second

        bkgCuts = ""
        if opts.bkgCuts not in ["", None]:
          bkgCuts = opts.bkgCuts + "&&"

        bkgCuts += opts.bkgBranch + "_" + PidCut

        bkg = float(t_Bkg.GetEntries(bkgCuts))
        bkg_err = sqrt(bkg)


        print 'Selected background events: {eff} +/- {err}'.format(
            eff=int(bkg), err=int(round(bkg_err)))


        fom = 0.
        fom_err = 0.

        if FoM == "punzi":
          sigma_over_two = 3./2.
          fom =  eff / (sigma_over_two + sqrt(bkg))
          if bkg>0:
            fom_err = sqrt( (eff_err/(sqrt(bkg) + sigma_over_two))**2.
                              + (sqrt(bkg)*(-eff)/(2.*sqrt(bkg)*(sqrt(bkg)+sigma_over_two)**2.))**2. )
        elif FoM == "sqrtsoverb":
          raise ValueError("FoM: sqrtsoverb no implemented")
        elif FoM == "soversqrtsplusb":
          es = eff*FoM_Scale
          s = FoM_Scale
          fom = es / sqrt(es + bkg)
          fom_err = sqrt((((eff_err*s)**2*(2.*bkg+es)**2)/(4.*(bkg+es)**3)) + (((bkg_err*es)**2)/(4.*(bkg+es)**3)))
          #fom_err = ((bkg*eff_err*FoM_Scale)**2 + bkg**3)/((bkg + eff*FoM_Scale)**4)

        print 'FoM: {eff:.3f} +/- {err:.3f}'.format(
            eff=fom, err=fom_err)

        EffGraph.SetPoint(CutNumber, CutVal, eff)
        EffGraph.SetPointError(CutNumber, 0., eff_err)

        BkgGraph.SetPoint(CutNumber, CutVal, bkg)
        BkgGraph.SetPointError(CutNumber, 0., bkg_err)

        FoMGraph.SetPoint(CutNumber, CutVal, fom)
        FoMGraph.SetPointError(CutNumber, 0., fom_err)

        CalibTool.Write("WeightTree_"+PidCutNameClean)
        del CalibTool
        #CalibTool=None
        #getattr(CalibTool,"~MultiTrackCalibTool")()

    f_Ref.Close()
    tmp_file.Close()
    f_Bkg.Close()

    #======================================================================
    # Open file to write TH1Fs to
    #======================================================================

    print "Saving optimisation plot to %s" %opts.outFilename
    f_Out = ROOT.TFile.Open(opts.outFilename, "RECREATE")
    if not f_Out:
        raise IOError("Failed to open file %s for writing" %fname)

    EffGraph.Write("Eff")
    BkgGraph.Write("Bkg")
    FoMGraph.Write("FoM")

    #======================================================================
    # Close file
    #======================================================================
    f_Out.Close()
    print "CutOpt ran successfully!"
