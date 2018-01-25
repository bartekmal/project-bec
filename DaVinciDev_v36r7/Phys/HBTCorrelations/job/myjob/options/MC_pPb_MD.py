from Configurables import HBT
HBTCorr = HBT("HBTCorr")
HBTCorr.Sim = 1
#HBTCorr.StrippingLine = "StrippingProtonIonMagDownMinBiasLineDecision"

from Configurables import DaVinci
if HBTCorr.Sim == 0:
  DaVinci().Simulation = False
else:
  DaVinci().Simulation = True

DaVinci().DataType = "2012"
DaVinci().DDDBtag = "dddb-20170721-2"
DaVinci().CondDBtag = "sim-20170407-2013pPb-vc-md100"
