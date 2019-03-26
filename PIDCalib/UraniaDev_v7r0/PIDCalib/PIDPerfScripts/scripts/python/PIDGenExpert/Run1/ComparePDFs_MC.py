import os, sys
from PlotPIDComparison import plotComparison

import ConfigMC as ConfigMCSim08
import ConfigMCSim09 as ConfigMCSim09

config = None
if len(sys.argv)>2 : 
  simversion = sys.argv[1]
  configname = sys.argv[2]
else : 
  print "Usage: ComparePDFs.py [sim08/sim09] [config]"
  print "  configs for sim08 are: "
  for i in sorted(ConfigMCSim08.configs.keys()) : 
    print "    ", i
  print "  configs for sim09 are: "
  for i in sorted(ConfigMCSim09.configs.keys()) : 
    print "    ", i
  sys.exit(0)

if simversion == "sim08" : Config = ConfigMCSim08
elif simversion == "sim09" : Config = ConfigMCSim09
else : 
  print "Simulation version %s unknown" % simversion
  sys.exit(0)

eosdir = Config.eosdir
eosrootdir = Config.eosrootdir

var = configname
config = Config.configs[configname]
bins = config["bins"]
syst = 1
stat = 10
if 'syst' in config : syst = config['syst']
if 'stat' in config : stat = config['stat']
limits = None
if 'limits' in config : limits = config['limits']

polarity = [ "MagDown", "MagUp" ]
year = [ 2011, 2012 ]
#polarity = [ "MagDown"]
#year = [ 2012 ]

# Do not plot for now
syst = None
stat = None

outdir = eosrootdir + "/" + configname

filelist = []

for p in polarity : 
  for y in year : 
    calibfile = "%s/%s_%s.root" % (outdir, p, y)
    name = "%s_%s" % (p, y)
    filelist   += [ ( calibfile, outdir + "/control/" + name + "_control.root", "plots_mc/" + var + "_" + name) ] 
    if stat : 
      for i in range(0, stat) : 
        name = "%s_%s_stat_%d" % (p, y, i)
        filelist += [ ( calibfile, outdir + "/control/" + name + "_control.root", "plots_mc/" + var + "_" + name) ] 
    if syst : 
      for i in range(1, syst+1) : 
        name = "%s_%s_syst_%d" % (p, y, i)
        filelist += [ ( calibfile, outdir + "/control/" + name + "_control.root", "plots_mc/" + var + "_" + name) ] 

for f in filelist : 
  print f
  if limits : 
    plotComparison(f[0], f[1], f[2], var, bins, limits[0], limits[1] )
  else : 
    plotComparison(f[0], f[1], f[2], var, bins)
#  os.system("eos cp %s %s/plots/%s" % (f[2], eosdir, ))
