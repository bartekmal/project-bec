import os, sys
from ROOT import TFile, TNtuple
from math import sqrt, log

from Config import *

def convert_single_file(infile, f2, nt2, treenames, pidvar, ptvar, etavar, ntracksvar, weightvar, gamma) :
  f1 = TFile.Open(infile)
  print "file ", infile
  if not f1 : return False

  for treename in treenames : 
    nt1 = f1.Get(treename)
    f2.cd()
    nentries = nt1.GetEntries()
    print "  tuple %s: %d entries" % (treename, nentries )
    n = 0
    if gamma<0 : 
      pid_code = compile("(1.-(1.-i.%s)**%f)" % (pidvar, abs(gamma)), '<string>', 'eval')
    else : 
      pid_code = compile("(i.%s)**%f" % (pidvar, abs(gamma)), '<string>', 'eval')
    pt_code = compile("log(i.%s)" % ptvar, '<string>', 'eval')
    eta_code = compile("i.%s" % etavar, '<string>', 'eval')
    if ntracksvar : 
      ntracks_code = compile("log(i.%s)" % ntracksvar, '<string>', 'eval')
    weight_code = compile("i.%s" % weightvar, '<string>', 'eval')
    for i in nt1 :
      n += 1
      if (n % 10000 == 0) : 
        print "    event %d/%d" % (n, nentries)
      try : 
        pid = eval(pid_code)
        pt  = eval(pt_code)
        eta = eval(eta_code)
        ntracks = eval(ntracks_code)
        weight = eval(weight_code)
        nt2.Fill(pid, pt, eta, ntracks, weight)
      except : 
        continue

  f1.Close()
  return True

if len(sys.argv)>1 : 
  # config name, e.g. "p_ProbNNp"
  configname = sys.argv[1]
else : 
  print "Usage: MakeTuples [config] [option1:option2:...] [dataset1:dataset2:]"
  print "  configs are: "
  for i in sorted(configs.keys()) : 
    print "    ", i
  print "  options are: "
  print "    test - run for just a single PIDCalib file rather than whole dataset"
  print "  datasets can be, e.g. "
  print "    MagDown_2012:MadUp_2011"
  print "    or leave empty to process all available datasets"
  sys.exit(0)

config = configs[configname]
# sample name, e.g. "p" for proton
samplename = config['sample']
sample = samples[samplename]

dslist = datasets.keys()
options = ""
if len(sys.argv)>2 : options = sys.argv[2].split(":")
if len(sys.argv)>3 : dslist = sys.argv[3].split(":")

# resampled variable name
var = configname

# other variable names
pidvar = config['var']
ptvar = sample['pt']
etavar = sample['eta']
ntracksvar = sample['ntracks']
weightvar = sample['weight']
treename = sample['trees']
gamma = config['gamma']

# Create dictionary with the lists of PIDCalib datasets for each year and magnet polarity
dsdict = {}
for ds in dslist : 
  if not isinstance(datasets[ds], tuple) : 
    dsdict[ds] = [ datasets[ds] ] 
  else : 
    dsdict[ds] = []
    n = datasets[ds][1]
    for i in range(0, n) : 
      dsdict[ds] += [ datasets[ds][0] % i ]

os.system("/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select mkdir -p %s/%s" % (eosdir, configname))

# Loop over PIDCalib datasets
for pol,dss in dsdict.iteritems() : 

  calibfile = pol + ".root"

  f2 = TFile.Open(calibfile,"RECREATE")
  nt2 = TNtuple("pid","pid", "PID:Pt:Eta:Ntracks:w")

  print "Polarity " + pol

  nds = 0
  for ds in dss : 
    ok = convert_single_file(ds, f2, nt2, treename, pidvar, ptvar, etavar, ntracksvar, weightvar, gamma)
    if ok : nds += 1
    if nds >= 2 and "test" in options : 
      break

  nt2.Write()
  f2.Close()
  os.system("/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select cp %s %s/%s/" % (calibfile, eosdir, configname))
  os.remove(calibfile)

  if "test" in options : 
    print "Now run:"
    print "root -l %s/%s/%s" % (eosrootdir, configname, calibfile)
