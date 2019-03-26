eosdir = "/eos/lhcb/wg/PID/PIDGen/Data/Run2"
eosrootdir = "root://eoslhcb.cern.ch/" + eosdir

tmpdir = "/tmp/poluekt/"

datasets = {
  'MagDown_2016'   : ("/eos/lhcb/grid/prod/lhcb/LHCb/Collision16/PIDCALIB.ROOT/00056408/0000/00056408_%8.8d_1.pidcalib.root", 182), 
  'MagUp_2016'     : ("/eos/lhcb/grid/prod/lhcb/LHCb/Collision16/PIDCALIB.ROOT/00056409/0000/00056409_%8.8d_1.pidcalib.root", 185), 
  'MagDown_2015'   : ("/eos/lhcb/grid/prod/lhcb/LHCb/Collision15/PIDCALIB.ROOT/00057802/0000/00057802_%8.8d_1.pidcalib.root", 87), 
  'MagUp_2015'     : ("/eos/lhcb/grid/prod/lhcb/LHCb/Collision15/PIDCALIB.ROOT/00057800/0000/00057800_%8.8d_1.pidcalib.root", 48), 
}

samples = {
  "p_Lam0_Brunel" : {
    'trees'   : ("Lam0_PTuple/DecayTree", "Lam0_PbarTuple/DecayTree", "Lam0_HPT_PTuple/DecayTree", "Lam0_HPT_PbarTuple/DecayTree", "Lam0_VHPT_PTuple/DecayTree", "Lam0_VHPT_PbarTuple/DecayTree"), 
    'pt'      : "probe_Brunel_PT", 
    'eta'     : "probe_Brunel_ETA", 
    'ntracks' : "nTracks", 
    'weight'  : "probe_sWeight"
  }, 
  "K_DSt_Brunel" : {
    'trees'   : ("DSt_KPTuple/DecayTree", "DSt_KMTuple/DecayTree"), 
    'pt'      : "probe_Brunel_PT", 
    'eta'     : "probe_Brunel_ETA", 
    'ntracks' : "nTracks", 
    'weight'  : "probe_sWeight"
  }, 
  "pi_DSt_Brunel" : {
    'trees'   : ("DSt_PiPTuple/DecayTree", "DSt_PiMTuple/DecayTree"), 
    'pt'      : "probe_Brunel_PT", 
    'eta'     : "probe_Brunel_ETA", 
    'ntracks' : "nTracks", 
    'weight'  : "probe_sWeight"
  }, 
}

configs = {
  "p_MC15TuneV1_ProbNNp_Brunel" : {
    "sample"        :  "p_Lam0_Brunel", 
    "var"           :  "probe_Brunel_MC15TuneV1_ProbNNp", 
    "bins"          :  50, 
    "gamma"         : -0.3, 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'toystat'       : 20000000, 
    'controlstat'   :  1000000, 
    'nbootstrap'    :  5
  }, 
  "p_MC15TuneV1_ProbNNK_Brunel" : {
    "sample"        :  "p_Lam0_Brunel", 
    "var"           :  "probe_Brunel_MC15TuneV1_ProbNNk", 
    "bins"          :  50, 
    "gamma"         :  0.3, 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'toystat'       : 20000000, 
    'controlstat'   :  1000000, 
    'nbootstrap'    :  5
  }, 
  "p_MC15TuneV1_ProbNNpi_Brunel" : {
    "sample"        :  "p_Lam0_Brunel", 
    "var"           :  "probe_Brunel_MC15TuneV1_ProbNNpi", 
    "bins"          :  50, 
    "gamma"         :  0.15, 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'toystat'       : 20000000, 
    'controlstat'   :  1000000, 
    'nbootstrap'    :  5
  }, 
  "K_MC15TuneV1_ProbNNK_Brunel" : {
    "sample"        :  "K_DSt_Brunel", 
    "var"           :  "probe_Brunel_MC15TuneV1_ProbNNk", 
    "bins"          :  100, 
    "gamma"         : -0.2, 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'toystat'       : 20000000, 
    'controlstat'   :  1000000, 
    'nbootstrap'    :  5
  }, 
  "K_MC15TuneV1_ProbNNpi_Brunel" : {
    "sample"        :  "K_DSt_Brunel", 
    "var"           :  "probe_Brunel_MC15TuneV1_ProbNNpi", 
    "bins"          :  100, 
    "gamma"         :  0.2, 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'toystat'       : 20000000, 
    'controlstat'   :  1000000, 
    'nbootstrap'    :  5
  }, 
  "K_MC15TuneV1_ProbNNp_Brunel" : {
    "sample"        :  "K_DSt_Brunel", 
    "var"           :  "probe_Brunel_MC15TuneV1_ProbNNp", 
    "bins"          :  100, 
    "gamma"         :  0.3, 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'toystat'       : 20000000, 
    'controlstat'   :  1000000, 
    'nbootstrap'    :  5
  }, 
  "pi_MC15TuneV1_ProbNNpi_Brunel" : {
    "sample"        :  "pi_DSt_Brunel", 
    "var"           :  "probe_Brunel_MC15TuneV1_ProbNNpi", 
    "bins"          :  100, 
    "gamma"         : -0.2, 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'toystat'       : 20000000, 
    'controlstat'   :  1000000, 
    'nbootstrap'    :  5
  }, 
  "pi_MC15TuneV1_ProbNNK_Brunel" : {
    "sample"        :  "pi_DSt_Brunel",
    "var"           :  "probe_Brunel_MC15TuneV1_ProbNNk",
    "bins"          :  100,
    "gamma"         :  0.05,
    'scale_default' :  0.1,
    'scale_syst'    :  0.15,
    'scale_pid'     :  1.0,
    'toystat'       : 20000000,
    'controlstat'   :  1000000,
    'nbootstrap'    :  5
  },
  "pi_MC15TuneV1_ProbNNp_Brunel" : {
    "sample"        :  "pi_DSt_Brunel", 
    "var"           :  "probe_Brunel_MC15TuneV1_ProbNNp", 
    "bins"          :  100, 
    "gamma"         :  0.15, 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'toystat'       : 20000000, 
    'controlstat'   :  1000000, 
    'nbootstrap'    :  5
  }, 
  "pi_CombDLLK_Brunel" : {
    "sample"        : "pi_DSt_Brunel", 
    "var"           : "probe_Brunel_PIDK", 
    "bins"          : 100, 
    "gamma"         : 1., 
    'scale_default' : 0.1, 
    'scale_syst'    : 0.15, 
    'scale_pid'     : 1.0,
    'limits'        : (-150., 50.), 
    'toystat'       : 10000000, 
    'controlstat'   : 5000000, 
    'nbootstrap'    : 5
  },
  "pi_CombDLLp_Brunel" : {
    "sample"        :  "pi_DSt_Brunel", 
    "var"           :  "probe_Brunel_PIDp", 
    "bins"          :  100, 
    "gamma"         :  1., 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'limits'        : (-150., 70),
    'toystat'       : 10000000, 
    'controlstat'   : 5000000, 
    'nbootstrap'    : 5
  },
  "p_CombDLLp_Brunel" : {
    "sample"        : "p_Lam0_Brunel", 
    "var"           : "probe_Brunel_PIDp", 
    "bins"          : 100, 
    "gamma"         : 1., 
    'scale_default' : 0.1, 
    'scale_syst'    : 0.15, 
    'scale_pid'     : 1.0, 
    'limits'        : (-30., 120), 
    'toystat'       : 20000000, 
    'controlstat'   : 1000000, 
    'nbootstrap'    : 5
  }, 
  "p_CombDLLK_Brunel" : {
    "sample"        : "p_Lam0_Brunel", 
    "var"           : "probe_Brunel_PIDK", 
    "bins"          : 100, 
    "gamma"         : 1., 
    'scale_default' : 0.1, 
    'scale_syst'    : 0.15, 
    'scale_pid'     : 1.0, 
    'limits'        : (-50., 60), 
    'toystat'       : 20000000, 
    'controlstat'   : 1000000, 
    'nbootstrap'    : 5
  }, 
  "K_CombDLLK_Brunel" : {
    "sample"        :  "K_DSt_Brunel", 
    "var"           :  "probe_Brunel_PIDK", 
    "bins"          :  100, 
    "gamma"         :  1., 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'limits'        : (-50., 150), 
    'toystat'       : 10000000, 
    'controlstat'   : 5000000, 
    'nbootstrap'    : 5
  }, 
  "K_CombDLLp_Brunel" : {
    "sample"        :  "K_DSt_Brunel", 
    "var"           :  "probe_Brunel_PIDp", 
    "bins"          :  100, 
    "gamma"         :  1., 
    'scale_default' :  0.1, 
    'scale_syst'    :  0.15, 
    'scale_pid'     :  1.0, 
    'limits'        : (-150., 100), 
    'toystat'       : 10000000, 
    'controlstat'   : 5000000, 
    'nbootstrap'    : 5
  }, 
}
