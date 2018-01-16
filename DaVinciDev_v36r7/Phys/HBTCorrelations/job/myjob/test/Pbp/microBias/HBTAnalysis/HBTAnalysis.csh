#!/bin/sh
source /afs/cern.ch/user/b/bmalecki/myscripts/setup/BEC.sh
source /cvmfs/lhcb.cern.ch/lib/LbLogin.sh -c x86_64-slc6-gcc49-opt
SetupProject DaVinci v36r7
#source /cvmfs/lhcb.cern.ch/lib/lcg/app/releases/ROOT/5.34.10/x86_64-slc6-gcc48-opt/root/bin/thisroot.sh

#edit
cd ${BASE_BEC_PROD}/test/Pbp/microBias/HBTAnalysis

CODE=HBTAnalysis
#SetupProject DaVinci v36r7
rm $CODE.exe
#g++ -std=c++11 -pthread -m64 -I/cvmfs/lhcb.cern.ch/lib/lcg/app/releases/ROOT/6.02.08/x86_64-slc6-gcc48-opt/include $CODE.cpp -o $CODE.exe -L/cvmfs/lhcb.cern.ch/lib/lcg/app/releases/ROOT/6.02.08/x86_64-slc6-gcc48-opt/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore -lCint  -pthread -lm -ldl -rdynamic -lTreePlayer -lMinuit -lTMVA -lXMLIO -lMLP
g++ -std=c++11  -pthread -m64 -I/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/include $CODE.cpp -o $CODE.exe -L/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore  -pthread -lm -ldl -rdynamic -lTreePlayer -lMinuit -lTMVA -lXMLIO -lMLP
#edit
./$CODE.exe "RD" > "basicVars.log" 
