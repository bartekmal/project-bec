#!/bin/sh
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh 
SetupProject DaVinci v36r7

#edit
cd /mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/data/Pbp_MD/HBTAnalysis

CODE=HBTAnalysis
#SetupProject DaVinci v36r7
rm $CODE.exe
#g++ -std=c++11 -pthread -m64 -I/cvmfs/lhcb.cern.ch/lib/lcg/app/releases/ROOT/6.02.08/x86_64-slc6-gcc48-opt/include $CODE.cpp -o $CODE.exe -L/cvmfs/lhcb.cern.ch/lib/lcg/app/releases/ROOT/6.02.08/x86_64-slc6-gcc48-opt/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore -lCint  -pthread -lm -ldl -rdynamic -lTreePlayer -lMinuit -lTMVA -lXMLIO -lMLP
g++ -std=c++11  -pthread -m64 -I/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/include $CODE.cpp -o $CODE.exe -L/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore  -pthread -lm -ldl -rdynamic -lTreePlayer -lMinuit -lTMVA -lXMLIO -lMLP
#edit
./$CODE.exe "RD" > "basicVars.log" &
#./$CODE.exe "MC" > "basicVars_central.log" &
