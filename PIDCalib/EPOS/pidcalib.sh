#!/bin/sh
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh

echo '
#!/bin/sh
source /mnt/nfs/storage/plgbmalecki/BEC_pPb/setupBEC.sh
BEAM=${1} #pPb/Pbp
MAG=${2} #MU/MD
if [ "${MAG}" = "MU" ]; then
    MAGNET=MagUp
elif [ "${MAG}" = "MD" ]; then 
    MAGNET=MagDown
else
    echo "Wrong magnet settings (MU/MD)"
    exit
fi

#PIDcalib configuration
STRIPPING=20
TUNE=MC12TuneV2_
PROTON_SAMPLE=P #P / P_TotLc
BINNING=BEC_${BEAM}
#P in MeV
HBT_CUTS="nPVs==1 && nTracks>4 && trackcharge!=0 && P>2000 && PT>100 && ${TUNE}ProbNNghost<0.5"
#HBT_CUTS=ALL

#set job dir and file to correct (change BINNING in CALIB_FILE if empty)
JOB_DIR=${SLURM_SUBMIT_DIR}
CALIB_DIR=${BASE_BEC_EOS}/PIDCalib

#running PIDcalib
CALIB_FILE=${CALIB_DIR}/PIDHists_Strip${STRIPPING}_${MAGNET}_${BINNING}.root
URANIA_DIR=${BASE_BEC}/PIDCalib/UraniaDev_v7r0
SCRIPT_DIR=${URANIA_DIR}/PIDCalib/PIDPerfScripts/scripts/python/MCResampling

cd ${CALIB_DIR}
${URANIA_DIR}/run python ${SCRIPT_DIR}/MakePIDdistributionsRunRange.py "$STRIPPING" "$MAGNET" "[K,Mu,Pi,${PROTON_SAMPLE},e]" "[${TUNE}ProbNNK,${TUNE}ProbNNmu,${TUNE}ProbNNp,${TUNE}ProbNNpi,${TUNE}ProbNNe]" --cuts="[lab:${HBT_CUTS}]" --schemeName="${BINNING}" > PIDdistribution_${BEAM}_${MAG}.log
mv PIDdistribution_${BEAM}_${MAG}.log $JOB_DIR
sleep 1000
OUTPUT_DIR=${BASE_BEC_EOS}/ntuple/MC_${BEAM}_${MAG}/EPOS_local
cd ${OUTPUT_DIR}
${URANIA_DIR}/run python ${SCRIPT_DIR}/PerTrack_MakePIDCorrection_neg_optimal.py "${OUTPUT_DIR}/hbtNTuple.root" "HBT" "[pi:lab/Pi,K:lab/K,p:lab/${PROTON_SAMPLE},mu:lab/Mu,e:lab/e]" "[${TUNE}ProbNNpi,${TUNE}ProbNNK,${TUNE}ProbNNp,${TUNE}ProbNNmu,${TUNE}ProbNNe]" "$CALIB_FILE"  > MCresample_local.log

mv MCresample_local.log $JOB_DIR
mv newfilename.root hbtNTuple_corrected.root

OUTPUT_DIR=${BASE_BEC_EOS}/ntuple/MC_${BEAM}_${MAG}
cd ${OUTPUT_DIR}
${URANIA_DIR}/run python ${SCRIPT_DIR}/PerTrack_MakePIDCorrection_neg_optimal.py "${OUTPUT_DIR}/hbtNTuple.root" "HBT" "[pi:lab/Pi,K:lab/K,p:lab/${PROTON_SAMPLE},mu:lab/Mu,e:lab/e]" "[${TUNE}ProbNNpi,${TUNE}ProbNNK,${TUNE}ProbNNp,${TUNE}ProbNNmu,${TUNE}ProbNNe]" "$CALIB_FILE"  > MCresample.log

mv MCresample.log $JOB_DIR
mv newfilename.root hbtNTuple_corrected.root
' >> pidcalib_tmp.sh

lb-run -c best LHCbDirac/prod bash pidcalib_tmp.sh ${1} ${2}
rm pidcalib_tmp.sh





