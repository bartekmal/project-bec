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
        description=("""Create a TTree from a PID Calib dataset. """
        """This is particulaly useful when debugging.

For a full list of arguments, do: 'python {0} -h'
e.g. python {0}  inputFile.root outputFile.root
""").format(os.path.basename(sys.argv[0])))

    ## add the positional arguments
    parser.add_argument('inFile', metavar='<inFile>',
                        help="File from which the RooDataSets are loaded")
    parser.add_argument('outFile', metavar='<outFile>',
                        help="File in which the TTrees are saved")
    
    addGroup = parser.add_argument_group("further options")
    addGroup.add_argument("-q", "--quiet", dest="verbose", action="store_false",
                          default=True,
                          help="Suppresses the printing of verbose information")

    opts = parser.parse_args()
    
    import ROOT
    
    inFile = ROOT.TFile.Open(opts.inFile)
    outFile = ROOT.TFile.Open(opts.outFile, "RECREATE")
    
    print "Finding workspaces..."
    
    inKeys = inFile.GetListOfKeys()
    it = inKeys.MakeIterator()
    workspaces = []
    while True:
      k = it.Next()
      if not k: break
      
      if k.GetClassName() == "RooWorkspace":
        workspaces.append( k.ReadObj() )
        
    datasets = {}
    for w in workspaces:
      
      name = w.GetName()
      print "Getting dataset and trigger info from %s" % name
      
      data = w.obj("data")
      triggers = w.obj("trigger")
      
      ROOT.RooAbsData.setDefaultStorageType(ROOT.RooAbsData.Tree)
      
      datasets[name] = ROOT.RooDataSet( name+"_combined", name+"_combined", 
                                        data.get(), ROOT.RooFit.Import(data) )
      if triggers:
        datasets[name].merge(triggers)
    
    outFile.cd()
    for name,data in datasets.items():
      print "Writing %s tree" % name
      tree = data.tree()
      tree.Write(name)
    outFile.Close()
