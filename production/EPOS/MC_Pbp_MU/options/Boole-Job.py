import os
os.environ['APPCONFIGOPTS'] = "/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/AppConfig/v3r321/options"

from  Configurables import LHCbApp
LHCbApp().DDDBtag = 'dddb-20170721-2'
LHCbApp().CondDBtag = 'sim-20170407-2013Pbp-vc-mu100'

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Boole/Default.py")
importOptions("$APPCONFIGOPTS/Boole/DataType-2012.py")
importOptions("$APPCONFIGOPTS/Boole/NoPacking.py")
importOptions("$APPCONFIGOPTS/Boole/Boole-SetOdinRndTrigger.py")
importOptions("$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py")

from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['PFN:Gauss.sim'], clear=True) 


