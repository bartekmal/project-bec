#!/usr/bin/env python

from PIDPerfScripts.StartScreen import *

import argparse
import sys
import os

class ShowArgumentsParser(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n\n' %message)
        parser.print_usage(sys.stderr)
        sys.stderr.write('\n'+self.description)
        sys.exit(2)

if __name__=='__main__':
    start()
    print ""

    parser = ShowArgumentsParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        prog=os.path.basename(sys.argv[0]),
        description=("""Plot the distribution of a variable from the PID Calib datasets, e.g. """
        """momentum, for a given:
        a) Stripping version <stripVersion> (e.g. \"20\")
        b) magnet polarity  <magPol> (\"MagUp\" or \"MagDown\")
        c) particle type <partName> (\"K\", \"P\", \"Pi\", \"e\" or \"Mu\")
        d) Number of bins required
        e) PID Cut
        f) Name of output scheme

For a full list of arguments, do: 'python {0} -h'
e.g. python {0}  --minRun=114205 --maxRun=114287 \"20\" \"MagUp\" \"K\" DLLK
This script provides an example binning. It is not optimised for your analysis - it simply gives you a possible starting point


""").format(os.path.basename(sys.argv[0])),
       
        )

    ## add the positional arguments
    parser.add_argument('stripVersion', metavar='<stripVersion>',
                        help="Sets the stripping version")
    parser.add_argument('magPol', metavar='<magPol>',
                        help="Sets the magnet polarity")
    parser.add_argument('partName', metavar='<partName>',
                        help="Sets the particle type")
    #parser.add_argument('misIDPartName', metavar='<misIDPartName>',
                        #help="Sets the misidentified particle type")
    parser.add_argument('nBins', metavar='<nBins>', type=int,
                        help="Number of bins in each dimension")
    parser.add_argument('pidCut', metavar='<pidCut>',
                        help="Set the PID cut")
    parser.add_argument('outputSchemeName', metavar='<outputSchemeName>',
                        help="The name of the scheme which is printed")

    ## add the optional arguments
    parser.add_argument('-e', '--dont-split-edge-bins', dest="splitEdgeBins",
                        action="store_false", default=True,
                        help="Don't split the first and last bins in each "
                        "dimension into two seperate bins")
    parser.add_argument('-x', '--minRun', dest="runMin", metavar="NUM",
                        help="Sets the minimum run number to process (if applicable)")
    parser.add_argument('-y', '--maxRun', dest="runMax", metavar="NUM",
                        help="Sets the maximum run number to process (if applicable)")
    parser.add_argument('-f', '--maxFiles', dest="maxFiles", metavar="NUM",
                        help="Sets the maximum number of calibration files to run over")
    parser.add_argument('-c', '--cuts', dest='cuts', metavar='CUTS', default='',
                        help=("Sets the list of cuts to apply to the calibration "
                              "sample(s) prior to determine the PID efficiencies "
                              "(default: (default)s)"))
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
                          default="highres",
                          help="Sets the NAME of the binning scheme, "
                          "as defined in the module 'PIDPerfScripts.binning'. "
                          "If this option is not set, the default "
                          "highres binning scheme is used.")

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
    from PIDPerfScripts.Exceptions import *
    from PIDPerfScripts.RunDictFuncs import *
    from PIDPerfScripts.PerfResults import *
    from PIDPerfScripts.PlotFuncs import CalculateBinning
    from ROOT import TCanvas, TLine, RooBinning

    StripVersion = None
    MagPolarity = None
    PartName = None
    #MisIDPartName = None
    PlotVar = None

    PIDCuts = []
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

    TriggerList = []
#    if (len(opts.triggerList)>0):
#        if isinstance(opts.triggerList,str):
#            TriggerList = [opts.triggerList]
#        elif isinstance(opts.triggerList,list):
#            TriggerList = opts.triggerList
#        if opts.verbose:
#            print "Triggers to load:", TriggerList

    # set the misID particle name
    #MisIDPartName=opts.misIDPartName
    #CheckPartType(MisIDPartName)

    # set the plot variable
    #PlotVar=opts.varName
    #if not CheckCuts(opts.varName,TriggerList):
        #parser.error("Invalid variable %s" %str(opts.varName))

    if (len(opts.cuts)>0):
        if isinstance(opts.cuts,str):
            if not CheckCuts(opts.cuts,TriggerList,StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cuts))
        elif isinstance(opts.cuts,list):
            if not CheckCuts(opts.cuts.join(" "),TriggerList,StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cuts))

    #CheckBinVarName(PlotVar)

    #set the PID cuts
    PIDCut = opts.pidCut

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
    else:
        MaxFiles = -1

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

    for vname_calib in (XVarName, YVarName, ZVarName):
        if vname_calib=='': continue
        CheckVarName(vname_calib)

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
    ROOT.gROOT.SetBatch(True)
    ROOT.gROOT.ProcessLine(".x lhcbstyle.C")
    cintex=ROOT.Cintex
    cintex.Enable()

    Dimensions = [XVarName]

    X_Bin = GetBinScheme(PartName, XVarName, SchemeName)
    Y_Bin = None
    if YVarName!='':
        Dimensions.append(YVarName)
        Y_Bin = GetBinScheme(PartName, YVarName, SchemeName)
    Z_Bin = None
    if ZVarName!='':
        Dimensions.append(ZVarName)
        Z_Bin = GetBinScheme(PartName, ZVarName, SchemeName)


    BinSchema = ROOT.std.vector('RooBinning*')()
    BinSchema.push_back(X_Bin)
    if Y_Bin is not None:
        BinSchema.push_back(Y_Bin)
    if Z_Bin is not None:
        BinSchema.push_back(Z_Bin)

    if not CheckCuts(PIDCut, TriggerList,StripVersion):
        raise ValueError("Invalid PID cut: \"%s\""%(PIDCut))

    #======================================================================
    # Check DLL Cuts etc.
    #======================================================================
    if opts.verbose:
        print '========= Requested data samples ==========='
        print "Stripping version: %s" %StripVersion
        print "Magnet polarity: %s" %MagPolarity
        print "Particle name: %s" %PartName
        if len(opts.cuts)>0:
            print "Initial cuts: %s" %opts.cuts
        #print "PID cut %d: %s" %(PIDCut)
        print '============================================'


    Plots=PlotVarsAndReturn(
              PartName, Dimensions, StripVersion, MagPolarity, RunMin, RunMax,
              opts.cuts, opts.verbose, opts.allowMissing, SchemeName,
              MaxFiles, TriggerList
            )

    tc = TCanvas("tc","tc",800,600)
    BinSchemas = {}
    BinningArray = []
    Points = {}
    ExtraPoints = {}
    for i,(var) in enumerate(Dimensions):
        Plots[var][-1].GetXaxis().SetTitle(var)
        Plots[var][-1].GetYaxis().SetTitle("Arb. Scale")
        Plots[var][-1].Scale(1./Plots[var][-1].GetMaximum())
        Plots[var][-1].GetYaxis().SetRangeUser(0.,1.1)

        Points[var] = CalculateBinning(Plots[var][-1], opts.nBins)
        ExtraPoints[var] = []

        if opts.splitEdgeBins:
            DoublePoints = CalculateBinning(Plots[var][-1], 2*opts.nBins)
            ExtraPoints[var] = [DoublePoints[0],DoublePoints[-1]]

        BinSchemas[var] = ROOT.std.vector('RooBinning*')()
        BinningArray.append(RooBinning(BinSchema.at(i).lowBound(), BinSchema.at(i).highBound(), var))

        for p in Points[var]+ExtraPoints[var]:
            BinningArray[-1].addBoundary(p)
            #print "AddBinBoundary(trType, '%s', '%s', %f)"%(var,opts.outputSchemeName,p)

        print BinningArray[-1]
        BinSchemas[var].push_back(BinningArray[-1])

        Dict = {}
        Dict[var] =  {
            'TrackType' : PartName,
            'DLLCuts'   : (PIDCut,),
            'Binning'   : BinSchemas[var],
            'TrackCuts' : opts.cuts
            }

        EffPlots = CalcAveragePerfPlotsAndReturn(Dict,
                                       StripVersion,
                                       MagPolarity,
                                       RunMin,
                                       RunMax,
                                       opts.verbose,
                                       opts.allowMissing,
                                       MaxFiles,
                                       TriggerList
                                       )

        print EffPlots

        tlines = []
        Plots[var][-1].SetLineStyle(2)
        Plots[var][-1].Draw("HIST")
        Plots[var][-1].GetYaxis().SetRangeUser(0.,1.1)
        EffPlots[var][0][-1][0].SetMarkerColor(ROOT.kRed)
        EffPlots[var][0][-1][0].Draw("SAME")

        for PointList,colour in [(Points[var],ROOT.kRed),(ExtraPoints[var], ROOT.kRed+2)]:
            for p in PointList:
                tlines.append(TLine(p,1.1,p,1.0))
                tlines[-1].SetLineColor(colour)
                tlines[-1].Draw()
                print "AddBinBoundary(trType, '%s', '%s', %f)"%(var,opts.outputSchemeName,p)

        tc.SaveAs("BinningScheme_%s_%s_%s_%s.pdf"%(var, StripVersion, MagPolarity, PIDCut))
        tc.SaveAs("BinningScheme_%s_%s_%s_%s.png"%(var, StripVersion, MagPolarity, PIDCut))

