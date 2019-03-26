import sys, os, signal, math
def signal_handler(signal, frame):
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)

os.environ["ROOT_INCLUDE_PATH"] = os.pathsep + os.environ["MEERKATROOT"]

sigma_pid_1d = 0.01
sigma_mom_1d = 0.1
sigma_eta_1d = 0.1 
sigma_ntr_1d = 0.1 

sigma_pid_2d = 0.025
sigma_mom_2d = 0.25 
sigma_eta_2d = 0.25 

sigma_pid_3d = 0.05
sigma_mom_3d = 0.5 
sigma_eta_3d = 0.5 

sigma_pid_4d = 0.1
sigma_mom_4d = 1. 
sigma_eta_4d = 1. 
sigma_ntr_4d = 0.5 

draw = False
bootstrap_file = None

import argparse

parser = argparse.ArgumentParser(description='PIDPdf')
parser.add_argument('-a', type=str, default = None, 
                    help='Bootstrap sample with temporary file as the argument')
parser.add_argument('-b', type=str, default = "pidmometantr.root", 
                    help='Binning file name')
parser.add_argument('-c', type=str, default = "pid_estim.root", 
                    help='Control ROOT file name')
parser.add_argument('-e', help='Electron PID mode (reverse the order of Pt and Eta expansion)', 
                    action = "store_const", const = True, default = False)
parser.add_argument('-f', type=str, default = "pid_estim.png", 
                    help='Control PNG file name')
parser.add_argument('-g', type=int, default = 1000000, 
                    help='MC generation sample size')
parser.add_argument('-i', type=float, default = 1.0, 
                    help='PID variable kernel width scale factor')
parser.add_argument('-m', help='MC generation mode', 
                    action = "store_const", const = True, default = False)
parser.add_argument('-n', type=str, default = "pid", 
                    help='Ntuple name')
parser.add_argument('-o', type=str, default = ".", 
                    help='output directory')
parser.add_argument('-p', type=int, default = 1000000, 
                    help='control ntuple size')
parser.add_argument('-r', type=int, default = 12345, 
                    help='Random seed')
parser.add_argument('-s', type=float, default = 1.0, 
                    help='common kernel width scale factor')
parser.add_argument('-t', type=int, default = 2000000, 
                    help='toy MC normalisation sample size')
parser.add_argument('-v', type=str, default = "ProbNNK", 
                    help='PID variable name')
parser.add_argument('infile', type=str, help='Input ROOT file')
parser.add_argument('-l', type=float, default = 0.0, 
                    help='Lower PID value')
parser.add_argument('-u', type=float, default = 1.0, 
                    help='Upper PID value')


parser.print_help()
args = parser.parse_args()

print args

sigma_scale = args.s
pid_scale = args.i
pidname = args.v
toystat = args.t
tupstat = args.p
outputdir = args.o
infilename = args.infile
ntname = args.n
mcgen = args.m
genstat = args.g
outputcontrol = args.c
outputfig = args.f
outputbin = args.b
seed = args.r
bootstrap_file = args.a
pidmin = args.l
pidmax = args.u
elec_mode = args.e

print "Common kernel width scale is %f" % sigma_scale
print "PID variable name is %s" % pidname

from ROOT import TNtuple, TFile, TH1F, TH2F, TCanvas, TMath, TRandom3, TStyle, RooRealVar, gSystem, gStyle

gSystem.Load("libMeerkatLib.so")

from ROOT import OneDimPhaseSpace, CombinedPhaseSpace, BinnedKernelDensity, BinnedDensity, AdaptiveKernelDensity
from ROOT import FactorisedDensity, FormulaDensity, TransposedFactorisedDensity

#pidlimits = {
#  'CombDLLe' : (-20., 20.), 
#  'CombDLLK' : (-20., 100.), 
#}
#pidmin = 0.
#pidmax = 1.
#if pidname in pidlimits : 
#  pidmin = pidlimits[pidname][0]
#  pidmax = pidlimits[pidname][1]

sigma_pid_1d *= (pidmax-pidmin)
sigma_pid_2d *= (pidmax-pidmin)
sigma_pid_3d *= (pidmax-pidmin)
sigma_pid_4d *= (pidmax-pidmin)

sigma_pid_1d *= sigma_scale*pid_scale
sigma_mom_1d *= sigma_scale
sigma_eta_1d *= sigma_scale
sigma_ntr_1d *= sigma_scale

sigma_pid_2d *= sigma_scale*pid_scale
sigma_mom_2d *= sigma_scale 

sigma_pid_3d *= sigma_scale*pid_scale
sigma_mom_3d *= sigma_scale 
sigma_eta_3d *= sigma_scale 

sigma_pid_4d *= sigma_scale*pid_scale
sigma_mom_4d *= sigma_scale 
sigma_eta_4d *= sigma_scale 
sigma_ntr_4d *= sigma_scale 

pid_phsp = OneDimPhaseSpace("PIDPhsp", pidmin, pidmax)
mom_phsp = OneDimPhaseSpace("MomPhsp", 5.5, 9.5)
eta_phsp = OneDimPhaseSpace("EtaPhsp", 1.5, 5.5)
ntr_phsp = OneDimPhaseSpace("NtrPhsp", 3.0, 6.5)

pidmom_phsp = CombinedPhaseSpace("PIDMomPhsp", pid_phsp, mom_phsp) 
pideta_phsp = CombinedPhaseSpace("PIDEtaPhsp", pid_phsp, eta_phsp) 
pidmometa_phsp = CombinedPhaseSpace("PIDMomEtaPhsp", pidmom_phsp, eta_phsp) 
phsp = CombinedPhaseSpace("FullPhsp", pidmometa_phsp, ntr_phsp) 

if mcgen : 

  bin_pdf = BinnedDensity("BinPDF", phsp, infilename) 
  tmpfile = outputdir + "/" + outputbin + ".tmp"
  outfile = TFile.Open(tmpfile, "RECREATE")
  kde_tuple = TNtuple(ntname, "MC tuple", "%s:Pt:Eta:Ntracks" % pidname) 
  bin_pdf.setSeed(seed)
  bin_pdf.generate(kde_tuple, genstat)
  outfile.Close()
  infile = TFile.Open(tmpfile)
  if not infile : 
    print "File %s not found!" % tmpfile
    quit()

else :

  if bootstrap_file :
    # Make temporary bootstrapped sample
    rnd = TRandom3()
    rnd.SetSeed(seed)
    f2 = TFile.Open(bootstrap_file,"RECREATE")
    nt2 = TNtuple(ntname + "_boot","pid", pidname + ":Pt:Eta:Ntracks:Charge:w")
    f1 = TFile.Open(infilename)
    if not f1 : 
      print "File %s not found!" % infilename
      quit()
    nt1 = f1.Get(ntname)
    if not nt1 : 
      print "Ntuple %s not found!" % ntname
      quit()
    f2.cd()
    for i in nt1 :
      nt2.Fill(eval("i." + pidname), i.Pt, i.Eta, i.Ntracks, i.Charge, i.w*rnd.Poisson(1.) )
    nt2.Write()
    f2.Close()
    f1.Close()
    ntname = ntname + "_boot"
    infile = TFile.Open(bootstrap_file)

  else :
    infile = TFile.Open(infilename)

  if not infile :
    print "File %s not found!" % infilename
    quit()

ntuple = infile.Get(ntname) 
if not ntuple : 
  print "Ntuple %s not found!" % ntname
  quit()

infile.cd()

kde_pid = BinnedKernelDensity("KDEPDF_PID", pid_phsp, ntuple,
                              pidname,"w", 200, sigma_pid_1d, 
                              0, toystat ) 

ada_pid = AdaptiveKernelDensity("AdaPDF_PID", pid_phsp, ntuple,
                                pidname,"w", 200, sigma_pid_1d,
                                kde_pid, 0, toystat ) 

kde_mom = BinnedKernelDensity("KDEPDF_Mom", mom_phsp, ntuple,
                              "Pt","w", 200, sigma_mom_1d,
                              0, toystat ) 

kde_eta = BinnedKernelDensity("KDEPDF_Eta", eta_phsp, ntuple,
                              "Eta","w", 200, sigma_eta_1d,
                              0, toystat ) 

kde_ntr = BinnedKernelDensity("KDEPDF_Ntr", ntr_phsp, ntuple,
                              "Ntracks","w", 200, sigma_ntr_1d,
                              0, toystat ) 

if not elec_mode : 
  fact_pidmom = FactorisedDensity("FactPDF_PIDMom", pidmom_phsp, ada_pid, kde_mom)

  kde_pidmom = BinnedKernelDensity("KDEPDF_PIDMom", pidmom_phsp, ntuple, 
                          pidname,"Pt","w", 100,100, sigma_pid_2d, sigma_mom_2d,
                          fact_pidmom, toystat )

  ada_pidmom = AdaptiveKernelDensity("AdaPDF_PIDMom", pidmom_phsp, ntuple,
                          pidname,"Pt","w", 100,100, sigma_pid_2d, sigma_mom_2d,
                          kde_pidmom, fact_pidmom, toystat )

  fact_pidmometa = FactorisedDensity("FactPDF_PIDMomEta", pidmometa_phsp, ada_pidmom, kde_eta)

  kde_pidmometa = BinnedKernelDensity("KDEPDF_PIDMomEta", pidmometa_phsp, ntuple, 
                          pidname,"Pt","Eta","w", 50,50,50, sigma_pid_3d, sigma_mom_3d, sigma_eta_3d,
                          fact_pidmometa, toystat )

  ada_pidmometa = AdaptiveKernelDensity("AdaPDF_PIDMomEta", pidmometa_phsp, ntuple, 
                          pidname,"Pt","Eta","w", 100,100,100, sigma_pid_3d, sigma_mom_3d, sigma_eta_3d,
                          kde_pidmometa, fact_pidmometa, toystat )

else : 
  fact_pideta = FactorisedDensity("FactPDF_PIDEta", pidmom_phsp, ada_pid, kde_eta)

  kde_pideta = BinnedKernelDensity("KDEPDF_PIDEta", pideta_phsp, ntuple, 
                          pidname,"Eta","w", 100,100, sigma_pid_2d, sigma_eta_2d,
                          fact_pideta, toystat )

  ada_pideta = AdaptiveKernelDensity("AdaPDF_PIDEta", pideta_phsp, ntuple,
                          pidname,"Eta","w", 100,100, sigma_pid_2d, sigma_eta_2d,
                          kde_pideta, fact_pideta, toystat )

  fact_pidmometa = TransposedFactorisedDensity("FactPDF_PIDMomEta", pidmometa_phsp, ada_pideta, kde_mom, 1)

  kde_pidmometa = BinnedKernelDensity("KDEPDF_PIDMomEta", pidmometa_phsp, ntuple, 
                          pidname,"Pt","Eta","w", 50,50,50, sigma_pid_3d, sigma_mom_3d, sigma_eta_3d,
                          fact_pidmometa, toystat )

  ada_pidmometa = AdaptiveKernelDensity("AdaPDF_PIDMomEta", pidmometa_phsp, ntuple, 
                          pidname,"Pt","Eta","w", 100,100,100, sigma_pid_3d, sigma_mom_3d, sigma_eta_3d,
                          kde_pidmometa, fact_pidmometa, toystat )

fact_pidmometantr = FactorisedDensity("FactPDF_PIDMomEtaNtr", phsp, ada_pidmometa, kde_ntr)

kde_pidmometantr = BinnedKernelDensity("KDEPDF_PIDMomEtaNtr", phsp, ntuple, 
                          pidname,"Pt","Eta","Ntracks","w", 50, 50, 50, 10, sigma_pid_4d, sigma_mom_4d, sigma_eta_4d, sigma_ntr_4d,
                          fact_pidmometantr, toystat )

ada_pidmometantr = AdaptiveKernelDensity("AdaPDF_PIDMomEtaNtr", phsp, ntuple, 
                          pidname,"Pt","Eta","Ntracks","w", 70, 70, 70, 20, sigma_pid_4d, sigma_mom_4d, sigma_eta_4d, sigma_ntr_4d,
                          kde_pidmometantr, fact_pidmometantr, toystat )

ada_pidmometantr.writeToFile(outputdir + "/" + outputbin)

bin_pidmometantr = BinnedDensity("BinPDF_PIDMomEtaNtr", phsp, outputdir + "/" + outputbin, 1000.)

outfile = TFile.Open(outputdir + "/" + outputcontrol, "RECREATE")
outfile.cd()

proj_mom_h = TH1F("proj_mom", "Log(Momentum) Hist", 100, mom_phsp.lowerLimit(0), mom_phsp.upperLimit(0)) 
kde_mom_h  = TH1F("kde_mom",  "Log(Momentum) KDE",  100, mom_phsp.lowerLimit(0), mom_phsp.upperLimit(0)) 
proj_eta_h = TH1F("proj_eta", "Eta Hist", 100, eta_phsp.lowerLimit(0), eta_phsp.upperLimit(0)) 
kde_eta_h  = TH1F("kde_eta",  "Eta KDE",  100, eta_phsp.lowerLimit(0), eta_phsp.upperLimit(0)) 
proj_pid_h = TH1F("proj_pid", "PID Hist", 100, pid_phsp.lowerLimit(0), pid_phsp.upperLimit(0)) 
kde_pid_h  = TH1F("kde_pid",  "PID KDE",  100, pid_phsp.lowerLimit(0), pid_phsp.upperLimit(0)) 
proj_ntr_h = TH1F("proj_ntr", "Ntr Hist", 100, ntr_phsp.lowerLimit(0), ntr_phsp.upperLimit(0)) 
kde_ntr_h  = TH1F("kde_ntr",  "Ntr KDE",  100, ntr_phsp.lowerLimit(0), ntr_phsp.upperLimit(0)) 

proj_pidmom_h = TH2F("proj_pidmom", "Log(Momentum) vs PID Hist", 20, phsp.lowerLimit(0), phsp.upperLimit(0), 
	                                   20, phsp.lowerLimit(1), phsp.upperLimit(1)) 
fact_pidmom_h = TH2F("fact_pidmom", "Log(Momentum) vs PID Fact", 100, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                           100, phsp.lowerLimit(1), phsp.upperLimit(1)) 
kde_pidmom_h = TH2F("kde_pidmom", "Log(Momentum) vs PID fixed KDE", 
                                           100, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                           100, phsp.lowerLimit(1), phsp.upperLimit(1)) 
ada_pidmom_h = TH2F("ada_pidmom", "Log(Momentum) vs PID adaptive KDE", 
                                           100, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                           100, phsp.lowerLimit(1), phsp.upperLimit(1)) 

proj_pideta_h = TH2F("proj_pideta", "Log(Momentum) vs PID Hist", 20, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                            20, phsp.lowerLimit(2), phsp.upperLimit(2)) 
fact_pideta_h = TH2F("fact_pideta", "Log(Momentum) vs PID Fact", 100, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                           100, phsp.lowerLimit(2), phsp.upperLimit(2)) 
kde_pideta_h = TH2F("kde_pideta", "Log(Momentum) vs PID fixed KDE", 
                                           100, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                           100, phsp.lowerLimit(2), phsp.upperLimit(2)) 
ada_pideta_h = TH2F("ada_pideta", "Log(Momentum) vs PID adaptive KDE", 
                                           100, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                           100, phsp.lowerLimit(2), phsp.upperLimit(2)) 


kde_pidmom_slice = TH2F("kde_pidmom_slice", "Log(Momentum) vs PID KDE, slice", 
                                           100, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                           100, phsp.lowerLimit(1), phsp.upperLimit(1)) 

kde_pideta_slice = TH2F("kde_pideta_slice", "Eta vs PID KDE, slice", 
                                           100, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                           100, phsp.lowerLimit(2), phsp.upperLimit(2)) 

kde_mometa_slice = TH2F("kde_mometa_slice", "Eta vs Log(Momentum) KDE, slice", 
                                           100, phsp.lowerLimit(1), phsp.upperLimit(1), 
                                           100, phsp.lowerLimit(2), phsp.upperLimit(2)) 

kde_pidntr_slice = TH2F("kde_pidntr_slice", "Ntracks vs PID KDE, slice", 
                                           100, phsp.lowerLimit(0), phsp.upperLimit(0), 
                                           100, phsp.lowerLimit(3), phsp.upperLimit(3)) 

from ROOT import std, Double

v = std.vector(Double)(2) 

if ntuple.GetBranch("w") : 
  ntuple.Project("proj_ntr", "Ntracks", "w")
  ntuple.Project("proj_mom", "Pt", "w")
  ntuple.Project("proj_eta", "Eta", "w")
  ntuple.Project("proj_pid", pidname, "w")
  ntuple.Project("proj_pidmom", "Pt:" + pidname, "w")
  ntuple.Project("proj_pideta", "Eta:" + pidname, "w")
else : 
  ntuple.Project("proj_ntr", "Ntracks")
  ntuple.Project("proj_mom", "Pt")
  ntuple.Project("proj_eta", "Eta")
  ntuple.Project("proj_pid", pidname)
  ntuple.Project("proj_pidmom", "Pt:" + pidname)
  ntuple.Project("proj_pideta", "Eta:" + pidname)

kde_ntr.project(kde_ntr_h) 
kde_mom.project(kde_mom_h) 
kde_eta.project(kde_eta_h) 
ada_pid.project(kde_pid_h) 

if not elec_mode : 
  fact_pidmom.project(fact_pidmom_h) 
  kde_pidmom.project(kde_pidmom_h) 
  ada_pidmom.project(ada_pidmom_h) 
else : 
  fact_pideta.project(fact_pideta_h) 
  kde_pideta.project(kde_pideta_h) 
  ada_pideta.project(ada_pideta_h) 

v4 = std.vector(Double)(4)
v4[0] = 0.7
v4[1] = 8.0
v4[2] = 3.5
v4[3] = 5

bin_pidmometantr.slice(v4, 0, 1, kde_pidmom_slice)
bin_pidmometantr.slice(v4, 0, 2, kde_pideta_slice)
bin_pidmometantr.slice(v4, 1, 2, kde_mometa_slice)
bin_pidmometantr.slice(v4, 0, 3, kde_pidntr_slice)

kde_tuple = TNtuple("kde_tuple", "KDE test tuple", "%s:Pt:Eta:Ntracks" % pidname) 
bin_pidmometantr.generate(kde_tuple, tupstat)

proj_mom_h.Write() 
proj_ntr_h.Write() 
proj_eta_h.Write() 
proj_pid_h.Write() 
proj_pidmom_h.Write() 
proj_pideta_h.Write() 
kde_mom_h.Write() 
kde_ntr_h.Write() 
kde_eta_h.Write() 
kde_pid_h.Write() 
if not elec_mode : 
  fact_pidmom_h.Write() 
  kde_pidmom_h.Write() 
  ada_pidmom_h.Write() 
else : 
  fact_pideta_h.Write() 
  kde_pideta_h.Write() 
  ada_pideta_h.Write() 
kde_pidmom_slice.Write() 
kde_pideta_slice.Write() 
kde_mometa_slice.Write() 
kde_pidntr_slice.Write() 

if draw : 
  gStyle.SetOptStat(0)

  canvas = TCanvas("canvas", "CombinedPdf", 1200, 900) 
  canvas.Divide(4,3) 

  kde_mom_h.Scale( proj_mom_h.GetSumOfWeights() / kde_mom_h.GetSumOfWeights() ) 
  kde_ntr_h.Scale( proj_ntr_h.GetSumOfWeights() / kde_ntr_h.GetSumOfWeights() ) 
  kde_eta_h.Scale( proj_eta_h.GetSumOfWeights() / kde_eta_h.GetSumOfWeights() ) 
  kde_pid_h.Scale( proj_pid_h.GetSumOfWeights() / kde_pid_h.GetSumOfWeights() ) 
  kde_mom_h.SetLineColor(2) 
  kde_ntr_h.SetLineColor(2) 
  kde_eta_h.SetLineColor(2) 
  kde_pid_h.SetLineColor(2) 

  canvas.cd(1);  proj_pid_h.Draw();  kde_pid_h.Draw("lsame")
  canvas.cd(2);  proj_mom_h.Draw();  kde_mom_h.Draw("lsame")
  canvas.cd(3);  proj_eta_h.Draw();  kde_eta_h.Draw("lsame")
  canvas.cd(4);  proj_ntr_h.Draw();  kde_ntr_h.Draw("lsame")

  if not elec_mode : 
    canvas.cd(5);  proj_pidmom_h.Draw("zcol") 
    canvas.cd(6);  fact_pidmom_h.Draw("zcol") 
    canvas.cd(7);  kde_pidmom_h.Draw("zcol") 
    canvas.cd(8);  ada_pidmom_h.Draw("zcol") 
  else : 
    canvas.cd(5);  proj_pideta_h.Draw("zcol") 
    canvas.cd(6);  fact_pideta_h.Draw("zcol") 
    canvas.cd(7);  kde_pideta_h.Draw("zcol") 
    canvas.cd(8);  ada_pideta_h.Draw("zcol") 

  canvas.cd(9);  kde_pidmom_slice.Draw("zcol") 
  canvas.cd(10); kde_pideta_slice.Draw("zcol") 
  canvas.cd(11); kde_mometa_slice.Draw("zcol") 
  canvas.cd(12); kde_pidntr_slice.Draw("zcol") 

  canvas.Print(outputdir + "/" + outputfig) 

infile.cd() 
infile.Close()
outfile.cd() 
outfile.Close()
