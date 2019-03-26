import os, sys, math
from ROOT import TFile, TNtuple, gDirectory, gROOT

import ConfigMC as ConfigMCSim08
import ConfigMCSim09 as ConfigMCSim09

def convert_single_file(infile, indir, f2, nt2, treename, pidvar, ptvar, etavar, ntracksvar, transform) :
  match_code = None
  f1 = TFile.Open(infile)
  gDirectory.cd(indir)
  nt1 = gROOT.FindObject(treename)
  f2.cd()
  nentries = nt1.GetEntries()
  print "tuple %s: %d entries" % (treename, nentries )
  n = 0
  x_code = compile("i.%s" % pidvar, '<string>', 'eval')
  pid_code = compile(transform, '<string>', 'eval')
  pt_code = compile("math.log(i.%s)" % ptvar, '<string>', 'eval')
  eta_code = compile("i.%s" % etavar, '<string>', 'eval')
  ntracks_code = compile("math.log(i.%s)" % ntracksvar, '<string>', 'eval')
  if 'match' in [ b.GetName() for b in nt1.GetListOfBranches() ] : 
    match_code = compile("i.match", '<string>', 'eval')
  for i in nt1 :
      n += 1
      if (n % 10000 == 0) : 
        print "  event %d/%d" % (n, nentries)
      if match_code : 
        m = eval(match_code)
        if m < 1. : continue
      x = eval(x_code)
      pid = eval(pid_code)
      pt  = eval(pt_code)
      eta = eval(eta_code)
      ntracks = eval(ntracks_code)
      nt2.Fill(pid, pt, eta, ntracks, 1., 1.)
  f1.Close()

if len(sys.argv)>2 : 
  # config name, e.g. "p_ProbNNp"
  simversion = sys.argv[1]
  configname = sys.argv[2]
else : 
  print "Usage: MakeTuples [sim08/sim09] [config] [option1:option2:...] [dataset1:dataset2:]"
  print "  configs for sim08 are: "
  for i in sorted(ConfigMCSim08.configs.keys()) : 
    print "    ", i
  print "  configs for sim09 are: "
  for i in sorted(ConfigMCSim09.configs.keys()) : 
    print "    ", i
  print "  options are: "
  print "    test - run for just a single PIDCalib file rather than whole dataset"
  print "  datasets can be, e.g. "
  print "    MagDown_2012:MadUp_2011"
  print "    or leave empty to process all available datasets"
  sys.exit(0)

if simversion == "sim08" : Config = ConfigMCSim08
elif simversion == "sim09" : Config = ConfigMCSim09
else : 
  print "Simulation version %s unknown" % simversion
  sys.exit(0)

config = Config.configs[configname]
# sample name, e.g. "p" for proton
samplename = config['sample']
sample = Config.samples[samplename]
datasets = sample['datasets']
dslist = datasets.keys()
eosdir = Config.eosdir
eosrootdir = Config.eosrootdir
options = ""
if len(sys.argv)>3 : options = sys.argv[3].split(":")
if len(sys.argv)>4 : dslist = sys.argv[4].split(":")

# resampled variable name
var = configname

# other variable names
pidvar = config['var']
ptvar = sample['pt']
etavar = sample['eta']
ntracksvar = sample['ntracks']
treename = sample['tree']
indir = sample['dir']
transform = config['transform_forward']

os.system("/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select mkdir -p %s/%s" % (eosdir, configname))

# Create dictionary with the lists of PIDCalib datasets for each year and magnet polarity
dsdict = {}
for ds in dslist : 
  dsdict[ds] = [ datasets[ds] ]
  if "test" in options : break

# Loop over PIDCalib datasets
for pol,dss in dsdict.iteritems() : 

  calibfile = eosrootdir + "/" + var + "/" + pol + ".root"

  f2 = TFile.Open(calibfile,"RECREATE")
  nt2 = TNtuple("pid","pid", "%s:Pt:Eta:Ntracks:Charge:w" % var)

  print "Polarity " + pol

  for ds in dss : 
    infile = eosrootdir + "/" + ds
    convert_single_file(infile, indir, f2, nt2, treename, pidvar, ptvar, etavar, ntracksvar, transform)

  nt2.Write()
  f2.Close()
