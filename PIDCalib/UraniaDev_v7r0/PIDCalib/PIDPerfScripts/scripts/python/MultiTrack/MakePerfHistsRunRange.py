#!/bin/env python
from PIDPerfScripts.StartScreen import *

import ROOT

import sys
import os.path
#import getopt
#import optparse
#import warnings
import itertools
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
    
    parser = ShowArgumentsParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        prog=os.path.basename(sys.argv[0]),
        description=("""Make performance histograms for a given:
        a) Sample version <sampleVersion> (e.g. \"20\" for Run 1 Stripping 20, \"Turbo16\" for Run 2 WGP)
        b) magnet polarity  <magPol> (\"MagUp\" or \"MagDown\")
        c) particle type <partName> (\"K\", \"P\", \"Pi\", \"e\" or \"Mu\")
        d) PID cut, <pidCut>
Multiple PID cuts can be specified if necessary, e.g. \"[DLLK > 0.0, DLLK > 4.0]\".
In this case, a performance histogram will be produced for each PID cut.

For a full list of arguments, do: 'python {0} -h'

e.g. Run 1: python {0} \"20\" \"MagUp\" \"K\" \\
    \"[DLLK > 0.0, DLLK > 4.0 && DLLp < 0.0]\" -c \"runNumber>=114205 && runNumber<=114287\"
    
e.g Run 2: python {0}  \"Turbo16\" \"MagUp\" \"K\" \\
    \"[DLLK > 0.0, DLLK > 4.0 && DLLp < 0.0]\" -c \"runNumber>=114205 && runNumber<=114287\"

 """).format(os.path.basename(sys.argv[0])),)

    ## add the positional arguments
    parser.add_argument('sampleVersion', metavar='<sampleVersion>',
                        help="Sets the stripping version for Run I data, or the Turbo WGP production version for Run II")
    parser.add_argument('magPol', metavar='<magPol>',
                        help="Sets the magnet polarity")
    parser.add_argument('partName', metavar='<partName>',
                        help="Sets the particle type")
    parser.add_argument('pidCut', metavar='<pidCut>',
                        help="Sets the PID cut(s)")
    
    ## add the optional arguments
    parser.add_argument('-x', '--minRun', dest="minRun", metavar="NUM",
                        help="Sets the minimum run number to process (if applicable, Run 1 only)")
    parser.add_argument('-y', '--maxRun', dest="maxRun", metavar="NUM",
                        help="Sets the maximum run number to process (if applicable, Run 1 only)")
    parser.add_argument('-f', '--maxFiles', dest="maxFiles", metavar="NUM",
                        help="Sets the maximum number of calibration files to run over")
    parser.add_argument('-c', '--cuts', dest='cuts', metavar='CUTS', default='',
                        help="Sets the list of cuts to apply to the calibration "
                        "sample(s) prior to determine the PID efficiencies "
                        "(default: %(default)s). "
                        "NB. It is up to the user to ensure that their reference "
                        "sample has the same cuts applied."
                        )
    
    parser.add_argument('-o', '--outputDir', dest='outputDir', metavar='DIR',
                        help="Save the performance histograms to directory DIR "
                        "(default: current directory)"
                        )
    
    binGroup = parser.add_argument_group("binning options")
    binGroup.add_argument('-X', '--xVarName', dest='xVarName', metavar='NAME', default='P',
                          help="Sets the NAME of the 1st (x) bin variable "
                          "(default: %(default)s)"
                          )
    binGroup.add_argument('-Y', '--yVarName', dest='yVarName', metavar='NAME',
                          default='ETA',
                          help="Sets the NAME of the 2nd (y) bin variable "
                          "(default: %(default)s). "
                          "If 1D binning is required, then this option should "
                          "be set to an empty string"
                          )
    binGroup.add_argument('-Z', '--zVarName', dest='zVarName', metavar='NAME',
                          default='nTracks',
                          help="Sets the NAME of the 3rd (z) bin variable "
                          "(default: %(default)s). "
                          "If 2D binning is required, then this option should "
                          "be set to an empty string"
                          )
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
    addGroup.add_argument("-a", "--old-averaging", dest="oldAveraging",
                          action="store_true", default=False,
                          help="Use an old style averging, which is incorrect "
                          "if the efficiency is close to zero. "
                          "N.B. You should only use this option if requested to "
                          "do so by the PIDCalib authors" )
    addGroup.add_argument("-M", "--allow-missing", dest="allowMissing",
                          action="store_true", default=False,
                          help="Allow missing calibration subsamples. "
                          "N.B. You should only use this option if requested to "
                          "do so by the PIDCalib authors")
   
    opts = parser.parse_args()


    from PIDPerfScripts.Definitions import *


    StripVersion = None
    MagPolarity = None
    PartName = None
    DLLCuts = []
    RunMin = None
    RunMax = None

    # set the sample version (stripping version in Run I, and Turbo version in Run II. Internally called StripVersion)
    StripVersion=opts.sampleVersion
    CheckStripVer(StripVersion)
    
    # set the magnet polarity
    MagPolarity=opts.magPol
    CheckMagPol(MagPolarity)

    # set the particle name
    PartName=opts.partName
    CheckPartType(PartName)
    
    #Additional sample check for pA/Ap samples
    if 'pA' in StripVersion or 'Ap' in StripVersion:
    	CheckStripVerPartNameMagPol(StripVersion,PartName,MagPolarity)
    

    # set the PID cuts
    DLLCuts = opts.pidCut

    
    if DLLCuts.startswith("["):
        if not DLLCuts.endswith("]"):
            parser.error("Invalid DLL cut string %s" %DLLCuts)
        DLLCuts = DLLCuts[1:-1].split(',')
    elif DLLCuts.startswith("("):
        if not DLLCuts.endswith(")"):
            parser.error("Invalid DLL cut string %s" %DLLCuts)
        DLLCuts = DLLCuts[1:-1].split(',')
    else:
        DLLCuts = (DLLCuts,)

    if (len(opts.cuts)>0):
        if isinstance(opts.cuts,str):
            if not CheckCuts(opts.cuts,StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cuts))
        elif isinstance(opts.cuts,list):
            if not CheckCuts(opts.cuts.join(" "),StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cuts))

    RunMin = opts.minRun
    RunMax = opts.maxRun
    MaxFiles = opts.maxFiles
    
    #If running on Run 2 data, do not allow run range to be set
    #if 'Turbo' in StripVersion and RunMin is not None:
    #	parser.error("Cannot set run range for Run 2 data. Please set the run range within a cut using -c option.")
    
    #if 'Turbo' in StripVersion and RunMax is not None:
    #	parser.error("Cannot set run range for Run 2 data. Please set the run range within a cut using -c option.")
    

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
#    ROOT.gSystem.Load('libCintex.so')
#    cintex=ROOT.Cintex
#    cintex.Enable()
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

    for icut, cut in enumerate(DLLCuts):
        if not CheckCuts(cut,StripVersion):
            raise ValueError("Invalid PID cut %i: \"%s\""%(icut,cut))

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
        for icut, cut in enumerate(DLLCuts):
            print "PID cut %d: %s" %(icut,cut)
        print '====================================='

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

    #======================================================================
    # Final list of plots
    #=====================================================================

    from PIDPerfScripts.DataFuncs import *
    from PIDPerfScripts.PerfResults import *
    from PIDPerfScripts.PerfCalcFuncs import *
        
    Plots = GetPerfPlotList(MakePerfPlotsList if opts.oldAveraging else MakePerfPlotsListPyth,
                            StripVersion,
                            MagPolarity,
                            PartName,
                            DLLCuts,
                            opts.cuts,
                            opts.pidCut,    #Added by Donal, use to filter the variables loaded 
                            opts.xVarName,
                            opts.yVarName,
                            opts.zVarName,
                            BinSchema,
                            RunMin,
                            RunMax,
                            opts.verbose,
                            opts.allowMissing)
                            

    #======================================================================
    # Make Weighted Average
    #======================================================================
    MakeAveragePlotList(Plots, opts.oldAveraging)
    if opts.verbose:
        print 'Plots:', Plots

    #======================================================================
    # Open file to write TH1Fs to
    #======================================================================
    fnameSuffix=''
    if SchemeName is not None:
        fnameSuffix+='_{0}'.format(SchemeName)
    for vname in BinVarNames:
      fnameSuffix+='_{0}'.format(vname)
    if 'Turbo' not in StripVersion:
    	fname = "PerfHists_{part}_Strip{strp}_{pol}{suf}.root".format(
        	part=PartName, strp=StripVersion, pol=MagPolarity,
        	suf=fnameSuffix)
    elif 'Turbo' in StripVersion:
    	fname = "PerfHists_{part}_{strp}_{pol}{suf}.root".format(
        	part=PartName, strp=StripVersion, pol=MagPolarity,
        	suf=fnameSuffix)
    
    if opts.outputDir is not None:
        fname = "%s/%s" %(opts.outputDir, fname)

    print "Saving performance histograms to %s" %fname
    f_Out = ROOT.TFile.Open(fname, "RECREATE")
    if not f_Out:
        raise IOError("Failed to open file %s for writing" %fname)

    FlatPlots = FlatternPlots(Plots)
    print "FlatPlots:",FlatPlots
    for iPlot in FlatPlots:
        iPlot.Write()

    #======================================================================
    # Create PIDTable for each PID cut and print to screen
    #======================================================================
 
    #   if opts.printTables:
    print "-----------------"
    print "Printing PIDTable"
    print "-----------------"
    for i in range(len(Plots)):
        plot = Plots[i][-1] if opts.oldAveraging else Plots[i][-1][0]
        print plot

        nDim = BinSchema.size()
        pidTable = None

        if nDim==3:
            print plot, X_Bin, Y_Bin, Z_Bin
            pidTable = ROOT.PIDTable(plot, X_Bin, Y_Bin, Z_Bin)
        elif nDim==2:
            pidTable = ROOT.PIDTable(plot, X_Bin, Y_Bin)
        else:
            pidTable = ROOT.PIDTable(plot, X_Bin)

        tabName=plot.GetName()
        tabHeader = "PID Table for histogram %s" %tabName
        if opts.tabFile is None:
            print tabHeader
            pidTable.PrintTable(ROOT.std.cout, opts.latexTables)
            #else:
            #   appMode = "w" if i==0 else "a"
            #    os = open(opts.tabFile, appMode)
            #    os.write(tabHeader+'\n')
            #    os.close()
            #    os = ROOT.std.fstream(opts.tabFile,
            #            ROOT.std.fstream.out|ROOT.std.fstream.app)
            #    pidTable.PrintTable(os, opts.latexTables)
    #======================================================================
    # Close file
    #======================================================================
    f_Out.Close()
