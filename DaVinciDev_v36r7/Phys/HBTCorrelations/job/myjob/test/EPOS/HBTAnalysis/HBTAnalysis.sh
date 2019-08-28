#!/bin/sh

#setenv
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh 
SetupProject DaVinci v36r7
MYDIR=/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/EPOS
SRC=${MYDIR}/HBTAnalysis

#run
PROD=$1
MODE=$2

cd ${MYDIR}/output/${MODE}
${SRC}/HBTAnalysis.exe "${PROD}" "${MODE}" > "${PROD}.log"


