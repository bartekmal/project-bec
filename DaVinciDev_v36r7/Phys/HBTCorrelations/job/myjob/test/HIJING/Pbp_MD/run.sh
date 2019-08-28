#!/bin/sh
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh
cd /mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/HIJING/Pbp_MD

OPTIONS_DIR=/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob

/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/run gaudirun.py $OPTIONS_DIR/data/test_HIJING_Pbp_MD.py $OPTIONS_DIR/options/MC_Pbp_MD.py $OPTIONS_DIR/options/HBTCorrelations.py > test_HIJING.log
