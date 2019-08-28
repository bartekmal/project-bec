import os
os.environ['APPCONFIGOPTS'] = "/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/AppConfig/v3r329/options"
os.environ['SQLDDDBROOT'] = "/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/Det/SQLDDDB/v7r9"

from  Configurables import LHCbApp
LHCbApp().DDDBtag = 'dddb-20170721-2'
LHCbApp().CondDBtag = 'sim-20170407-2013Pbp-vc-mu100'

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Brunel/DataType-2012.py")
importOptions("$APPCONFIGOPTS/Brunel/pA-GECs.py")
importOptions("$APPCONFIGOPTS/Brunel/MC-WithTruth.py")
importOptions("$APPCONFIGOPTS/Brunel/Sim09-Run1.py")
importOptions("$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py")

from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['PFN:Boole.digi'], clear=True)






