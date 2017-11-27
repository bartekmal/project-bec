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
           j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/RD_Pbp_MD.py']
           j.application.readInputData(appOpts+'data/test_ionproton.py')
       if collision == "pPb":
           j.application.options = [appOpts+'options/HBTCorrelations.py', appOpts+'options/RD_pPb_MD.py']
           j.application.readInputData(appOpts+'data/test_protonion.py')

       j.backend = Local()
       j.outputfiles = [LocalFile('*.root')]
       #j.splitter = SplitByFiles(filesPerJob = 2)
    
    print j
    queues.add(j.submit)  

#j.inputdata = BKQuery('/LHCb/Ionproton13/Beam4000GeV-VeloClosed-MagDown/Real Data/Reco14r1/Stripping20r3/90000000/ALL.DST').getDataset() 



