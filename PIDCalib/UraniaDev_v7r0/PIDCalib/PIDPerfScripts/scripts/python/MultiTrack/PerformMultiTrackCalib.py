#!/bin/env python
from PIDPerfScripts.StartScreen import *
from PIDPerfScripts.Definitions import *
from PIDPerfScripts.DataFuncs import *
from PIDPerfScripts.PerfCalcFuncs import *
from PIDPerfScripts.RunDictFuncs import *

import ROOT
import sys
#import optparse
import argparse
import warnings
import math

class ShowArgumentsParser(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n\n' %message)
        parser.print_usage(sys.stderr)
        sys.stderr.write('\n'+self.description)
        sys.exit(2)


if '__main__' == __name__:

    start()
    print ""

    parser = ShowArgumentsParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        prog=os.path.basename(sys.argv[0]),
        description="""Perform the PID calibration for a given:
        a) stripping version, <stripVersion> (e.g. \"20\")
        b) magnet polarity <magPol> (\"MagUp\" or \"MagDown\")
        c) reference (i.e. signal or control) sample <refTree>,
           located in file <refFilename>

The per-event PID efficiencies will be stored in the ROOT file <outputFilename>.

The remaining positional argument, <track>, specifies the reference track,
and takes the form [<trackName>, <trackType>, <pidCut>], where <trackName>
is the name of the reference track in the TTree, <trackType> is the type of
track and <pidCut> is the PID cut to evaluate for that track.

If the reference sample contains multiple final-state tracks, the remaining tracks
can be specified as additional arguments.

Valid track types are: \"K\", \"P\",\"P_IncLc"\, \"Pi\", \"e\" or \"Mu\".

For a full list of arguments, do: 'python {0} -h'

e.g. python {0} \"20\" \"MagUp\" \"$HOME/MyAnalysis/MySignalSample.root\" \"SignalTree\" \\
    \"MyPIDResults.root\" \"[Kaon,K,DLLK>4.0]\" \"[Pion,Pi,DLLK<4.0]\"


""".format(os.path.basename(sys.argv[0]))
        )

    ## add the positional arguments
    parser.add_argument('stripVersion', metavar='<stripVersion>',
                        help="Sets the stripping version")
    parser.add_argument('magPol', metavar='<magPol>',
                        help="Sets the magnet polarity")
    parser.add_argument('refFilename', metavar='<refFileName>',
                        help="Sets the name of the file containing the reference TTree")
    parser.add_argument('refTree', metavar='<refTree>',
                        help="Sets the name of the reference TTree")
    parser.add_argument('outputFilename', metavar='<outputFilename>',
                        help="Sets the name of the output file")
    parser.add_argument('tracks', metavar='<track>', nargs='+',
                        help="Sets the name of one of the reference tracks, "
                        "its track type and the PID cut to apply")

    ## add the optional arguments

    parser.add_argument("-i", "--inputDir", dest="inputDir", metavar="DIR",
                        help=("directory containing the performance histograms "
                        "(default: current directory)"))

    binGroup = parser.add_argument_group("binning options")
    binGroup.add_argument("-X", "--xVarName", dest="xVarName", metavar="NAME",
                          default="P",
                          help=("Sets the NAME of the 1st (x) bin variable "
                          "in the calibration sample "
                          "(default: %(default)s)"))
    binGroup.add_argument("-Y", "--yVarName", dest="yVarName", metavar="NAME",
                          default="ETA",
                          help=("Sets the NAME of the 2nd (y) bin variable "
                          "in the calibration sample "
                          "(default: %(default)s). "
                          "If 1D binning is required, then this option should "
                          "be set to an empty string"))
    binGroup.add_argument("-Z", "--zVarName", dest="zVarName", metavar="NAME",
                          default="nTracks",
                          help=("Sets the NAME of the 3rd (z) bin variable "
                          "in the calibration sample "
                          "(default: %(default)s). "
                          "If 2D binning is required, then this option should "
                          "be set to an empty string"))
    binGroup.add_argument("-s", "--schemeName", dest="schemeNames", nargs=2,
                          action='append',
                          metavar=("TYPE", "NAME"),
                          help=("Sets the NAME of the binning scheme for particles "
                          "of type TYPE, as defined in the module "
                          "'PIDPerfScripts.binning'. This option can be used "
                          "multiple times to specify the binning scheme for "
                          "each particle type in the reference sample"
                          "If this option is not set for a particular particle "
                          "type, the default binning scheme is used."))

    addGroup = parser.add_argument_group("further options")
    addGroup.add_argument("-q", "--quiet", dest="verbose", action="store_false",
                          default=True,
                          help="suppresses the printing of verbose information")
    addGroup.add_argument("-N", "--noBinLimitCheck", dest="setRefLimits",
                          action="store_false", default=True,
                          help=("Don't remove events in the reference sample "
                          "that are outside the bin range of the performance "
                          "histograms (will produce an exception if there are "
                          "events outside the bin range)"))
    addGroup.add_argument("-x", "--xRefVarName", dest="xRefVarName", metavar="NAME",
                          default="P",
                          help=("Sets the NAME of the 1st (x) bin variable "
                          "in the reference sample (default: %(default)s)"))
    addGroup.add_argument("-y", "--yRefVarName", dest="yRefVarName", metavar="NAME",
                          default="TRACK_Eta",
                          help=("Sets the NAME of the 2nd (y) bin variable "
                          "in the reference sample (default: %(default)s)"))
    addGroup.add_argument("-z", "--zRefVarName", dest="zRefVarName",
                          metavar="NAME", default="nTracks",
                          help=("Sets the NAME of the 3rd (z) bin variable "
                          "in the reference sample (default: %(default)s)"))
    addGroup.add_argument("-P", "--printFreq", dest="printFreq", metavar="NUM",
                          type=int, default=100,
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

    StripVersion=opts.stripVersion
    CheckStripVer(StripVersion)

    MagPolarity=opts.magPol
    CheckMagPol(MagPolarity)

    fname_Ref = opts.refFilename
    tname_Ref = opts.refTree
    fname_out = opts.outputFilename

    trackArgs = opts.tracks

    trackList = []
    trackTypes = []
    for arg in trackArgs:
        trackName = None
        trackType = None
        DLLCut = None
        if not arg.startswith("[") or not arg.endswith("]"):
            parser.error("Invalid track variable string '{0}'".format(arg))
        trackInfo=arg[1:-1].split(",")
        if len(trackInfo) != 3:
            parser.error("Invalid track variable string '{0}'".format(arg))
        trackName = trackInfo[0].strip()
        trackType = trackInfo[1].strip()
        DLLCut = trackInfo[2].strip().replace("/", "_div_")

        CheckPartType(trackType)

        if trackType not in trackTypes:
            trackTypes.append(trackType)
        trackList.append((trackName,trackType,DLLCut))

    XVarName = opts.xVarName
    if XVarName=='':
        parser.error("Argument to --xBinVarName is an empty string.")

    YVarName = opts.yVarName
    ZVarName = opts.zVarName

    if ZVarName!='' and YVarName=='':
        msg=("Argument to --yVarName is an empty string,"
             "but argument to --zVarName. If you planned to"
             "include only two binning variables. Did you"
             "mean to do --yVarName='{0}' --zVarName='{1}'?").format(
            ZVarName, YVarName)
        parser.error(msg)

    for vname_calib in (XVarName, YVarName, ZVarName):
      if vname_calib=='': continue
      CheckVarName(vname_calib)

    SchemeNames = {} if opts.schemeNames is None else dict(opts.schemeNames)

    if opts.use_sWeights:
        warnings.warn(("WARNING: The '--use-sWeights' option is experimental, and "
                       "is still being tested. Use at you own risk!"))
    if opts.verbose:
        print '===================================='
        print "Stripping version: %s" %StripVersion
        print "Magnet polarity: %s" %MagPolarity
        print "Reference sample (filename): {tname} ({fname})".format(
            tname=tname_Ref, fname=fname_Ref)
        print "Output filename: {0}".format(fname_out)
        for i, v in enumerate(trackList):
            print "Track name, type and PID cut (track {num}): {trInfo}".format(
                num=i, trInfo=str(v))
        print '===================================='

    #=============================================================================
    # Source reference sample TTree
    #=============================================================================
    #ROOT.gSystem.Load('libRooStats.so')

    ##  f_Ref = ROOT.TFile.Open('$PIDPERFSCRIPTSROOT/scripts/root/'
    ##                             'B2DX/Ref_K0SPiPi_DD_S20_DOWN.root')

    ##     f_Ref = ROOT.TFile.Open('$HOME/CalibTests/Ref_K0SPiPi_DD_S20_DOWN.root')
    ##     t_Ref = f_Ref.Get('TTT')
    f_Ref = ROOT.TFile.Open(fname_Ref)
    if not f_Ref:
        raise IOError("Failed to open file {0} for reading".format(fname_Ref))
    t_Ref = f_Ref.Get(tname_Ref)
    if not t_Ref:
        msg="Failed to retrieve TTree {tname} from file {fname}".format(
            tname=tname_Ref, fname=fname_Ref)
        raise ValueError(msg)

    #=============================================================================
    # Create instance of MultiTrackCalibTool
    #=============================================================================
    #ROOT.gSystem.Load('libCintex.so')
    #cintex=ROOT.Cintex
    #cintex.Enable()
    ROOT.gSystem.Load('libPIDPerfToolsLib.so')
    ROOT.gSystem.Load('libPIDPerfToolsDict.so')

    CalibTool = ROOT.MultiTrackCalibTool("CalibTool", t_Ref, fname_out, "RECREATE",
                                         opts.verbose, opts.printFreq)

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
            
        elif vname_calib=="nSPDHits":
            CalibTool.SetNSPDHitsVarName(vname_ref)

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
    #
    # Open files containing performance histograms for each track type to consider
    #
    perfFiles = {}
    for trackType in trackTypes:
        fnameSuffix=''
        if trackType in SchemeNames:
            fnameSuffix+='_{0}'.format(SchemeNames[trackType])
        for vname in (XVarName, YVarName, ZVarName):
            if vname=='': continue
            fnameSuffix+='_{0}'.format(vname)
        if 'Turbo' not in StripVersion:
        	fname = "PerfHists_{part}_Strip{strp}_{pol}{suf}.root".format(
            	part=trackType, strp=StripVersion, pol=MagPolarity, suf=fnameSuffix)
        elif 'Turbo' in StripVersion:
        	fname = "PerfHists_{part}_{strp}_{pol}{suf}.root".format(
            	part=trackType, strp=StripVersion, pol=MagPolarity, suf=fnameSuffix)
        if opts.inputDir is not None:
            fname = "%s/%s" %(opts.inputDir, fname)

        f_Perf = ROOT.TFile.Open(fname)
        if not f_Perf:
            raise IOError("Failed to open file {0} for reading".format(fname))
            #del CalibTool
        perfFiles[trackType]=f_Perf

    #
    # For each reference track, retrieve the performance histogram
    #
    for trackName, trackType, DLLCut in trackList:
        f_Perf = perfFiles[trackType]
        # need to add particle name to DLL cut apparently
        histname="%s_%s_All" %(trackType, DLLCut)
        PerfHist=f_Perf.Get(histname)
        
        if not PerfHist:
            msg="Failed to retrieve histogram {hname} from file {fname}".format(
                hname=histname, fname=f_Perf.GetName())
            raise ValueError(msg)
            #del CalibTool
        CalibTool.DeclareSignalTrackAndPerfHist(trackName, PerfHist)
        if opts.verbose:
          msg=("Track name {tname}: Got performance histogram {hname} "
               "from file {fname}").format(
              tname=trackName, hname=PerfHist.GetName(), fname=f_Perf.GetName())
          print msg

    #===========================================================================
    # Ensure the reference sample tracks reside with the bin ranges of the
    # declared PID performance histograms
    #===========================================================================
    
    if opts.setRefLimits:
        print "setting reference track limits "
        CalibTool.SetRefDataInPerfHistLimits()

    #===========================================================================
    # Calculate the per track event efficiencies
    #===========================================================================
    print "Calculating per track event efficiencies"
    CalibTool.Calculate()

    #===========================================================================
    # Calculate the naive average event efficiency and associated error
    #===========================================================================
    NaiveResult = CalibTool.CalculateNaiveAverage();
    dp = int(math.floor(math.log(100*NaiveResult.second,10))-1)
    dp = -1 if dp > -1 else dp
    print ('Naive event efficiency and error: ({eff:.%if} +/- {err:.%if})%%'%(abs(dp),abs(dp))).format(
        eff=100*NaiveResult.first, err=100*NaiveResult.second)
    print (' The printed error is pointless. You need to consider the full statistical and systematic uncertainties. Look on the twiki page for ideas')

    if opts.use_sWeights:
      NaiveResultWeight = CalibTool.CalculateNaiveWeightAverage();
      print ('Naive event efficiency and error (weighted): ({eff:.%if} +/- {err:.%if})%%'%(abs(dp),abs(dp))).format(
          eff=100*NaiveResultWeight.first, err=100*NaiveResultWeight.second)

    CalibTool.Write("CalibTool_PIDCalibTree")
    #del CalibTool
    CalibTool=None

