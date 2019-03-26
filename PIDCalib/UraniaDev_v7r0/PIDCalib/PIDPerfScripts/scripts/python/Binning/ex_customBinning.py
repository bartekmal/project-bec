"""This file is an example of a user-defined binning scheme file, which """ \
"""can be passed as an argument to the multi-track calibration scripts.
The methods for constructing binning schema are defined in """ \
"""$PIDPERFSCRIPTSROOT/python/PIDPerfScripts/binning.py."""

from PIDPerfScripts.Binning import *
from PIDPerfScripts.Definitions import *


#### proton studies
for trType in ["P","P_IncLc"]:
  AddBinScheme(trType, 'P', 'wideprot',10000,80000);
  AddBinScheme(trType, 'ETA', 'wideprot',2.5,4.5);
  AddBinScheme(trType, 'nTracks', 'wideprot',50,350);

  AddBinScheme(trType, 'P', 'narprot',10000,80000);
  AddBinScheme(trType, 'ETA', 'narprot',2.5,4.5);
  AddBinScheme(trType, 'nTracks', 'narprot',50,350);

  AddBinBoundary(trType, 'P','wideprot',20000)
  AddBinBoundary(trType, 'P','wideprot',30000)
  AddBinBoundary(trType, 'P','wideprot',40000)
  AddBinBoundary(trType, 'P','wideprot',50000)
  AddBinBoundary(trType, 'P','wideprot',60000)
  AddBinBoundary(trType, 'P','wideprot',70000)

  AddBinBoundary(trType, 'P','narprot',20000)
  AddBinBoundary(trType, 'P','narprot',30000)
  AddBinBoundary(trType, 'P','narprot',40000)
  AddBinBoundary(trType, 'P','narprot',50000)
  AddBinBoundary(trType, 'P','narprot',60000)
  AddBinBoundary(trType, 'P','narprot',70000)

  AddBinBoundary(trType, 'P','narprot',15000)
  AddBinBoundary(trType, 'P','narprot',25000)
  AddBinBoundary(trType, 'P','narprot',35000)
  AddBinBoundary(trType, 'P','narprot',45000)
  AddBinBoundary(trType, 'P','narprot',55000)


  AddBinBoundary(trType, 'ETA','wideprot',3.0)
  AddBinBoundary(trType, 'ETA','wideprot',3.5)
  AddBinBoundary(trType, 'ETA','wideprot',4.0)

  AddBinBoundary(trType, 'ETA','narprot',3.0)
  AddBinBoundary(trType, 'ETA','narprot',3.5)
  AddBinBoundary(trType, 'ETA','narprot',4.0)

  AddBinBoundary(trType, 'ETA','narprot',2.75)
  AddBinBoundary(trType, 'ETA','narprot',3.25)
  AddBinBoundary(trType, 'ETA','narprot',3.75)
  AddBinBoundary(trType, 'ETA','narprot',4.25)

  AddBinBoundary(trType, 'nTracks','wideprot',130)
  AddBinBoundary(trType, 'nTracks','wideprot',200)

  AddBinBoundary(trType, 'nTracks','narprot',130)
  AddBinBoundary(trType, 'nTracks','narprot',200)

  AddBinBoundary(trType, 'nTracks','narprot',90)
  AddBinBoundary(trType, 'nTracks','narprot',170)
  AddBinBoundary(trType, 'nTracks','narprot',200)
  AddBinBoundary(trType, 'nTracks','narprot',250)
  AddBinBoundary(trType, 'nTracks','narprot',300)









### DLL(K-pi), RICH (alternative schemes)

AddBinScheme("K",'P', 'sneha',5000,100000)


AddBinBoundary("K",'P', 'sneha', 20000) 
AddBinBoundary("K",'P', 'sneha', 30000) 
AddBinBoundary("K",'P', 'sneha', 40000) 
AddBinBoundary("K",'P', 'sneha', 50000) 
AddBinBoundary("K",'P', 'sneha', 65000) 
AddBinBoundary("K",'P', 'sneha', 80000) 


 


for trType in ["e","Mu","Pi","K"]:
  AddBinScheme(trType, 'P', 'onebin', 3800, 150000)
  AddBinScheme(trType, 'ETA', 'onebin', 1.5, 5.)
  AddBinScheme(trType, 'nTracks', 'onebin', 0, 500)
  for year in [2011,2012]:
    # momentum
    AddBinScheme(trType, 'P', 'emu%se'%year, 3800, 150000)
    AddBinBoundary(trType, 'P', 'emu%se'%year, 10000)
    AddBinBoundary(trType, 'P', 'emu%se'%year, 18000)
    AddBinBoundary(trType, 'P', 'emu%se'%year, 27000)
    AddBinBoundary(trType, 'P', 'emu%se'%year, 42000)
    AddBinBoundary(trType, 'P', 'emu%se'%year, 60000)
    ##AddBinBoundary(trType, 'P', 'emu%s'%year, 120000)
    #AddUniformBins(trType, 'P', 'emu%s'%year, 5, 3800, 50000)
    ##AddUniformBins(trType, 'P', 'emu%s'%year, 3, 50000, 100000)
    #AddBinBoundary(trType, 'P', 'emu%s'%year, 50000)
    #AddBinBoundary(trType, 'P', 'emu%s'%year, 70000)
    #AddBinBoundary(trType, 'P', 'emu%s'%year, 100000)

    # eta
    AddBinScheme(trType, 'ETA', 'emu%se'%year, 1.5, 5)
    AddBinBoundary(trType, 'ETA', 'emu%se'%year, 2.3)
    AddBinBoundary(trType, 'ETA', 'emu%se'%year, 2.5)
    AddBinBoundary(trType, 'ETA', 'emu%se'%year, 2.85)
    AddBinBoundary(trType, 'ETA', 'emu%se'%year, 3.15)
    AddBinBoundary(trType, 'ETA', 'emu%se'%year, 3.45)
    AddBinBoundary(trType, 'ETA', 'emu%se'%year, 3.75)
    #AddBinBoundary(trType, 'ETA', 'emu%s'%year, 2.2)
    #AddBinBoundary(trType, 'ETA', 'emu%s'%year, 4)
    #AddUniformBins(trType, 'ETA', 'emu%s'%year, 4, 2.2, 4)

  year = 2012
  # nTracks
  AddBinScheme(trType, 'nTracks', 'emu%se'%year, 0, 500)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 70)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 100)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 135)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 170)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 220)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 270)

  year = 2011
  AddBinScheme(trType, 'nTracks', 'emu%se'%year, 0, 500)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 60)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 80)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 110)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 140)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 190)
  AddBinBoundary(trType, 'nTracks', 'emu%se'%year, 230)

for trType in ["e","Mu","Pi","K"]:
  for year in [2011,2012]:
    # momentum
    AddBinScheme(trType, 'P', 'emu%smu'%year, 3800, 150000)
    AddBinBoundary(trType, 'P', 'emu%smu'%year, 8000)
    AddBinBoundary(trType, 'P', 'emu%smu'%year, 12000)
    AddBinBoundary(trType, 'P', 'emu%smu'%year, 19000)
    AddBinBoundary(trType, 'P', 'emu%smu'%year, 28000)
    AddBinBoundary(trType, 'P', 'emu%smu'%year, 45000)
    AddBinBoundary(trType, 'P', 'emu%smu'%year, 65000)

    # eta
    AddBinScheme(trType, 'ETA', 'emu%smu'%year, 1.5, 5)
    AddBinBoundary(trType, 'ETA', 'emu%smu'%year, 2.25)
    AddBinBoundary(trType, 'ETA', 'emu%smu'%year, 2.5)
    AddBinBoundary(trType, 'ETA', 'emu%smu'%year, 2.9)
    AddBinBoundary(trType, 'ETA', 'emu%smu'%year, 3.25)
    AddBinBoundary(trType, 'ETA', 'emu%smu'%year, 3.7)
    AddBinBoundary(trType, 'ETA', 'emu%smu'%year, 4.1)

  year = 2012
  # nTracks
  AddBinScheme(trType, 'nTracks', 'emu%smu'%year, 0, 500)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 70)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 100)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 135)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 180)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 240)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 290)

  year = 2011
  AddBinScheme(trType, 'nTracks', 'emu%smu'%year, 0, 500)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 70)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 90)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 120)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 160)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 210)
  AddBinBoundary(trType, 'nTracks', 'emu%smu'%year, 250)







for trType in ["e","Mu","Pi","K"]:
  for year in [2011,2012]:
    AddBinScheme(trType, 'P', 'kpi%spi'%year, 3000, 200000)
    AddBinScheme(trType, 'ETA', 'kpi%spi'%year, 1.5, 5)
    AddBinScheme(trType, 'nTracks', 'kpi%spi'%year, 0, 500)
    AddBinScheme(trType, 'P', 'kpi%sk'%year, 3000, 200000)
    AddBinScheme(trType, 'ETA', 'kpi%sk'%year, 1.5, 5)
    AddBinScheme(trType, 'nTracks', 'kpi%sk'%year, 0, 500)

  AddBinBoundary(trType, 'P', 'kpi2011pi', 9300.000000)
  AddBinBoundary(trType, 'P', 'kpi2011pi', 15600.000000)
  #AddBinBoundary(trType, 'P', 'kpi2011pi', 12629.000000)
  AddBinBoundary(trType, 'P', 'kpi2011pi', 77375.000000)
  AddBinBoundary(trType, 'P', 'kpi2011pi', 16553.000000)
  AddBinBoundary(trType, 'P', 'kpi2011pi', 24401.000000)
  AddBinBoundary(trType, 'P', 'kpi2011pi', 36173.000000)
  AddBinBoundary(trType, 'P', 'kpi2011pi', 55793.000000)
  AddBinBoundary(trType, 'ETA', 'kpi2011pi', 2.462500)
  AddBinBoundary(trType, 'ETA', 'kpi2011pi', 3.967500)
  AddBinBoundary(trType, 'ETA', 'kpi2011pi', 2.707500)
  AddBinBoundary(trType, 'ETA', 'kpi2011pi', 3.022500)
  AddBinBoundary(trType, 'ETA', 'kpi2011pi', 3.337500)
  AddBinBoundary(trType, 'ETA', 'kpi2011pi', 3.687500)
  AddBinBoundary(trType, 'nTracks', 'kpi2011pi', 77.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011pi', 247.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011pi', 97.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011pi', 132.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011pi', 167.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011pi', 212.500000)
  AddBinBoundary(trType, 'P', 'kpi2011k', 9300.000000)
  AddBinBoundary(trType, 'P', 'kpi2011k', 15600.000000)
  #AddBinBoundary(trType, 'P', 'kpi2011k', 14591.000000)
  AddBinBoundary(trType, 'P', 'kpi2011k', 83261.000000)
  AddBinBoundary(trType, 'P', 'kpi2011k', 18515.000000)
  AddBinBoundary(trType, 'P', 'kpi2011k', 28325.000000)
  AddBinBoundary(trType, 'P', 'kpi2011k', 40097.000000)
  AddBinBoundary(trType, 'P', 'kpi2011k', 59717.000000)
  AddBinBoundary(trType, 'ETA', 'kpi2011k', 2.497500)
  AddBinBoundary(trType, 'ETA', 'kpi2011k', 4.002500)
  AddBinBoundary(trType, 'ETA', 'kpi2011k', 2.707500)
  AddBinBoundary(trType, 'ETA', 'kpi2011k', 3.057500)
  AddBinBoundary(trType, 'ETA', 'kpi2011k', 3.372500)
  AddBinBoundary(trType, 'ETA', 'kpi2011k', 3.722500)
  AddBinBoundary(trType, 'nTracks', 'kpi2011k', 77.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011k', 247.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011k', 97.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011k', 132.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011k', 167.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2011k', 212.500000)
  AddBinBoundary(trType, 'P', 'kpi2012pi', 9300.000000)
  AddBinBoundary(trType, 'P', 'kpi2012pi', 15600.000000)
  #AddBinBoundary(trType, 'P', 'kpi2012pi', 10667.000000)
  AddBinBoundary(trType, 'P', 'kpi2012pi', 73451.000000)
  AddBinBoundary(trType, 'P', 'kpi2012pi', 14591.000000)
  AddBinBoundary(trType, 'P', 'kpi2012pi', 24401.000000)
  AddBinBoundary(trType, 'P', 'kpi2012pi', 34211.000000)
  AddBinBoundary(trType, 'P', 'kpi2012pi', 53831.000000)
  AddBinBoundary(trType, 'ETA', 'kpi2012pi', 2.427500)
  AddBinBoundary(trType, 'ETA', 'kpi2012pi', 3.897500)
  AddBinBoundary(trType, 'ETA', 'kpi2012pi', 2.637500)
  AddBinBoundary(trType, 'ETA', 'kpi2012pi', 2.952500)
  AddBinBoundary(trType, 'ETA', 'kpi2012pi', 3.267500)
  AddBinBoundary(trType, 'ETA', 'kpi2012pi', 3.617500)
  AddBinBoundary(trType, 'nTracks', 'kpi2012pi', 82.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012pi', 272.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012pi', 102.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012pi', 142.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012pi', 177.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012pi', 232.500000)
  AddBinBoundary(trType, 'P', 'kpi2012k', 9300.000000)
  AddBinBoundary(trType, 'P', 'kpi2012k', 15600.000000)
  #AddBinBoundary(trType, 'P', 'kpi2012k', 12629.000000)
  AddBinBoundary(trType, 'P', 'kpi2012k', 79337.000000)
  AddBinBoundary(trType, 'P', 'kpi2012k', 16553.000000)
  AddBinBoundary(trType, 'P', 'kpi2012k', 26363.000000)
  AddBinBoundary(trType, 'P', 'kpi2012k', 38135.000000)
  AddBinBoundary(trType, 'P', 'kpi2012k', 57755.000000)
  AddBinBoundary(trType, 'ETA', 'kpi2012k', 2.462500)
  AddBinBoundary(trType, 'ETA', 'kpi2012k', 3.897500)
  AddBinBoundary(trType, 'ETA', 'kpi2012k', 2.672500)
  AddBinBoundary(trType, 'ETA', 'kpi2012k', 2.987500)
  AddBinBoundary(trType, 'ETA', 'kpi2012k', 3.302500)
  AddBinBoundary(trType, 'ETA', 'kpi2012k', 3.652500)
  AddBinBoundary(trType, 'nTracks', 'kpi2012k', 82.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012k', 272.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012k', 102.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012k', 142.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012k', 177.500000)
  AddBinBoundary(trType, 'nTracks', 'kpi2012k', 232.500000)





for trType in ["e","Mu","Pi","K"]:
  for year in [2011,2012]:
    # momentum
    AddBinScheme(trType, 'P', 'emu%spi'%year, 3800, 200000)
    AddBinBoundary(trType, 'P', 'emu%spi'%year, 8000)
    AddBinBoundary(trType, 'P', 'emu%spi'%year, 12000)
    AddBinBoundary(trType, 'P', 'emu%spi'%year, 19000)
    AddBinBoundary(trType, 'P', 'emu%spi'%year, 28000)
    AddBinBoundary(trType, 'P', 'emu%spi'%year, 45000)
    AddBinBoundary(trType, 'P', 'emu%spi'%year, 65000)

    # eta
    AddBinScheme(trType, 'ETA', 'emu%spi'%year, 1.5, 5)
    AddBinBoundary(trType, 'ETA', 'emu%spi'%year, 2.3)
    AddBinBoundary(trType, 'ETA', 'emu%spi'%year, 2.55)
    AddBinBoundary(trType, 'ETA', 'emu%spi'%year, 2.7)
    AddBinBoundary(trType, 'ETA', 'emu%spi'%year, 3.05)
    AddBinBoundary(trType, 'ETA', 'emu%spi'%year, 3.4)
    AddBinBoundary(trType, 'ETA', 'emu%spi'%year, 3.7)
    AddBinBoundary(trType, 'ETA', 'emu%spi'%year, 4.0)

  year = 2012
  # nTracks
  AddBinScheme(trType, 'nTracks', 'emu%spi'%year, 0, 500)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 70)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 100)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 135)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 180)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 240)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 290)

  year = 2011
  AddBinScheme(trType, 'nTracks', 'emu%spi'%year, 0, 500)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 70)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 90)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 120)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 160)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 210)
  AddBinBoundary(trType, 'nTracks', 'emu%spi'%year, 250)


for trType in ["e","Mu","Pi","K"]:
  for i in ["e","mu"]:
    AddBinScheme(trType, 'P', '100bins%s'%(i), 3800, 150000)
    AddUniformBins(trType, 'P', '100bins%s'%(i), 100, 3800, 150000)

    AddBinScheme(trType, 'ETA', '100bins%s'%(i), 1.5, 5)
    AddUniformBins(trType, 'ETA', '100bins%s'%(i), 100, 1.5, 5)

    AddBinScheme(trType, 'nTracks', '100bins%s'%(i), 0, 500)
    AddUniformBins(trType, 'nTracks', '100bins%s'%(i), 100, 0, 500)
    
for trType in ["e","Mu","Pi","K"]:
  for i in ["pi","k"]:
    AddBinScheme(trType, 'P', '100bins%s'%(i), 3800, 200000)
    AddUniformBins(trType, 'P', '100bins%s'%(i), 100, 3800, 200000)

    AddBinScheme(trType, 'ETA', '100bins%s'%(i), 1.5, 5)
    AddUniformBins(trType, 'ETA', '100bins%s'%(i), 100, 1.5, 5)

    AddBinScheme(trType, 'nTracks', '100bins%s'%(i), 0, 500)
    AddUniformBins(trType, 'nTracks', '100bins%s'%(i), 100, 0, 500)

for trType in ["e","Mu","Pi","K"]:
  AddBinScheme(trType, 'P', '100binsspi', 1000, 20000)
  AddUniformBins(trType, 'P', '100binsspi', 100, 1000, 20000)

  AddBinScheme(trType, 'ETA', '100binsspi', 1.5, 5)
  AddUniformBins(trType, 'ETA', '100binsspi', 100, 1.5, 5)

  AddBinScheme(trType, 'nTracks', '100binsspi', 0, 500)
  AddUniformBins(trType, 'nTracks', '100binsspi', 100, 0, 500)

for trType in ["P"]:
  AddBinScheme(trType, 'P', 'P_comp',5000,100000)
  AddBinScheme(trType, 'ETA', 'P_comp',2,4.6)
  AddUniformBins(trType,'P','P_comp',19,5000,100000)
  AddUniformBins(trType,'ETA','P_comp',13,2,4.6)
