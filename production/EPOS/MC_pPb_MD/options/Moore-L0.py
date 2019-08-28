import os
os.environ['APPCONFIGOPTS'] = "/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/AppConfig/v3r316/options"

from  Configurables import L0App
L0App().DDDBtag = 'dddb-20170721-2'
L0App().CondDBtag = 'sim-20170407-2013pPb-vc-mu100'

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/L0App/L0AppSimProduction.py")
importOptions("$APPCONFIGOPTS/L0App/L0AppTCK-0x1710.py")
importOptions("$APPCONFIGOPTS/L0App/DataType-2012.py")
importOptions("$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py")

from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['PFN:Boole.digi'], clear=True)
from Configurables import Moore
fileList = ['Boole.digi']
Moore().inputFiles = fileList
Moore().outputFile = Moore().inputFiles[0].replace('Boole','Moore')

