import os, sys, signal

"""
Create PID PDFs from calibration ntuples, as well as some auxiliary
PDFs (for systematic and stat. error evaluation)
"""

cwd = os.getcwd()

import ConfigMC as ConfigMCSim08
import ConfigMCSim09 as ConfigMCSim09

def signal_handler(signal, frame):
    print 'Exiting'
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

if len(sys.argv)>2 : 
  simversion = sys.argv[1]
  configname = sys.argv[2]
else : 
  print "Usage: python CreatePIDPdf_MC.py [sim08/sim09] [config] [option1:option2:...]"
  print "  configs for Sim08 are: "
  for i in sorted(ConfigMCSim08.configs.keys()) : 
    print "    ", i
  print "  configs for Sim09 are: "
  for i in sorted(ConfigMCSim09.configs.keys()) : 
    print "    ", i
  print "  options are: "
  print "     dry - Dry run (do not submit jobs, just print command lines)"
  print "     continue - Continue unfinished jobs (only run on the datasets where control plots are missing in eos)"
  sys.exit(0)

opt = ""
if len(sys.argv)>3 : 
  opt = sys.argv[3]

if simversion == "sim08" : Config = ConfigMCSim08
elif simversion == "sim09" : Config = ConfigMCSim09
else : 
  print "Simulation version %s unknown" % simversion
  sys.exit(0)

config = Config.configs[configname]
scale_default = config['scale_default']
scale_syst = config['scale_syst']
scale_pid = config['scale_pid']
var = configname
toystat = config['toystat']
controlstat = config['controlstat']
nbootstrap = config['nbootstrap']
sample = config['sample']
eosrootdir = Config.eosrootdir
eosdir = Config.eosdir

file_list = Config.samples[sample]['datasets'].keys()

limits = None
if 'limits' in config : 
  limits = config['limits']

scr = "bsub -q 1nd lb-run --nightly-cvmfs --nightly lhcb-prerelease Urania/master python %s/PIDPdf.py" % (cwd)
#scr = "python PIDPdf.py"

# Electron sample needs additional option
if sample == "e" : 
  scr += " -e"

ready_list = []
if "continue" in opt.split(":") : 
  os.system("/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select ls %s/%s/control/ > all.txt" % (eosdir, var) )
  f = open("all.txt")
  for l in f : ready_list += [ "control/" + l.strip() ]
  f.close()
  print ready_list


outdir = eosrootdir + "/" + configname + "/"
#os.system("mkdir -p %s/control/" % outdir)
os.system("/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select mkdir -p %s/%s/control/" % (eosdir, configname))

for filename in file_list : 

  infile = configname + "/" + filename + ".root"

  nominalfile = filename + "_distrib.root"
  controlfile = "control/" + filename + "_control.root"
  figfile = "plots_mc/" + filename + "_control.png"

  command = "%s -s %f -i %f -v %s -o %s -t %d -p %d -b %s -c %s -f %s %s/%s" % \
          (scr, scale_default, scale_pid, var, outdir, toystat, controlstat, nominalfile, controlfile, figfile, eosrootdir, infile)
  if limits : 
    command += " -l %f -u %f" % (limits[0], limits[1])

  if controlfile not in ready_list : 
    print command
    if ("dry" not in opt.split(":")) : os.system(command)

  systfile    = filename + "_syst_1.root"
  controlfile = "control/" + filename + "_syst_1_control.root"
  figfile     = "plots_mc/" + filename + "_syst_1_control.png"

  if controlfile not in ready_list and scale_syst : 
    command = "%s -s %f -i %f -v %s -o %s -t %d -p %d -b %s -c %s -f %s %s/%s" % \
          (scr, scale_syst, scale_pid, var, outdir, toystat, controlstat, systfile, controlfile, figfile, eosrootdir, infile)
    if limits : 
      command += " -l %f -u %f" % (limits[0], limits[1])
    print command
    if ("dry" not in opt.split(":")) : os.system(command)

  for i in range(0, nbootstrap) : 

    statfile    = filename + "_stat_%d.root" % i
    tmpfile     = "%s/%s_stat_%d_tmp.root" % (tmpdir, filename, i)
    controlfile = "control/" + filename + "_stat_%d_control.root" % i
    figfile     = "plots_mc/" + filename + "_stat_%d_control.png" % i
    command = "%s -a %s -r %d -s %f -i %f -v %s -o %s -t %d -p %d -b %s -c %s -f %s %s/%s" % \
            (scr, tmpfile, i+1, scale_default, scale_pid, var, outdir, toystat, controlstat, statfile, controlfile, figfile, eosrootdir, infile)
    if limits : 
      command += " -l %f -u %f" % (limits[0], limits[1])

    if controlfile not in ready_list : 
      print command
      if ("dry" not in opt.split(":")) : os.system(command)
