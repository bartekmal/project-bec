#!/bin/sh

#needs as input: HBT_SRC DATA_TYPE BEAM_MODE

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
if [ "${USER}" = "plgbmalecki" ] ; then
  MYDIR=${SLURM_SUBMIT_DIR}
fi
if [ "${USER}" = "bmalecki" ] ; then
  MYDIR=${LS_SUBCWD}
fi


SRC=$1
DATA_TYPE=$2
BEAM_MODE=$3
PROD_TYPE=$4

#run
cd ${MYDIR}
${BASE_BEC}/DaVinciDev_v36r7/run  ${SRC}/HBTAnalysis.exe "${DATA_TYPE}" "${BEAM_MODE}" "${PROD_TYPE}" > "${DATA_TYPE}_${BEAM_MODE}_${PROD_TYPE}.log"


