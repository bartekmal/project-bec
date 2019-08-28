#!/bin/sh

#setenv
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh 
SetupProject DaVinci v36r7
MYDIR=/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/EPOS
SRC=${MYDIR}/HBTAnalysis

#run
#MODE="Pbp_MU"
rm -rf pdf; mkdir pdf
for MODE in "Pbp_MU" "pPb_MU" "pPb_MD" "Pbp_MD"; do

cd ${MYDIR}/pdf
mkdir ${MODE}; cd ${MODE}

root -l -q -b "${SRC}/drawSame.C(\"${MODE}\")"

done

