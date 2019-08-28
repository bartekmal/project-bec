import os
os.environ['APPCONFIGOPTS'] = "/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/AppConfig/v3r323/options"

from  Configurables import LHCbApp
LHCbApp().DDDBtag = 'dddb-20170721-2'
LHCbApp().CondDBtag = 'sim-20170407-2013Pbp-vc-mu100'

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/DaVinci/DV-Stripping20r3p1-Stripping-MC-NoPrescaling.py")
importOptions("$APPCONFIGOPTS/DaVinci/DataType-2012.py")
importOptions("$APPCONFIGOPTS/DaVinci/InputType-DST.py")
importOptions("$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py")


from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['PFN:Brunel.dst'], clear=True)



