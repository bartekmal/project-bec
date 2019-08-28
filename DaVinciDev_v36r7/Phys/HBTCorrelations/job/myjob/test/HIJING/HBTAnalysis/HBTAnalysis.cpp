//-------------------------------------------------------
// Analysis of the HBT correlations
// Author: Marcin Kucharczyk
// Date: 18-04-2013
//-------------------------------------------------------

#include "HBTAnalysis.hpp"

int main(int argc, char** argv) {

  //counters
  int nrEvts = 0; //all available events in input files
  //int nrClones   = 0;
  int nrPartsEvmixLost = 0;
  time_t start;
  time(&start);

  //read input args
  const TString dataType = argc>1 ? TString(argv[1]) : TString("MC");
  const TString beamMode = argc>2 ? TString(argv[2]) : TString("pPb_MU");
  const TString prodType = argc>3 ? TString(argv[3]) : TString("local");
  const bool MC = dataType.Contains("MC") ? 1 : 0;
  const bool pPb = beamMode.Contains("pPb") ? 1 : 0;
  const bool localProd = prodType.Contains("local") ? 1 : 0;
  
  //config 
  const int nMax = 1000000; //max #events from a single file
  const int nrBins = 1;

  const TString dataPath = TString("/mnt/nfs/storage/plgbmalecki/BEC_pPb/DaVinciDev_v36r7/Phys/HBTCorrelations/job/myjob/test/HIJING");

  const int corrBinning = 200;
  const double corrRangeQ0 = 2.0;
  const double corrRangeQLSO = 1.0;

  //config EVMIX
  const int nrBinsRef = pPb ? 56 : 96;
  const int partsToBeMixedMaxSize = 200; 
  const int partPVMultIndexOffset = 5;
  
  //selection
  const double trkChi2Cut = 2.0;
  const double ipCut      = 0.4;
  const double pCut       = 2.0;
  const double ptCut      = 0.1;
  const double ghostNNCut = 0.25;
  const double piNNCut    = 0.65;
  const double kNNCut     = 0.5;
  const double pNNCut     = 0.5;
  const double shareVHitsCut = 0.999999;
  const double qCut = 0.05;

  //set input files
  std::vector<TString> fl;
  TString f1_name;
  if(localProd) f1_name = TString(dataPath+"/"+dataType+"_"+beamMode+"/hbtNTuple.root");
  if(!localProd) f1_name = TString(dataPath+"/"+dataType+"_"+beamMode+"/Oscar/hbtNTuple.root");
  fl.push_back(f1_name);

  //input check
  std::cout << "000000.0000 Input files: " << std::endl;
  for(int i=0; i<fl.size(); ++i) std::cout << "000000.0000 " << fl[i] << std::endl;
  printf("000000.0001 Exec args (dataType): %s\n", dataType.Data());
  printf("000000.0001 Exec args (beamMode): %s\n", beamMode.Data());

  //selection check
  printf("000000.0002 Selection: trkChi2 < %.2f\tp > %.2f GeV\tpT > %.2f GeV\tghostNN < %.2f\n",trkChi2Cut,pCut,ptCut,ghostNNCut);
  printf("000000.0002          : IP < %.2f mm\tsharedVHits < %.2f\tpiNN > %.2f\tkNN > %.2f\tpNN > %.2f\n",ipCut,shareVHitsCut,piNNCut,kNNCut,pNNCut);
  
  const TString label = TString(dataType+"_"+beamMode);
  TFile fHisto(TString(label) + ".root","RECREATE");
  // Insert histograms ----------
  TH1D* h100 = new TH1D("h100","trkMult", 200, 0.0, 200.0);
  TH1D* h101 = new TH1D("h101","PVMult1", 150, 0.0, 150.0);
  TH1D* h102 = new TH1D("h102","SPDhits", 65, 0.0, 650.0);
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
  TH1D* h1018 = new TH1D("h1018","pt", 100, 0.0, 10.0);
  TH1D* h1019 = new TH1D("h1019","phi", 100, -180.0, 180.0);
  TH1D* h1020 = new TH1D("h1020","theta", 100, 0.0, 1.0);
  TH1D* h1021 = new TH1D("h1021","rapidity", 100, 0.0, 1.0);
  TH1D* h1022 = new TH1D("h1022","eta", 100, 1.5, 5.5);
  TH1D* h1023 = new TH1D("h1023","ip", 100, 0.0, 1.0);
  TH1D* h1024 = new TH1D("h1024","ipChi2", 100, 0.0, 5.0);
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
  TH1D* h1035 = new TH1D("h1035","shareVHits", 200, -1.0, 1.0);
  TH1D* h1036 = new TH1D("h1036","isMuon", 4, -2.0, 2.0);

  TH2D* h2000 = new TH2D("h2000","SPDhitsVSPVmult1", 60,0,60,15, 0, 150); 

  //after-selection
  TH1D* h1101 = new TH1D("h1101","pvIndex", 100, 0.0, 100.0);
  TH1D* h1102 = new TH1D("h1102","charge", 100, -5.0, 5.0);
  TH1D* h1103 = new TH1D("h1103","kDLL", 100, -25.0, 25.0);
  TH1D* h1104 = new TH1D("h1104","pDLL", 100, -25.0, 25.0);
  TH1D* h1105 = new TH1D("h1105","piNN", 100, 0.0, 1.0);
  TH1D* h1106 = new TH1D("h1106","kNN", 100, 0.0, 1.0);
  TH1D* h1107 = new TH1D("h1107","pNN", 100, 0.0, 1.0);
  TH1D* h1108 = new TH1D("h1108","ghostNN", 100, 0., 1.0);
  TH1D* h1109 = new TH1D("h1109","xBestPV", 100, 0.0, 1.0);
  TH1D* h1110 = new TH1D("h1110","yBestPV", 100, 0.0, 1.0);
  TH1D* h1111 = new TH1D("h1111","zBestPV", 100, 0.0, 1.0);
  TH1D* h1112 = new TH1D("h1112","pxBestPV", 100, 0.0, 1.0);
  TH1D* h1113 = new TH1D("h1113","pyBestPV", 100, 0.0, 1.0);
  TH1D* h1114 = new TH1D("h1114","pzBestPV", 100, 0.0, 1.0);
  TH1D* h1115 = new TH1D("h1115","chi2BestPV", 100, 0.0, 1.0);
  TH1D* h1116 = new TH1D("h1116","e", 100, 0.0, 10.0);
  TH1D* h1117 = new TH1D("h1117","p", 100, 0.0, 10.0);
  TH1D* h1118 = new TH1D("h1118","pt", 100, 0.0, 1.0);
  TH1D* h1119 = new TH1D("h1119","phi", 100, -180.0, 180.0);
  TH1D* h1120 = new TH1D("h1120","theta", 100, 0.0, 1.0);
  TH1D* h1121 = new TH1D("h1121","rapidity", 100, 0.0, 1.0);
  TH1D* h1122 = new TH1D("h1122","eta", 100, 1.5, 5.5);
  TH1D* h1123 = new TH1D("h1123","ip", 100, 0.0, 1.0);
  TH1D* h1124 = new TH1D("h1124","ipChi2", 100, 0.0, 5.0);
  TH1D* h1125 = new TH1D("h1125","trkChi2", 100, 0.0, 5.0);
  TH1D* h1126 = new TH1D("h1126","chi2PV1", 100, 0.0, 1.0);
  TH1D* h1127 = new TH1D("h1127","chi2PV2", 100, 0.0, 1.0);
  TH1D* h1128 = new TH1D("h1128","chi2PV3", 100, 0.0, 1.0);     
  TH1D* h1129 = new TH1D("h1129","chi2PV4", 100, 0.0, 1.0);
  TH1D* h1130 = new TH1D("h1130","isClone", 20, -10.0, 10.0);
  TH1D* h1131 = new TH1D("h1131","cloneDist", 100, -10000.0, 10000.0);
  TH1D* h1132 = new TH1D("h1132","piNNcorr", 100, 0.0, 1.0);
  TH1D* h1133 = new TH1D("h1133","kNNcorr", 100, 0.0, 1.0);
  TH1D* h1134 = new TH1D("h1134","pNNcorr", 100, 0.0, 1.0);
  TH1D* h1135 = new TH1D("h1135","shareVHits", 200, -1.0, 1.0);
  TH1D* h1136 = new TH1D("h1136","isMuon", 4, -2.0, 2.0);

  // Special checks
  TH1D* h2101 = new TH1D("h2101","Q_CLONES", 20, 0.0, corrRangeQ0);
  TH1D* h2201 = new TH1D("h2201","Q_LIKE_GHOSTS", 20, 0.0, corrRangeQ0);
  TH1D* h2301 = new TH1D("h2301","Q_LIKE_CLONES", 20, 0.0, corrRangeQ0);
  TH1D* h2401[nrBins];
  for(int i_bins = 0; i_bins < nrBins; ++i_bins)
  {
    char buffer[5];
    sprintf(buffer,"%d",i_bins+1);
    h2401[i_bins] = new TH1D(TString("h2401_"+string(buffer)),"MCID", 20000, -10000.,10000.);
    
  }
  TH1D* h2501 = new TH1D("h2501","#pions per mult; N_{ch};dN / dN_{ch}", 200, 0.0, 200);
  TH1D* h2502 = new TH1D("h2502","signal pairs per mult; N_{ch}; dN/dN_{ch}", 200, 0.0, 200);
 
  // Pairs
  TH1D* h3002 = new TH1D("h3002","Qm like", corrBinning, 0.0, corrRangeQ0);
  TH1D* h3003 = new TH1D("h3003","delPhi like", 50, 0.0, 180.0);
  TH1D* h3004 = new TH1D("h3004","delY like", 50, 0.0, 5.0);
  TH1D* h3005 = new TH1D("h3005","delPt like", 50, 0.0, 3.0);
  TH1D* h3006 = new TH1D("h3006","Q_LCMS like", corrBinning, 0.0, corrRangeQ0);
  TH1D* h3007 = new TH1D("h3007","Q_long like", corrBinning, -1.0 * corrRangeQLSO, corrRangeQLSO);
  TH1D* h3008 = new TH1D("h3008","Q_side like", corrBinning, -1.0 * corrRangeQLSO, corrRangeQLSO);
  TH1D* h3009 = new TH1D("h3009","Q_out like", corrBinning, -1.0 * corrRangeQLSO, corrRangeQLSO);
  TH1D* h3010 = new TH1D("h3010","delPhi LCMS like", 50, 0.0, 180.0);
  TH1D* h3011 = new TH1D("h3011","delY LCMS like", 50, 0.0, 5.0);
  TH1D* h3012 = new TH1D("h3012","delPt LCMS like", 50, 0.0, 20.0);

  TH1D* h3001[nrBins];
  TH1D* h3015[nrBins];
  for(int i_bins = 0; i_bins < nrBins; ++i_bins)
  {
    //string i_bins_string = std::to_string(i_bins);
    char buffer[5];
    sprintf(buffer,"%d",i_bins+1);
    h3001[i_bins] = new TH1D(TString("h3001_"+string(buffer)),"Qp like", corrBinning, 0.0, corrRangeQ0);
    h3015[i_bins] = new TH1D(TString("h3015_"+string(buffer)),"Qp like", corrBinning, 0.0, corrRangeQ0);
  }

  //////////
  TH1D* h3102 = new TH1D("h3102","Qm unlike", corrBinning, 0.0, corrRangeQ0);
  TH1D* h3103 = new TH1D("h3103","delPhi unlike", 50, 0.0, 180.0);
  TH1D* h3104 = new TH1D("h3104","delY unlike", 50, 0.0, 5.0);
  TH1D* h3105 = new TH1D("h3105","delPt unlike", 50, 0.0, 3.0);
  TH1D* h3106 = new TH1D("h3106","Q_LCMS unlike", corrBinning, 0.0, corrRangeQ0);
  TH1D* h3107 = new TH1D("h3107","Q_long unlike", corrBinning, 0.0, corrRangeQLSO);
  TH1D* h3108 = new TH1D("h3108","Q_side unlike", corrBinning, 0.0, corrRangeQLSO);
  TH1D* h3109 = new TH1D("h3109","Q_out unlike", corrBinning, 0.0, corrRangeQLSO);
  TH1D* h3110 = new TH1D("h3110","delPhi LCMS unlike", 50, 0.0, 180.0);
  TH1D* h3111 = new TH1D("h3111","delY LCMS unlike", 50, 0.0, 5.0);
  TH1D* h3112 = new TH1D("h3112","delPt LCMS unlike", 50, 0.0, 20.0);

  TH1D* h3101[nrBins];
  TH1D* h3115[nrBins];
  for(int i_bins = 0; i_bins < nrBins; ++i_bins)
  {
    char buffer[5];
    sprintf(buffer,"%d",i_bins+1);
    h3101[i_bins] = new TH1D(TString("h3101_"+string(buffer)),"Qp unlike", corrBinning, 0.0, corrRangeQ0);
    h3115[i_bins] = new TH1D(TString("h3115_"+string(buffer)),"Qp unlike", corrBinning, 0.0, corrRangeQ0);
  }
  
  TH1D* h3402 = new TH1D("h3402","Qm Event MIX", corrBinning, 0.0, corrRangeQ0);
  TH1D* h3403 = new TH1D("h3403","delPhi Event MIX", 50, 0.0, 180.0);
  TH1D* h3404 = new TH1D("h3404","delY Event MIX", 50, 0.0, 5.0);
  TH1D* h3405 = new TH1D("h3405","delPt Event MIX", 50, 0.0, 3.0);
  TH1D* h3406 = new TH1D("h3406","Q_LCMS Event MIX", corrBinning, 0.0, corrRangeQ0);
  TH1D* h3407 = new TH1D("h3407","Q_long Event MIX", corrBinning, -1.0 * corrRangeQLSO, corrRangeQLSO);
  TH1D* h3408 = new TH1D("h3408","Q_side Event MIX", corrBinning, -1.0 * corrRangeQLSO, corrRangeQLSO);
  TH1D* h3409 = new TH1D("h3409","Q_out Event MIX", corrBinning, -1.0 * corrRangeQLSO, corrRangeQLSO);
  TH1D* h3410 = new TH1D("h3410","delPhi Event MIX", 50, 0.0, 180.0);
  TH1D* h3411 = new TH1D("h3411","delY Event MIX", 50, 0.0, 5.0);
  TH1D* h3412 = new TH1D("h3412","delPt Event MIX", 50, 0.0, 20.0);
 
  TH1D* h3401[nrBins];
  for(int i_bins = 0; i_bins < nrBins; ++i_bins)
  {
    char buffer[5];
    sprintf(buffer,"%d",i_bins+1);
    h3401[i_bins] = new TH1D(TString("h3401_"+string(buffer)),"Qp Event MIX", corrBinning, 0.0, corrRangeQ0);
  }

  ////////// Container for event mixing
  int enoughForMixing = 0;  
  std::list<chPart> partsToBeMixed[nrBinsRef];
  for(int i=0; i<nrBinsRef; ++i) partsToBeMixed[i].clear();
  TRandom3 rand3;
  
  //////////
  std::vector<TString>::iterator ifile;
  for(ifile = fl.begin(); ifile != fl.end(); ++ifile) {
    TFile* f = new TFile(*ifile);
    if(!f) {cout << "\nFile not found:" << (*ifile) << endl; return 0;}
    TTree* HBTTuple = (TTree*)gDirectory->Get("HBT");
    if(!HBTTuple) cout << "Could not find TTree or HBT nTuple" << endl;
    nrEvts += (Int_t)HBTTuple->GetEntries();
    cout << "End loop over files" << endl;
    setBranches(HBTTuple);
    const int nMaxInFile = std::min((int)HBTTuple->GetEntries(),nMax);
    cout << "000000.1000 Reading "<< nMaxInFile <<" entries from: "<< (*ifile) << endl;
    // Loop over events ------------------------
    for(int ievt = 0; ievt < nMaxInFile; ievt++) {

      HBTTuple->GetEntry(ievt); // Fill branches
      //1PV
      if(nrPVs != 1) continue; 
      //event variables
      h100->Fill(multNoPreSel);
      h101->Fill(PVMult1);
      h102->Fill(SPDhits); 
      h2000->Fill(PVMult1,SPDhits);
         
      // Loop over particles
      std::vector<chPart> inputParts[nrBins];
      std::vector<chPart> inputPions[nrBins];
      std::vector<chPart> inputKaons[nrBins];
      std::vector<chPart> inputProts[nrBins];
      std::vector<chPart> inputTruePions[nrBins];
      std::vector<chPart> inputTrueKaons[nrBins];
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
        h1032->Fill(piNNcor[i]);
        h1033->Fill(kNNcor[i]);
        h1034->Fill(pNNcor[i]);
        h1035->Fill(shareVHits[i]);
        h1036->Fill(isMuon[i]);
        
        ///////// Basic cuts on particles
        if(charge[i] == 0) continue;
        if(isMuon[i] != 0) continue;      
        if(eta[i] < 2.0 || eta[i] > 5.0) continue;

        if(trkChi2[i] > trkChi2Cut) continue;
        if(ip[i] > ipCut)           continue;
        if(p[i] < pCut)             continue;
        if(pt[i] < ptCut)           continue;
        if(ghostNN[i] > ghostNNCut) continue;
        if(shareVHits[i] < - (shareVHitsCut)) continue; 
        
        //after-preselecction x-check
        h1101->Fill(pvIndex[i]);
        h1102->Fill(charge[i]);
        h1103->Fill(kDLL[i]);
        h1104->Fill(pDLL[i]);
        h1105->Fill(piNN[i]);
        h1106->Fill(kNN[i]);
        h1107->Fill(pNN[i]);
        h1108->Fill(ghostNN[i]);
        h1109->Fill(xBestPV[i]);
        h1110->Fill(yBestPV[i]);
        h1111->Fill(zBestPV[i]);
        h1112->Fill(pxBestPV[i]);
        h1113->Fill(pyBestPV[i]);
        h1114->Fill(pzBestPV[i]);
        h1115->Fill(chi2BestPV[i]);
        h1116->Fill(e[i]);
        h1117->Fill(p[i]);
        h1118->Fill(pt[i]);
        h1119->Fill(phi[i] * 57.29);
        h1120->Fill(theta[i]);
        h1121->Fill(rapidity[i]);
        h1122->Fill(eta[i]);
        h1123->Fill(ip[i]);
        h1124->Fill(ipChi2[i]);
        h1125->Fill(trkChi2[i]);
        h1126->Fill(chi2PV1[i]);
        h1127->Fill(chi2PV2[i]);
        h1128->Fill(chi2PV3[i]);     
        h1129->Fill(chi2PV4[i]);
        h1130->Fill(isClone[i]);
        h1131->Fill(cloneDist[i]);
        h1132->Fill(piNNcor[i]);
        h1133->Fill(kNNcor[i]);
        h1134->Fill(pNNcor[i]);
        h1135->Fill(shareVHits[i]);
        h1136->Fill(isMuon[i]);

        /////////
        // Create chPart objects and load them to the containers
        chPart tmpPart;
        tmpPart.prtIndex = i;
        tmpPart.pvIndex = pvIndex[i];
        tmpPart.charge = charge[i];
        tmpPart.kDLL = kDLL[i];
        tmpPart.pDLL = pDLL[i];
	if(!MC){
	  tmpPart.piNN = piNN[i];
	  tmpPart.kNN = kNN[i];
	  tmpPart.pNN = pNN[i];
	}
	else{
	  tmpPart.piNN = fabs(piNNcor[i]);
	  tmpPart.kNN = fabs(kNNcor[i]);
	  tmpPart.pNN = fabs(pNNcor[i]);
	}
        tmpPart.ghostNN = ghostNN[i];
        tmpPart.xBestPV = xBestPV[i];
        tmpPart.yBestPV = yBestPV[i];
        tmpPart.zBestPV = zBestPV[i];
        tmpPart.pxBestPV = pxBestPV[i];
        tmpPart.pyBestPV = pyBestPV[i];
        tmpPart.pzBestPV = pzBestPV[i];
        tmpPart.chi2BestPV = chi2BestPV[i];
        tmpPart.e = e[i];
        tmpPart.p = p[i];
        tmpPart.pt = pt[i];
        tmpPart.phi = phi[i];
        tmpPart.theta = theta[i];
        tmpPart.rapidity = rapidity[i];
        tmpPart.eta = eta[i];
        tmpPart.ip = ip[i];
        tmpPart.ipChi2 = ipChi2[i];
        tmpPart.trkChi2 = trkChi2[i];
        tmpPart.chi2PV1 = chi2PV1[i];
        tmpPart.chi2PV2 = chi2PV2[i];
        tmpPart.chi2PV3 = chi2PV3[i];     
        tmpPart.chi2PV4 = chi2PV4[i];
        tmpPart.isClone = isClone[i];
        tmpPart.MCID = MCID[i];
        
	//multiplicity bins
        tmpPart.bin = -1;
        tmpPart.pvMult = PVMult1;
	if(pPb){
      	  if(tmpPart.pvMult < 61)  tmpPart.bin = 0;
          /*else if(tmpPart.pvMult < 34)  tmpPart.bin = 1;
          else if(tmpPart.pvMult < 41)  tmpPart.bin = 2;
	  else if(tmpPart.pvMult < 48)  tmpPart.bin = 3;
          else if(tmpPart.pvMult < 55)  tmpPart.bin = 4;
          else if(tmpPart.pvMult < 81)  tmpPart.bin = 5;*/
        }
        if(!pPb){
          if(tmpPart.pvMult < 101)  tmpPart.bin = 0;
          /*else if(tmpPart.pvMult < 46)  tmpPart.bin = 1;
          else if(tmpPart.pvMult < 56)  tmpPart.bin = 2;
          else if(tmpPart.pvMult < 66)  tmpPart.bin = 3;
          else if(tmpPart.pvMult < 81)  tmpPart.bin = 4;
          else if(tmpPart.pvMult < 141)  tmpPart.bin = 5;*/
        }
	if(tmpPart.bin == -1) continue;	
	       
	//PID
	tmpPart.PID = 0;
	if(tmpPart.pNN > pNNCut) {
	  tmpPart.PID = 2212;
	} else if(tmpPart.kNN > kNNCut) {
	  tmpPart.PID = 321;
	} else   if(tmpPart.piNN > piNNCut) {      
	  tmpPart.PID = 211;
	} 
	      
        if(tmpPart.PID == 0) continue;
        if(tmpPart.PID == 211) {
          tmpPart.mass           = 0.140;
          tmpPart.fourVec        = build4V(p[i],pt[i],phi[i],0.140);
          tmpPart.transverse4Vec = buildTrans4V(pt[i],phi[i]);
          inputPions[tmpPart.bin].push_back(tmpPart);
        }
        if(tmpPart.PID == 321) {
          tmpPart.mass           = 0.494;
          tmpPart.fourVec        = build4V(p[i],pt[i],phi[i],0.494);
          tmpPart.transverse4Vec = buildTrans4V(pt[i],phi[i]);
          inputKaons[tmpPart.bin].push_back(tmpPart);
        }
        if(tmpPart.PID == 2212) {
          tmpPart.mass           = 0.938;
          tmpPart.fourVec        = build4V(p[i],pt[i],phi[i],0.938);
          tmpPart.transverse4Vec = buildTrans4V(pt[i],phi[i]);
          inputProts[tmpPart.bin].push_back(tmpPart);
        }
        inputParts[tmpPart.bin].push_back(tmpPart);
        if(abs(tmpPart.MCID) == 211) {
          inputTruePions[tmpPart.bin].push_back(tmpPart);
        }
        if(abs(tmpPart.MCID) == 321) {
          inputTrueKaons[tmpPart.bin].push_back(tmpPart);
        }
        //---------------------------------
      }
      
      ///////// Correlations analysis
      // Loop over particles
      std::vector<chPart> parts[nrBins];
      std::vector<chPart> partsTrue[nrBins];
      
      for(int i_bins = 0; i_bins < nrBins; ++i_bins)
      {
        parts[i_bins] = inputPions[i_bins];
        partsTrue[i_bins] = inputTruePions[i_bins];
      }
      
      /*
      if(partsTrue.size() > 1) {
        // Clones
        for(std::vector<chPart>::iterator it = partsTrue.begin();
            it != partsTrue.end(); it++) {
          chPart part1 = *it;
          if(part1.isClone == 1) {
            nrClones++;
            for(std::vector<chPart>::iterator it = partsTrue.begin();
                it != partsTrue.end(); it++) {
              chPart part2 = *it;
              if(part1.prtIndex != part2.prtIndex) {
                if(part2.isClone == 1) {
                  double Qp, Qm, dPhi, dY, dPt;
                  LAB(part1,part2,Qp,Qm,dPhi,dY,dPt);
                  h2101->Fill(Qp);
                }
              }
            }
          }
        }
      }
      */
      
      if(!enoughForMixing){
        enoughForMixing=1;
        for(int i=0; i<nrBinsRef;++i){
          if(partsToBeMixed[i].size() < partsToBeMixedMaxSize){
            enoughForMixing=0;
       	    break;
	  }	
        } 	  
      }
      
      for(int i_bins = 0; i_bins < nrBins; ++i_bins) //dla kaæego binu...
      {
        if(parts[i_bins].size() > 1) {
          // Correlations
          for(std::vector<chPart>::iterator it1 = parts[i_bins].begin();
              it1 != parts[i_bins].end() - 1; it1++) {

            chPart part1 = *it1;

	    h2501->Fill(part1.pvMult);

            if(MC)
            {
              h2401[i_bins]->Fill(part1.MCID);
            }

            for(std::vector<chPart>::iterator it2 = it1 + 1;
                it2 != parts[i_bins].end(); it2++) {
              chPart part2 = *it2;
              // LHCb frame variables
              double Qp, Qm, dPhi, dY, dPt;
              LAB(part1,part2,Qp,Qm,dPhi,dY,dPt);
              // LCMS frame variables
              double Q_LCMS, Q_long, Q_side, Q_out, delPhiLCMS, delYLCMS, 
                delPtLCMS;
              LCMS(part1,part2,Q_LCMS,Q_long,Q_side,Q_out,delPhiLCMS,delYLCMS,
                   delPtLCMS);

             
              if(Q_LCMS < qCut) continue;

              // LIKE
              if((part1.charge  == part2.charge) && 
                 (part1.pvIndex == part2.pvIndex)) {
		h2502->Fill(part2.pvMult);
                h3001[i_bins]->Fill(Qp);
                h3002->Fill(Qm);
                h3003->Fill(dPhi);
                h3004->Fill(dY);
                h3005->Fill(dPt);
                h3006->Fill(Q_LCMS);
                h3007->Fill(Q_long);
                h3008->Fill(Q_side);
                h3009->Fill(Q_out);
                h3010->Fill(delPhiLCMS);
                h3011->Fill(delYLCMS);
                h3012->Fill(delPtLCMS);
		//Gamov correction
                if(!MC)
                {
                  double w = 1./f_like(Qp);
                  h3015[i_bins]->Fill(Qp,w);
                }           
              } 
              // UNLIKE
              if((part1.charge  != part2.charge) &&
                 (part1.pvIndex == part2.pvIndex)) {
                h3101[i_bins]->Fill(Qp);
                h3102->Fill(Qm);
                h3103->Fill(dPhi);
                h3104->Fill(dY);
                h3105->Fill(dPt);
                h3106->Fill(Q_LCMS);
                h3107->Fill(Q_long);
                h3108->Fill(Q_side);
                h3109->Fill(Q_out);
                h3110->Fill(delPhiLCMS);
                h3111->Fill(delYLCMS);
                h3112->Fill(delPtLCMS);
		//Gamov correction
                if(!MC)
                {
                  double w = 1./f_unlike(Qp);
                  h3115[i_bins]->Fill(Qp,w);
                }           
              }
            }
          }
          ////////// EVENT MIXING 
          if(enoughForMixing) {  
            for(std::vector<chPart>::iterator it = parts[i_bins].begin();
                it != parts[i_bins].end(); it++) {
              chPart part1 = *it;
              double indRand = rand3.Rndm();
	
	      int partPvMultIndex = part1.pvMult - partPVMultIndexOffset;
	      if(partsToBeMixed[partPvMultIndex].size() == 0){
		++nrPartsEvmixLost;
		continue;
	      }
              int iRand = int(indRand * partsToBeMixed[partPvMultIndex].size());

              //access element of index iRand
              std::list<chPart>::iterator it_partsToBeMixed = partsToBeMixed[partPvMultIndex].begin();
              for(int i=0;i<iRand;++i) ++it_partsToBeMixed;
              chPart part2 = *it_partsToBeMixed;

              partsToBeMixed[partPvMultIndex].erase(it_partsToBeMixed);
         
              // LHCb frame variables
              double Qp, Qm, dPhi, dY, dPt;
              LAB(part1,part2,Qp,Qm,dPhi,dY,dPt);
              // LCMS frame variables
              double Q_LCMS, Q_long, Q_side, Q_out, delPhiLCMS, delYLCMS, 
                delPtLCMS;
              LCMS(part1,part2,Q_LCMS,Q_long,Q_side,Q_out,delPhiLCMS,delYLCMS,
                   delPtLCMS);
            
              if(Q_LCMS < qCut) continue;

              // EVENT MIXED
              h3401[i_bins]->Fill(Qp);
              h3402->Fill(Qm);
              h3403->Fill(dPhi);
              h3404->Fill(dY);
              h3405->Fill(dPt);
              h3406->Fill(Q_LCMS);
              h3407->Fill(Q_long);
              h3408->Fill(Q_side);
              h3409->Fill(Q_out);
              h3410->Fill(delPhiLCMS);
              h3411->Fill(delYLCMS);
              h3412->Fill(delPtLCMS);
            }
          }
               
          for(std::vector<chPart>::iterator it = parts[i_bins].begin();
              it != parts[i_bins].end(); it++) {
	    int partPvMultIndex = (it->pvMult)- partPVMultIndexOffset;
	    if(partsToBeMixed[partPvMultIndex].size() < partsToBeMixedMaxSize){
	      partsToBeMixed[partPvMultIndex].push_back(*it);
	    }
          }
	  
        }
        
      }
      
    }
    
    f->Close();
  }

  time_t end;
  time(&end);
  
  //summary
  cout << "End loop over files" << endl;
  cout << "----------------------------------------------------------" << endl;
  cout << "Number of read / available events: " << h101->GetEntries() << " / "<< nrEvts << endl;
  cout << "----------------------------------------------------------" << endl;
  double time_diff = difftime(end,start); //in seconds
  double time_min = time_diff/60.;
  cout << "Time of execution [min] : " << time_min << endl;
  cout << "----------------------------------------------------------" << endl;
 
  // Write histrograms
  fHisto.Write();
  cout << "Histograms wrote to " + TString(label) + ".root \n"; 
  fHisto.Close();

  return 0;
}
