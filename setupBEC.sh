#!/bin/sh
source /cvmfs/lhcb.cern.ch/group_login.sh -c x86_64-slc6-gcc49-opt

export BASE_BEC=/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb
export BASE_BEC_PROD=${BASE_BEC}/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob
export BASE_BEC_EOS=/eos/lhcb/user/b/bmalecki/BEC_pPb

alias gbec='cd ${BASE_BEC}'
alias gbec_prod='cd ${BASE_BEC_PROD}'
alias gbec_eos='cd ${BASE_BEC_EOS}'
