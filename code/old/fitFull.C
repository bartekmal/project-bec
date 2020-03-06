void fit1(TFile*,int,TString,Double_t,Double_t,Bool_t,Bool_t);
void drawFit( TH1F* h , TH1F* pull = new TH1F() );
void makePull( TH1F* h, TF1* fitResult, TH1F* pull );
TH1F* createHistogramWithoutSubranges( TH1F* hOriginal, const int nrOfResonances, Double_t *mean, Double_t *sig );
void setStyle();

//fit config
Double_t xminUnlike = 0.25;
Double_t xminLike = 0.05;

///resonances info
const int nrOfResonances = 2;
Double_t resonanceMeans[nrOfResonances] = { 0.2625, 0.9375 };
Double_t resonanceWidths[nrOfResonances] = { 0.0625, 0.0375 };

///////////////////////////////////////////////////// -- constants -- ///////////////////////////////////////

const Double_t massPion = 0.13957; //GeV

const Double_t fineStructureConstant = 1.0 / 137.0;

const Double_t pi = TMath::Pi();

///////////////////////////////////////////////////// -- end of constants -- ///////////////////////////////////////

///////////////////////////////////////////////////// -- functions -- ///////////////////////////////////////

//'auxiliary' functions
Double_t funcLevyAlphaOne( Double_t *x, Double_t *p )  {
  return ( 1 + p[0] * exp( - ( x[0] * p[1] ) ) ) * ( 1 + x[0] * p[2] );
}

Double_t funcGamov( Double_t* x, const Int_t signDueToCharge = 1, const Double_t massOfParticle =  massPion )  { // signDueToCharge = 1 (-1) for LIKE (UNLIKE)
  const Double_t tmpTwoPiDzetaWithSign = 2 * pi * signDueToCharge * fineStructureConstant * massOfParticle / x[0];
  return ( tmpTwoPiDzetaWithSign / ( exp( tmpTwoPiDzetaWithSign ) - 1 ) );
}

//final elements
//BEC effect description
Double_t funcBEC( Double_t *x, Double_t *p ){
  return funcLevyAlphaOne( x, p );
}

//non-femtoscopic background 
//( CMS-like for now: 1 + p[0] * gausn(); p[0] is a weight for UNLIKE/LIKE difference - to be fixed to 1 in fit to UNLIKE )
Double_t funcBkg( Double_t *x, Double_t *p ){
  return ( 1 + p[0] * p[1] / ( p[2] * TMath::Sqrt( 2 * pi ) ) * exp( -0.5 * TMath::Power( x[0] / p[2] , 2 ) ) );
}

/*
//( ATLAS-like )
Double_t funcBkg( Double_t *x, Double_t *p ){
  //return ( 1 + p[0] * p[1] * exp( - TMath::Power(fabs( x[0] * p[2] ), p[4] ) ) ) * ( 1 + x[0] * p[3] );
  return ( 1 + p[0] * p[1] * exp( - TMath::Power(fabs( x[0] * p[2] ), p[3] ) ) );
}
*/
//Coulomb correction 
//( component K(Q) from Bowler-Sinyukov approach )
Double_t funcCoulombUnlike( Double_t *x, Double_t *p ){
  return funcGamov( x, -1 );
  //return 0.9;
}

Double_t funcCoulombLike( Double_t *x, Double_t *p ){
  return funcGamov( x, 1 );
}

Double_t funcFitUnlike( Double_t *x, Double_t *p )  {
  return ( p[0] * funcCoulombUnlike( x, &p[1] ) * funcBkg( x, &p[1] ) );
}

//correlation function for LIKE
Double_t funcFitLike( Double_t *x, Double_t *p )  {
  return ( p[0] * funcCoulombLike( x, &p[1] ) * funcBEC( x, &p[1] ) * funcBkg( x, &p[4] ) ) ;
}

///////////////////////////////////////////////////// -- end of functions -- ///////////////////////////////////////




void fit(TString path, TString fName);
/*
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
*/
void fit1(TFile* fIn, int mult =-1,TString opt = "E",Double_t ymin =0.5, Double_t ymax = 1.6, Bool_t xlog = kFALSE, Bool_t ylog = kFALSE)
{

  //setStyle
  setStyle();

  //get histograms
  string i_str = std::to_string(mult); 
  TString hLikeName = TString("h3001_"+i_str);
  TString hLikePullName = TString("pull_h3001_"+i_str);
  TString hUnlikeName = TString("h3120_"+i_str);
  TString hUnlikePullName = TString("pull_h3120_"+i_str);

  TH1F* hLike = (TH1F*)fIn->Get(hLikeName);
  //TH1F* hUnlike = (TH1F*)fIn->Get(hUnlikeName);
  TH1F* hUnlike = createHistogramWithoutSubranges( (TH1F*)fIn->Get(hUnlikeName), nrOfResonances, resonanceMeans, resonanceWidths );
  
  hLike->SetTitle(";Q [GeV]; C_{2}(Q)");
  hUnlike->SetTitle(";Q [GeV]; C_{2}(Q)");
  
  if(!( TMath::Abs(ymin -0.)<0.0000001 && TMath::Abs(ymax -0.)<0.0000001 ))
  {
    hLike->GetYaxis()->SetRangeUser(ymin,ymax);
    hUnlike->GetYaxis()->SetRangeUser(ymin,ymax);
  }
  
  //fit
  std::cout << "Start fitting..." << std::endl;
  
  TF1 *finalUnlike = new TF1("finalUnlike", funcFitUnlike, 0., 2, 4);  
  finalUnlike->SetParNames("N","z","C_{bkg}","#sigma_{bkg}");
  finalUnlike->SetParameters(1.0, 1.0, 0.5, 1.);
  finalUnlike->FixParameter(1, 1);
    
  finalUnlike->SetLineColor(kRed);
  finalUnlike->SetLineWidth(finalUnlike->GetLineWidth()*2.0);
  
  TFitResultPtr fitResultUnlike = hUnlike->Fit("finalUnlike","S","N", xminUnlike,2);  
  //fitResultUnlike->Print("v");

  const Double_t bkgParamConst = finalUnlike->GetParameter(2);
  const Double_t bkgParamSigma = finalUnlike->GetParameter(3);

  //LIKE
  TF1 *finalLike = new TF1("finalLike", funcFitLike, 0., 2, 7);  
  finalLike->SetParNames("N","#lambda", "R", "#delta", "z","C_{bkg}","#sigma_{bkg}");
  finalLike->SetParameters(1.0, 0.5, 7, 0., 1., bkgParamConst, bkgParamSigma);
  finalLike->FixParameter(4, 1);
  finalLike->FixParameter(5, bkgParamConst);
  finalLike->FixParameter(6, bkgParamSigma);
    
  finalLike->SetLineColor(kRed);
  finalLike->SetLineWidth(finalLike->GetLineWidth()*2.0);
  
  TFitResultPtr fitResultLike = hLike->Fit("finalLike","S","N", xminLike,2);  
  //fitResultUnlike->Print("v");

  //get pulls
  TH1F* hUnlikePull = new TH1F( hUnlikePullName,"pull;Q [GeV]; pull",hUnlike->GetNbinsX(),0,2 );
  makePull( hUnlike, finalUnlike, hUnlikePull );
  TH1F* hLikePull = new TH1F( hLikePullName,"pull;Q [GeV]; pull",hLike->GetNbinsX(),0,2 );
  makePull( hLike, finalLike, hLikePull );

  //draw results
  drawFit( hLike, hLikePull );
  drawFit( hUnlike, hUnlikePull );
  
}

void fit(TString path, TString fName)
{
  
  std::cout << "Used constants: " << std::endl;
  std::cout << "\t pion mass: \t" << massPion << std::endl;
  std::cout << "\t pi: \t" << pi << std::endl;
  std::cout << "\t fine structure: \t" << fineStructureConstant << std::endl;

  TFile* fIn = new TFile(TString(path + "/" + fName),"READ");

  for(int j=1; j<7; ++j)
  {    
    fit1( fIn, j );
  }

  fIn->Close();

}

void drawFit( TH1F* h , TH1F* pull = new TH1F() ){

  
  
  //rysowanie
  TString title = TString( h->GetName() );
  TCanvas* tc = new TCanvas(title,title,1600,2400);
  TPad* p1 = new TPad("p1","p1",0.,0.33,1.,1.);
  TPad* p2 = new TPad("p2","p2",0.,0.,1.,0.33);
  p2->SetBottomMargin(0.2);
  p1->Draw();
  p2->Draw();
  
  //config
  h->GetYaxis()->SetTitleOffset(1.15);
  /*
  tc->SetLeftMargin(0.15);
  tc->SetTopMargin(0.02);
  tc->SetRightMargin(0.02);
  tc->SetBottomMargin(0.1);

  if(xlog) tc->SetLogx();
  if(ylog) tc->SetLogy();
  */

  //h
  h->SetMarkerColor(kBlue);
  h->SetLineColor(kBlue); 
  h->SetLineWidth(2);
  p1->cd();
  h->Draw("E"); 
  
  //pull 
  p2->cd();
  pull->SetBarWidth(0.4);
  pull->SetFillColor(kBlue);
  pull->GetYaxis()->SetRangeUser(-6,6);
  pull->Draw("B");
  TLine* lineUp = new TLine(0,3,2,3);
  lineUp->SetLineColor(kRed);
  lineUp->Draw();
  TLine* lineDown = new TLine(0,-3,2,-3);
  lineDown->SetLineColor(kRed);
  lineDown->Draw();
  
  // ------------------- descriptions ---------------- //

  // Comment
  TLatex *Ltext = new TLatex;
  Ltext->SetNDC();
  Ltext->SetTextSize(0.04);
    
  Double_t commPosX = 0.3;
  Double_t commPosY = 0.3;
  
  Ltext->DrawLatex(commPosX,commPosY, TString( title ) );
  
  //TLegend* tl = new TLegend(0.65,0.75,0.95,0.95);
  //tl->SetHeader("ch. particles multiplicity");
  //tl->AddEntry(h1, "oldEVMIX" ,"pe");
  
  // save plots 
  gStyle->SetOptFit(111); 
  gROOT->ForceStyle();
  tc->Update();
  tc->SaveAs(".pdf");
}

void makePull( TH1F* h, TF1* fitResult, TH1F* pull ){
  
  pull->SetLabelSize(0.06,"x");    // X numbers label size ()
  pull->SetLabelSize(0.06,"y");    // Y numbers label size ()
  pull->SetTitleSize(0.08,"x");    // X title size ()
  pull->SetTitleSize(0.08,"y");    // Y title size ()
  pull->GetYaxis()->SetTitleOffset(0.5);
    
  for(Int_t i = 0; i<=pull->GetNbinsX();++i){
    Double_t value = h->GetBinContent(i);
    Double_t x = h->GetBinCenter(i);
    Double_t sigma = h->GetBinError(i);
    Double_t mi = fitResult->Eval(x);
    if(sigma>0.0000000001) pull->SetBinContent(i,(value-mi)/sigma); //sigma != 0  
  }
}

TH1F* createHistogramWithoutSubranges( TH1F* hOriginal, const int nrOfResonances, Double_t *mean, Double_t *sig ){

  auto* hOut = ( TH1F* ) hOriginal->Clone();

  for ( int i=0; i<hOut->GetNbinsX(); ++i ){
    const auto x = hOut->GetBinCenter(i);
    for ( int j=0; j<nrOfResonances; ++j ){
      if( fabs( x - mean[j] ) < sig[j] ){
	hOut->SetBinContent(i,0);
	hOut->SetBinError(i,0);
      }
    }
  }

  return hOut;

}
  
void setStyle(){
  //style
  gROOT->Reset();                    // Reset options
  gStyle->SetPadGridX(kFALSE);       // No grid in x (0)
  gStyle->SetPadGridY(kFALSE);       // No grid in y (0)
  gStyle->SetOptTitle(kFALSE);       // No title
  gStyle->SetStatBorderSize(0);      // Stats box shadow size (2)
  gStyle->SetStatColor(18);          // Stats box fill color (0)
  gStyle->SetStatFont(62);           // Stats font style (62)
  gStyle->SetStatH(0.19);             // Stats box height
  gStyle->SetStatW(0.19);            // Stats box width
  gStyle->SetStatX(0.892);            // Stats box x coordinate
  gStyle->SetStatY(0.895);            // Stats box y coordinate
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
  gStyle->SetMarkerSize(1.2);       // Marker size ()
  gStyle->SetLineColor(kBlack);           // Line color (1)
  gStyle->SetLineWidth(2);           // Line width (1.0)
  gStyle->SetTextSize(0.15);         // Text size (1.0)
  gStyle->SetLabelSize(0.03,"x");    // X numbers label size ()
  gStyle->SetLabelSize(0.03,"y");    // Y numbers label size ()
  gStyle->SetTitleSize(0.04,"x");    // X title size ()
  gStyle->SetTitleSize(0.04,"y");    // Y title size ()
  gStyle->SetErrorX(0);              // No errors along x
  gROOT -> ForceStyle();
  //style end
}
  





