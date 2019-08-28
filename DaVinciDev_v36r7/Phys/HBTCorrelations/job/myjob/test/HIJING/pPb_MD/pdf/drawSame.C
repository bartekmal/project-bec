void drawSame1(TString, TString,TString, TString, TString, TString, TString,TString, TString);
void drawSame();

#ifndef __CINT__
#include "Riostream.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TTree.h"
#include "TMath.h"
#include "TChain.h"
#include "TLegend.h"
#include "TText.h"
#include "TLatex.h"
#include "TRandom.h"

#include <string>

int main()
{
  drawSame();
  return 0;
}
#endif

void drawSame1(TString hName,TString f1Name,TString f2Name,TString f3Name,TString f4Name, TString title = "")
{

  

  TString opt = "E";
  Double_t ymin =0.;
  Double_t ymax = 0.;
  Bool_t xlog = kFALSE;
  Bool_t ylog = kFALSE;

  //style
  gROOT->Reset();                    // Reset options
  gStyle->SetPadGridX(kFALSE);       // No grid in x (0)
  gStyle->SetPadGridY(kFALSE);       // No grid in y (0)
  gStyle->SetOptTitle(kFALSE);       // No title
  gStyle->SetStatBorderSize(0);      // Stats box shadow size (2)
  gStyle->SetStatColor(18);          // Stats box fill color (0)
  gStyle->SetStatFont(62);           // Stats font style (62)
  gStyle->SetStatH(0.1);             // Stats box height
  gStyle->SetStatW(0.15);            // Stats box width
  gStyle->SetStatX(0.91);            // Stats box x coordinate
  gStyle->SetStatY(0.91);            // Stats box y coordinate
  gStyle->SetStatStyle(1001);        // Stat box fill style
  gStyle->SetStatTextColor(1);       // Stat text color (1)
  gStyle->SetOptStat(0);    // No statistics (0) (1000001110)
  gStyle->SetOptFit(111);            // No fit box (0) (111)
  gStyle->SetFrameBorderMode(0);     // No red box
  gStyle->SetHistFillColor(0);       // Histogram fill color (0) (18)
  gStyle->SetHistFillStyle(1001);    // Histogram fill style (1001) (0)
  gStyle->SetHistLineColor(kBlue);       // Histogram line color (1)
  gStyle->SetHistLineStyle(0);       // Histogram line style (0)
  gStyle->SetHistLineWidth(3);       // Histogram line width (1.0)
  gStyle->SetMarkerStyle(21);        // Marker style (0)
  gStyle->SetMarkerColor(kBlack);         // Marker color (1)
  gStyle->SetMarkerSize(1.8);       // Marker size ()
  gStyle->SetLineColor(kBlack);           // Line color (1)
  gStyle->SetLineWidth(2);           // Line width (1.0)
  gStyle->SetTextSize(0.07);         // Text size (1.0)
  gStyle->SetLabelSize(0.02,"x");    // X numbers label size ()
  gStyle->SetLabelSize(0.02,"y");    // Y numbers label size ()
  gStyle->SetTitleSize(0.04,"x");    // X title size ()
  gStyle->SetTitleSize(0.04,"y");    // Y title size ()
  gStyle->SetErrorX(0);              // No errors along x
  gROOT -> ForceStyle();
  //style end
  
  //TString title = TString(h1Name+"_compare");
  //title.ReplaceAll("/","_");
  TCanvas* tc = new TCanvas(title,title,1600,1600);
  tc->SetLeftMargin(0.15);
  tc->SetTopMargin(0.02);
  tc->SetRightMargin(0.02);
  tc->SetBottomMargin(0.1);

  if(xlog) tc->SetLogx();
  if(ylog) tc->SetLogy();

  TFile* f_in_1 = new TFile(f1Name,"READ");
  TFile* f_in_2 = new TFile(f2Name,"READ");
  TFile* f_in_3 = new TFile(f3Name,"READ");
  TFile* f_in_4 = new TFile(f4Name,"READ");

  TH1F* h1 = (TH1F*)f_in_1->Get(hName);
  TH1F* h2 = (TH1F*)f_in_2->Get(hName);
  TH1F* h3 = (TH1F*)f_in_3->Get(hName);
  TH1F* h4 = (TH1F*)f_in_4->Get(hName);

  h1->Sumw2();
  h2->Sumw2();
  h3->Sumw2();
  h4->Sumw2();
 
  h1->Scale(1./(h1->GetEntries()));
  h2->Scale(1./(h2->GetEntries()));
  h3->Scale(1./(h3->GetEntries()));
  h4->Scale(1./(h4->GetEntries()));
 
  h1->SetTitle(title);
  h2->SetTitle(title);
  h3->SetTitle(title);
  h4->SetTitle(title);

  Float_t max = h4->GetMaximum();
  h2->GetYaxis()->SetRangeUser(0.,max*1.2);
 
  if(!( TMath::Abs(ymin -0.)<0.0000001 && TMath::Abs(ymax -0.)<0.0000001 ))
  {
    h1->GetYaxis()->SetRangeUser(ymin,ymax);
  }
  
  gROOT -> ForceStyle();

  h4->GetYaxis()->SetTitleOffset(2.5);

  // Comment
  TLatex *Ltext = new TLatex;
  Ltext->SetNDC();
  Ltext->SetTextSize(0.035);
  //Ltext->DrawLatex(0.35,0.80, "Comment");

  //////////////////////////////////////////////////////-----------SETTINGS-------------//////////////////////////////////////////
  TLegend* tl = new TLegend(0.8,0.85,0.95,0.95);
  //tl->SetHeader("IP cut [mm]");
  tl->AddEntry(h1, "EPOS" ,"pe");
  tl->AddEntry(h2, "HIJING - Alvaro", "pe");
  tl->AddEntry(h3, "DATA" ,"l");
  tl->AddEntry(h4, "HIJING - local", "pe");
  
 
  tc->cd();
 
  h2->SetLineColor(kRed); 
  h2->SetLineWidth(2); 
  h2->SetMarkerColor(kRed);
  h2->Draw("E");
  h1->SetMarkerColor(kBlue);
  h1->SetLineColor(kBlue); 
  h1->SetLineWidth(2); 
  h1->Draw("ESAME"); 
  h3->SetMarkerColor(kViolet);
  h3->SetLineColor(kViolet); 
  h3->SetLineWidth(2); 
  h3->Draw("HISTSAME");
  h4->SetMarkerColor(kGreen);
  h4->SetLineColor(kGreen); 
  h4->SetLineWidth(2); 
  h4->Draw("ESAME");

  tl->Draw("SAME");

  //comment
  Double_t commPosX = 0.7;
  Double_t commPosY = 0.75;
  Ltext->DrawLatex(commPosX,commPosY, TString("pPb - MD"));
  
  tc->SaveAs(TString(h1->GetName())+".pdf");
  //tc->SaveAs(".eps");
  
  f_in_1->Close();
  f_in_2->Close();
  f_in_3->Close();
  f_in_4->Close();
}

void drawSame()
{
  TString path = TString("/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/");
  
  TString f1Name = TString("EPOS/output/pPb_MD/basicVars_local.root");
  TString f2Name = TString("HIJING/pPb_MD/Oscar/HBTAnalysis/basicVars.root");
  TString f3Name = TString("data/pPb_MD/HBTAnalysis/basicVars.root");
  TString f4Name = TString("HIJING/pPb_MD/HBTAnalysis/basicVars.root");

  TString hName = TString("h101");
  TString title = TString("N_{ch};N_{ch};(1/N)(dN/dN_{ch})");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);

  hName = TString("h102");
  title = TString("SPD hits;SPD hits;(1/N)(dN/d(SPD hits))");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);
  
  /*
  hName = TString("h1005");
  title = TString("pionNN;pionNN;(1/N)(dN/d(pionNN))");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);

  hName = TString("h1006");
  title = TString("kaonNN;kaonNN;(1/N)(dN/d(kaonNN))");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);

  hName = TString("h1007");
  title = TString("protonNN;protonNN;(1/N)(dN/d(protonNN))");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);
  */

  hName = TString("h1008");
  title = TString("ghostNN;ghostNN;(1/N)(dN/d(ghostNN))");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);
  
  hName = TString("h1017");
  title = TString("p;p [GeV];(1/N)(dN/dp)");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);

  hName = TString("h1018");
  title = TString("p_{T};p_{T} [GeV];(1/N)(dN/dp_{T})");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);

  hName = TString("h1019");
  title = TString("#phi;#phi [#circ];(1/N)(dN/d#phi)");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);

  hName = TString("h1022");
  title = TString("#eta;#eta;(1/N)(dN/d#eta)");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);

  hName = TString("h1023");
  title = TString("IP;IP [mm];(1/N)(dN/d(IP))");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);
     
  hName = TString("h1025");
  title = TString("track #chi^{2};track #chi^{2};(1/N)(dN/d(track #chi^{2}))");
  drawSame1(hName,TString(path+f1Name),TString(path+f2Name),TString(path+f3Name),TString(path+f4Name),title);
}

  

  





