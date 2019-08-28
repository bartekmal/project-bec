import os
os.environ['APPCONFIGOPTS'] = "/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/AppConfig/v3r323/options"

from  Configurables import L0App
L0App().DDDBtag = 'dddb-20170721-2'
L0App().CondDBtag = 'sim-20170407-2013pPb-vc-mu100'

from Gaudi.Configuration import *
importOptions("$APPCONFIGOPTS/Moore/MooreSimProductionForSeparateL0AppStep.py")
importOptions("$APPCONFIGOPTS/Conditions/TCK-0x406a1710.py")
importOptions("$APPCONFIGOPTS/Moore/DataType-2012.py")
importOptions("$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py")


from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles(['PFN:Moore.digi'], clear=True)
from Configurables import Moore
fileList = ['Moore.digi']
Moore().inputFiles = fileList
Moore().outputFile = Moore().inputFiles[0].replace('Moore','Moore_Flagged')

