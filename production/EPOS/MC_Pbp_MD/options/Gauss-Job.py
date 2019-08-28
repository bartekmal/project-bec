import os
os.environ['APPCONFIGOPTS'] = "/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/AppConfig/v3r335/options"
os.environ['DECFILESROOT'] = "/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/Gen/DecFiles/v30r1"

from Gauss.Configuration import *
from Gaudi.Configuration import *
from  Configurables import LHCbApp

LHCbApp().DDDBtag = 'dddb-20170721-2'
LHCbApp().CondDBtag = 'sim-20170407-2013Pbp-vc-md100'

Gauss().OutputType = 'SIM'

importOptions("$APPCONFIGOPTS/Gauss/Pbp-Beam1580GeV-4000GeV-md100-2013-fix1.py")
importOptions("$APPCONFIGOPTS/Gauss/DataType-2013.py")
importOptions("$APPCONFIGOPTS/Gauss/RICHRandomHits.py")
importOptions("$APPCONFIGOPTS/Gauss/NoPacking.py")
importOptions("$DECFILESROOT/options/30000000.py")
importOptions("$LBCRMCROOT/options/EPOS.py")
importOptions("$APPCONFIGOPTS/Gauss/G4PL_FTFP_BERT_EmNoCuts.py")

