#!/bin/sh

# functions
function bec-setup-env() {

    # useful dirs
    export BEC_BASE_SOFTWARE=${BEC_BASE}/software
    export BEC_BASE_DAVINCI=${BEC_BASE_SOFTWARE}/DaVinci
    export BEC_BASE_PROD=${BEC_BASE_DAVINCI}/Phys/HBTCorrelations/job/myjob
    export BEC_BASE_CODE=${BEC_BASE}/code
    export BEC_BASE_CODE_HBT=${BEC_BASE_CODE}/HBTAnalysis
    export BEC_BASE_CODE_SCRIPTS=${BEC_BASE_CODE}/scripts
    export BEC_BASE_ANALYSIS=${BEC_BASE}/analysis

    # common variables (needed for running jobs/scripts etc)
    export EOS_PREFIX="root://eoslhcb.cern.ch/"
    export SLEEP_TIME=10 # after finishing one application, before using its output

    # software versions
    export MY_CMTCONFIG="x86_64-centos7-gcc9-opt"
    export MY_ROOT_SETUP="/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.22.02/x86_64-centos7-gcc48-opt/bin/thisroot.sh"
    
    lb-set-platform ${MY_CMTCONFIG}
    source ${MY_ROOT_SETUP} # ! overrides the 'general' available software (to use a fixed ROOT version)

    # Python version for running scripts that need PyROOT etc
    # ! some black bash magic to get only names of the BEC-specific environment variables
    VARS_TO_EXPORT=`env -0 | sed -e ':a;N;$!ba;s/\([^=]\+\)=\([^\x00]*\)\x00/\1\n/g' | grep BEC_BASE`
    VARS_LIST=""; for ITEM in ${VARS_TO_EXPORT}; do VARS_LIST+="-e ${ITEM}=\${$ITEM} "; done
    export MY_PYTHON_SETUP="lb-conda ${VARS_LIST} default python" 

    # useful aliases
    alias bec-cd-base='cd ${BEC_BASE}'
    alias bec-cd-data='cd ${BEC_DATA}'
    alias bec-cd-software='cd ${BEC_BASE_SOFTWARE}'
    alias bec-cd-davinci='cd ${BEC_BASE_DAVINCI}'
    alias bec-cd-prod='cd ${BEC_BASE_PROD}'
    alias bec-cd-code='cd ${BEC_BASE_CODE}'
    alias bec-cd-hbt='cd ${BEC_BASE_CODE_HBT}'
    alias bec-cd-scripts='cd ${BEC_BASE_CODE_SCRIPTS}'
    alias bec-cd-analysis='cd ${BEC_BASE_ANALYSIS}'
    alias bec-make='ipython -i runHBT.py'
}

function bec-setup-summary() {
    echo -e "\nBEC environment for ${BEC_HOST} ready!"
    echo "--> project path: ${BEC_BASE}"
    echo "--> data path: ${BEC_DATA}"
    echo "--> storage path: ${BEC_STORAGE}"
    echo "--> platform: ${CMTCONFIG}"
    echo "--> Python: ${MY_PYTHON_SETUP}"
    echo -e "--> ROOT: ${MY_ROOT_SETUP} \n"
}

function bec-make-study() {
  # take the study name from the command line or use a default
  STUDY_PATH=${BEC_BASE_ANALYSIS}/${1:-test}

  # sanitise
  if [ -d ${STUDY_PATH} ]; then
    echo "Directory already exists:"
    echo ${STUDY_PATH}
    return 1
  else
  # create a study
    echo "Creating a new study in:"
    echo ${STUDY_PATH}
    mkdir -p ${STUDY_PATH}
    cp ${BEC_BASE_CODE}/runHBT.py ${STUDY_PATH}
    cd ${STUDY_PATH}
  fi
}

######## setupBEC - default ##############

# get LHCb environment (should also provide access to general needed software, e.g. Python) 
# TODO could be improved by providing a fixed software container
# source /cvmfs/lhcb.cern.ch/lib/LbEnv.sh # ! activate only if not already available

# host-specific configuration
# ! define a host-specific 'workspace' (it depends on the available storage 'resources')
# ! more environment variables are set in code/runHBT.py:setEnvironmentBEC() - they are job-specific, so they are kept separately from here
# TODO maybe all host-specific variables could be set up in a single place
export BEC_HOST=0

# ! lxplus: assumes that this repository resides in the user /afs/cern.ch/work area (see ${LXPLUS_WORKING_DIR})
# TODO lxplus: could be improved (also any output files could be kept separately from scripts in BEC_DATA - not done for now)
if [[ $(hostname) = "lxplus"*".cern.ch" ]] || [[ $(hostname) = "lbd"*".cern.ch" ]]; then
    export BEC_HOST=lxplus

    # get whatever comes after USER name/path in the directory where this scripts resides (~relative path wrt 'user area')
    USER_PATH=${USER:0:1}/${USER}
    BEC_SETUP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
    BEC_BASE_FOLDER=$(echo ${BEC_SETUP_DIR} | sed -n -e 's/^.*'${USER}'\///p')

    LXPLUS_WORKING_DIR=/afs/cern.ch/work
    export BEC_BASE=${LXPLUS_WORKING_DIR}/${USER_PATH}/${BEC_BASE_FOLDER}
    export BEC_DATA=${EOS_PREFIX}/eos/lhcb/user/${USER_PATH}/${BEC_BASE_FOLDER}
    export BEC_STORAGE=${EOS_PREFIX}/eos/lhcb/wg/IonPhysics/analyses/bec_pPb_2013
fi

# TODO update the following lines to work @ cyfronet
# if [[ $(hostname) = "zeus.cyfronet.pl" ]]; then
#     export BEC_HOST=plgrid
#     BEC_SETUP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
#     BEC_BASE_FOLDER=$(echo ${BEC_SETUP_DIR} | sed -n -e 's/^.*plgbmalecki\///p')
#     export BEC_BASE=${STORAGE}/${BEC_BASE_FOLDER}
#     export BEC_DATA=${STORAGE}/${BEC_BASE_FOLDER}
#     export BEC_STORAGE=${SCRATCH}/${BEC_BASE_FOLDER}
# fi

# check if the BEC environment is set (needs a known host)
if [ -z ${BEC_HOST+x} ]; then
    echo -e "\nBEC environment not set. Exitting.\n"
    exit 1
fi

# BEC-specific configuration
bec-setup-env
bec-setup-summary
