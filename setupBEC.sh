#!/bin/sh
source /cvmfs/lhcb.cern.ch/group_login.sh -c x86_64-slc6-gcc49-opt

if [[ $(hostname) = "lxplus"[0-9][0-9][0-9]".cern.ch" ]] ; then
  echo 'Setting up BEC environment for lxplus.'
  export BASE_BEC=/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb
  export BASE_BEC_PROD=${BASE_BEC}/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob
  export BASE_BEC_EOS=/eos/lhcb/user/b/bmalecki/BEC_pPb
fi

if [ `hostname` = "zeus.cyfronet.pl" ] ; then
  echo 'Setting up BEC environment for zeus.'
  export BASE_BEC=${STORAGE}/BEC_pPb
  export BASE_BEC_PROD=${BASE_BEC}/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob
  export BASE_BEC_EOS=${SCRATCH}/BEC_pPb
fi

alias gbec='cd ${BASE_BEC}'
alias gbec_prod='cd ${BASE_BEC_PROD}'
alias gbec_eos='cd ${BASE_BEC_EOS}'
