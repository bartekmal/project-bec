from PIDPerfScripts.DataFuncs import *
from PIDPerfScripts.PerfCalcFuncs import *
from PIDPerfScripts.PerfResults import *
from PIDPerfScripts.StartScreen import *

import ROOT
import sys
#import getopt
import argparse

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
        description=("""Make a graph of PID performance vs. misID rate """
                     """over a range of PID cuts for a given:
                      a) Stripping version <stripVersion> (e.g. \"20\")
                      b) magnet polarity  <magPol> (\"MagUp\" or \"MagDown\")
                      c) particle type <partName> (\"K\", \"P\", \"Pi\", \"e\" or \"Mu\")
                      d) misID particle type <misIDPartName>
                      e) DLL or ProbNN variable <pidVar>, including the """
                     """inequality (e.g. \"DLLK>\")
                      f) one or more PID cut values <pidCut> (e.g. \"0\")

For a full list of arguments, do: 'python {0} -h'
e.g. python {0}  --minRun=114205 --maxRun=114287 \"20\" \"MagUp\" \"K\" \\
    \"Pi\" \"DLLK>\" \"-15\" \"15\" \"1\".
    """).format(os.path.basename(sys.argv[0]))
        )

    ## add the positional arguments
    parser.add_argument('stripVersion', metavar='<stripVersion>',
                        help="Sets the stripping version")
    parser.add_argument('magPol', metavar='<magPol>',
                        help="Sets the magnet polarity")
    parser.add_argument('partName', metavar='<partName>',
                        help="Sets the particle type")
    parser.add_argument('misIDPartName', metavar='<misIDPartName>',
                        help="Sets the misidentified particle type")
    parser.add_argument('pidVar', metavar='<varName>',
                        help="Sets the PID cut variable (including the inequality)")
    parser.add_argument('cutVals', metavar='<pidCut>', nargs='+',
                        help="Sets the PID cut value(s) to plot")

    ## add the optional arguments
    parser.add_argument('-x', '--minRun', dest="runMin", metavar="NUM",
                        help="Sets the minimum run number to process (if applicable)")
    parser.add_argument('-y', '--maxRun', dest="runMax", metavar="NUM",
                        help="Sets the maximum run number to process (if applicable)")
    parser.add_argument('-f', '--maxFiles', dest="maxFiles", metavar="NUM",
                        help="Sets the maximum number of calibration files to run over")
    #parser.add_argument('-c', '--cuts', dest='cuts', metavar='CUTS', default='',
                        #help=("Sets the list of cuts to apply to the calibration "
                              #"sample(s) prior to determine the PID efficiencies "
                              #"(default: (default)s)"))
    parser.add_argument('-cPart', '--cutsPart', dest='cutsPart', metavar='CUTSPart', default='',                                                       
                        help=("Sets the list of cuts to apply to the calibration particle "                                                            
                              "sample(s) prior to determine the PID efficiencies "                                                                     
                              "(default: (default)s)"))                                                                                                
    parser.add_argument('-cMisPart', '--cutsMisPart', dest='cutsMisPart', metavar='CUTSMisPart', default='',                                           
                        help=("Sets the list of cuts to apply to the calibration mis-id particle "                                                     
                              "sample(s) prior to determine the PID efficiencies "                                                                     
                              "(default: (default)s)"))  
                        
    parser.add_argument('-n', '--pidVarNickname', dest='pidVarNickname', metavar='NAME',
                        default=None,
                        help=("Sets the nickname of the PID variable. This nickname "
                              "is used when parsing the output filename. If this option "
                              "is not used, then the name of the PID variable will be "
                              "used instead."))
    parser.add_argument("-o", "--outputDir", dest="outputDir", metavar="DIR",
                        help="Save the performance histograms to directory DIR "
                        "(default: current directory)")

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

    StripVersion = None
    MagPolarity = None
    PartName = None
    MisIDPartName = None
    PIDVar = None
    CutVals = []

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

    # set the misID particle name
    MisIDPartName=opts.misIDPartName
    CheckPartType(MisIDPartName)

    TriggerList=[]

    # set the PID variable
    PIDVar=opts.pidVar
    PIDVarNickname = PIDVar if opts.pidVarNickname is None else opts.pidVarNickname
    
    if not CheckCuts(opts.pidVar,TriggerList,StripVersion):
        parser.error("Invalid pid variable %s" %str(opts.pidVar))
        
    if (len(opts.cutsPart)>0):
        if isinstance(opts.cutsPart,str):
            if not CheckCuts(opts.cutsPart,TriggerList,StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cutsPart))
        elif isinstance(opts.cutsPart,list):
            if not CheckCuts(opts.cutsPart.join(" "),TriggerList,StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cutsPart))
        
    if (len(opts.cutsMisPart)>0):
        if isinstance(opts.cutsMisPart,str):
            if not CheckCuts(opts.cutsMisPart,TriggerList,StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cutsMisPart))
        elif isinstance(opts.cutsMisPart,list):
            if not CheckCuts(opts.cutsMisPart.join(" "),TriggerList,StripVersion):
                parser.error("Invalid cut string %s" %str(opts.cutsMisPart))

    # set the PID cut values
    for v in opts.cutVals:
        v=v.strip()
        f_v = None
        try:
            f_v=float(v)
        except ValueError:
            parser.error("Cannot convert PID cut value '{0}' to float".format(v))
        CutVals.append(v)

    RunMin = opts.runMin
    RunMax = opts.runMax
    MaxFiles = opts.maxFiles

    if RunMin is not None:
        try:
            int(RunMin)
        except ValueError:
            parser.error(
                "Argument to --minRun ('{0}') is not an integer'.".format(
                RunMin))

        if RunMax is None:
            parser.error(
                "Min run was specified as {0}, but no max run was given.".format(
                RunMin))

    if RunMax is not None:
        try:
            int(RunMax)
        except ValueError:
            parser.error(
                "Argument to --maxRun ('{0}') is not an integer'.".format(
                RunMax))
        if RunMin is None:
            parser.error(
                "Max run was specified as {0}, but no min run was given.".format(
                RunMax))

    if MaxFiles is not None:
        try:
            int(MaxFiles)
        except ValueError:
            parser.error(
                "Argument to --maxFiles ('%s') is not an integer'." %MaxFiles)
        if MaxFiles is None:
            parser.error(
                "Max files was specified as %s, but no min run was given." %MaxFiles)

    if opts.verbose:
        print '========== Requested performance curve =========='
        print "Stripping version: {0}".format(StripVersion)
        print "Magnet polarity: {0}".format(MagPolarity)
        print "Particle name: {0}".format(PartName)
        print "MisID particle name: {0}".format(MisIDPartName)
        print "PID cut variable: {0}".format(PIDVar)
        if opts.pidVarNickname is not None:
            print "PID cut variable nickname: {0}".format(PIDVarNickname)
        print "PID cut values: {0}".format(str(CutVals))
        if len(opts.cutsPart)>0:
            print "Initial cuts on particle: {0}".format(opts.cutsPart)
        if len(opts.cutsMisPart)>0:
            print "Initial cuts on mis-id particle: {0}".format(opts.cutsMisPart)  
        print '===================================='


    #======================================================================
    # Get the parsed list of cuts
    #======================================================================
    import ROOT

    PIDCutList = ROOT.list('string')()
    for val in CutVals:
        PIDCutList.push_back('{0}{1}'.format(PIDVar, str(val)))

    #======================================================================
    # Get the ID and misID performance histograms
    #======================================================================
    PIDResults = GetPerfResultList(MakePIDResultList,
                                   StripVersion,
                                   MagPolarity,
                                   PartName,
                                   PIDCutList,
                                   opts.cutsPart,
                                   RunMin,
                                   RunMax,
                                   opts.verbose,
                                   opts.allowMissing,
                                   MaxFiles)

    MisIDResults = GetPerfResultList(MakePIDResultList,
                                     StripVersion,
                                     MagPolarity,
                                     MisIDPartName,
                                     PIDCutList,
                                     opts.cutsMisPart,
                                     RunMin,
                                     RunMax,
                                     opts.verbose,
                                     opts.allowMissing,
                                     MaxFiles)


    #======================================================================
    # Make Weighted Averages
    #======================================================================
    PID_AvResult = WeightedAveragePIDResults(PIDResults, opts.verbose)
    MisID_AvResult = WeightedAveragePIDResults(MisIDResults, opts.verbose)

    #======================================================================
    # Make TGraph
    #======================================================================
    plotType='{0}Vs{1}'.format(PartName, MisIDPartName)
    plotTitle="{typ};{part} efficiency;{misIDPart} misID rate".format(
       typ=plotType, part=PartName, misIDPart=MisIDPartName)
    graph = makeTGraph(PID_AvResult, MisID_AvResult, plotType, plotTitle)

    #======================================================================
    # Create file and write TGraph
    #======================================================================
    fileSuffix=''
    if RunMin is not None:
        fileSuffix='_min{0}_max{1}'.format(RunMin, RunMax)

    fname='PerfCurve_{plotType}_Strip{strp}_{pol}_{pidvar}{suf}{cutsPart}.root'.format(
        plotType=plotType, strp=StripVersion, pol=MagPolarity,
        suf=fileSuffix, pidvar = PIDVarNickname, cutsPart=opts.cutsPart)
    f_Out = ROOT.TFile(fname, 'RECREATE')
    graph.Write()

    #======================================================================
    # Close file
    #======================================================================
    f_Out.Close()
