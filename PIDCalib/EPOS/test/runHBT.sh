#!/bin/sh
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh
SetupProject DaVinci v36r7

#edit
BASE=${PWD}
cd scripts

CODE=HBTAnalysis
#SetupProject DaVinci v36r7
rm $CODE.exe
#g++ -std=c++11 -pthread -m64 -I/cvmfs/lhcb.cern.ch/lib/lcg/app/releases/ROOT/6.02.08/x86_64-slc6-gcc48-opt/include $CODE.cpp -o $CODE.exe -L/cvmfs/lhcb.cern.ch/lib/lcg/app/releases/ROOT/6.02.08/x86_64-slc6-gcc48-opt/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore -lCint  -pthread -lm -ldl -rdynamic -lTreePlayer -lMinuit -lTMVA -lXMLIO -lMLP
g++ -std=c++11  -pthread -m64 -I/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/include $CODE.cpp -o $CODE.exe -L/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore  -pthread -lm -ldl -rdynamic -lTreePlayer -lMinuit -lTMVA -lXMLIO -lMLP

cd ${BASE}; mkdir output; cd output

for MODE in  "pPb_MU" "pPb_MD" "Pbp_MU" "Pbp_MD"; do

  ${BASE}/scripts/$CODE.exe "RD" "central" "${MODE}" > "RD_${MODE}.log" &
  ${BASE}/scripts/$CODE.exe "MC" "central" "${MODE}" > "MC_central_${MODE}.log" &
  ${BASE}/scripts/$CODE.exe "MC" "local" "${MODE}" > "MC_local_${MODE}.log" &


done


