from Gaudi.Configuration import * 
from GaudiConf import IOHelper

#myFiles = []
#for i in range(1,1000):
#	myFiles.append("/eos/lhcb/user/b/bmalecki/BEC_pPb/dst/EPOS/MC_pPb_MU_test/data/MC_pPb_MU_test_{0}.dst".format(i))

myFiles = []
for i in range(0,49):
	myFiles.append("/mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/dst/ref_Oscar/data/MC_v33_pA_MD_sRunNumber-{0}.dst".format(i))



IOHelper().inputFiles(myFiles, clear=True)
