#-- GAUDI jobOptions generated on Wed Oct 25 14:59:40 2017
#-- Contains event types : 
#--   90000000 - 5259 files - 143235880 events - 25552.25 GBytes


#--  Extra information about the data processing phases:


#--  Processing Pass Step-129613 

#--  StepId : 129613 
#--  StepName : Stripping20r3p1-Merging-DV-v32r2p18 
#--  ApplicationName : DaVinci 
#--  ApplicationVersion : v32r2p18 
#--  OptionFiles : $APPCONFIGOPTS/Merging/DV-Stripping-Merging.py 
#--  DDDB : dddb-20120831 
#--  CONDDB : cond-20130404 
#--  ExtraPackages : AppConfig.v3r273 
#--  Visible : N 

from Configurables import HBT
HBTCorr = HBT("HBTCorr")
HBTCorr.Sim = 0
HBTCorr.StrippingLine = "StrippingProtonIonMagDownMinBiasLineDecision"

from Configurables import DaVinci
if HBTCorr.Sim == 0:
  DaVinci().Simulation = False
else:
  DaVinci().Simulation = True

DaVinci().DataType = "2011"


from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper().inputFiles([
	'${BASE_BEC_EOS}/dst/test/pPb/00032900_00000001_1.all.dst'
	,'${BASE_BEC_EOS}/dst/test/pPb/00032900_00000002_1.all.dst'
	,'${BASE_BEC_EOS}/dst/test/pPb/00032900_00000003_1.all.dst'
	,'${BASE_BEC_EOS}/dst/test/pPb/00032900_00000004_1.all.dst'
	,'${BASE_BEC_EOS}/dst/test/pPb/00032900_00000005_1.all.dst'
], clear=True)
