from Gaudi.Configuration import * 
from GaudiConf import IOHelper

#myFiles = []
#for i in range(1,1000):
#	myFiles.append("/eos/lhcb/user/b/bmalecki/BEC_pPb/dst/EPOS/MC_pPb_MU_test/data/MC_pPb_MU_test_{0}.dst".format(i))

myFiles = []
for i in range(1,1000):
	myFiles.append("/mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/EPOS/dst/MC_pPb_MU_test/data/MC_pPb_MU_test_{0}.dst".format(i))



IOHelper().inputFiles(myFiles, clear=True)
