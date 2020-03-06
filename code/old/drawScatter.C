void drawScatter1(TString, TString,TString,int,TString,Double_t,Double_t,Bool_t,Bool_t);
void drawScatter();
void set_plot_style();


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
  drawScatter();
  return 0;
}
#endif

void drawScatter1(TString f1Name,TString h1Name,TString axisTitle,int mult =-1,TString opt = "COLZ",Double_t ymin =0.,Double_t ymax = 0.,Bool_t xlog = kFALSE, Bool_t ylog = kFALSE)
{
   
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
  //gStyle->SetHistFillColor(0);       // Histogram fill color (0) (18)
  gStyle->SetHistFillStyle(1001);    // Histogram fill style (1001) (0)
  //gStyle->SetHistLineColor(kBlue);       // Histogram line color (1)
  gStyle->SetHistLineStyle(0);       // Histogram line style (0)
  gStyle->SetHistLineWidth(3);       // Histogram line width (1.0)
  gStyle->SetMarkerStyle(21);        // Marker style (0)
  // gStyle->SetMarkerColor(kBlack);         // Marker color (1)
  gStyle->SetMarkerSize(1.7);       // Marker size ()
  //gStyle->SetLineColor(kBlack);           // Line color (1)
  gStyle->SetLineWidth(2);           // Line width (1.0)
  gStyle->SetTextSize(0.07);         // Text size (1.0)
  gStyle->SetLabelSize(0.03,"x");    // X numbers label size ()
  gStyle->SetLabelSize(0.03,"y");    // Y numbers label size ()
  gStyle->SetTitleSize(0.04,"x");    // X title size ()
  gStyle->SetTitleSize(0.04,"y");    // Y title size ()
  gStyle->SetErrorX(0);              // No errors along x
  
  set_plot_style();

  gROOT -> ForceStyle();
  //style end

  TString title = TString(f1Name(0,f1Name.Index(".")) +"_"+h1Name+"_scatter");
  title.ReplaceAll("/","_");
  TCanvas* tc = new TCanvas(title,title,1600,1600);
  tc->SetLeftMargin(0.13);
  tc->SetTopMargin(0.13);
  tc->SetRightMargin(0.13);
  tc->SetBottomMargin(0.13);

  if(xlog) tc->SetLogx();
  if(ylog) tc->SetLogy();
  tc->SetLogz();

  TFile* f_in_1 = new TFile(f1Name,"READ");
 
	
  TH2F* h1 = (TH2F*)f_in_1->Get(h1Name);
  
  
  h1->SetTitle(axisTitle);
  
  if(!( TMath::Abs(ymin -0.)<0.0000001 && TMath::Abs(ymax -0.)<0.0000001 ))
  {
    h1->GetYaxis()->SetRangeUser(ymin,ymax);
  }
  
  gROOT -> ForceStyle();
  
  h1->GetYaxis()->SetTitleOffset(1.15);

  ////////////// descriptions
  TString multString;
  if(mult==1) multString = TString("< 10");
  else if(mult==2) multString = TString("11 - 20");
  else if(mult==3) multString = TString("21 - 60");
 
  
   // Comment
  TLatex *Ltext = new TLatex;
  Ltext->SetNDC();
  Ltext->SetTextSize(0.035);
  //Ltext->DrawLatex(0.35,0.80, "Comment");

  //////////////////////////////////////////////////////-----------SETTINGS-------------//////////////////////////////////////////
  //TLegend* tl = new TLegend(0.65,0.75,0.95,0.95);
  //tl->SetHeader("ch. particles multiplicity");
  //tl->AddEntry(h1, "oldEVMIX" ,"pe");
  
  
  tc->cd();

  //h1->SetMarkerColor(kBlue);
  //h1->SetLineColor(kBlue); 
  //h1->SetLineWidth(2); 
  h1->Draw(opt); 
 
  //tl->Draw("SAME");
  
  //comment
  Int_t indexOfFirst = f1Name.Index("_");
  TString data = f1Name(0,indexOfFirst);
  TString tmp = f1Name(indexOfFirst+1,f1Name.Length());
  TString part = tmp(0,tmp.Index("_"));
  part.ToUpper();

  //////////////////////////////////////////////////////-----------SETTINGS-------------//////////////////////////////////////////
  Double_t commPosX = 0.3;
  Double_t commPosY = 0.962;
  TString comment = TString(data + " " + part);
  if(mult != -1) comment = TString(comment + " (" + TString("VELO track mult.: " + multString) + ")");
  Ltext->DrawLatex(commPosX,commPosY, comment);
  //Ltext->DrawLatex(commPosX,commPosY, "Comment");
  //Ltext->DrawLatex(commPosX,commPosY-0.05, "ref. sample: EVENT MIX");
  //if(mult != -1) Ltext->DrawLatex(commPosX,commPosY-0.1, TString("ch. part. multiplicity: " + multString));
  
  

  tc->SaveAs(".pdf");

  f_in_1->Close();
  
}

void drawScatter()
{
  for(int i=1; i<4; ++i)
  { 
   
    string i_str = std::to_string(i);
    TString hName;

    //RD
    //hName = TString("h2501_all_"+i_str);
    //drawScatter1("RD_pion_0c65.root",hName,";delta(p_{x});delta(p_{y})",i);
    hName = TString("h2501_like_"+i_str);
    drawScatter1("RD_pion_0c65.root",hName,";delta(p_{x});delta(p_{y})",i);
    //hName = TString("h2501_evmix_"+i_str);
    //drawScatter1("RD_pion_0c65.root",hName,";delta(p_{x});delta(p_{y})",i);
 
    /*
     //MC
    hName = TString("h2501_all_"+i_str);
    drawScatter1("MC_pion_0c65.root",hName,";delta(p_{x});delta(p_{y})",i);
    hName = TString("h2501_like_"+i_str);
    drawScatter1("MC_pion_0c65.root",hName,";delta(p_{x});delta(p_{y})",i);
    hName = TString("h2501_evmix_"+i_str);
    drawScatter1("MC_pion_0c65.root",hName,";delta(p_{x});delta(p_{y})",i);
    */
   
    
    
   
   
    
  }
  
 
  
}

void set_plot_style()
{
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
}

  

  





