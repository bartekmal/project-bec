#ifndef PIDPERFTOOLS_MULTITRACKCALIBTOOL_H
#define PIDPERFTOOLS_MULTITRACKCALIBTOOL_H 1

// Include files
#include <list>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TString.h"
#include "TEntryList.h"
#include "PIDPerfTools/TrkPIDParams.h"

/** @class MultiTrackCalibTool MultiTrackCalibTool.h PIDPerfTools/MultiTrackCalibTool.h
 *
 *
 *  @author Andrew Powell (LHCb)04
 *  @date   2013-01-31
 */
class MultiTrackCalibTool {
public:
  /// Standard constructor
  MultiTrackCalibTool(std::string Name, TTree* RefTreeName,
                      const std::string& OutputFileName,
                      const std::string& mode="UPDATE",
                      Bool_t verbose=kFALSE,
                      Int_t printFreq=-1);

  virtual ~MultiTrackCalibTool(); ///< Destructor

  void SetTrackMomVarName(const std::string& NameInTree);

  void SetTrackPtVarName(const std::string& NameInTree);

  void SetTrackEtaVarName(const std::string& NameInTree);

  void SetNTracksVarName(const std::string& NameInTree);
  
  void SetNSPDHitsVarName(const std::string& NameInTree);
  
  void SetNVeloClustersVarName(const std::string& NameInTree);

  void SetSWeightVarName(const std::string& NameInTree);

  void DeclareSignalTrackAndPerfHist(const std::string& TrackNameInTree,
                                     TH1* PerfHist);

  void SetRefDataInPerfHistLimits();

  void CheckSanityOfPerfHists();

  void Calculate();

  void Calculate(const std::string& IDVar_suffix);

  std::pair<Float_t, Float_t> CalculateNaiveAverage();
  std::pair<Float_t, Float_t> CalculateNaiveWeightAverage();

  void Write(std::string name);

  //void CloseFile();

protected:

  void SetOutputFile(const std::string& FileName, const std::string& Mode);

  Bool_t validBranch(TTree* tt, const TString& brName);

  std::string getBranchType(TTree* tt, const TString& brName);

  void ReOrderBinningVector();

  void CreateTrkPIDEffBranches();

  void CreateTotPIDEffBranches(TTree* tt);

  void SetTrackKinVarBranchAddressInInputTree();

  void SetTrackIDVarBranchAddressInInputTree(const std::string& IDVar_suffix);

  Float_t CalcEventFracError(std::vector<std::pair<Int_t,Float_t> > vTrckIDandFracErrs);

  Float_t CalcPerEventErrorCrossTerm(std::vector<std::pair<Int_t,Float_t> > vTrckIDandFracErrs);

private:

  //==========================================================================
  // Reorder a vector according to the indices defined in the vector order
  //==========================================================================
  template< class T >
  void ReOrderVector(std::vector<T>& v, const std::vector<size_t>& order)
  {
    for ( unsigned int s = 1, d; s < order.size(); ++ s )
    {
      for ( d = order[s]; d < s; d = order[d] ) ;
      if ( d == s )
        while ( d = order[d], d != s )
          swap( v[s], v[d] );
    }
  }


  double CastVoidPointerToDouble(void *val, TTree* tt, std::string branchName);

  int CastVoidPointerToInt(void *val, TTree* tt, std::string branchName);

  std::string GetKinVarBranchName(std::string trkName, std::string binVarIntName, std::string binVarName);

  void Loop();

  void DeleteHeapMemVars();

  std::string m_name;

  Bool_t m_verbose;
  Int_t m_printFreq;

  std::map<std::string, TH1*> m_TrackEffMap;

  std::map<std::string, std::string> m_PerfParamMap;

  std::vector<std::pair<std::string, std::string> > m_BinningParam;

  unsigned int m_indexNTracks;
  unsigned int m_indexNSPDHits;
  unsigned int m_indexNVeloClusters;
  unsigned int m_BinningDimensions;

  Bool_t m_BinningVectorSorted;

  Bool_t m_KinVarBranchesSet;

  Bool_t m_IDVarBranchesSet;

  TTree* m_InputRefTree;

  TEntryList* m_EntryList;

  TFile* m_OutputFile;

  TTree* m_OutputTree;

  void* m_sWeight_var;
  std::string m_sWeight_name;

  std::map<std::string, std::vector<void*> > m_TrackVars;

  std::map<std::string, TrkPIDParams> m_TrackPIDVals;

  Float_t m_TotPIDEff;

  Float_t m_TotPIDErr;

  Float_t m_TotPIDEffWeight;

  Float_t m_TotPIDErrWeight;

  Float_t m_TotWeight;

  std::pair<Float_t, Float_t> m_NaiveAverageCounters;
  std::pair<Float_t, Float_t> m_NaiveAverageWeightCounters;

  std::vector<std::pair<Int_t,Float_t> > m_vTrckIDandFracErrs;
  std::vector<std::pair<Int_t,Float_t> > m_vTrckIDandFracErrsWeight;

  std::string m_IDVar_suffix;

};
#endif // PIDPERFTOOLS_MULTITRACKCALIBTOOL_H
