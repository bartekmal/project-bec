#!/bin/sh
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh
BASE_EPOS=${BASE_BEC}/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/EPOS
OPTIONS_DIR=${BASE_BEC}/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob


#run
for MODE in "Pbp_MU" "pPb_MU" "pPb_MD" "Pbp_MD"; do

cd ${BASE_EPOS}; mkdir ${MODE}; cd ${MODE}
mkdir ${BASE_BEC_EOS}/EPOS/ntuple/MC_${MODE}

for I in 1 2 3 4 5; do

cd ${BASE_EPOS}/${MODE}; mkdir $I; cd $I

echo "#Input for mode: ${MODE} - $I

from Gaudi.Configuration import *
from GaudiConf import IOHelper

myFiles = []
for i in range(1,1000):
	if \"${MODE}\"==\"pPb_MU\":
        	myFiles.append(\"/mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/EPOS/dst/MC_${MODE}/MC_${MODE}_${I}/data/MC_${MODE}_${I}_{0}.dst\".format(i+(${I}-1)*1000))
	else:
		myFiles.append(\"/mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/EPOS/dst/MC_${MODE}/MC_${MODE}_${I}/data/MC_${MODE}_${I}_{0}.dst\".format(i))

IOHelper().inputFiles(myFiles, clear=True)
" >> local_EPOS_${MODE}_${I}.py

echo "#!/bin/sh

#Batch job for mode: ${MODE} - $I

source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh

cd "'${SLURM_SUBMIT_DIR}'"

${BASE_BEC}/DaVinciDev_v36r7/run gaudirun.py local_EPOS_${MODE}_${I}.py $OPTIONS_DIR/options/MC_${MODE}.py $OPTIONS_DIR/options/HBTCorrelations.py

mkdir ${BASE_BEC_EOS}/EPOS/ntuple/MC_${MODE}/$I
mv *.root ${BASE_BEC_EOS}/EPOS/ntuple/MC_${MODE}/$I

" >> run_${MODE}_$I.sh

sbatch -p plgrid-long -t 7-00:00:00 run_${MODE}_$I.sh

done

done
