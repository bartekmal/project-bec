import os, sys
from PlotPIDComparison import plotComparison
#from PlotPIDComparison3D import plotComparison3D

from Config import *

config = None
if len(sys.argv)>1 : 
  config = sys.argv[1]
else : 
  print "Usage: ComparePDFs.py [config] [datasets]"
  print "  configs are: "
  for i in sorted(configs.keys()) : 
    print "    ", i
  sys.exit(0)

var = config
bins = configs[config]["bins"]
syst = 1
stat = 10
if 'syst' in configs[config] : syst = configs[config]['syst']
if 'stat' in configs[config] : stat = configs[config]['stat']
limits = None
if 'limits' in configs[config] : limits = configs[config]['limits']

if len(sys.argv)>2 : 
  datasets = sys.argv[2].split(":")
else : 
  datasets = samples[configs[config]["sample"]]['datasets'].keys()

# Do not plot for now
syst = None
stat = None

outdir = eosrootdir + "/" + config

filelist = []

for ds in datasets : 
    calibfile = "%s/%s.root" % (outdir, ds)
    name = ds
    filelist   += [ ( calibfile, outdir + "/control/" + name + "_control.root", "plots/" + var + "_" + name) ] 
    if stat : 
      for i in range(0, stat) : 
        name = "%s_stat_%d" % (ds, i)
        filelist += [ ( calibfile, outdir + "/control/" + name + "_control.root", "plots/" + var + "_" + name) ] 
    if syst : 
      for i in range(1, syst+1) : 
        name = "%s_syst_%d" % (ds, i)
        filelist += [ ( calibfile, outdir + "/control/" + name + "_control.root", "plots/" + var + "_" + name) ] 

for f in filelist : 
  print f
  if config in ["gamma_CL"] : 
    plotComparison3D(f[0], f[1], f[2], var, bins)
  else : 
    if limits : 
      plotComparison(f[0], f[1], f[2], var, bins, limits[0], limits[1] )
    else : 
      plotComparison(f[0], f[1], f[2], var, bins)
#  os.system("eos cp %s %s/plots/%s" % (f[2], eosdir, ))
