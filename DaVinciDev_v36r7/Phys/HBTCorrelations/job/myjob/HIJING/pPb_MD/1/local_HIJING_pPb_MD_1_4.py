#Input for mode: pPb_MD - 1 / 4

from Gaudi.Configuration import *
from GaudiConf import IOHelper

myFiles = []
for i in range((4-1)*200+1,4*200):
	myFiles.append("/mnt/lustre/scratch2/people/plgbmalecki/BEC_pPb/HIJING/dst/MC_pPb_MD/MC_pPb_MD_1/data/MC_pPb_MD_1_{0}.dst".format(i))

IOHelper().inputFiles(myFiles, clear=True)

