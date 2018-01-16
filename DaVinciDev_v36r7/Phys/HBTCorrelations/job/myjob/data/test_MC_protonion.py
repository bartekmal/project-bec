

from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper().inputFiles([
	'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00000875_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00000975_5.AllStreams.dst'
        ,'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00000980_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00001179_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00001224_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00000004_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00000007_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00000013_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00000010_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/pPb/00067399_00000012_5.AllStreams.dst'

], clear=True)
