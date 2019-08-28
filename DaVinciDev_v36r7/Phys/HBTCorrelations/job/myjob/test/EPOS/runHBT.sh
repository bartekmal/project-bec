#!/bin/sh

#setenv
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh 
SetupProject DaVinci v36r7
MYDIR=/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/EPOS
SRC=${MYDIR}/HBTAnalysis

#compile
cd ${SRC}
CODE=HBTAnalysis
rm $CODE.exe
g++ -std=c++11  -pthread -m64 -I/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/include $CODE.cpp -o $CODE.exe -L/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore  -pthread -lm -ldl -rdynamic -lTreePlayer -lMinuit -lTMVA -lXMLIO -lMLP


#run
#MODE="Pbp_MU"
cd ${MYDIR}; rm output; mkdir output
for MODE in "Pbp_MU" "pPb_MU" "pPb_MD" "Pbp_MD"; do

cd ${MYDIR}/output; mkdir ${MODE}; cd ${MODE}

for PROD in "local" "central"; do

echo "Running HBTAnalysis for ${PROD} ${MODE} in:"
echo "${PWD}"
sbatch -p plgrid-short -t 01:00:00  ${SRC}/${CODE}.sh ${PROD} ${MODE} 

done

done
