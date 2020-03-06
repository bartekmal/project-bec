double proc(int nr, int sum){
  return (double)nr / sum * 100;
}

void getProcents(TString path, TString fName, TString hBaseName = TString("h2401"), TString cutsBase = TString(""), int nrBins = 6)
{
  const int binZero = 10001;

  //open files
  TObjArray* cutsTmp = cutsBase.Tokenize(";");
  int nrCuts = cutsTmp->GetEntries();
  TString cuts[nrCuts];
  for(int j=0; j<nrCuts; ++j) cuts[j] = ((TObjString*)(cutsTmp->At(j)))->String();
  TFile** fIn = malloc(nrCuts * sizeof(TFile*));
  for(int j=0; j<nrCuts; ++j) fIn[j] = new TFile(TString(path+"/"+cuts[j]+"/MC/"+fName+".root"),"READ");
  
  for(int i=0; i<nrBins; ++i){
    
    if(!hBaseName.CompareTo("h2401")) printf("pionNN>\tpions[%]\tkaons[%]\tprotons[%]\telectrons[%]f\tmuons[%]\tghosts[%]\tothers[%]\r\n");
    else if(!hBaseName.CompareTo("h2402") || !hBaseName.CompareTo("h2403")) printf("pionNN>\tpions[%]\tkaons[%]\tprotons[%]\telectrons[%]f\tmuons[%]\tghosts[%]\tothers[%]\tmixed[%]\r\n");
    
    //get histograms
    TString hName = TString(hBaseName+"_")+=(i+1);
    TH1D** h = malloc(nrCuts * sizeof(TH1D*));
    for(int j=0; j<nrCuts; ++j){
      h[j] = (TH1D*)fIn[j]->Get(hName);
      
      //count
      int nrPions = (h[j]->GetBinContent(binZero + 211)) + (h[j]->GetBinContent(binZero - 211));
      int nrKaons = (h[j]->GetBinContent(binZero + 321)) + (h[j]->GetBinContent(binZero - 321));
      int nrProtons = (h[j]->GetBinContent(binZero + 2212)) + (h[j]->GetBinContent(binZero - 2212));
      int nrElectrons = (h[j]->GetBinContent(binZero + 11)) + (h[j]->GetBinContent(binZero - 11));
      int nrMuons = (h[j]->GetBinContent(binZero + 13)) + (h[j]->GetBinContent(binZero - 13));
      int nrGhosts = h[j]->GetBinContent(binZero);

      int nrMixed = h[j]->GetBinContent(binZero - 1);

      int nrAll = h[j]->GetEntries();
      int nrOthers = nrAll - (nrPions + nrKaons + nrProtons + nrGhosts + nrElectrons + nrMuons + nrMixed);
      //print
      if(!hBaseName.CompareTo("h2401")) printf("%s \t %8.2f\t%8.2f\t%8.2f\t%8.2f\t%8.2f\t%8.2f\t%8.2f\r\n",cuts[j].Data(),proc(nrPions,nrAll),proc(nrKaons,nrAll),proc(nrProtons,nrAll),proc(nrElectrons,nrAll),proc(nrMuons,nrAll),proc(nrGhosts,nrAll),proc(nrOthers,nrAll));
      else if(!hBaseName.CompareTo("h2402") || !hBaseName.CompareTo("h2403")) printf("%s \t %8.2f\t%8.2f\t%8.2f\t%8.2f\t%8.2f\t%8.2f\t%8.2f\t%8.2f\r\n",cuts[j].Data(),proc(nrPions,nrAll),proc(nrKaons,nrAll),proc(nrProtons,nrAll),proc(nrElectrons,nrAll),proc(nrMuons,nrAll),proc(nrGhosts,nrAll),proc(nrOthers,nrAll),proc(nrMixed,nrAll));

    }
   
    free(h);
  }
 
  //cleanup
  for(int j=0; j<nrCuts; ++j){
    fIn[j]->Close();
    delete fIn[j];
  }
  free(fIn);
}
    




