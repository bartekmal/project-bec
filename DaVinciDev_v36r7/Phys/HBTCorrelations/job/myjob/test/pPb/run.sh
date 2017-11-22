#!/bin/sh
source /cvmfs/lhcb.cern.ch/lib/LbLogin.sh -c x86_64-slc6-gcc48-opt
./../../../../../../run gaudirun.py ../../options/HBTCorrelations.py ../../data/test_protonion.py > pPb.log &
