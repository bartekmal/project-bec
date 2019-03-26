import os, sys, math
from ROOT import TFile, TNtuple

from Config import *

def convert_single_file(infile, f2, nt2, treename, pidvar, ptvar, etavar, ntracksvar, transform, limits) :
  f1 = TFile(infile)
  nt1 = f1.Get(treename)
  f2.cd()
  nentries = nt1.GetEntries()
  print "tuple %s: %d entries" % (treename, nentries )
  n = 0
  x_code = compile("i.%s" % pidvar, '<string>', 'eval')
  pid_code = compile(transform, '<string>', 'eval')
  pt_code = compile("math.log(i.%s)" % ptvar, '<string>', 'eval')
  eta_code = compile("i.%s" % etavar, '<string>', 'eval')
  if ntracksvar : 
    ntracks_code = compile("math.log(i.%s)" % ntracksvar, '<string>', 'eval')
  for i in nt1 :
      n += 1
      if (n % 10000 == 0) : 
        print "  event %d/%d" % (n, nentries)
      x = eval(x_code)
      if x < limits[0] or x > limits[1] : continue
      pid = eval(pid_code)
      pt  = eval(pt_code)
      eta = eval(eta_code)
      ntracks = eval(ntracks_code)
      nt2.Fill(pid, pt, eta, ntracks, i.Charge_idx, i.nsig_sw)
  f1.Close()

def convert_calo_file(infile, f2, nt2, treename, pidvar, ptvar, etavar, transform) :
  f1 = TFile(infile)
  nt1 = f1.Get(treename)
  f2.cd()
  nentries = nt1.GetEntries()
  print "tuple %s: %d entries" % (treename, nentries )
  n = 0
  x_code = compile("i.%s" % pidvar, '<string>', 'eval')
  pid_code = compile(transform, '<string>', 'eval')
  pt_code = compile("math.log(i.%s)" % ptvar, '<string>', 'eval')
  eta_code = compile("i.%s" % etavar, '<string>', 'eval')
  for i in nt1 :
      n += 1
      if (n % 10000 == 0) : 
        print "  event %d/%d" % (n, nentries)
      x = eval(x_code)
      pid = eval(pid_code)
      pt  = eval(pt_code)
      eta = eval(eta_code)
      nt2.Fill(pid, pt, eta, i.nkstgWeight)
  f1.Close()

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
datasets = sample['datasets']
dslist = datasets.keys()
reco = sample['reco']
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
treename = sample['tree']
transform = config['transform_forward']

limits = (0., 1.)
if 'limits' in config : 
  limits = config['limits']

# Create dictionary with the lists of PIDCalib datasets for each year and magnet polarity
dsdict = {}
for ds in dslist : 
  if not isinstance(datasets[ds], tuple) : 
    dsdict[ds] = [ datasets[ds] ] 
  else : 
    dsdict[ds] = []
    n = datasets[ds][1]
    if "test" in options : n = 1
    for i in range(0, n) : 
      dsdict[ds] += [ datasets[ds][0] % i ]
    if "test" in options : break

os.system("/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select mkdir -p %s/%s" % (eosdir, configname))

# Loop over PIDCalib datasets
for pol,dss in dsdict.iteritems() : 

  calibfile = pol + ".root"

  f2 = TFile.Open(calibfile,"RECREATE")
  if reco : 
    nt2 = TNtuple("pid","pid", "%s:Pt:Eta:Ntracks:Charge:w" % var)
  else : 
    nt2 = TNtuple("pid","pid", "%s:Pt:Eta:w" % var)

  print "Polarity " + pol

  for ds in dss : 
    if reco :   # PIDCalib samples need to be converted to TTree first
#      loc = "${CALIBDATAURLPROTOCOL}://${CALIBDATASTORE}/%s/%s/%s" % (reco, pol.split("_")[0], ds)
      loc = "/${CALIBDATASTORE}/%s/%s/%s" % (reco, pol.split("_")[0], ds)
      out = tmpdir + "/" + ds
      cmd = "python $PIDPERFSCRIPTSROOT/scripts/python/Plots/CreateTTreeFromDataset.py %s %s" % (loc, out)
      print "  File " + out
      print "  " + cmd
      os.system(cmd)
      convert_single_file(out, f2, nt2, treename, pidvar, ptvar, etavar, ntracksvar, transform, limits)
      os.remove(out)
    else : 
      # Calo samples are already trees
      convert_calo_file(ds, f2, nt2, treename, pidvar, ptvar, etavar, transform)

  nt2.Write()
  f2.Close()
  os.system("/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select cp %s %s/%s/" % (calibfile, eosdir, configname))
  os.remove(calibfile)
  if "test" in options : 
    print "Now run:"
    print "root -l %s/%s/%s" % (eosrootdir, configname, calibfile)
