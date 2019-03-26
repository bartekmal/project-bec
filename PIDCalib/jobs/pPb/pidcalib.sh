#!/bin/sh
source /afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb/setupBEC.sh

#set job dir and file to correct (change BINNING in CALIB_FILE if empty)
JOB_DIR=${BASE_BEC}/PIDCalib/jobs/pPb
FOLDER_TO_CORRECT=MC_pPb_MU
MAGNET=MagUp #MagDown/MagUp

#PIDcalib configuration
STRIPPING=20
TUNE=MC12TuneV2_
PROTON_SAMPLE=P #P / P_TotLc
BINNING=BEC_pPb
#P in MeV
HBT_CUTS="nPVs==1 && nTracks>4 && trackcharge!=0 && P>2000 && PT>100 && ${TUNE}ProbNNghost<0.5"
#HBT_CUTS=ALL

#running PIDcalib
FILE_TO_CORRECT=${BASE_BEC_EOS}/ntuple/${FOLDER_TO_CORRECT}/hbtNTuple.root
CALIB_FILE=${JOB_DIR}/PIDHists_Strip${STRIPPING}_${MAGNET}_${BINNING}.root
URANIA_DIR=${BASE_BEC}/PIDCalib/UraniaDev_v7r0
SCRIPT_DIR=${URANIA_DIR}/PIDCalib/PIDPerfScripts/scripts/python/MCResampling

cd $JOB_DIR
#${URANIA_DIR}/run python ${SCRIPT_DIR}/MakePIDdistributionsRunRange.py "$STRIPPING" "$MAGNET" "[K,Mu,Pi,${PROTON_SAMPLE},e]" "[${TUNE}ProbNNK,${TUNE}ProbNNmu,${TUNE}ProbNNp,${TUNE}ProbNNpi,${TUNE}ProbNNe]" --cuts="[lab:${HBT_CUTS}]" --schemeName="${BINNING}" > PIDdistribution.log
#sleep 100
${URANIA_DIR}/run python ${SCRIPT_DIR}/PerTrack_MakePIDCorrection_neg_optimal.py "${FILE_TO_CORRECT}" "HBT" "[pi:lab/Pi,K:lab/K,p:lab/${PROTON_SAMPLE},mu:lab/Mu,e:lab/e]" "[${TUNE}ProbNNpi,${TUNE}ProbNNK,${TUNE}ProbNNp,${TUNE}ProbNNmu,${TUNE}ProbNNe]" "$CALIB_FILE"  > MCresample.log






