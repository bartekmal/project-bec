#!/bin/sh
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh
BASE=${BASE_BEC}/PIDCalib/EPOS

cd ${BASE}; mkdir jobs

#run
for BEAM in "pPb" "Pbp"; do

    for MAGNET in "MU" "MD"; do

	DIR_TMP=${BEAM}_${MAGNET}
	cd ${BASE}/jobs; rm -rf ${DIR_TMP}; mkdir ${DIR_TMP}; cd ${DIR_TMP}
	sbatch -p plgrid -t 40:00:00 --mem-per-cpu 16GB -J pid_${BEAM}_${MAGNET} ${BASE}/pidcalib.sh ${BEAM} ${MAGNET}

    done

done
