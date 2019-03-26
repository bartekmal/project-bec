"""This file is an example of a user-defined binning scheme file, which """ \
"""can be passed as an argument to the multi-track calibration scripts.
The methods for constructing binning schema are defined in """ \
"""$PIDPERFSCRIPTSROOT/python/PIDPerfScripts/binning.py."""

from PIDPerfScripts.Binning import *
from PIDPerfScripts.Definitions import *

### DLL(K-pi), RICH (alternative schemes)

for trType in ["e","Mu"]:
    # momentum
    AddBinScheme(trType, 'P', 'emu', 3800, 150000)
    AddBinBoundary(trType, 'P', 'emu', 120000)
    AddBinBoundary(trType, 'P', 'emu', 100000)
    AddUniformBins(trType, 'P', 'emu', 10, 3800, 100000)

    # eta
    AddBinScheme(trType, 'ETA', 'emu', 1.5, 5)
    AddBinBoundary(trType, 'ETA', 'emu', 2.2)
    AddBinBoundary(trType, 'ETA', 'emu', 4)
    AddUniformBins(trType, 'ETA', 'emu', 2, 2.2, 4)

    # nTrack
    AddBinScheme(trType, 'nTracks', 'emu', 0, 500)
    AddBinBoundary(trType, 'nTracks', 'emu', 300)
    AddUniformBins(trType, 'nTracks', 'emu', 4, 0, 300)

for trType in ["Pi"]:
    # momentum
    AddBinScheme(trType, 'P', 'emu', 1000, 20000)
    AddBinBoundary(trType, 'P', 'emu', 15000)
    AddUniformBins(trType, 'P', 'emu', 10, 1000, 15000)

    # eta
    AddBinScheme(trType, 'ETA', 'emu', 1.5, 5)
    AddBinBoundary(trType, 'ETA', 'emu', 2.2)
    AddBinBoundary(trType, 'ETA', 'emu', 4)
    AddUniformBins(trType, 'ETA', 'emu', 2, 2.2, 4)

    # nTrack
    AddBinScheme(trType, 'nTracks', 'emu', 0, 500)
    AddBinBoundary(trType, 'nTracks', 'emu', 300)
    AddUniformBins(trType, 'nTracks', 'emu', 4, 0, 300)
 
for trType in ["P_IncLc","P", "K", "P_LcfB"]:
    # momentum
    AddBinScheme(trType, 'P', 'P_Expand', 0, 100000)
    AddUniformBins(trType, 'P', 'P_Expand', 10, 0, 100000)

    # eta
    AddBinScheme(trType, 'ETA', 'P_Expand', 2.0, 5.0)
    AddUniformBins(trType, 'ETA', 'P_Expand', 10, 2.0, 5.0)

    # nTracks
#    AddBinScheme(trType, 'nTracks', 'P_Expand', 0, 700)
#    AddBinBoundary(trType, 'nTracks', 'P_Expand', 300)
#    AddUniformBins(trType, 'nTracks', 'P_Expand', 4, 0, 300)


# example binning scheme using Tesla variables for K/pi/p
for trType in GetRICHPIDPartTypes() :
    # momentum
    AddBinScheme(trType, 'Tesla_P', 'DummyTesla', 3000, 100000)
    AddBinBoundary(trType, 'Tesla_P', 'DummyTesla', 9300) # R1 Kaon threshold
    AddBinBoundary(trType, 'Tesla_P', 'DummyTesla', 15600) # R2 Kaon threshold
    AddUniformBins(trType, 'Tesla_P', 'DummyTesla', 10, 19000, 100000)

    # eta
    AddBinScheme(trType, 'Tesla_ETA', 'DummyTesla', 1.5, 5)
    AddUniformBins(trType, 'Tesla_ETA', 'DummyTesla', 3, 1.5, 5)

    # nTracks
    AddBinScheme(trType, 'nTracks', 'DummyTesla', 0, 500)
    AddBinBoundary(trType, 'nTracks', 'DummyTesla', 50)
    AddBinBoundary(trType, 'nTracks', 'DummyTesla', 200)
    AddBinBoundary(trType, 'nTracks', 'DummyTesla', 300)

# nSPDHits

    AddBinScheme(trType, 'nSPDHits','DummyTesla',0,1000)
    AddUniformBins(trType, 'nSPDHits', 'DummyTesla', 5,0,1000)


for trType in ["Pi","K"]:
    # momentum
    AddBinScheme(trType, 'P', 'Broad', 0, 100000)
    AddUniformBins(trType, 'P', 'Broad', 10, 0, 100000)

    # eta
    AddBinScheme(trType, 'ETA', 'Broad', 2.0, 5.0)
    AddUniformBins(trType, 'ETA', 'Broad', 4, 2.0, 5.0)

    # nTracks
    AddBinScheme(trType, 'nTracks', 'Broad', 0, 500)
#    AddBinBoundary(trType, 'nTracks', 'Broad', 300)
    AddUniformBins(trType, 'nTracks', 'Broad', 4, 0,500)

