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
