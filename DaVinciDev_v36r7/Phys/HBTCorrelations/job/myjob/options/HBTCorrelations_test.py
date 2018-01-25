# #####################################################################
# HBT correlations
#
# @author: Marcin Kucharczyk
# @date 2013-04-08
# 
## #####################################################################
from os import environ, path
from math import *
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
from CommonParticles.Utils import *
from Configurables import HBT
## #####################################################################
HBTCorr = HBT("HBTCorr")
HBTCorr.Inputs = ["Phys/StdAllNoPIDsPions/Particles",
                  "Phys/StdLooseAllPhotons/Particles"]
#HBTCorr.Sim        = 0
HBTCorr.TrkChi2    = 2.6
HBTCorr.IP         = 0.4
HBTCorr.IPChi2     = 999999.9
HBTCorr.piNN       = 0.0
HBTCorr.kNN        = 0.0
HBTCorr.pNN        = 0.0
HBTCorr.ghostNN    = 0.5
HBTCorr.partP      = 2.0
HBTCorr.partPt     = 0.1
## #####################################################################
DVSeq = GaudiSequencer("DVSeq")
DVSeq.IgnoreFilterPassed = False
DVSeq.Members += [HBTCorr]
## #####################################################################
from Configurables import DaVinci
DaVinci().EvtMax = 1000
DaVinci().PrintFreq = 10000
DaVinci().SkipEvents = 0
#DaVinci().DataType = "2011"
#DaVinci().Simulation = False
DaVinci().HistogramFile = "hbtHisto.root"
DaVinci().TupleFile = "hbtNTuple.root"
DaVinci().UserAlgorithms = [DVSeq]
########################################################################

