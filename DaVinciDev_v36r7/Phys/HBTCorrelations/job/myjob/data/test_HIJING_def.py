from Gaudi.Configuration import * 
from GaudiConf import IOHelper

#myFiles = []
#for i in range(1,1000):
#	myFiles.append("/eos/lhcb/user/b/bmalecki/BEC_pPb/dst/EPOS/MC_pPb_MU_test/data/MC_pPb_MU_test_{0}.dst".format(i))

myFiles = []
for i in range(1,400):
	myFiles.append("/mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/dst/MC_pPb_MU_test_def/data/MC_pPb_MU_test_def_{0}.dst".format(i))



IOHelper().inputFiles(myFiles, clear=True)