# #####################################################################
# Clone Checker
#
# @author: Marcin Kucharczyk
# @date 2014-02-10
# 
## #####################################################################
from os import environ, path
from math import *
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from GaudiKernel.SystemOfUnits import *
from CommonParticles.Utils import *
from Configurables import CloneChecker
## #####################################################################
clChecker = CloneChecker("clChecker")
clChecker.Inputs = ["Phys/StdAllNoPIDsPions/Particles",
                    "Phys/StdLooseAllPhotons/Particles"]
clChecker.partsToCheck = 1  # 1-pions, 2-kaons, 3-protons 
clChecker.TrkChi2    = 2.6
clChecker.IP         = 0.2
clChecker.IPChi2     = 2.6
clChecker.piNN       = 0.0
clChecker.kNN        = 0.0
clChecker.pNN        = 0.0
clChecker.ghostNN    = 0.25
clChecker.partP      = 2.0
clChecker.partPt     = 0.1
## #####################################################################
DVSeq = GaudiSequencer("DVSeq")
DVSeq.IgnoreFilterPassed = False
DVSeq.Members += [clChecker]
## #####################################################################
from Configurables import DaVinci
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 10000
DaVinci().SkipEvents = 0
DaVinci().DataType = "2011"
DaVinci().Simulation = True
DaVinci().HistogramFile = "clChecker.root"
DaVinci().TupleFile = "clChecker.root"
DaVinci().UserAlgorithms = [DVSeq]
########################################################################
