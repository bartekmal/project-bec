##############################################################################
# File for running Gauss with latest tags
# Hijing MC - pA
#
##############################################################################

import sys
import GaudiPython
from pA_MCparameters_v12 import *

DEBUG=0

print "sys.argv", sys.argv
pars,Def=params(DEBUG,sys.argv)
print pars

nEvts=int(pars["snEvts"])
RunNumber=int(pars["sRunNumber"])
Config_pA=pars["Config_pA"]
Config_M=pars["Config_M"]

#Gauss-Job.py
from Configurables import Gauss
theApp = Gauss()

##############################################################################
# latest database tags 
##############################################################################

from Configurables import DDDBConf, CondDB, LHCbApp
DDDBConf(DataType = "2012")
LHCbApp().DDDBtag   = "dddb-20130929-1"
if Config_M=="MD": LHCbApp().CondDBtag = "sim-20130522-1-vc-md100"
else: LHCbApp().CondDBtag = "sim-20130522-1-vc-mu100"


from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = RunNumber

GaussGen.OutputLevel = 1

LHCbApp().EvtMax = nEvts

Gauss().Histograms = 'NONE'

FileName=pars["DataType"]+"_"+pars["version"]+"_"+pars["Config_pA"]+"_"+pars["Config_M"]
noPrintInfo=["version", "snEvts", "Config_pA", "Config_M", "year", "Simulation", "DataType", "hipr1_2", "hipr1_4", "hipr1_6", "hipr1_8"]
for p in pars.keys():
    if p in noPrintInfo: continue
    if pars[p] != Def[p]: FileName+="_"+p+"-"+pars[p]

Gauss().DatasetName="GaussSim_"+FileName
OutputStream("GaussTape").Output = "DATAFILE='PFN:/scratch25/pA/Gauss/"+FileName+".sim' TYP='POOL_ROOTTREE' OPT='RECREATE'"


#30000000.py

from Gaudi.Configuration import *
from Configurables import Generation
from Configurables import MinimumBias
from Configurables import HijingProduction
from Configurables import ToolSvc
from Configurables import EvtGenDecay

# This replaces the event type options: 60001....
Generation().EventType = 30000000
ToolSvc().addTool( EvtGenDecay )
ToolSvc().EvtGenDecay.UserDecayFile = "$DECFILESROOT/dkfiles/minbias.dec"

Generation().addTool( MinimumBias )
Generation().MinimumBias.CutTool = ""
Generation().MinimumBias.addTool( HijingProduction )

# Beam p on lead in cms
BeamCommands=[]
if Config_pA=="pA":
    BeamCommands+=[
        "hijinginit proj P",
        "hijinginit targ A",
        "hijinginit iap 1",
        "hijinginit izp 1", 
        "hijinginit iat 208",
        "hijinginit izt 82"]
else:
    BeamCommands+=[
        "hijinginit proj A",
        "hijinginit targ P",
        "hijinginit iap 208",
        "hijinginit izp 82", 
        "hijinginit iat 1",
        "hijinginit izt 1"]

ParCommands=[]
for p in pars.keys():
    if ("hipr1" in p) or ("ihpr2" in p):
        ParCommands+=["hiparnt "+p.split("_")[0]+" "+p.split("_")[1]+" "+pars[p]]

Generation().MinimumBias.HijingProduction.Commands += [
  "hijinginit frame CMS",
  "hijinginit beam1",
  "hijinginit bmin 0.",
  "hijinginit bmax 10."]
Generation().MinimumBias.HijingProduction.Commands += BeamCommands
Generation().MinimumBias.HijingProduction.Commands += ParCommands



# This replaces the 'use hijing': Hijing.opts
ApplicationMgr().Dlls += [ "LbHijing" ] # <===
Generation().SampleGenerationTool = "MinimumBias"
Generation().MinimumBias.ProductionTool = "HijingProduction" # <===
Generation().PileUpTool = "FixedNInteractions"
#Generation().PileUpTool.NInteractions = 1
Generation().DecayTool = ""


print "\n", "#"*60
print "\n", Generation()
print "\n", Generation().MinimumBias
print "\n", Generation().MinimumBias.HijingProduction.Commands
print "#"*60, "\n"




#Beam5023GeV-md100-2013-nu1.0-pA.py

#from Configurables import EventClockSvc, FakeEventTime
from GaudiKernel import SystemOfUnits

#--Set the L/nbb, total cross section and revolution frequency and configure
#--the pileup tool
#-- nu=2.5 (i.e. mu=1.76, L/bunch below)
# FIXME divide Lumi by 2.5?!

#Not changing this. Forcing 1 interaction per bunch crossing
Gauss().Luminosity        = 1.086*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s) #MD Ap = 0.122 / MU pA = 0.302 / MU Ap = 0.122
Gauss().TotalCrossSection = 3768*SystemOfUnits.millibarn 


if Config_pA=="pA":
    if Config_M=="MD":  
        #--Set the average position of the IP -> better z=-3.0
        Gauss().InteractionPosition = [  0.680*SystemOfUnits.mm , #MD Ap = 0.690 / MU pA = 0.697 / MU Ap = 0.688
                                         0.210*SystemOfUnits.mm , #MD Ap = 0.190 / MU pA = 0.209 / MU Ap = 0.199
                                         2.570*SystemOfUnits.mm ] #MD Ap = -1.000 / MU pA = 3.815 / MU Ap = 3.200
        
        #--Set the bunch RMS, this will be used for calculating the sigmaZ of the
        #Interaction Region. SigmaX and SigmaY are calculated from the beta* and
        #emittance
        Gauss().BunchRMS = 45.00*SystemOfUnits.mm #MD Ap = 40.00 / MU pA = 60.00 / MU Ap = 60.00
    else:
        Gauss().InteractionPosition = [  0.697*SystemOfUnits.mm , #MD Ap = 0.690 / MU pA = 0.697 / MU Ap = 0.688
                                         0.209*SystemOfUnits.mm , #MD Ap = 0.190 / MU pA = 0.209 / MU Ap = 0.199
                                         3.815*SystemOfUnits.mm ] #MD Ap = -1.000 / MU pA = 3.815 / MU Ap = 3.200
        
        Gauss().BunchRMS = 60.00*SystemOfUnits.mm #MD Ap = 40.00 / MU pA = 60.00 / MU Ap = 60.00

if Config_pA=="Ap":
    if Config_M=="MD":  
        Gauss().InteractionPosition = [  0.690*SystemOfUnits.mm , #MD Ap = 0.690 / MU pA = 0.697 / MU Ap = 0.688
                                         0.190*SystemOfUnits.mm , #MD Ap = 0.190 / MU pA = 0.209 / MU Ap = 0.199
                                         -3.332*SystemOfUnits.mm ] #MD Ap = -1.000 / MU pA = 3.815 / MU Ap = 3.200

        Gauss().BunchRMS = 40.00*SystemOfUnits.mm #MD Ap = 40.00 / MU pA = 60.00 / MU Ap = 60.00
    else:
        Gauss().InteractionPosition = [  0.688*SystemOfUnits.mm , #MD Ap = 0.690 / MU pA = 0.697 / MU Ap = 0.688
                                         0.199*SystemOfUnits.mm , #MD Ap = 0.190 / MU pA = 0.209 / MU Ap = 0.199
                                         3.200*SystemOfUnits.mm ] #MD Ap = -1.000 / MU pA = 3.815 / MU Ap = 3.200

        Gauss().BunchRMS = 60.00*SystemOfUnits.mm #MD Ap = 40.00 / MU pA = 60.00 / MU Ap = 60.00


#--Set the energy of the beam,
#--the half effective crossing angle (in LHCb coordinate system),
#--beta* and emittance (e_norm ~ 2 microns)
Gauss().BeamMomentum      = 5.023*SystemOfUnits.TeV

if Config_M=="MD":
    Gauss().BeamHCrossingAngle = -0.236*SystemOfUnits.mrad #MU = 0.236
else:
    Gauss().BeamHCrossingAngle = 0.236*SystemOfUnits.mrad 

Gauss().BeamVCrossingAngle =  0.100*SystemOfUnits.mrad
Gauss().BeamLineAngles    = [0.0, 0.0]

Gauss().BeamEmittance     = 0.0022*SystemOfUnits.mm
Gauss().BeamBetaStar      = 3.0*SystemOfUnits.m


Gauss().OutputType = "XSIM"



gaudi = GaudiPython.AppMgr()
#gaudi.config()
gaudi.initialize()
gaudi.run(nEvts)
gaudi.stop()
gaudi.finalize()
