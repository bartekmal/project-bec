#!/bin/sh
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh
BASE_HIJING=${BASE_BEC}/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/HIJING
OPTIONS_DIR=${BASE_BEC}/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob

#run
for MODE in "pPb_MD" "Pbp_MD"; do

cd ${BASE_HIJING}; mkdir ${MODE}; cd ${MODE}
mkdir ${BASE_BEC_EOS}/HIJING/ntuple/MC_${MODE}

for I in 1 ; do

for J in 1 2 3 4 5; do

cd ${BASE_HIJING}/${MODE}; mkdir $I; cd $I

echo "#Input for mode: ${MODE} - $I / $J

from Gaudi.Configuration import *
from GaudiConf import IOHelper

myFiles = []
for i in range(($J-1)*200+1,$J*200):
	myFiles.append(\"/mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/dst/MC_${MODE}/MC_${MODE}_${I}/data/MC_${MODE}_${I}_{0}.dst\".format(i))

IOHelper().inputFiles(myFiles, clear=True)
" >> local_HIJING_${MODE}_${I}_${J}.py

echo "#!/bin/sh

#Batch job for mode: ${MODE} - $I / $J

source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh

mkdir ${BASE_BEC_EOS}/HIJING/ntuple/MC_${MODE}/$I
cd ${BASE_BEC_EOS}/HIJING/ntuple/MC_${MODE}/$I
mkdir ${BASE_BEC_EOS}/HIJING/ntuple/MC_${MODE}/$I/$J
cd ${BASE_BEC_EOS}/HIJING/ntuple/MC_${MODE}/$I/$J

${BASE_BEC}/DaVinciDev_v36r7/run gaudirun.py ${BASE_HIJING}/${MODE}/${I}/local_HIJING_${MODE}_${I}_${J}.py $OPTIONS_DIR/options/MC_${MODE}.py $OPTIONS_DIR/options/HBTCorrelations.py

" >> run_${MODE}_$I_$J.sh

sbatch -p plgrid -t 3-00:00:00 run_${MODE}_$I_$J.sh

done

done

done
