//-------------------------------------------------------
// Analysis of the HBT correlations
// Author: Marcin Kucharczyk
// Date: 18-04-2013
//-------------------------------------------------------

//#include <string>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include "HBTAnalysis.h"

int main(int argc, char** argv) {

  //CHANGE
  time_t start;
  time(&start);

  const float nMax = 100000; 
  
  // Choose particles to be analysed (1-pions, 2-kaons, 3-protons)
  int MC = 1;
  TString label = TString("basicVars");

  if(argc>1)
  {
    if(!strcmp(argv[1], "RD")) MC=0;
    if(!strcmp(argv[1], "MC")) MC=1;
  }

  std::vector<TString> fl;
  string path = "/afs/cern.ch/work/b/bmalecki/analysis/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/";
  TString f1_name;
  if(!MC)
  {
    f1_name = TString(path+"Pbp/microBias/hbtNTuple_Pbp-MD.root");
  }else
  {
    f1_name = TString(path+"Pbp/MC/hbtNTuple.root");
  }

  fl.push_back(f1_name);
  
  //CHANGE - input check
  std::cout << "000000.0000 Input files: " << std::endl;
  for(int i=0; i<fl.size(); ++i) std::cout << "000000.0000 " << fl[i] << std::endl;
  printf("000000.0001 Exec args: MC = %d\n",MC);
 
  TFile fHisto(TString(label) + ".root","RECREATE");
  // Insert histograms ----------
  TH1D* h100 = new TH1D("h100","trkMult", 130, 0.0, 130.0);
  TH1D* h101 = new TH1D("h101","PVMult1", 150, 0.0, 150.0);
  TH1D* h102 = new TH1D("h102","SPDhits", 120, 0.0, 1200.0);

  TH1D* h1001 = new TH1D("h1001","pvIndex", 100, 0.0, 100.0);
  TH1D* h1002 = new TH1D("h1002","charge", 100, -5.0, 5.0);
  TH1D* h1003 = new TH1D("h1003","kDLL", 100, -25.0, 25.0);
  TH1D* h1004 = new TH1D("h1004","pDLL", 100, -25.0, 25.0);
  TH1D* h1005 = new TH1D("h1005","piNN", 100, 0.0, 1.0);
  TH1D* h1006 = new TH1D("h1006","kNN", 100, 0.0, 1.0);
  TH1D* h1007 = new TH1D("h1007","pNN", 100, 0.0, 1.0);
  TH1D* h1008 = new TH1D("h1008","ghostNN", 100, 0., 1.0);
  TH1D* h1009 = new TH1D("h1009","xBestPV", 100, 0.0, 1.0);
  TH1D* h1010 = new TH1D("h1010","yBestPV", 100, 0.0, 1.0);
  TH1D* h1011 = new TH1D("h1011","zBestPV", 100, 0.0, 1.0);
  TH1D* h1012 = new TH1D("h1012","pxBestPV", 100, 0.0, 1.0);
  TH1D* h1013 = new TH1D("h1013","pyBestPV", 100, 0.0, 1.0);
  TH1D* h1014 = new TH1D("h1014","pzBestPV", 100, 0.0, 1.0);
  TH1D* h1015 = new TH1D("h1015","chi2BestPV", 100, 0.0, 1.0);
  TH1D* h1016 = new TH1D("h1016","e", 100, 0.0, 100.0);
  TH1D* h1017 = new TH1D("h1017","p", 100, 0.0, 100.0);
  TH1D* h1018 = new TH1D("h1018","pt", 100, 0.0, 4.0);
  TH1D* h1019 = new TH1D("h1019","phi", 100, -180.0, 180.0);
  TH1D* h1020 = new TH1D("h1020","theta", 100, 0.0, 1.0);
  TH1D* h1021 = new TH1D("h1021","rapidity", 100, 0.0, 1.0);
  TH1D* h1022 = new TH1D("h1022","eta", 100, 1.5, 5.5);
  TH1D* h1023 = new TH1D("h1023","ip", 100, 0.0, 0.4);
  TH1D* h1024 = new TH1D("h1024","ipChi2", 100, 0.0, 20.0);
  TH1D* h1025 = new TH1D("h1025","trkChi2", 100, 0.0, 3.0);
  TH1D* h1026 = new TH1D("h1026","chi2PV1", 100, 0.0, 1.0);
  TH1D* h1027 = new TH1D("h1027","chi2PV2", 100, 0.0, 1.0);
  TH1D* h1028 = new TH1D("h1028","chi2PV3", 100, 0.0, 1.0);     
  TH1D* h1029 = new TH1D("h1029","chi2PV4", 100, 0.0, 1.0);
  TH1D* h1030 = new TH1D("h1030","isClone", 20, -10.0, 10.0);
  TH1D* h1031 = new TH1D("h1031","cloneDist", 100, -10000.0, 10000.0);
  TH1D* h1032 = new TH1D("h1032","piNNcorr", 100, 0.0, 1.0);
  TH1D* h1033 = new TH1D("h1033","kNNcorr", 100, 0.0, 1.0);
  TH1D* h1034 = new TH1D("h1034","pNNcorr", 100, 0.0, 1.0);
  TH1D* h1035 = new TH1D("h1035","piNNcorr", 100, 0.0, 1.0);
  TH1D* h1036 = new TH1D("h1036","kNNcorr", 100, 0.0, 1.0);
  TH1D* h1037= new TH1D("h1037","pNNcorr", 100, 0.0, 1.0);
  TH1D* h1038 = new TH1D("h1038","piNNcorr", 100, 0.0, 1.0);
  TH1D* h1039 = new TH1D("h1039","kNNcorr", 100, 0.0, 1.0);
  TH1D* h1040 = new TH1D("h1040","pNNcorr", 100, 0.0, 1.0);
  TH1D* h1041 = new TH1D("h1041","piNNcorr", 100, 0.0, 1.0);
  TH1D* h1042 = new TH1D("h1042","kNNcorr", 100, 0.0, 1.0);
  TH1D* h1043 = new TH1D("h1043","pNNcorr", 100, 0.0, 1.0);
  TH1D* h1044 = new TH1D("h1044","piNNcorr", 100, 0.0, 1.0);
  TH1D* h1045 = new TH1D("h1045","kNNcorr", 100, 0.0, 1.0);
  TH1D* h1046 = new TH1D("h1046","pNNcorr", 100, 0.0, 1.0);
  TH1D* h1047 = new TH1D("h1047","piNNcorr", 100, 0.0, 1.0);
  TH1D* h1048 = new TH1D("h1048","kNNcorr", 100, 0.0, 1.0);
  TH1D* h1049 = new TH1D("h1049","pNNcorr", 100, 0.0, 1.0);
  TH1D* h1050 = new TH1D("h1050","piNNcorr", 100, 0.0, 1.0);
  TH1D* h1051 = new TH1D("h1051","kNNcorr", 100, 0.0, 1.0);
  TH1D* h1052 = new TH1D("h1052","pNNcorr", 100, 0.0, 1.0);
  TH1D* h1053 = new TH1D("h1053","shareVHits", 200, -1.0, 1.0);
  TH1D* h1054 = new TH1D("h1054","isMuon", 10, -5.0, 5.0);
  

  // Loop over ntuples ------------------------
  int nFile      = 1;
  int nSel       = 0;
  int nrEvts     = 0;
  double nEvts   = 0;
  int nrClones   = 0;
  int nrGhosts   = 0;
  int nrParts    = 0;
  int nrPartsAll = 0;
  //CHANGE
  int nrPartsSelected = 0;
  //CHANGE EVMIX_mult 
  int nrPartsEvmixLost = 0;

  int iEvt = 0;
  //////////
  std::vector<TString>::iterator ifile;
  for(ifile = fl.begin(); ifile != fl.end(); ++ifile, ++nFile) {
    if(nSel >= nMax) break;
    TFile* f = new TFile(*ifile);
    if(!f) {cout << "\nFile not found:" << (*ifile) << endl; return 0;}
    f->ls();
    TTree* HBTTuple = (TTree*)gDirectory->Get("HBT");
    if(!HBTTuple) cout << "Could not find TTree or HBT nTuple" << endl;
    nrEvts += (Int_t)HBTTuple->GetEntries();
    cout << "End loop over files" << endl;
    setBranches(HBTTuple);
    nEvts = std::min((float)HBTTuple->GetEntries(),nMax);
    cout << endl
         << nFile << "/" << fl.size()
         << " reading "<< (int)nEvts <<" entries from: "<< (*ifile) << endl;
    //nSel = nSel + nrEvts;
    // Loop over events ------------------------
    for(int ievt = 0; ievt < nEvts; ievt++) {

      HBTTuple->GetEntry(ievt); // Fill branches
      nSel++;
      iEvt++;
      h100->Fill(multNoPreSel);
      h101->Fill(PVMult1);
      h102->Fill(SPDhits); 
      // Loop over particles
      for(int i = 0; i < N; i++) {
        // Fill x-check histrograms for particles
        h1001->Fill(pvIndex[i]);
        h1002->Fill(charge[i]);
        h1003->Fill(kDLL[i]);
        h1004->Fill(pDLL[i]);
        h1005->Fill(piNN[i]);
        h1006->Fill(kNN[i]);
        h1007->Fill(pNN[i]);
        h1008->Fill(ghostNN[i]);
        h1009->Fill(xBestPV[i]);
        h1010->Fill(yBestPV[i]);
        h1011->Fill(zBestPV[i]);
        h1012->Fill(pxBestPV[i]);
        h1013->Fill(pyBestPV[i]);
        h1014->Fill(pzBestPV[i]);
        h1015->Fill(chi2BestPV[i]);
        h1016->Fill(e[i]);
        h1017->Fill(p[i]);
        h1018->Fill(pt[i]);
        h1019->Fill(phi[i] * 57.29);
        h1020->Fill(theta[i]);
        h1021->Fill(rapidity[i]);
        h1022->Fill(eta[i]);
        h1023->Fill(ip[i]);
        h1024->Fill(ipChi2[i]);
        h1025->Fill(trkChi2[i]);
        h1026->Fill(chi2PV1[i]);
        h1027->Fill(chi2PV2[i]);
        h1028->Fill(chi2PV3[i]);     
        h1029->Fill(chi2PV4[i]);
        h1030->Fill(isClone[i]);
        h1031->Fill(cloneDist[i]);
        // All particles
        h1032->Fill(piNNcor[i]);
        h1033->Fill(kNNcor[i]);
        h1034->Fill(pNNcor[i]);
        // Pions
        if(abs(MCID[i]) == 211) {
          h1035->Fill(piNN[i]);
          h1036->Fill(kNN[i]);
          h1037->Fill(pNN[i]);
          h1038->Fill(piNNcor[i]);
          h1039->Fill(kNNcor[i]);
          h1040->Fill(pNNcor[i]);
        }
        // Kaons
        if(abs(MCID[i]) == 321) {
          h1041->Fill(piNN[i]);
          h1042->Fill(kNN[i]);
          h1043->Fill(pNN[i]);
          h1044->Fill(piNNcor[i]);
          h1045->Fill(kNNcor[i]);
          h1046->Fill(pNNcor[i]);
        }
        // Protons
        if(abs(MCID[i]) == 2212) {
          h1047->Fill(piNN[i]);
          h1048->Fill(kNN[i]);
          h1049->Fill(pNN[i]);
          h1050->Fill(piNNcor[i]);
          h1051->Fill(kNNcor[i]);
          h1052->Fill(pNNcor[i]);
        }
        h1053->Fill(shareVHits[i]);
        h1054->Fill(isMuon[i]);
      

      }
    }
    
    f->Close();
  }

  //CHANGE
  time_t end;
  time(&end);
  

  double time_diff = difftime(end,start); //in seconds
  double time_min = time_diff/60.;
  cout << "000000.0006 Time of execution [min] : " << time_min << endl;
  cout << "----------------------------------------------------------" << endl;

  // Write histrograms
  fHisto.Write();
  cout << "Histograms wrote to " + TString(label) + ".root \n"; 
  fHisto.Close();

  return 0;
}

//-----------------------------------------------------------------------------
// Set branch addresses
//-----------------------------------------------------------------------------
void setBranches(TTree* HBTTuple) {
  HBTTuple->SetBranchAddress("nrPVs",&nrPVs);
  HBTTuple->SetBranchAddress("multNoPreSel",&multNoPreSel);
  HBTTuple->SetBranchAddress("multPreSel",&multPreSel);
  HBTTuple->SetBranchAddress("N_J",&N_J);
  HBTTuple->SetBranchAddress("pxJet",&pxJet);
  HBTTuple->SetBranchAddress("pyJet",&pyJet);
  HBTTuple->SetBranchAddress("pzJet",&pzJet);
  HBTTuple->SetBranchAddress("eJet",&eJet);
  HBTTuple->SetBranchAddress("pJet",&pJet);
  HBTTuple->SetBranchAddress("multJet",&multJet);
  HBTTuple->SetBranchAddress("N",&N);
  HBTTuple->SetBranchAddress("pvIndex",&pvIndex);
  HBTTuple->SetBranchAddress("charge",&charge);
  HBTTuple->SetBranchAddress("kDLL",&kDLL);
  HBTTuple->SetBranchAddress("pDLL",&pDLL);
  HBTTuple->SetBranchAddress("piNN",&piNN);
  HBTTuple->SetBranchAddress("kNN",&kNN);
  HBTTuple->SetBranchAddress("pNN",&pNN);
  HBTTuple->SetBranchAddress("ghostNN",&ghostNN);
  HBTTuple->SetBranchAddress("xBestPV",&xBestPV);
  HBTTuple->SetBranchAddress("yBestPV",&yBestPV);
  HBTTuple->SetBranchAddress("zBestPV",&zBestPV);
  HBTTuple->SetBranchAddress("pxBestPV",&pxBestPV);
  HBTTuple->SetBranchAddress("pyBestPV",&pyBestPV);
  HBTTuple->SetBranchAddress("pzBestPV",&pzBestPV);
  HBTTuple->SetBranchAddress("chi2BestPV",&chi2BestPV);
  HBTTuple->SetBranchAddress("e",&e);
  HBTTuple->SetBranchAddress("p",&p);
  HBTTuple->SetBranchAddress("pt",&pt);
  HBTTuple->SetBranchAddress("phi",&phi);
  HBTTuple->SetBranchAddress("theta",&theta);
  HBTTuple->SetBranchAddress("rapidity",&rapidity);
  HBTTuple->SetBranchAddress("eta",&eta);
  HBTTuple->SetBranchAddress("ip",&ip);
  HBTTuple->SetBranchAddress("ipChi2",&ipChi2);
  HBTTuple->SetBranchAddress("trkChi2",&trkChi2);
  HBTTuple->SetBranchAddress("cloneDist",&cloneDist);
  HBTTuple->SetBranchAddress("chi2PV1",&chi2PV1);
  HBTTuple->SetBranchAddress("chi2PV2",&chi2PV2);
  HBTTuple->SetBranchAddress("chi2PV3",&chi2PV3);     
  HBTTuple->SetBranchAddress("chi2PV4",&chi2PV4);
  HBTTuple->SetBranchAddress("isClone",&isClone);
  HBTTuple->SetBranchAddress("MCID",&MCID);
  HBTTuple->SetBranchAddress("piNNcor",&piNNcor);
  HBTTuple->SetBranchAddress("kNNcor",&kNNcor);
  HBTTuple->SetBranchAddress("pNNcor",&pNNcor);
  HBTTuple->SetBranchAddress("shareVHits",&shareVHits);
  HBTTuple->SetBranchAddress("isMuon",&isMuon);
  HBTTuple->SetBranchAddress("PVMult1",&PVMult1);
  HBTTuple->SetBranchAddress("PVMult2",&PVMult2);
  HBTTuple->SetBranchAddress("PVMult3",&PVMult3);
  HBTTuple->SetBranchAddress("PVMult4",&PVMult4);
  HBTTuple->SetBranchAddress("SPDhits",&SPDhits);
}
