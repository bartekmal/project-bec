#!/bin/csh
#source ~/.login

setenv MYSITEROOT /cvmfs/lhcb.cern.ch/lib

setenv TOP_DIR_SIM ins_topdirsim
setenv JOB_NAME    ins_job
setenv FIRST_EVENT ins_firstevent
setenv NUM_EVENTS  ins_numevents
setenv RUN_NUMBER  ins_runnumber
setenv PROD_NAME   ins_prodname
setenv PROD_SRC    ins_prodsrc

if ( ! -e $TOP_DIR_SIM ) then
  echo "TOP_DIR_SIM does not exists: $TOP_DIR_SIM"
  echo "Fatal error. Exit."
  exit
endif

# Work dir related to PROD_NAME 
setenv PROD_DIR $TOP_DIR_SIM/$PROD_NAME
if ( ! -e $PROD_DIR ) then
  echo "PROD_DIR does not exists: $PROD_DIR"
  echo "Fatal error. Exit."
  exit
endif

# dir related to PROD_NAME to store dst 
setenv PROD_DIR_DATA $TOP_DIR_SIM/$PROD_NAME/data
if ( ! -e $PROD_DIR_DATA ) then
  echo "PROD_DIR_DATA does not exists: $PROD_DIR_DATA"
  echo "Fatal error. Exit."
  exit
endif

# dir related to PROD_NAME to store logs etc 
setenv PROD_DIR_LOGS $TOP_DIR_SIM/$PROD_NAME/logs
if ( ! -e $PROD_DIR_LOGS ) then
  echo "PROD_DIR_LOGS does not exists: $PROD_DIR_LOGS"
  echo "Fatal error. Exit."
  exit
endif
 
# Dir where job is executed
setenv JOB_DIR $PROD_DIR/work/$JOB_NAME
# check if PBS TMPDIR is available and switch if so
if ( $?TMPDIR ) then
    if ( -w $TMPDIR ) then
      #setenv JOB_DIR $TMPDIR/$JOB_NAME
    endif
endif  

mkdir $JOB_DIR


#
# GAUSS
#
cd $JOB_DIR
mkdir tmpdir1
cd tmpdir1

cp $PROD_SRC/options/Gauss-Job.py Gauss-Job.py
echo "GaussGen = GenInit('GaussGen')" >> Gauss-Job.py
echo "GaussGen.FirstEventNumber = $FIRST_EVENT" >> Gauss-Job.py
echo "GaussGen.RunNumber        = $RUN_NUMBER"  >> Gauss-Job.py
echo "LHCbApp().EvtMax = $NUM_EVENTS"           >> Gauss-Job.py

source $MYSITEROOT/LbLogin.csh -c x86_64-slc6-gcc49-opt
/mnt/nfs/storage/plgbmalecki/BEC_pPb/GaussDev_v49r10/run gaudirun.py Gauss-Job.py > gauss_${JOB_NAME}.log

setenv sim_file_name `ls *.sim`
mv $sim_file_name  ../Gauss.sim
#mv gauss_${JOB_NAME}.log $PROD_DIR_LOGS/
mv GeneratorLog.xml $PROD_DIR_LOGS/GeneratorLog_${JOB_NAME}.xml
cd ../
rm -rf tmpdir1

#
# BOOLE
#
cd $JOB_DIR
mkdir tmpdir2
cd tmpdir2
mv ../Gauss.sim Gauss.sim

cp $PROD_SRC/options/Boole-Job.py Boole-Job.py
source $MYSITEROOT/LbLogin.csh -c x86_64-slc6-gcc49-opt
lb-run Boole/v30r2 gaudirun.py Boole-Job.py > boole_${JOB_NAME}.log

setenv digi_file_name `ls *.digi`
mv $digi_file_name  ../Boole.digi
mv boole_${JOB_NAME}.log $PROD_DIR_LOGS/
cd ../
rm -rf tmpdir2

#
# BRUNEL
#
cd $JOB_DIR
mkdir tmpdir5
cd tmpdir5
mv ../Boole.digi Boole.digi 

cp $PROD_SRC/options/Brunel-Job.py Brunel-Job.py
source $MYSITEROOT/LbLogin.csh -c x86_64-slc5-gcc46-opt
lb-run Brunel/v43r2p12 gaudirun.py Brunel-Job.py > brunel_${JOB_NAME}.log

mv brunel_${JOB_NAME}.log $PROD_DIR_LOGS/
mv Brunel.dst ../Brunel.dst
cd ../
rm -rf tmpdir5

#
# DAVINCI
#
cd $JOB_DIR
mkdir tmpdir6
cd tmpdir6
mv ../Brunel.dst Brunel.dst

cp $PROD_SRC/options/DaVinci-Job.py DaVinci-Job.py
source $MYSITEROOT/LbLogin.csh -c x86_64-slc5-gcc46-opt
lb-run DaVinci/v32r2p18 gaudirun.py DaVinci-Job.py > davinci_${JOB_NAME}.log

setenv dst_file_name `ls *.AllStreams.dst`
mv davinci_${JOB_NAME}.log $PROD_DIR_LOGS/
mv $dst_file_name $PROD_DIR_DATA/${PROD_NAME}_${JOB_NAME}.dst
cd ../
rm -rf tmpdir6
cd ../
rm -rf $JOB_DIR



 

