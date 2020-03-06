#!/bin/sh

#config part

NR_BINS_MULT="6"
NR_BINS_KT="3"

DATA_PATH=$PWD
DATA_PATH_REF=/mnt/nfs/storage/plgbmalecki/BEC_pPb/analysis/evMixCache/compareOld/pPb
INPUT_FOLDER="correlations"
OUTPUT_FOLDER="compare"

OUTPUT_PATH=${DATA_PATH}/output/${OUTPUT_FOLDER}

rm -r ${OUTPUT_PATH}; mkdir ${OUTPUT_PATH}

####execute

HISTOGRAM_NAME_1="h4012"
HISTOGRAM_NAME_2="h4012"
HISTOGRAM_COMMON_END="_0_400"

for DATA_TYPE in "RD"; do

    for BEAM_MODE in "pPb"; do

        OUTPUT_PATH_SINGLE="${OUTPUT_PATH}/${DATA_TYPE}_${BEAM_MODE}"
        mkdir ${OUTPUT_PATH_SINGLE}

        INPUT_PATH_1="${DATA_PATH}/output/${INPUT_FOLDER}/${DATA_TYPE}_${BEAM_MODE}"
        INPUT_PATH_2="${DATA_PATH_REF}/output/${INPUT_FOLDER}/${DATA_TYPE}_${BEAM_MODE}"
        INPUT_FILE_1="correlations.root"
        INPUT_FILE_2="correlations.root"

        root -l -b -q '$BEC_BASE_CODE/old/drawSame.C(
            "'${INPUT_PATH_1}'",
            "'${INPUT_FILE_1}'",
            "'${HISTOGRAM_NAME_1}'",
            "'${INPUT_PATH_2}'",
            "'${INPUT_FILE_2}'",
            "'${HISTOGRAM_NAME_2}'",
            "'${OUTPUT_PATH_SINGLE}'",
            "'${HISTOGRAM_COMMON_END}'",
            '${NR_BINS_MULT}',
            '${NR_BINS_KT}'           
        )'

    done

done

HISTOGRAM_NAME_1="h4010"
HISTOGRAM_NAME_2="h4010"
HISTOGRAM_COMMON_END="_0_400"

for DATA_TYPE in  "MC"; do

    for BEAM_MODE in "pPb"; do

        OUTPUT_PATH_SINGLE="${OUTPUT_PATH}/${DATA_TYPE}_${BEAM_MODE}"
        mkdir ${OUTPUT_PATH_SINGLE}

        INPUT_PATH_1="${DATA_PATH}/output/${INPUT_FOLDER}/${DATA_TYPE}_${BEAM_MODE}"
        INPUT_PATH_2="${DATA_PATH_REF}/output/${INPUT_FOLDER}/${DATA_TYPE}_${BEAM_MODE}"
        INPUT_FILE_1="correlations.root"
        INPUT_FILE_2="correlations.root"

        root -l -b -q '$BEC_BASE_CODE/old/drawSame.C(
            "'${INPUT_PATH_1}'",
            "'${INPUT_FILE_1}'",
            "'${HISTOGRAM_NAME_1}'",
            "'${INPUT_PATH_2}'",
            "'${INPUT_FILE_2}'",
            "'${HISTOGRAM_NAME_2}'",
            "'${OUTPUT_PATH_SINGLE}'",
            "'${HISTOGRAM_COMMON_END}'",
            '${NR_BINS_MULT}',
            '${NR_BINS_KT}'           
        )'

    done

done

HISTOGRAM_NAME_1="h5012"
HISTOGRAM_NAME_2="h5012"
HISTOGRAM_COMMON_END="_0_400"

for DATA_TYPE in  "DR"; do

    for BEAM_MODE in "pPb"; do

        OUTPUT_PATH_SINGLE="${OUTPUT_PATH}/${DATA_TYPE}_${BEAM_MODE}"
        mkdir ${OUTPUT_PATH_SINGLE}

        INPUT_PATH_1="${DATA_PATH}/output/${INPUT_FOLDER}/${DATA_TYPE}_${BEAM_MODE}"
        INPUT_PATH_2="${DATA_PATH_REF}/output/${INPUT_FOLDER}/${DATA_TYPE}_${BEAM_MODE}"
        INPUT_FILE_1="correlations.root"
        INPUT_FILE_2="correlations.root"

        root -l -b -q '$BEC_BASE_CODE/old/drawSame.C(
            "'${INPUT_PATH_1}'",
            "'${INPUT_FILE_1}'",
            "'${HISTOGRAM_NAME_1}'",
            "'${INPUT_PATH_2}'",
            "'${INPUT_FILE_2}'",
            "'${HISTOGRAM_NAME_2}'",
            "'${OUTPUT_PATH_SINGLE}'",
            "'${HISTOGRAM_COMMON_END}'",
            '${NR_BINS_MULT}',
            '${NR_BINS_KT}'           
        )'

    done

done

