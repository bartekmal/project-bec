

from Gaudi.Configuration import * 
from GaudiConf import IOHelper
IOHelper().inputFiles([
	'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000004_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000014_5.AllStreams.dst'
        ,'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000026_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000029_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000033_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000002_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000005_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000006_5.AllStreams.dst'
	,'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000007_5.AllStreams.dst'
	#,'${BASE_BEC_EOS}/dst/testMC/Pbp/00067559_00000011_5.AllStreams.dst'
	
], clear=True)
