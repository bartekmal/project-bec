import os, sys, signal

"""
Create PID PDFs from calibration ntuples, as well as some auxiliary
PDFs (for systematic and stat. error evaluation)
"""

cwd = os.getcwd()

from Config import *

def signal_handler(signal, frame):
    print 'Exiting'
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

if len(sys.argv)>1 : 
  config = sys.argv[1]
else : 
  print "Usage: CreatePIDPdf [config] [option1:option2:...]"
  print "  configs are: "
  for i in sorted(configs.keys()) : 
    print "    ", i
  print "  options are: "
  print "     dry - Dry run (do not submit jobs, just print command lines)"
  print "     continue - Continue unfinished jobs (only run on the datasets where control plots are missing in eos)"
  sys.exit(0)

opt = ""
if len(sys.argv)>2 : 
  opt = sys.argv[2]

scale_default = configs[config]['scale_default']
scale_syst = configs[config]['scale_syst']
scale_pid = configs[config]['scale_pid']
var = config
toystat = configs[config]['toystat']
controlstat = configs[config]['controlstat']
nbootstrap = configs[config]['nbootstrap']
sample = configs[config]['sample']

file_list = samples[sample]['datasets'].keys()

limits = None
if 'limits' in configs[config] : 
  limits = configs[config]['limits']

if config in ["gamma_CL"] : 
  scr = "bsub -q 2nw lb-run --nightly-cvmfs --nightly lhcb-prerelease Urania/master python %s/PIDPdf3D.py" % (cwd)
else : 
  if scale_default < 0.5 : 
    scr = "bsub -q 2nw lb-run --nightly-cvmfs --nightly lhcb-prerelease Urania/master python %s/PIDPdf_narrow.py" % (cwd)
  else : 
    scr = "bsub -q 2nw lb-run --nightly-cvmfs --nightly lhcb-prerelease Urania/master python %s/PIDPdf.py" % (cwd)

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

for filename in file_list : 

  infile = config + "/" + filename + ".root"
  outdir = eosrootdir + "/" + config + "/"

  nominalfile = filename + "_distrib.root"
  controlfile = "control/" + filename + "_control.root"
  figfile = "plots/" + filename + "_control.png"

  command = "%s -s %f -i %f -v %s -o %s -t %d -p %d -b %s -c %s -f %s %s/%s" % \
          (scr, scale_default, scale_pid, var, outdir, toystat, controlstat, nominalfile, controlfile, figfile, eosrootdir, infile)
  if limits : 
    command += " -l %f -u %f" % (limits[0], limits[1])


  if controlfile not in ready_list : 
    print command
    if ("dry" not in opt.split(":")) : os.system(command)

  systfile    = filename + "_syst_1.root"
  controlfile = "control/" + filename + "_syst_1_control.root"
  figfile     = "plots/" + filename + "_syst_1_control.png"

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
    figfile     = "plots/" + filename + "_stat_%d_control.png" % i
    command = "%s -a %s -r %d -s %f -i %f -v %s -o %s -t %d -p %d -b %s -c %s -f %s %s/%s" % \
            (scr, tmpfile, i+1, scale_default, scale_pid, var, outdir, toystat, controlstat, statfile, controlfile, figfile, eosrootdir, infile)
    if limits : 
      command += " -l %f -u %f" % (limits[0], limits[1])

    if controlfile not in ready_list : 
      print command
      if ("dry" not in opt.split(":")) : os.system(command)
