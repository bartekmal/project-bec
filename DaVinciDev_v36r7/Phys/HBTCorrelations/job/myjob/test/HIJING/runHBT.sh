#!/bin/sh

#set SRC, QUEUE, MAXTIME, MYDIR

#setenv BEC
BEC_SET=0
if [ "${USER}" = "bmalecki" ] ; then
  source /afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb/setupBEC.sh
  BEC_SET=1
fi
if [ "${USER}" = "plgbmalecki" ] ; then
  source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh
  BEC_SET=1
fi

if [ "$BEC_SET" = "0" ]; then
  echo "BEC environemnt not set! -> exiting"
  exit
fi

#config job
SRC=${BASE_BEC}/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/HIJING/HBTAnalysis
MYDIR=${PWD}
if [ "${USER}" = "plgbmalecki" ] ; then
  QUEUE="plgrid"
  MAXTIME="01:00:00"
fi
if [ "${USER}" = "bmalecki" ] ; then
  QUEUE="1nh"
#  MAXTIME="01:00:00"
fi

#compile
cd ${SRC}
CODE=HBTAnalysis
rm $CODE.exe
${BASE_BEC}/DaVinciDev_v36r7/run g++ -std=c++11 -O2  -pthread -m64 -I/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/include $CODE.cpp -o $CODE.exe -L/cvmfs/lhcb.cern.ch/lib/lcg/releases/LCG_76root6/ROOT/6.02.08/x86_64-slc6-gcc49-opt/lib -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore  -pthread -lm -ldl -rdynamic -lTreePlayer -lMinuit -lTMVA -lXMLIO -lMLP

cd ${MYDIR}; mkdir output;

for PROD in "local" "Oscar"; do
  cd ${MYDIR}/output; mkdir ${PROD}; cd ${PROD}
  for BEAM in "Pbp" "pPb"; do
    for MAGNET in "MD"; do

      sbatch -p ${QUEUE} -t ${MAXTIME} -J ${BEAM}_${MAGNET}_${PROD} ${SRC}/${CODE}.sh ${SRC} ${BEAM} ${MAGNET} ${PROD}
      #${SRC}/${CODE}.sh ${SRC} ${BEAM} ${MAGNET} ${PROD}


    done
  done
done
