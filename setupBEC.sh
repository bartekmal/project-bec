#!/bin/sh

#functions
function bec_setup_env {

    #common variables (for running jobs etc)
    export MY_CMTCONFIG=x86_64-slc6-gcc49-opt

    #useful dirs
    export BEC_BASE_DAVINCI=${BEC_BASE}/DaVinciDev_v36r7
    export BEC_BASE_PROD=${BEC_BASE_DAVINCI}/Phys/HBTCorrelations/job/myjob
    export BEC_BASE_CODE=${BEC_BASE}/code
    export BEC_BASE_CODE_HBT=${BEC_BASE_CODE}/HBTAnalysis
    export BEC_BASE_CODE_SCRIPTS=${BEC_BASE_CODE}/scripts
    export BEC_BASE_ANALYSIS=${BEC_BASE}/analysis

    #set ccache
    export CCACHE_DIR=$User_release_area/.ccache
    export CMAKEFLAGS="-DCMAKE_USE_CCACHE=ON"

    #useful aliases
    alias bec_go_base='cd ${BEC_BASE}'
    alias bec_go_data='cd ${BEC_DATA}'
    alias bec_go_code='cd ${BEC_BASE_CODE}'
    alias bec_go_hbt='cd ${BEC_BASE_CODE_HBT}'
    alias bec_go_scripts='cd ${BEC_BASE_CODE_SCRIPTS}'
    alias bec_go_davinci='cd ${BEC_BASE_DAVINCI}'
    alias bec_go_prod='cd ${BEC_BASE_PROD}'
    alias bec_go_analysis='cd ${BEC_BASE_ANALYSIS}'

    #setup LbEnv
    source /cvmfs/lhcb.cern.ch/lib/LbEnv.sh -c ${MY_CMTCONFIG}
    #lb-set-platform ${MY_CMTCONFIG}
    #lb-set-workspace
}

function bec_setup_loginInfo {
    echo -e "\nBEC environment for $BEC_HOST ready!"
    echo "--> project path: $BEC_BASE"
    echo "--> data path: $BEC_DATA"
    echo -e "--> platform: $CMTCONFIG \n"
}


######## setupBEC - default ##############

#check host
export BEC_HOST=0
if [[ $USER = "bmalecki" ]] ; then
    export BEC_HOST=lxplus
    BEC_SETUP_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
    BEC_BASE_FOLDER=`echo $BEC_SETUP_DIR | sed -n -e 's/^.*bmalecki\///p' `
    export BEC_BASE=/afs/cern.ch/work/b/bmalecki/${BEC_BASE_FOLDER}
    export BEC_DATA=/eos/lhcb/user/b/bmalecki/${BEC_BASE_FOLDER}
fi

if [[ $USER = "plgbmalecki" ]] ; then
    export BEC_HOST=zeus
    BEC_SETUP_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
    BEC_BASE_FOLDER=`echo $BEC_SETUP_DIR | sed -n -e 's/^.*plgbmalecki\///p' `
    export BEC_BASE=$STORAGE/${BEC_BASE_FOLDER}
    export BEC_DATA=$SCRATCH/${BEC_BASE_FOLDER}
fi
if [ "$BEC_HOST" = "0" ]; then
    echo -e "\nBEC environemnt not set! -> exiting"
    exit 1
fi

#set BEC environment variables
bec_setup_env
bec_setup_loginInfo



