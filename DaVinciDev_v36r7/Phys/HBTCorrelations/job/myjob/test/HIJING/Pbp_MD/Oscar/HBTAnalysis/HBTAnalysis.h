#include <Riostream.h>
#include "TFile.h"
#include "TNtuple.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TMultiLayerPerceptron.h"
#include "TMLPAnalyzer.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TROOT.h"
#include <TRandom3.h>

#include <list>
#include <vector>
#include <iostream>
#include <string>

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

class Jet {
  public:
     int jetIndex;
     double pxJet;
     double pyJet;
     double pzJet;
     double eJet;
     double pJet;
     double multJet;
};




//-----------------------------------------------------------------------------
// Set branch addresses
//-----------------------------------------------------------------------------
// Declaration of types for branches
int nrPVs;
int multNoPreSel;
int multPreSel;
int N_J;

int PVMult1;
int PVMult2;
int PVMult3;
int PVMult4;

int SPDhits;

double pxJet[10];
double pyJet[10];
double pzJet[10];
double eJet[10];
double pJet[10];
double multJet[10];
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



void setBranches(TTree* HBTTuple);


//-----------------------------------------------------------------------------
// Get files
//-----------------------------------------------------------------------------
#ifdef __CINT__
#undef __attribute__
#define __attribute__(X)
#endif
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
std::vector<TString> getFiles(string dir, const char* datatype) 
{
  std::vector<TString> filelist(0);
  ifstream fin;
  string filepath;
  DIR *dp;
  struct dirent *dirp;
  struct stat filestat;
  dp = opendir(dir.c_str());
  if(dp == NULL) {
    return filelist;
  }
  while((dirp = readdir(dp))) {
    filepath = dir + "/" + dirp->d_name;
    // If the file is a directory (or is in some way invalid) we'll skip it 
    if(stat(filepath.c_str(),&filestat)) continue;
    if(S_ISDIR(filestat.st_mode)) continue;
    const char* astr = "root"; // Check that filepath word contains root
    const char* conroot = strstr(filepath.c_str(),astr);
    if(!conroot) continue;
    const char* contype = strstr(filepath.c_str(),datatype);
    if(!contype) continue;
    filelist.push_back(filepath);
  }
  closedir(dp);
  std::sort(filelist.begin(),filelist.end());
  return filelist;
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
