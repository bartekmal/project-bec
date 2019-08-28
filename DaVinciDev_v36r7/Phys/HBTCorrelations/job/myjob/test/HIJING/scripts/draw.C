void draw(TString path, TString fName,TString hBaseName, TString comment = TString(""), int nrBins = 1, TString axes = TString(""), Double_t ymin = 0.5,Double_t ymax = 1.6,Bool_t xlog = kFALSE, Bool_t ylog = kFALSE)
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
  gStyle->SetTextSize(0.07);         // Text size (1.0)
  gStyle->SetLabelSize(0.03,"x");    // X numbers label size ()
  gStyle->SetLabelSize(0.03,"y");    // Y numbers label size ()
  gStyle->SetTitleSize(0.04,"x");    // X title size ()
  gStyle->SetTitleSize(0.04,"y");    // Y title size ()
  gStyle->SetErrorX(0);              // No errors along x
  gROOT -> ForceStyle();
  
  //histogram type
  TString inputType = fName(0,2); //RD etc
  if(!inputType.CompareTo("RD")) inputType = TString("DATA");
  TString beamMode = fName(3,fName.Length()-3); //pPb_MU OR pPb etc
  TString histType = !hBaseName.CompareTo("h3001") ? TString("LIKE") : TString("UNLIKE");

  //get histogram to draw
  TFile* fIn1 = new TFile(TString(path+"/local/"+fName+".root"),"READ");
  TFile* fIn2 = new TFile(TString(path+"/Oscar/"+fName+".root"),"READ");

  for(int i=0; i<nrBins; ++i){
    TString hName = TString(hBaseName+"_")+=(i+1);
    TH1D* h1 = (TH1D*)fIn1->Get(hName);
    TH1D* h2 = (TH1D*)fIn2->Get(hName);
 
    //canvas
    TCanvas* tc = new TCanvas(TString(histType+"_")+=(i+1),h1->GetTitle(),1600,1600);
    tc->SetLeftMargin(0.15);
    tc->SetTopMargin(0.02);
    tc->SetRightMargin(0.02);
    tc->SetBottomMargin(0.1);

    //settings / style
    gROOT -> ForceStyle();
    if(xlog) tc->SetLogx();
    if(ylog) tc->SetLogy();

    if(!ylog) h1->GetYaxis()->SetRangeUser(ymin,ymax);
    else{
      if(!(TMath::Abs(ymin -0.)<0.0000001)) h1->GetYaxis()->SetRangeUser(ymin,ymax);
      else{
	h1->GetYaxis()->SetRangeUser(0.0000001,ymax);
	cout << "Setting ymin to very close to 0 (log scale)" << endl;
      }
    }
  
    if(axes.CompareTo("")) h1->SetTitle(axes);
    h1->GetYaxis()->SetTitleOffset(1.45);
  
    //draw
    tc->cd();
    h1->SetMarkerColor(kBlue);
    h1->SetLineColor(kBlue); 
    h1->SetLineWidth(2); 
    h1->Draw("E"); 
    h2->SetMarkerColor(kRed);
    h2->SetLineColor(kRed);
    h2->SetLineWidth(2);
    h2->Draw("ESAME");


    //description
    Double_t commPosX = 0.55;
    Double_t commPosY = 0.35;
  
    TLatex *Ltext = new TLatex;
    Ltext->SetNDC();
    Ltext->SetTextSize(0.035);
 
    Ltext->DrawLatex(commPosX,commPosY, TString(inputType + " " + beamMode + " - " + histType )); //MC LIKE pPb
    //Ltext->DrawLatex(commPosX,commPosY-0.05, TString("Multiplicity bin: ")+=(i+1));
    //if(comment.CompareTo("")) Ltext->DrawLatex(commPosX,commPosY-0.15, comment);
  
    //legend
    
      TLegend* tl = new TLegend(0.65,0.75,0.95,0.95);
      //tl->SetHeader("probNNpi cut");
      tl->AddEntry(h1, "local" ,"pe");
      tl->AddEntry(h2, "Oscar", "pe");
      tl->Draw("SAME");
    

    //save
    tc->SaveAs(".pdf");
    tc->SaveAs(".eps");

    delete h1;
    delete h2;
    delete tc;
  }

  fIn1->Close();
  fIn2->Close();
}
