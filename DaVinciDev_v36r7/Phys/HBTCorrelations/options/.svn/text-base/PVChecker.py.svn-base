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
from Configurables import PVChecker
## #####################################################################
PVCh = PVChecker("PVCh")
PVCh.Inputs = ["Phys/StdAllNoPIDsPions/Particles",
               "Phys/StdLooseAllPhotons/Particles"]
PVCh.Presel     = 0
PVCh.TrkChi2    = 2.6
PVCh.IP         = 0.2
PVCh.IPChi2     = 2.6
PVCh.piNN       = 0.0
PVCh.kNN        = 0.0
PVCh.pNN        = 0.0
PVCh.ghostNN    = 0.25
PVCh.partP      = 2.0
PVCh.partPt     = 0.1
## #####################################################################
DVSeq = GaudiSequencer("DVSeq")
DVSeq.IgnoreFilterPassed = False
DVSeq.Members += [PVCh]
## #####################################################################
from Configurables import DaVinci
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 10000
DaVinci().SkipEvents = 0
DaVinci().DataType = "2011"
DaVinci().Simulation = True
DaVinci().HistogramFile = "hbtHisto.root"
DaVinci().TupleFile = "hbtNTuple.root"
DaVinci().UserAlgorithms = [DVSeq]
########################################################################
