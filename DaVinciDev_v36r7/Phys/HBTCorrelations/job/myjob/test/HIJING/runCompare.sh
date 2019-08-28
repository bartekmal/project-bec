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
INPUT=${MYDIR}/corrs
OUTPUT=${MYDIR}/pdfs

#run
cd ${MYDIR}; rm -rf ${OUTPUT}; mkdir ${OUTPUT}; cd ${OUTPUT}

for DATA_TYPE in "MC"; do

  for BEAM_MODE in "Pbp_MD" "pPb_MD"; do

    echo "--> Drawing:${BEAM_MODE}"
    F=${BEAM_MODE}
    cd ${OUTPUT}; mkdir ${F}; cd ${F}
    for H in "h3001" "h3101"; do
     
      root -l -q -b "${SRC}/draw.C(\"${INPUT}\", \"${F}\", \"${H}\")"
    done
    
  done

done

