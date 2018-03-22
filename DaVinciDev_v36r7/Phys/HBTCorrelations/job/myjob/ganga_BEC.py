myApp = GaudiExec()
myApp.directory = "${BASE_BEC}/DaVinciDev_v36r7"
myApp.platform = "x86_64-slc6-gcc49-opt"
appOpts = '${BASE_BEC}/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/'

def jobBEC(data,collision,magnet,test=False):
    """define and run a job for given data type (RD/MC), collision type (pPb/Pbp) and magnet polarity (MU/MD)"""
    j = Job(application=myApp)
    j.name="_".join([data,collision,magnet])
      
    if (data == "RD" and test == False):
        if collision == "Pbp":
            if magnet == "MD":
                j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/RD_Pbp_MD.py']
                j.application.readInputData(appOpts+'data/LHCb_Ionproton13_90000000_Beam4000GeVVeloClosedMagDown_RealData_Reco14r1_Stripping20r3_ALL.py')
            if magnet == "MU":
                j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/RD_Pbp_MU.py']
                j.application.readInputData(appOpts+'data/LHCb_Ionproton13_90000000_Beam4000GeVVeloClosedMagUp_RealData_Reco14r1_Stripping20r3_ALL.py')
        if collision == "pPb":
            if magnet == "MD":
                j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/RD_pPb_MD.py']
                j.application.readInputData(appOpts+'data/LHCb_Protonion13_90000000_Beam4000GeVVeloClosedMagDown_RealData_Reco14r1_Stripping20r3_ALL.py')
            if magnet == "MU":
                j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/RD_pPb_MU.py']
                j.application.readInputData(appOpts+'data/LHCb_Protonion13_90000000_Beam4000GeVVeloClosedMagUp_RealData_Reco14r1_Stripping20r3_ALL.py')

        j.backend = Dirac()
        j.outputfiles = [DiracFile('*.root')]
        j.splitter = SplitByFiles(filesPerJob = 10)
             
    if (data == "RD" and test == True):
       j.name = j.name+"_test"

       if collision == "Pbp":
           if magnet == "MD":
               j.application.options = [appOpts+'options/HBTCorrelations_test.py', appOpts+'options/RD_Pbp_MD.py']
               j.application.readInputData(appOpts+'data/LHCb_Ionproton13_90000000_Beam4000GeVVeloClosedMagDown_RealData_Reco14r1_Stripping20r3_ALL.py')
       if collision == "pPb":
           if magnet == "MD":
               j.application.options = [appOpts+'options/HBTCorrelations_test.py', appOpts+'options/RD_pPb_MD.py']
               j.application.readInputData(appOpts+'data/LHCb_Protonion13_90000000_Beam4000GeVVeloClosedMagDown_RealData_Reco14r1_Stripping20r3_ALL.py')

       j.backend = Dirac()
       j.outputfiles = [DiracFile('*.root')]
       j.splitter = SplitByFiles(filesPerJob = 10, maxFiles = 20)

    if (data == "MC" and test == False):
        if collision == "Pbp":
            if magnet == "MD":
                j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/MC_Pbp_MD.py']
                j.application.readInputData(appOpts+'data/MC_2013_30000000_PbpBeam1580GeV4000GeV2013MagDownFix1Epos_Sim09c_L0Trig0x1710_Trig0x406a1710_Reco14r1_Stripping20r3p1_ALLSTREAMS.py')
            if magnet == "MU":
                j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/MC_Pbp_MU.py']
                j.application.readInputData(appOpts+'data/MC_2013_30000000_PbpBeam1580GeV4000GeV2013MagUpFix1Epos_Sim09c_L0Trig0x1710_Trig0x406a1710_Reco14r1_Stripping20r3p1_ALLSTREAMS.py')
        if collision == "pPb":
            if magnet == "MD":
                j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/MC_pPb_MD.py']
                j.application.readInputData(appOpts+'data/MC_2013_30000000_pPbBeam4000GeV1580GeV2013MagDownFix1Epos_Sim09c_L0Trig0x1710_Trig0x406a1710_Reco14r1_Stripping20r3p1_ALLSTREAMS.py')
            if magnet == "MU":
                j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/MC_pPb_MU.py']
                j.application.readInputData(appOpts+'data/MC_2013_30000000_pPbBeam4000GeV1580GeV2013MagUpFix1Epos_Sim09c_L0Trig0x1710_Trig0x406a1710_Reco14r1_Stripping20r3p1_ALLSTREAMS.py')

        j.backend = Dirac()
        j.outputfiles = [DiracFile('*.root')]
        j.splitter = SplitByFiles(filesPerJob = 5, ignoremissing = True)

    if (data == "MC" and test == True):
       j.name = j.name+"_test"

       if collision == "Pbp":
           if magnet == "MD":
               j.application.options = [appOpts+'options/HBTCorrelations_test.py', appOpts+'options/MC_Pbp_MD.py']
               j.application.readInputData(appOpts+'data/MC_2013_30000000_PbpBeam1580GeV4000GeV2013MagDownFix1Epos_Sim09c_L0Trig0x1710_Trig0x406a1710_Reco14r1_Stripping20r3p1_ALLSTREAMS.py')
       if collision == "pPb":
           if magnet == "MD":
               j.application.options = [appOpts+'options/HBTCorrelations_test.py', appOpts+'options/MC_pPb_MD.py']
               j.application.readInputData(appOpts+'data/MC_2013_30000000_pPbBeam4000GeV1580GeV2013MagDownFix1Epos_Sim09c_L0Trig0x1710_Trig0x406a1710_Reco14r1_Stripping20r3p1_ALLSTREAMS.py')

       j.backend = Dirac()
       j.outputfiles = [DiracFile('*.root')]
       j.splitter = SplitByFiles(filesPerJob = 5, maxFiles = 10, ignoremissing = True)
    
    print j
    queues.add(j.submit)  

#j.inputdata = BKQuery('/LHCb/Ionproton13/Beam4000GeV-VeloClosed-MagDown/Real Data/Reco14r1/Stripping20r3/90000000/ALL.DST').getDataset() 
def merge_RD_Ap_MU():
    merge_root_output(102,"hbtNTuple.root","HBT","/eos/lhcb/user/b/bmalecki/BEC_pPb/ntuple/RD_Pbp_MU/hbtNTuple.root")
def merge_RD_pA_MU():
    merge_root_output(94,"hbtNTuple.root","HBT","/eos/lhcb/user/b/bmalecki/BEC_pPb/ntuple/RD_pPb_MU/hbtNTuple.root")
def merge_RD_Ap_MD():
    merge_root_output(83,"hbtNTuple.root","HBT","/eos/lhcb/user/b/bmalecki/BEC_pPb/ntuple/RD_Pbp_MD/hbtNTuple.root")
def merge_RD_pA_MD():
    merge_root_output(93,"hbtNTuple.root","HBT","/eos/lhcb/user/b/bmalecki/BEC_pPb/ntuple/RD_pPb_MD/hbtNTuple.root")

def merge_MC_Ap_MU():
    merge_root_output(112,"hbtNTuple.root","HBT","/eos/lhcb/user/b/bmalecki/BEC_pPb/ntuple/MC_Pbp_MU/hbtNTuple.root")
def merge_MC_pA_MU():
    merge_root_output(107,"hbtNTuple.root","HBT","/eos/lhcb/user/b/bmalecki/BEC_pPb/ntuple/MC_pPb_MU/hbtNTuple.root")
def merge_MC_Ap_MD():
    merge_root_output(113,"hbtNTuple.root","HBT","/eos/lhcb/user/b/bmalecki/BEC_pPb/ntuple/MC_Pbp_MD/hbtNTuple.root")
def merge_MC_pA_MD():
    merge_root_output(108,"hbtNTuple.root","HBT","/eos/lhcb/user/b/bmalecki/BEC_pPb/ntuple/MC_pPb_MD/hbtNTuple.root")





