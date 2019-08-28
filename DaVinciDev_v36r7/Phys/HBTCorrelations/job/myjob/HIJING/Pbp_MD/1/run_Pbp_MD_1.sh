#!/bin/sh

#Batch job for mode: Pbp_MD - 1 / 1

source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh

mkdir /mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/ntuple/MC_Pbp_MD/1
cd /mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/ntuple/MC_Pbp_MD/1
mkdir /mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/ntuple/MC_Pbp_MD/1/1
cd /mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/ntuple/MC_Pbp_MD/1/1

/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/run gaudirun.py /mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/HIJING/Pbp_MD/1/local_HIJING_Pbp_MD_1_1.py /mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/options/MC_Pbp_MD.py /mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/options/HBTCorrelations.py


