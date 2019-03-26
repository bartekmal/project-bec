################################################################################
# Package: PIDPerfScripts
################################################################################
gaudi_subdir(PIDPerfScripts v12r0)

gaudi_depends_on_subdirs(PIDCalib/PIDPerfTools)

find_package(Boost)
find_package(ROOT)
find_package(Xrootd)

gaudi_install_python_modules()

gaudi_env(SET CALIBDATAEXTRA       eoslhcb.cern.ch
	  SET CALIBDATASTORE       eos/lhcb/grid/prod/lhcb/calib/lhcb/calib/pid/CalibData
          SET CALIBDATAURLPROTOCOL root:) 
#          SET OVERRIDECALIBDATASTORE /home/qian/outputURLs15MD.dat)



