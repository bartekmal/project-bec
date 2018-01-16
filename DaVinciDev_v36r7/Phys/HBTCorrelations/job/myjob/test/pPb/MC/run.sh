#!/bin/sh
source /cvmfs/lhcb.cern.ch/lib/LbLogin.sh -c x86_64-slc6-gcc49-opt
source /afs/cern.ch/user/b/bmalecki/myscripts/setup/BEC.sh
#BASE_BEC=/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb
#BASE_BEC_PROD=/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob
#export BASE_BEC_EOS=/eos/lhcb/user/b/bmalecki/BEC_pPb

cd ${BASE_BEC_PROD}/test/MC/pPb

${BASE_BEC}/DaVinciDev_v36r7/run gaudirun.py ${BASE_BEC_PROD}/options/HBTCorrelations.py ${BASE_BEC_PROD}/options/MC_pPb_MD.py ${BASE_BEC_PROD}/data/test_MC_protonion.py > pPb.log 
