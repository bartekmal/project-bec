#ifndef TREEDATA_H
#define TREEDATA_H

#include <TNtuple.h>

#include "Units.hpp"

// Declaration of types for branches
int nrPVs;
int multNoPreSel;
int multPreSel;

int PVMult1;
int PVMult2;
int PVMult3;
int PVMult4;

int SPDhits;

int N;
int pvIndex[2000];
int charge[2000];
HBT::Units::FloatType kDLL[2000];
HBT::Units::FloatType pDLL[2000];
HBT::Units::FloatType piNN[2000];
HBT::Units::FloatType kNN[2000];
HBT::Units::FloatType pNN[2000];
HBT::Units::FloatType ghostNN[2000];
HBT::Units::FloatType xBestPV[2000];
HBT::Units::FloatType yBestPV[2000];
HBT::Units::FloatType zBestPV[2000];
HBT::Units::FloatType pxBestPV[2000];
HBT::Units::FloatType pyBestPV[2000];
HBT::Units::FloatType pzBestPV[2000];
HBT::Units::FloatType chi2BestPV[2000];
HBT::Units::FloatType e[2000];
HBT::Units::FloatType p[2000];
HBT::Units::FloatType pt[2000];
HBT::Units::FloatType phi[2000];
HBT::Units::FloatType theta[2000];
HBT::Units::FloatType rapidity[2000];
HBT::Units::FloatType eta[2000];
HBT::Units::FloatType ip[2000];
HBT::Units::FloatType ipChi2[2000];
HBT::Units::FloatType trkChi2[2000];
HBT::Units::FloatType cloneDist[2000];
HBT::Units::FloatType chi2PV1[2000];
HBT::Units::FloatType chi2PV2[2000];
HBT::Units::FloatType chi2PV3[2000];
HBT::Units::FloatType chi2PV4[2000];
int isClone[2000];
int MCID[2000];
HBT::Units::FloatTypeForNNCorrectedValues piNNcor[2000];
HBT::Units::FloatTypeForNNCorrectedValues kNNcor[2000];
HBT::Units::FloatTypeForNNCorrectedValues pNNcor[2000];
HBT::Units::FloatType shareVHits[2000];
int isMuon[2000];

//-----------------------------------------------------------------------------
// Set branch addresses
//-----------------------------------------------------------------------------
void setBranches(TTree *HBTTuple)
{
  HBTTuple->SetBranchAddress("nrPVs", &nrPVs);
  HBTTuple->SetBranchAddress("multNoPreSel", &multNoPreSel);
  HBTTuple->SetBranchAddress("multPreSel", &multPreSel);
  HBTTuple->SetBranchAddress("N", &N);
  HBTTuple->SetBranchAddress("pvIndex", &pvIndex);
  HBTTuple->SetBranchAddress("charge", &charge);
  HBTTuple->SetBranchAddress("kDLL", &kDLL);
  HBTTuple->SetBranchAddress("pDLL", &pDLL);
  HBTTuple->SetBranchAddress("piNN", &piNN);
  HBTTuple->SetBranchAddress("kNN", &kNN);
  HBTTuple->SetBranchAddress("pNN", &pNN);
  HBTTuple->SetBranchAddress("ghostNN", &ghostNN);
  HBTTuple->SetBranchAddress("xBestPV", &xBestPV);
  HBTTuple->SetBranchAddress("yBestPV", &yBestPV);
  HBTTuple->SetBranchAddress("zBestPV", &zBestPV);
  HBTTuple->SetBranchAddress("pxBestPV", &pxBestPV);
  HBTTuple->SetBranchAddress("pyBestPV", &pyBestPV);
  HBTTuple->SetBranchAddress("pzBestPV", &pzBestPV);
  HBTTuple->SetBranchAddress("chi2BestPV", &chi2BestPV);
  HBTTuple->SetBranchAddress("e", &e);
  HBTTuple->SetBranchAddress("p", &p);
  HBTTuple->SetBranchAddress("pt", &pt);
  HBTTuple->SetBranchAddress("phi", &phi);
  HBTTuple->SetBranchAddress("theta", &theta);
  HBTTuple->SetBranchAddress("rapidity", &rapidity);
  HBTTuple->SetBranchAddress("eta", &eta);
  HBTTuple->SetBranchAddress("ip", &ip);
  HBTTuple->SetBranchAddress("ipChi2", &ipChi2);
  HBTTuple->SetBranchAddress("trkChi2", &trkChi2);
  HBTTuple->SetBranchAddress("cloneDist", &cloneDist);
  HBTTuple->SetBranchAddress("chi2PV1", &chi2PV1);
  HBTTuple->SetBranchAddress("chi2PV2", &chi2PV2);
  HBTTuple->SetBranchAddress("chi2PV3", &chi2PV3);
  HBTTuple->SetBranchAddress("chi2PV4", &chi2PV4);
  HBTTuple->SetBranchAddress("isClone", &isClone);
  HBTTuple->SetBranchAddress("MCID", &MCID);
  HBTTuple->SetBranchAddress("piNNcor", &piNNcor);
  HBTTuple->SetBranchAddress("kNNcor", &kNNcor);
  HBTTuple->SetBranchAddress("pNNcor", &pNNcor);
  HBTTuple->SetBranchAddress("shareVHits", &shareVHits);
  HBTTuple->SetBranchAddress("isMuon", &isMuon);
  HBTTuple->SetBranchAddress("PVMult1", &PVMult1);
  HBTTuple->SetBranchAddress("PVMult2", &PVMult2);
  HBTTuple->SetBranchAddress("PVMult3", &PVMult3);
  HBTTuple->SetBranchAddress("PVMult4", &PVMult4);
  HBTTuple->SetBranchAddress("SPDhits", &SPDhits);
}

#endif /* !TREEDATA_H */
