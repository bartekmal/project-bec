#include <TFile.h>
#include <TNtuple.h>
#include <TLorentzVector.h>
#include <TRandom3.h>
#include <TH1D.h>
#include <TH2D.h>

#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <time.h>

using namespace std;

class chPart {
  public:
     int prtIndex;
     int pvIndex;
     int charge;
     double kDLL;
     double pDLL;
     double piNN;
     double kNN;
     double pNN;
     double ghostNN;
     double xBestPV;
     double yBestPV;
     double zBestPV;
     double pxBestPV;
     double pyBestPV;
     double pzBestPV;
     double chi2BestPV;
     double e;
     double p;
     double pt;
     double phi;
     double theta;
     double rapidity;
     double eta;
     double ip;
     double ipChi2;
     double trkChi2;
     double cloneDist;
     double chi2PV1;
     double chi2PV2;
     double chi2PV3;     
     double chi2PV4;
     int isClone;
     int MCID;
     int PID;
     int bin;
     int pvMult;
     double mass;
     TLorentzVector fourVec;
     TLorentzVector transverse4Vec;
};

class partPair {
  public:
     int pvIndex1;
     int pvIndex2;
     int charge1;
     int charge2;
     double Q_LAB;
     double deltaPhi_LAB;
     double deltaY_LAB;
     double deltaPt_LAB;
     TLorentzVector fourVec1;
     TLorentzVector fourVec2;
     double Q_LCMS;
     double Q_long;
     double Q_side;
     double Q_out;
     double deltaPhi_LCMS;
     double deltaY_LCMS;
     double deltaPt_LCMS;
  
};

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
double kDLL[2000];
double pDLL[2000];
double piNN[2000];
double kNN[2000];
double pNN[2000];
double ghostNN[2000];
double xBestPV[2000];
double yBestPV[2000];
double zBestPV[2000];
double pxBestPV[2000];
double pyBestPV[2000];
double pzBestPV[2000];
double chi2BestPV[2000];
double e[2000];
double p[2000];
double pt[2000];
double phi[2000];
double theta[2000];
double rapidity[2000];
double eta[2000];
double ip[2000];
double ipChi2[2000];
double trkChi2[2000];
double cloneDist[2000];
double chi2PV1[2000];
double chi2PV2[2000];
double chi2PV3[2000];     
double chi2PV4[2000];
int isClone[2000];
int MCID[2000];
float piNNcor[2000];
float kNNcor[2000];
float pNNcor[2000];
double shareVHits[2000];
int isMuon[2000];


//-----------------------------------------------------------------------------
// Build 4-momentum vector
//-----------------------------------------------------------------------------
TLorentzVector build4V(double p, double pt, double phi, double m) 
{
  double a = p * p - pt * pt;
  if(a < 0) {
    cout << "Error in build4V: negative value detected!" << endl;
    return TLorentzVector(0.,0.,0.,0.);
  }
  TLorentzVector v(pt * cos(phi), pt * sin(phi), sqrt(a), sqrt(p * p + m * m));
  return v;
}


//-----------------------------------------------------------------------------
// Build transverse 4-momentum vector
//-----------------------------------------------------------------------------
TLorentzVector buildTrans4V(double pt, double phi) 
{
  TLorentzVector v(pt * cos(phi), pt * sin(phi), 0.0, pt);
  return v;
}


//-----------------------------------------------------------------------------
// Variables in LAB frame
//-----------------------------------------------------------------------------
void LAB(chPart part1, chPart part2, double& Qp, double& Qm, double& dPhi, double& dY, double& dPt) {
  TLorentzVector LVp = part1.fourVec + part2.fourVec;
  TLorentzVector LVm = part1.fourVec - part2.fourVec;
  Qp = sqrt(LVp.M2() - 4 * part1.mass * part2.mass);
  Qm = sqrt((-1.0) * LVm.M2());
  dPhi = fabs(part1.phi - part2.phi);
  if(dPhi > 3.1415) dPhi = 6.283 - dPhi;
  dPhi = dPhi * 57.29;
  dY = fabs(part1.fourVec.Rapidity() - part2.fourVec.Rapidity());
  dPt = fabs(part1.pt - part2.pt);
}


//-----------------------------------------------------------------------------
// Build LCMS reference frame
//-----------------------------------------------------------------------------
void LCMS(chPart part1, chPart part2, double& Q_LCMS, double& Q_long,double& Q_side, double& Q_out, double& delPhiLCMS, double& delYLCMS, double& delPtLCMS) {
  // z-axis as a reference axis
  TVector3 direction = TVector3(0.0,0.0,1.0);
  direction *= (1.0 / direction.Mag());
  TLorentzVector Qp = part1.fourVec + part2.fourVec;
  TVector3 boost;
  boost.SetX((-1.0) * direction.X() * Qp.X() / Qp.E());
  boost.SetY((-1.0) * direction.Y() * Qp.Y() / Qp.E());
  boost.SetZ((-1.0) * direction.Z() * Qp.Z() / Qp.E());
  TLorentzVector q1Boosted = part1.fourVec;
  TLorentzVector q2Boosted = part2.fourVec;
  q1Boosted.Boost(boost);
  q2Boosted.Boost(boost);
  TLorentzVector QpBoosted = q1Boosted + q2Boosted;
  TLorentzVector QmBoosted = q1Boosted - q2Boosted;
  TVector3 Q3p = QpBoosted.Vect();
  TVector3 Q3m = QmBoosted.Vect();
  /////////// Out, Long, Side
  // Out
  TVector3 V3Out = Q3p;
  V3Out *= (1.0 / V3Out.Mag()); // out versor
  double vOut = Q3m.Dot(V3Out);
  V3Out *= vOut;
  // Long
  TVector3 V3Long = direction;
  V3Long *= (1.0 / V3Long.Mag()); // long versor
  double vLong = V3Long.Dot(Q3m);
  V3Long *= vLong;  
  // Side
  TVector3 V3Side = V3Long.Cross(V3Out);
  V3Side *= (1.0 / V3Side.Mag());  // side versor  
  double vSide = V3Side.Dot(Q3m);
  V3Side *= vSide;
  // Q, Q_long, Q_side, Q_out
  Q_LCMS  = sqrt(QpBoosted.M2() - 4 * q1Boosted.M() * q2Boosted.M());
  Q_long = V3Long.Mag();
  if(V3Long.Theta() != 0.0) Q_long = -1.0 * Q_long;
  Q_side = V3Side.Mag();
  if(V3Side.Theta() != 0.0) Q_side = -1.0 * Q_side;
  Q_out = V3Out.Mag();
  if(V3Out.Theta() != 0.0) Q_out = -1.0 * Q_out;
  // Delta phi, delta y, delta pt in LCMS
  delPhiLCMS = fabs(q1Boosted.Phi() - q2Boosted.Phi());
  if(delPhiLCMS > 3.1415) delPhiLCMS = 6.283 - delPhiLCMS;
  delPhiLCMS = delPhiLCMS * 57.29;
  delYLCMS = fabs(q1Boosted.Rapidity() - q2Boosted.Rapidity());
  delPtLCMS = fabs(q1Boosted.Perp() - q1Boosted.Perp());
}

//-----------------------------------------------------------------------------
// Set branch addresses
//-----------------------------------------------------------------------------
void setBranches(TTree* HBTTuple) {
  HBTTuple->SetBranchAddress("nrPVs",&nrPVs);
  HBTTuple->SetBranchAddress("multNoPreSel",&multNoPreSel);
  HBTTuple->SetBranchAddress("multPreSel",&multPreSel);
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

//-------------------------------------------------
//Coulomb corren\ction by Gamov factor
//---------------------------------------------------
double f_like(double x)
{
  double a = 0.13957 * 2.0 * 3.1415926535 * (1.0 / 137.0);
  double f = (1.0 * a / x) / (exp(1.0 * a / x) - 1.0);
  return f;
}

double f_unlike(double x)
{
  double a = 0.13957 * 2.0 * 3.1415926535 * (1.0 / 137.0);
  double f = (-1.0 * a / x) / (exp(-1.0 * a / x) - 1.0);
  return f;
}
