#!/bin/sh

#Batch job for mode: pPb_MD - 1 / 1

source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh

mkdir /mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/ntuple/MC_pPb_MD/1
cd /mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/ntuple/MC_pPb_MD/1
mkdir /mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/ntuple/MC_pPb_MD/1/1
cd /mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/ntuple/MC_pPb_MD/1/1

/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/run gaudirun.py /mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/HIJING/pPb_MD/1/local_HIJING_pPb_MD_1_1.py /mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/options/MC_pPb_MD.py /mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/options/HBTCorrelations.py


