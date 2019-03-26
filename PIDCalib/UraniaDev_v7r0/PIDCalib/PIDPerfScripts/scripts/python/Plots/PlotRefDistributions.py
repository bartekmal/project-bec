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
        description=("""Plot the distribution of a variable from a given reference sample using the PID Calib binning scheme, """
        """for a given:
        a) Particle type <partName> (\"K\", \"P\", \"Pi\", \"e\" or \"Mu\")
        b) Variable e.g P
a & b are used to define the binning scheme used
        c) <your file name> <your ref tree> <your branch name>
        

For a full list of arguments, do: 'python {0} -h'
e.g. python PlotRefDistributions.py "P_IncLc" "P" "PID_Modes.root" "IncLc2PKPiTuple/CalibPID" "Lamc"
""")
)

    ## add the positional arguments
    parser.add_argument('partName', metavar='<partName>',
                        help="Sets the particle type")
    parser.add_argument('varName', metavar='<varName>',
                        help="Variable to plot")
    parser.add_argument('refFilename', metavar='<refFileName>',
                        help="Sets the name of the file containing the reference TTree")
    parser.add_argument('refTree', metavar='<refTree>',
                        help="Sets the name of the reference TTree")
    parser.add_argument('refBranch', metavar='<refBranch>',
                        help="Sets the name of the particle in the reference TTree")

    ## add the optional arguments
    parser.add_argument("-n", "--refVarName", dest="refVarName", metavar="NAME",
                        help=("Sets the NAME of the variable "
                        "in the calibration sample."))
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
    binGroup.add_argument("-s", "--schemeName", dest="schemeName", metavar="NAME",
                          default=None,
                          help="Sets the NAME of the binning scheme, "
                          "as defined in the module 'PIDPerfScripts.binning'. "
                          "If this option is not set, the default 'PerfPlots'"
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

    #addGroup.add_argument("-L", "--loadTriggers", dest="triggerList",
                          #metavar="TRIGGERS", action="append", default=[],
                          #help="A comma seperated list of triggers to load, into "
                          #"the data set. Can be specified multiple times.")

    opts = parser.parse_args()

    from PIDPerfScripts.Definitions import *
    from PIDPerfScripts.Exceptions import *
    from PIDPerfScripts.RunDictFuncs import *
    from PIDPerfScripts.PerfResults import *

    PartName = None
    PlotVar = None

    DLLCuts = []
    RunMin = None
    RunMax = None

    fname_Ref = opts.refFilename
    tname_Ref = opts.refTree

    outputDir = opts.outputDir

    # set the particle name
    PartName=opts.partName
    CheckPartType(PartName)

    TriggerList = []
    #if (len(opts.triggerList)>0):
        #if isinstance(opts.triggerList,str):
            #TriggerList = [opts.triggerList]
        #elif isinstance(opts.triggerList,list):
            #TriggerList = opts.triggerList
        #if opts.verbose:
            #print "Triggers to load:", TriggerList

    # set the plot variable
    PlotVar=opts.varName
    if not CheckCuts(opts.varName,TriggerList,StripVersion):
        parser.error("Invalid variable %s" %str(opts.varName))

    refVarName = opts.refVarName if opts.refVarName else PlotVar

    if (len(opts.cuts)>0):
        if isinstance(opts.cuts,str):
            if not CheckCuts(opts.cuts,TriggerList,StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cuts))
        elif isinstance(opts.cuts,list):
            if not CheckCuts(opts.cuts.join(" "),TriggerList,StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cuts))

    if opts.verbose:
        print '========== Requested publication plots =========='
        #print "Stripping version: {0}".format(StripVersion)
        #print "Magnet polarity: {0}".format(MagPolarity)
        print "Particle name: {0}".format(PartName)
        #print "MisID particle name: {0}".format(MisIDPartName)
        print "Plot variable: {0}".format(PlotVar)
        if opts.schemeName is not None:
            print "Scheme name: {0}".format(opts.schemeName)
        if len(opts.cuts)>0:
            print "Initial cuts: {0}".format(opts.cuts)
        #for icut, cut in enumerate(PIDCuts):
            #print "PID cut {num:d}: {val}".format(num=icut, val=cut)
        print '===================================='

    #======================================================================
    # Get the binning scheme
    #======================================================================
    import ROOT
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
    #ROOT.gSystem.Load('libCintex.so')
    #cintex=ROOT.Cintex
    #cintex.Enable()
    if opts.schemeName is None:
        schemeName = 'PerfPlots_{IDPart}'.format(
            IDPart=PartName)
    else:
        schemeName=opts.schemeName

    BinSchema = GetBinScheme(PartName, PlotVar, schemeName)

    #======================================================================
    # Make Performance Plots
    #======================================================================
    f_Ref = ROOT.TFile.Open(fname_Ref)
    if not f_Ref:
        raise IOError("Failed to open reference file {0} for reading".format(fname_Ref))
    t_Ref = f_Ref.Get(tname_Ref)
    if not t_Ref:
        msg="Failed to retrieve reference TTree {tname} from file {fname}".format(
            tname=tname_Ref, fname=fname_Ref)
        raise ValueError(msg)

    #======================================================================
    # Loop over all calibration files
    #======================================================================
    histname = "%s_%s" %(PartName, PlotVar)

    RawHist = ROOT.TH1D(histname, histname, BinSchema.numBins(), BinSchema.array())
    if "{particle}" in DataSetVariables(PlotVar):
      t_Ref.Draw("%s_%s>>%s"%(opts.refBranch, refVarName, histname),opts.cuts)
    else:
      t_Ref.Draw("%s>>%s"%(refVarName, histname),opts.cuts)

    #======================================================================
    # Construct file name
    #======================================================================
    fileSuffix=""
    if schemeName is not None:
        fileSuffix+='_{0}'.format(schemeName)
    fileSuffix+='_{0}'.format(BinSchema.GetName())
    fname="PlotRefDistribution_{particle}_{var}{suf}.root".format(
        particle=PartName, var=PlotVar, suf=fileSuffix)
    if outputDir is not None:
        fname="{0}/{1}".format(outputDir, fname)

    f_Out = ROOT.TFile.Open(fname, 'RECREATE')
    if not f_Out:
        raise IOError("Failed to open file {0} for writing".format(fname))

    RawHist.Write()

    f_Out.Close()


