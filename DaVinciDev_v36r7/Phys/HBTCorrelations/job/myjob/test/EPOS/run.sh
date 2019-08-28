#!/bin/sh
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh
cd /mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/EPOS

OPTIONS_DIR=/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob

/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/run gaudirun.py $OPTIONS_DIR/data/test_EPOS.py $OPTIONS_DIR/options/MC_pPb_MU.py $OPTIONS_DIR/options/HBTCorrelations.py > test_EPOS.log
