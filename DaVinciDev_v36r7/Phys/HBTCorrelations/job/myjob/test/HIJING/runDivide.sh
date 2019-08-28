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
SRC=${PWD}/scripts
MYDIR=${PWD}
INPUT=${MYDIR}/output
OUTPUT=${MYDIR}/corrs

#run
cd ${MYDIR}; rm -rf ${OUTPUT}; mkdir ${OUTPUT}; cd ${OUTPUT}

for DATA_TYPE in "local" "Oscar"; do

  cd ${OUTPUT}; mkdir ${DATA_TYPE}; cd ${DATA_TYPE}

  for BEAM_MODE in "Pbp_MD" "pPb_MD"; do

    P=${INPUT}/${DATA_TYPE}/${BEAM_MODE}

    echo "--> Correlation functions: ${DATA_TYPE} ${BEAM_MODE}"
    F1=${BEAM_MODE}
    F2=${F1}
    H2="h3401"
    for H1 in "h3001" "h3101"; do
     
      root -l -q -b "${SRC}/makeDivided.C(\"${P}\", \"${F1}\", \"${H1}\", \"${P}\", \"${F2}\", \"${H2}\" ,1)"
    done
    
  done

done


