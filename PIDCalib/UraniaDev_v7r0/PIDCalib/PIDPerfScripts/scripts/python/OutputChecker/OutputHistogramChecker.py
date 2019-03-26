#!/usr/bin/env python

from PIDPerfScripts.StartScreen import *

import argparse
import operator
import sys
import os
import ROOT

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
        description=( """Check histograms of PID performance across different PIDCalib versions.
Simply give the filenames of the two files to check, all histograms in the files 
will be processed. This script requires the content and error of each bin to be 
the same in each histogram across all files. 

For a full list of arguments, do: 'python {0} -h'

e.g. python {0} file1.root file2.root
""").format(os.path.basename(sys.argv[0])),
        )

    ## add the positional arguments
    parser.add_argument('file1', metavar='<fileName>',
                        help="The first file to check")
    parser.add_argument('file2', metavar='<fileName>',
                        help="The second file to check")
    
    addGroup = parser.add_argument_group("further options")
    addGroup.add_argument("-q", "--quiet", dest="verbose", action="store_false",
                          default=True,
                          help="suppresses the printing of verbose information")
    
    opts = parser.parse_args()
    
    #from PIDPerfScripts.Definitions import *
    #from PIDPerfScripts.Exceptions import *
    #from PIDPerfScripts.RunDictFuncs import *
    #from PIDPerfScripts.PerfResults import *
    
    fnames = [ opts.file1 , opts.file2 ]
    
    if opts.verbose:
        print '========== Requested comparason =========='
        print "Files: {0}".format(fnames.__repr__())
        print '=========================================='

  
    #======================================================================
    # Open files
    #======================================================================
    fs = []
    for f in fnames:
      fs.append(ROOT.TFile.Open(f))
      if not fs[-1]:
          raise IOError("Failed to open reference file {0} for reading".format(f))
    
    #======================================================================
    # Check the number of histograms in each file
    #======================================================================
    NumberOfKeys = False
    for f in fs:
      if not NumberOfKeys:
        NumberOfKeys = f.GetNkeys()
      elif NumberOfKeys != f.GetNkeys():
        IOError("Files {0} dont have the same histograms in them.".format(fnames.__repr__()))
    
    #======================================================================
    # load the histograms into a list 
    #======================================================================
    iters = [f.GetListOfKeys().MakeIterator() for f in fs]
    histos = []
    Loop = True
    while Loop:
      keys = [i.Next() for i in iters]
      histos.append([])
      for i,k in enumerate(keys):
        if not k:
          #print "End of histograms"
          Loop = False
        else:
          print fnames[i]+":", k.GetName()
          histos[-1].append(k.ReadObj())
          
    #======================================================================
    # Loop over the histograms and compare them all
    #======================================================================
    for hist_array in histos:
      if len(hist_array) == 0:
        continue
      all_three_dee = reduce(operator.__and__, [h.Class_Name()[:3] == "TH3" for h in hist_array])
      all_two_dee   = reduce(operator.__and__, [h.Class_Name()[:3] == "TH2" for h in hist_array])
      all_one_dee   = reduce(operator.__and__, [h.Class_Name()[:3] == "TH1" for h in hist_array])
            
      if not all_three_dee and not all_two_dee and not all_one_dee:
        raise ValueError("Histograms not all of the same type: %s"%(hist_array.__repr__()))
      
      print "Looking at histograms:", " ".join([h.GetName() for h in hist_array])
      print "All histograms are... 3D: %s, 2D: %s, 1D: %s"%(all_three_dee.__repr__(),all_two_dee.__repr__(),all_one_dee.__repr__())
      
      axies = [ ("X",hist_array[0].GetXaxis()), ("Y",hist_array[0].GetYaxis()), ("Z",hist_array[0].GetZaxis()) ]
      ranges = []
      
      for name,axis in axies:
        if axis:
          bins = axis.GetNbins()
          ranges.append(bins+2)
          for h in hist_array:
            if bins != getattr(h,"Get%saxis"%name)().GetNbins():
              raise ValueError("Histograms do not have the same binning scheme: %s"%(hist_array.__repr__()))
        else:
          ranges.append(0)
      
      for i in xrange(ranges[0]):
        for j in xrange(ranges[1]):
          for k in xrange(ranges[2]):
            comparason = False
            for h in hist_array:
              if not comparason:
                comparason = [h.GetBinContent(i,j,k),h.GetBinError(i,j,k)]
              else:
                if comparason[0] != h.GetBinContent(i,j,k) or comparason[1] != h.GetBinError(i,j,k):
                  print (
                    """\nHistograms dont have same value in bin (%i,%i,%i).\n"""
                    """Histograms are called: %s.\n"""
                    """Histograms from files: %s.\n""" % (
                      i,j,k,
                      ", ".join([h.GetName() for h in hist_array]),
                      ", ".join(fnames),
                      )
                    +"\n".join(["Value of bin in histogram %i: %f +/- %f"%(n,h.GetBinContent(i,j,k),h.GetBinError(i,j,k)) for n,h in enumerate(hist_array)])
                    )
                else:
                  sys.stdout.write(".")
      sys.stdout.write("\n")
      
      print "Histograms are equal!"
    
    for f in fs:
      f.Close()
      
    
    
    