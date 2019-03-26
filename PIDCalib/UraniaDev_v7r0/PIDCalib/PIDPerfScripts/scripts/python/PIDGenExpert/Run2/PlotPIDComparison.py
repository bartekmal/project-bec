from ROOT import TFile, TH1F, TCanvas, gROOT, TText
import sys

def plotComparison(calibfile, controlfile, outfilename, nbins = 100, pidmin = 0., pidmax = 1.) : 

  gROOT.ProcessLine(".x lhcbstyle.C")

  print nbins

  bins = [
  "%s<7.5&&%s<2.9&&%s<5.0",
  "%s<7.5&&%s<2.9&&abs(%s-5.2)<0.2", 
  "%s<7.5&&%s<2.9&&%s>5.4", 

  "%s<7.5&&abs(%s-3.3)<0.4&&%s<5.0", 
  "%s<7.5&&abs(%s-3.3)<0.4&&abs(%s-5.2)<0.2", 
  "%s<7.5&&abs(%s-3.3)<0.4&&%s>5.4", 

  "%s<7.5&&%s>3.7&&%s<5.0", 
  "%s<7.5&&%s>3.7&&abs(%s-5.2)<0.2", 
  "%s<7.5&&%s>3.7&&%s>5.4", 

  "abs(%s-8.)<0.5&&%s<2.9&&%s<5.0", 
  "abs(%s-8.)<0.5&&%s<2.9&&abs(%s-5.2)<0.2", 
  "abs(%s-8.)<0.5&&%s<2.9&&%s>5.4", 

  "abs(%s-8.)<0.5&&abs(%s-3.3)<0.4&&%s<5.0", 
  "abs(%s-8.)<0.5&&abs(%s-3.3)<0.4&&abs(%s-5.2)<0.2", 
  "abs(%s-8.)<0.5&&abs(%s-3.3)<0.4&&%s>5.4", 

  "abs(%s-8.)<0.5&&%s>3.7&&%s<5.0", 
  "abs(%s-8.)<0.5&&%s>3.7&&abs(%s-5.2)<0.2", 
  "abs(%s-8.)<0.5&&%s>3.7&&%s>5.4", 

  "%s>8.5&&%s<2.9&&%s<5.0", 
  "%s>8.5&&%s<2.9&&abs(%s-5.2)<0.2", 
  "%s>8.5&&%s<2.9&&%s>5.4", 

  "%s>8.5&&abs(%s-3.3)<0.4&&%s<5.0", 
  "%s>8.5&&abs(%s-3.3)<0.4&&abs(%s-5.2)<0.2", 
  "%s>8.5&&abs(%s-3.3)<0.4&&%s>5.4", 

  "%s>8.5&&%s>3.7&&%s<5.0", 
  "%s>8.5&&%s>3.7&&abs(%s-5.2)<0.2", 
  "%s>8.5&&%s>3.7&&%s>5.4", 

  "%s>0&&%s>0&&%s>0", 

  ]

  binnames = [
  'Mom low, eta low, Ntr low', 
  'Mom low, eta low, Ntr mid', 
  'Mom low, eta low, Ntr hi', 

  'Mom low, eta mid, Ntr low', 
  'Mom low, eta mid, Ntr mid', 
  'Mom low, eta mid, Ntr hi', 

  'Mom low, eta hi, Ntr low', 
  'Mom low, eta hi, Ntr mid', 
  'Mom low, eta hi, Ntr hi', 

  'Mom mid, eta low, Ntr low', 
  'Mom mid, eta low, Ntr mid', 
  'Mom mid, eta low, Ntr hi', 

  'Mom mid, eta mid, Ntr low', 
  'Mom mid, eta mid, Ntr mid', 
  'Mom mid, eta mid, Ntr hi', 

  'Mom mid, eta hi, Ntr low', 
  'Mom mid, eta hi, Ntr mid', 
  'Mom mid, eta hi, Ntr hi', 

  'Mom hi, eta low, Ntr low', 
  'Mom hi, eta low, Ntr mid', 
  'Mom hi, eta low, Ntr hi', 

  'Mom hi, eta mid, Ntr low', 
  'Mom hi, eta mid, Ntr mid', 
  'Mom hi, eta mid, Ntr hi', 

  'Mom hi, eta hi, Ntr low', 
  'Mom hi, eta hi, Ntr mid', 
  'Mom hi, eta hi, Ntr hi', 

  'All'
  ]

  hists1 = []
  hists2 = []

  var = "PID"

  n = 1
  f1 = TFile.Open(controlfile)
  nt1 = f1.Get("kde_tuple")
  for i in bins : 
    h1 = TH1F("h1_%d" % n, "", nbins, pidmin, pidmax)
    cut1 = i % ("Pt","Eta","Ntracks")
#    nt1.Project("h1_%d" % n, "sqrt(1.-(1.-pid))", cut1)
    nt1.Project("h1_%d" % n, var, cut1)
    print cut1, h1.GetSumOfWeights()
    h1.SetDirectory(0)
    hists1 += [ h1 ]
  f1.Close()

  n = 1
  f2 = TFile.Open(calibfile)
  nt2 = f2.Get("pid")
  for i in bins : 
    h2 = TH1F("h2_%d" % n, "", nbins, pidmin, pidmax)
    cut2 = i % ("Pt","Eta","Ntracks")
#    nt2.Project("h2_%d" % n, "sqrt(1.-(1.-ProbNNK))","w*(%s)" % cut2)
    nt2.Project("h2_%d" % n, var,"w*(%s)" % cut2)
    print cut2, h2.GetSumOfWeights()
    h2.SetDirectory(0)
    hists2 += [ h2 ]
  f2.Close()

  c = TCanvas("c", "c", 1200, 700)
  c.Divide(7,4)

  t = TText()

  for n in range(1, len(bins)+1) : 
    c.cd(n)
    h1 = hists1[n-1]
    h2 = hists2[n-1]
    h1.Scale(h2.GetSumOfWeights() / h1.GetSumOfWeights() )
    h1.SetLineColor(2)
    h1.GetXaxis().SetTitle(var + "'")
    h1.Draw("h")
    h2.SetLineColor(4)
    h2.SetLineWidth(1)
    h2.SetMarkerSize(0.5)
    h2.Draw("e same")
#    h1.Draw("same")
    t.DrawText(0.05, 0.9*h1.GetMaximum(), binnames[n-1])

  c.Update()
  c.Print(outfilename + "_hist.pdf")

  f1 = TFile.Open(controlfile)
  proj_mom_h = f1.Get("proj_mom")
  proj_ntr_h = f1.Get("proj_ntr")
  proj_eta_h = f1.Get("proj_eta")
  proj_pid_h = f1.Get("proj_pid")
  kde_mom_h = f1.Get("kde_mom")
  kde_ntr_h = f1.Get("kde_ntr")
  kde_eta_h = f1.Get("kde_eta")
  kde_pid_h = f1.Get("kde_pid")
  proj_pidmom_h = f1.Get("proj_pidmom")
  fact_pidmom_h = f1.Get("fact_pidmom")
  kde_pidmom_h = f1.Get("kde_pidmom")
  ada_pidmom_h = f1.Get("ada_pidmom")
  proj_pideta_h = f1.Get("proj_pideta")
  fact_pideta_h = f1.Get("fact_pideta")
  kde_pideta_h = f1.Get("kde_pideta")
  ada_pideta_h = f1.Get("ada_pideta")
  kde_pidmom_slice = f1.Get("kde_pidmom_slice")
  kde_pideta_slice = f1.Get("kde_pideta_slice")
  kde_mometa_slice = f1.Get("kde_mometa_slice")
  kde_pidntr_slice = f1.Get("kde_pidntr_slice")

  c2 = TCanvas("c2", "CombinedPdf", 1200, 900)
  c2.Divide(4,3)

  kde_mom_h.Scale( proj_mom_h.GetSumOfWeights() / kde_mom_h.GetSumOfWeights() )
  kde_ntr_h.Scale( proj_ntr_h.GetSumOfWeights() / kde_ntr_h.GetSumOfWeights() )
  kde_eta_h.Scale( proj_eta_h.GetSumOfWeights() / kde_eta_h.GetSumOfWeights() )
  kde_pid_h.Scale( proj_pid_h.GetSumOfWeights() / kde_pid_h.GetSumOfWeights() )
  kde_mom_h.SetLineColor(2)
  kde_ntr_h.SetLineColor(2)
  kde_eta_h.SetLineColor(2)
  kde_pid_h.SetLineColor(2)

  c2.cd(1);  proj_pid_h.Draw("h");  kde_pid_h.Draw("hist l same"); proj_pid_h.GetXaxis().SetTitle(var);
  c2.cd(2);  proj_mom_h.Draw("h");  kde_mom_h.Draw("hist l same"); proj_mom_h.GetXaxis().SetTitle("log(P_{T})");
  c2.cd(3);  proj_eta_h.Draw("h");  kde_eta_h.Draw("hist l same"); proj_eta_h.GetXaxis().SetTitle("#eta");
  c2.cd(4);  proj_ntr_h.Draw("h");  kde_ntr_h.Draw("hist l same"); proj_ntr_h.GetXaxis().SetTitle("log(N_{tracks})");

  if ada_pidmom_h : 
    c2.cd(5);  proj_pidmom_h.Draw("zcol"); proj_pidmom_h.GetXaxis().SetTitle(var); proj_pidmom_h.GetYaxis().SetTitle("log(P_{T})");
    c2.cd(6);  fact_pidmom_h.Draw("zcol"); fact_pidmom_h.GetXaxis().SetTitle(var); fact_pidmom_h.GetYaxis().SetTitle("log(P_{T})");
    c2.cd(7);  kde_pidmom_h.Draw("zcol"); kde_pidmom_h.GetXaxis().SetTitle(var); kde_pidmom_h.GetYaxis().SetTitle("log(P_{T})");
    c2.cd(8);  ada_pidmom_h.Draw("zcol"); ada_pidmom_h.GetXaxis().SetTitle(var); ada_pidmom_h.GetYaxis().SetTitle("log(P_{T})");
  else : 
    c2.cd(5);  proj_pideta_h.Draw("zcol"); proj_pideta_h.GetXaxis().SetTitle(var); proj_pideta_h.GetYaxis().SetTitle("log(P_{T})");
    c2.cd(6);  fact_pideta_h.Draw("zcol"); fact_pideta_h.GetXaxis().SetTitle(var); fact_pideta_h.GetYaxis().SetTitle("log(P_{T})");
    c2.cd(7);  kde_pideta_h.Draw("zcol"); kde_pideta_h.GetXaxis().SetTitle(var); kde_pideta_h.GetYaxis().SetTitle("log(P_{T})");
    c2.cd(8);  ada_pideta_h.Draw("zcol"); ada_pideta_h.GetXaxis().SetTitle(var); ada_pideta_h.GetYaxis().SetTitle("log(P_{T})");

  c2.cd(9);  kde_pidmom_slice.Draw("zcol"); kde_pidmom_slice.GetXaxis().SetTitle(var); kde_pidmom_slice.GetYaxis().SetTitle("log(P_{T})");
  c2.cd(10); kde_pideta_slice.Draw("zcol"); kde_pideta_slice.GetXaxis().SetTitle(var); kde_pideta_slice.GetYaxis().SetTitle("#eta");
  c2.cd(11); kde_mometa_slice.Draw("zcol"); kde_mometa_slice.GetXaxis().SetTitle("log(P_{T})"); kde_mometa_slice.GetYaxis().SetTitle("#eta");
  c2.cd(12); kde_pidntr_slice.Draw("zcol"); kde_pidntr_slice.GetXaxis().SetTitle(var); kde_pidntr_slice.GetYaxis().SetTitle("log(N_{tracks})");

  c2.Update()
  c2.Print(outfilename + "_slices.pdf")
  f1.Close()

