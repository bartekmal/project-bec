#!/bin/sh

#functions
function bec_setup_env() {

    #common variables (for running jobs etc)
    export EOS_PREFIX="root://eoslhcb.cern.ch/"
    export SLEEP_TIME=10 #after finishing one application, before using output
    export MY_CMTCONFIG="x86_64-centos7-gcc9-opt"
    export MY_ROOT_SETUP="/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.22.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh"

    #useful dirs
    export BEC_BASE_DAVINCI=${BEC_BASE}/DaVinciDev_v36r7
    export BEC_BASE_PROD=${BEC_BASE_DAVINCI}/Phys/HBTCorrelations/job/myjob
    export BEC_BASE_CODE=${BEC_BASE}/code
    export BEC_BASE_CODE_HBT=${BEC_BASE_CODE}/HBTAnalysis
    export BEC_BASE_CODE_SCRIPTS=${BEC_BASE_CODE}/scripts
    export BEC_BASE_ANALYSIS=${BEC_BASE}/analysis

    #useful aliases
    alias bec-cd-base='cd ${BEC_BASE}'
    alias bec-cd-data='cd ${BEC_DATA}'
    alias bec-cd-code='cd ${BEC_BASE_CODE}'
    alias bec-cd-hbt='cd ${BEC_BASE_CODE_HBT}'
    alias bec-cd-scripts='cd ${BEC_BASE_CODE_SCRIPTS}'
    alias bec-cd-davinci='cd ${BEC_BASE_DAVINCI}'
    alias bec-cd-prod='cd ${BEC_BASE_PROD}'
    alias bec-cd-analysis='cd ${BEC_BASE_ANALYSIS}'

}

function bec_setup_loginInfo() {
    echo -e "\nBEC environment for $BEC_HOST ready!"
    echo "--> project path: $BEC_BASE"
    echo "--> data path: $BEC_DATA"
    echo "--> storage path: $BEC_STORAGE"
    echo "--> platform: $CMTCONFIG"
    echo -e "--> ROOT: $MY_ROOT_SETUP \n"
}

######## setupBEC - default ##############

#setup LbEnv
source /cvmfs/lhcb.cern.ch/lib/LbEnv.sh

#check host
export BEC_HOST=0
if [[ $(hostname) = "lxplus"*".cern.ch" ]] || [[ $(hostname) = "lbd"*".cern.ch" ]]; then
    export BEC_HOST=lxplus
    BEC_SETUP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
    BEC_BASE_FOLDER=$(echo $BEC_SETUP_DIR | sed -n -e 's/^.*bmalecki\///p')
    export BEC_BASE=/afs/cern.ch/work/b/bmalecki/${BEC_BASE_FOLDER}
    export BEC_DATA=$EOS_PREFIX/eos/lhcb/user/b/bmalecki/${BEC_BASE_FOLDER}
    export BEC_STORAGE=$EOS_PREFIX/eos/lhcb/wg/IonPhysics/analyses/bec_pPb_2013
fi

if [[ $(hostname) = "zeus.cyfronet.pl" ]]; then
    export BEC_HOST=plgrid
    BEC_SETUP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
    BEC_BASE_FOLDER=$(echo $BEC_SETUP_DIR | sed -n -e 's/^.*plgbmalecki\///p')
    export BEC_BASE=$STORAGE/${BEC_BASE_FOLDER}
    export BEC_DATA=$STORAGE/${BEC_BASE_FOLDER}
    export BEC_STORAGE=$SCRATCH/${BEC_BASE_FOLDER}
fi
# check if the BEC environment is set (needs a known host)
if [ -z ${BEC_HOST+x} ]; then
    echo -e "\nBEC environment not set. Exitting.\n"
    exit 1
fi

#set BEC environment variables
bec_setup_env
lb-set-platform ${MY_CMTCONFIG}
# lb-set-workspace ${BEC_BASE_CODE}
source $MY_ROOT_SETUP 
bec_setup_loginInfo
