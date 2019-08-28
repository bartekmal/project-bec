from Gaudi.Configuration import * 
from GaudiConf import IOHelper

mode = "Pbp_MU"



myFiles = []
for i in range(1,100):
	myFiles.append("/mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/EPOS/dst/MC_"+mode+"/MC_"+mode+"_1/data/MC_"+mode+"_1_{0}.dst".format(i))



IOHelper().inputFiles(myFiles, clear=True)
