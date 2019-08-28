void makeDivided(TString path1, TString f1Name, TString h1BaseName, TString path2, TString f2Name, TString h2BaseName, int nrBins = 6){
  //This script divides h1 and h2.

  //get histograms to divide
  TFile* f1 = new TFile(TString(path1+"/"+f1Name+".root"),"READ");
  TFile* f2 = new TFile(TString(path2+"/"+f2Name+".root"),"READ");

  //output type (corr. func. / DR)
  bool DR = !f1Name.CompareTo(f2Name) ? 0 : 1;
  TString fOut = DR ? TString(f1Name.ReplaceAll("RD","DR")+".root") : TString(f1Name+".root");
  TFile* fHisto = new TFile(fOut,"UPDATE");

  for(int i=0; i<nrBins; ++i){
    TString h1Name = TString(h1BaseName+"_")+=(i+1);
    TString h2Name = TString(h2BaseName+"_")+=(i+1);
    TH1D* h1 = (TH1D*)f1->Get(h1Name);
    TH1D* h2 = (TH1D*)f2->Get(h2Name);

    h1->Rebin(4);
    h2->Rebin(4);
    h1->Scale(1./4);
    h2->Scale(1./4);

    //watch out for binning
    TH1D* hOut = (TH1D*)h1->Clone();
    for(Int_t ii = 0; ii < hOut->GetNbinsX(); ++ii) {
      hOut->SetBinError(ii+1,1000.0);
    }

    //title 
    TString title = DR ? TString(h1Name+"; Q [GeV]; DR(Q)") : TString(h1Name+"; Q [GeV]; C(Q)");
    hOut->SetTitle(title);

    //divide
    if(!DR) hOut->Divide(h1,h2,h2->Integral(),h1->Integral());
    else hOut->Divide(h1,h2);
     
    hOut->Write();
    delete hOut;
  }

  if(!DR) cout << "Correlation function:\t" << h1BaseName << " / " << h2BaseName  << "\t( " << f1Name << " )" << endl;
  else cout << "Double ratio:\t" << h1BaseName << " / " << h2BaseName  << "\t( " << f1Name << " )" << endl;
    
  fHisto->Close();
  f1->Close();
  f2->Close(); 
}
