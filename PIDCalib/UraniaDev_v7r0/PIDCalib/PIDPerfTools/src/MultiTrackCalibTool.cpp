// Include files
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include "TBranch.h"
#include "TLeaf.h"
#include "TList.h"
#include <stdio.h>
#include <string.h>

// Boost
#include "boost/lexical_cast.hpp"

// local
#include "PIDPerfTools/MultiTrackCalibTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MultiTrackCalibTool
//
// 2013-01-31 : Andrew Powell (LHCb)04
//-----------------------------------------------------------------------------

using std::cout;
using std::endl;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MultiTrackCalibTool::MultiTrackCalibTool(std::string Name,
                                         TTree* RefTree,
                                         const std::string& OutputFileName,
                                         const std::string& mode,
                                         Bool_t verbose,
                                         Int_t printFreq)
  : m_name(Name),
    m_verbose(verbose),
    m_printFreq(printFreq),
    m_indexNTracks(0),
    m_indexNSPDHits(0),
    m_indexNVeloClusters(0),
    m_BinningDimensions(0),
    m_BinningVectorSorted(0),
    m_KinVarBranchesSet(0),
    m_IDVarBranchesSet(0),
    m_InputRefTree(NULL),
    m_EntryList(NULL),
    m_OutputFile(NULL),
    m_OutputTree(NULL),
    m_sWeight_var(NULL),
    m_sWeight_name(""),
    m_TotPIDEff(1.0),
    m_TotPIDErr(0.0),
    m_TotPIDEffWeight(1.0),
    m_TotPIDErrWeight(0.0),
    m_TotWeight(1.0),
    m_IDVar_suffix("")
{
  // Set PIDPerfTools
  //m_PerfParamMap["Track Momentum [MeV/c]"] = "P";
  //m_PerfParamMap["Transverse Momentum [MeV/c]"] = "PT";
  //m_PerfParamMap["Psuedo Rapidity"] = "ETA";
  //m_PerfParamMap["No. Best Tracks in Event"] = "nTRACKS";
  
  this->SetOutputFile(OutputFileName, mode);
  m_OutputFile->cd();

  // Declare output tree on the heap
  m_OutputTree = new TTree(TString(m_name+"_PIDCalibTree").Data(), "PID Calibration Results");

  m_InputRefTree = RefTree;
  if ( (m_InputRefTree==NULL) )
  {
    cout << "ERROR: Failed to obtain pointer to TTree "
         << endl;
    exit (EXIT_FAILURE);
  }

  // Ensure all maps and vectors are empty
  m_TrackEffMap.clear();
  m_BinningParam.clear();
  m_TrackVars.clear();
  m_TrackPIDVals.clear();
  m_vTrckIDandFracErrs.clear();
  m_vTrckIDandFracErrsWeight.clear();
}

void MultiTrackCalibTool::Write(std::string name = "")
{
  if ( name.empty() ) name = m_name+"_PIDCalibTree";
  if ( (m_OutputTree==NULL) ) return;
  if ( (m_OutputFile!=NULL) ) {
    m_OutputFile->cd();
    m_OutputTree->Write(name.c_str());
  }
}

//=============================================================================
// Destructor
//=============================================================================
MultiTrackCalibTool::~MultiTrackCalibTool()
{
  //m_OutputFile->cd();

  // Write output TTree to output TFile
  //m_OutputTree->Write();
  //if (m_verbose) cout << "Wrote output TTree" <<endl;

  if( (m_OutputTree!=NULL) )
  {
    if (m_verbose) cout << "Deleting output TTree" << endl;
    m_OutputTree->Delete();
    m_OutputTree=0;
    if (m_verbose) cout << "Deleted output TTree" << endl;
  }

  //if (m_verbose) cout << "Closing output TFile" <<endl;
  //m_OutputFile->Close();
  //if (m_verbose) cout << "Closed output TFile" <<endl;

  // Delete dynamically allocated output TTree and TFile
  if (m_verbose) cout << "Deleting output TFile" <<endl;
  if( (m_OutputFile!=NULL) ) {
    m_OutputFile->Delete();
    m_OutputFile=0;
  }
  if (m_verbose) cout << "Deleted output TFile" <<endl;
}

//=============================================================================
// Set output TFile
//=============================================================================
void MultiTrackCalibTool::SetOutputFile(const std::string& FileName, const std::string& Mode)
{
  if ((Mode.compare("NEW")!=0)&&(Mode.compare("CREATE")!=0)&&(Mode.compare("RECREATE")!=0)&&(Mode.compare("UPDATE")!=0)) {
    cout << "ERROR: Expected NEW, CREATE, RECREATE or UPDATE as access mode, got " << Mode << endl;
    exit(EXIT_FAILURE);
  }

  m_OutputFile = TFile::Open(FileName.c_str(), Mode.c_str());
  if ( (m_OutputFile==NULL) ) {
    cout << "ERROR: Failed to create output file. Check the file permissions."
         << " If opening in NEW/CREATE mode, ensure that the specified file does not exist"
         << endl;
    exit(EXIT_FAILURE);
  }
}

//=============================================================================
// Confirm branch exists in TTree
//=============================================================================
Bool_t MultiTrackCalibTool::validBranch(TTree* tt, const TString& brName)
{
  if ( (tt==NULL) ) {
    cout << "No TTree specified" << endl;
    return kFALSE;
  }

  if ( !(tt->GetBranch(brName.Data())) ) return kFALSE;
  return kTRUE;
}

//=============================================================================
// Return branch type
//=============================================================================
std::string MultiTrackCalibTool::getBranchType(TTree* tt, const TString& brName)
{
  if (!tt) {
    cout << "No TTree specified" << endl;
    exit(EXIT_FAILURE);
  }
 
  TBranch* br = tt->GetBranch(brName.Data());
  if (!br) {
    cout << "Failed to retrieve branch " << brName
              << " from TTree " << tt->GetName() << endl;
    exit(EXIT_FAILURE);
  }

  return std::string(((TLeaf*)br->GetListOfLeaves()->At(0))->GetTypeName());
}

//=============================================================================
// Declare momentum variable name in reference tree
//=============================================================================
void MultiTrackCalibTool::SetTrackMomVarName(const std::string& NameInTree)
{
  m_BinningParam.push_back( make_pair("P", NameInTree) );

  m_BinningVectorSorted = false;
}

//=============================================================================
// Declare transverse momentum variable name in reference tree
//=============================================================================
void MultiTrackCalibTool::SetTrackPtVarName(const std::string& NameInTree)
{
  m_BinningParam.push_back( make_pair("PT", NameInTree) );

  m_BinningVectorSorted = false;
}

//=============================================================================
// Declare pseudo-rapdity variable name in reference tree
//=============================================================================
void MultiTrackCalibTool::SetTrackEtaVarName(const std::string& NameInTree)
{
  m_BinningParam.push_back( make_pair("ETA",NameInTree) );

  m_BinningVectorSorted = false;
}

//=============================================================================
// Declare nTracks variable name in reference tree
//=============================================================================
void MultiTrackCalibTool::SetNTracksVarName(const std::string& NameInTree)
{
  m_BinningParam.push_back( make_pair("nTRACKS",NameInTree) );

  m_BinningVectorSorted = false;
}

//=============================================================================
// Declare nSPDHits variable name in reference tree
//=============================================================================
void MultiTrackCalibTool::SetNSPDHitsVarName(const std::string& NameInTree)
{
  m_BinningParam.push_back( make_pair("nSPDHITS",NameInTree) );

  m_BinningVectorSorted = false;
}

//=============================================================================
// Declare nVeloClusters variable name in reference tree
//=============================================================================
void MultiTrackCalibTool::SetNVeloClustersVarName(const std::string& NameInTree)
{
  m_BinningParam.push_back( make_pair("nVeloClusters",NameInTree) );

  m_BinningVectorSorted = false;
}


//=============================================================================
// Declare sWeights variable name in reference tree
//=============================================================================
void MultiTrackCalibTool::SetSWeightVarName(const std::string& NameInTree)
{
  m_sWeight_name = NameInTree;
}

//=============================================================================
// Declare signal track and its associated performance histogram
//=============================================================================
void MultiTrackCalibTool::DeclareSignalTrackAndPerfHist(const std::string& TrackNameInTree, TH1* PerfHist)
{
 	if(m_TrackEffMap.empty())
  {
    m_BinningDimensions = PerfHist->GetDimension();
  }
  else if(m_BinningDimensions!=(unsigned int)PerfHist->GetDimension())
  {
     cout<<"**ERROR** : This histogram has different dimensions to those histograms already added" << endl;
     exit(EXIT_FAILURE);
  }

  // Need to confirm the x,y,z dimensions have the same name in each case...
  std::map<std::string, TH1*>::iterator itr;
  for(itr=m_TrackEffMap.begin(); itr!=m_TrackEffMap.end(); ++itr)
  {
    assert(itr->second->GetXaxis()->GetTitle() == PerfHist->GetXaxis()->GetTitle());

    if(m_BinningDimensions > 1)
      assert(itr->second->GetYaxis()->GetTitle() == PerfHist->GetYaxis()->GetTitle());

    if(m_BinningDimensions > 2)
      assert(itr->second->GetZaxis()->GetTitle() == PerfHist->GetZaxis()->GetTitle());
  }

  m_TrackEffMap.insert( std::pair<std::string, TH1*>(TrackNameInTree, PerfHist));

}

//=============================================================================
// Re-order m_BinningParams such that indicies 0,1,2 correspond to the x,y,z
// parameters, respectively, in the performance histograms.
//=============================================================================
void MultiTrackCalibTool::ReOrderBinningVector()
{
  // Check consistent dimensionality with histogram and
  if(m_BinningParam.size()!=m_BinningDimensions)
  {
    cout << "**ERROR** : Inconsistent dimensionality between "
      "histogram and binning parameter list" << endl;
    exit(EXIT_FAILURE);
  }

  //Declare iterator for m_TrackEffMap
  std::map<std::string, TH1*>::iterator itr_begin = m_TrackEffMap.begin();
  
  std::vector<size_t> order;
  std::vector<std::pair<std::string,std::string> >::iterator itr;

  // Simple case of a 1-dimensional binning (no ordering necessary)
  if(m_BinningDimensions == 1)
  {
  	m_BinningVectorSorted = true;
    /*if(m_PerfParamMap.find(itr_begin->second->GetXaxis()->GetTitle()) != m_PerfParamMap.end()) {
      m_BinningVectorSorted = true;
      return;
    }
    else
    {
      cout << "**ERROR** : Got invalid X bin type." << endl;
      exit(EXIT_FAILURE);
    }*/
  }

  // Complicated case of > 1-dimensional binning (ordering necessary)
  else if (m_BinningDimensions > 1)
  {
    /*std::vector<size_t> order;
    std::vector<std::pair<std::string,std::string> >::iterator itr;

    // Test x-Dimension
    if (m_PerfParamMap.find(itr_begin->second->GetXaxis()->GetTitle()) == m_PerfParamMap.end()) {
      cout << "**ERROR** : Got invalid X bin type." << endl;
      exit(EXIT_FAILURE);
    }
    else
    {
      // Loop over binning parameters
      for(itr=m_BinningParam.begin(); itr!=m_BinningParam.end(); ++itr)
      {
        if(itr->first==m_PerfParamMap[itr_begin->second->GetXaxis()->GetTitle()])
        {
          order.push_back(distance(m_BinningParam.begin(), itr));
          continue;
        }
      }
    }*/

    // Test y-Dimension
    /*if (m_PerfParamMap.find(itr_begin->second->GetYaxis()->GetTitle()) == m_PerfParamMap.end()) {
      cout << "**ERROR** : Got invalid Y bin type." << endl;
      exit(EXIT_FAILURE);
    }
    else
    {
      // Loop over binning parameters
      for(itr=m_BinningParam.begin(); itr!=m_BinningParam.end(); ++itr)
      {
        if(itr->first==m_PerfParamMap[itr_begin->second->GetYaxis()->GetTitle()])
        {
          order.push_back(distance(m_BinningParam.begin(), itr));
          continue;
        }
      }
    }*/

    /*if (m_BinningDimensions > 2)
    {
      // Test z-Dimension
      if (m_PerfParamMap.find(itr_begin->second->GetZaxis()->GetTitle()) == m_PerfParamMap.end()) {
        cout << "**ERROR** : Got invalid Z bin type." << endl;
        exit(EXIT_FAILURE);
      }
      else
      {
        // Loop over binning parameters
        for(itr=m_BinningParam.begin(); itr!=m_BinningParam.end(); ++itr)
        {
          if(itr->first==m_PerfParamMap[itr_begin->second->GetZaxis()->GetTitle()])
          {
            order.push_back(distance(m_BinningParam.begin(), itr));
            continue;
          }
        }
      }
    }*/

    cout<<"Before ReOrdering..."<<endl;
    for(itr=m_BinningParam.begin(); itr!=m_BinningParam.end(); ++itr)
    {
      cout<<itr->first<<'\t'<<itr->second<<endl;
    }

    // Re-order vector such that indicies {0,1,2} correspond to dimensions {x,y,z}
    this->ReOrderVector(m_BinningParam, order);

    cout<<"After ReOrdering..."<<endl;
    unsigned int index_ = 0;
    for(itr=m_BinningParam.begin(); itr!=m_BinningParam.end(); ++itr)
    {
      if (itr->first == "nTRACKS") {
        m_indexNTracks = index_;
      }
      if (itr->first == "nSPDHITS") {
        m_indexNSPDHits = index_;
      }
      if (itr->first == "nVeloClusters") {
        m_indexNVeloClusters = index_;
      }
      cout<<itr->first<<'\t'<<itr->second<<endl;
      index_++;
    }
    cout<<" Index nTracks = "<< m_indexNTracks <<endl;
    cout<<" Index nSPDHits = "<< m_indexNSPDHits <<endl;
    cout<<" Index nVeloClusters = "<< m_indexNVeloClusters <<endl;
  }

  m_BinningVectorSorted = true;

}


//=============================================================================
// Set addresses of kinematic variable branches to void pointers in m_TrackEffMap
//=============================================================================
void MultiTrackCalibTool::SetTrackKinVarBranchAddressInInputTree()
{
  // Check the binning parameter vector has been sorted
  if(!m_BinningVectorSorted)
    this->ReOrderBinningVector();

  // Assigning the Branch Address for SWeight variable
  if(m_sWeight_name!="") {
    std::string VarTypeSWeight = getBranchType(m_InputRefTree, m_sWeight_name);
    if (VarTypeSWeight.compare("Float_t")==0) {
      m_sWeight_var = new Float_t();
    }
    else if (VarTypeSWeight.compare("Double_t")==0) {
      m_sWeight_var = new Double_t();
    }
    else if (VarTypeSWeight.compare("Int_t")==0) {
      m_sWeight_var = new Int_t();
    }
    m_InputRefTree->SetBranchAddress(TString(m_sWeight_name).Data(), m_sWeight_var);
  }

  // Loop over all tracks
  std::map<std::string, TH1*>::iterator itr_trk;
  for(itr_trk=m_TrackEffMap.begin(); itr_trk!=m_TrackEffMap.end(); ++itr_trk)
  {
    // Insert key with an empty value (vector<void*>()) into m_TrackVars
    m_TrackVars.insert( std::pair<std::string, std::vector<void*> > (itr_trk->first, std::vector<void*>() ) );
    // Insert key with an empty value (TrkPIDParams()) into m_TrackPIDVals
    m_TrackPIDVals.insert( std::pair<std::string, TrkPIDParams> (itr_trk->first, TrkPIDParams() ) );

    // Get iterator to element just inserted into m_TrackVars
    std::map<std::string, std::vector<void*> >::iterator itr_trkVar =  m_TrackVars.find(itr_trk->first);

    // Loop over all binning variables
    std::vector<std::pair<std::string,std::string> >::iterator itr_binVar;
    for(itr_binVar=m_BinningParam.begin(); itr_binVar!=m_BinningParam.end(); ++itr_binVar)
    {
      // Construct variable name
      std::string var_string = (itr_binVar->first=="nTRACKS" || itr_binVar->first=="nSPDHITS" || itr_binVar->first=="nVeloClusters") ?
        itr_binVar->second : itr_trk->first+"_"+itr_binVar->second;

      if ((itr_trk!=m_TrackEffMap.begin())&&(itr_binVar->first=="nTRACKS")){
         itr_trkVar->second.push_back(m_TrackVars.begin()->second.at(m_indexNTracks));
      }
      else if ((itr_trk!=m_TrackEffMap.begin())&&(itr_binVar->first=="nSPDHITS")){
         itr_trkVar->second.push_back(m_TrackVars.begin()->second.at(m_indexNSPDHits));
      }
      else if ((itr_trk!=m_TrackEffMap.begin())&&(itr_binVar->first=="nVeloClusters")){
         itr_trkVar->second.push_back(m_TrackVars.begin()->second.at(m_indexNVeloClusters));
      }
      else {
        // Dynamically assign appropriate type to void pointer
        std::string VarType = getBranchType(m_InputRefTree, var_string);
        if (VarType.compare("Float_t")==0) {
          itr_trkVar->second.push_back(new Float_t());
        }
        else if (VarType.compare("Double_t")==0) {
          itr_trkVar->second.push_back(new Double_t());
        }
        else if (VarType.compare("Int_t")==0) {
          itr_trkVar->second.push_back(new Int_t());
        }

        // Assign variable to associated branch address in input Tree
        cout<< "Setting address for variable " << var_string
            << " of type " << getBranchType(m_InputRefTree, var_string)
            << endl;

        m_InputRefTree->SetBranchAddress(TString(var_string).Data(),
                                       itr_trkVar->second.back());
     }

    }//Loop over all binning variables

  }// Loop over all tracks

  m_KinVarBranchesSet = true;

}

//=============================================================================
// Append address for track ID variable to vector<void*> for each track
//=============================================================================
void MultiTrackCalibTool::SetTrackIDVarBranchAddressInInputTree(const std::string& IDVar_suffix)
{
  // Check that SetTrackBranchAddressInInputTree() has been run
  if(!m_KinVarBranchesSet)
    this->SetTrackKinVarBranchAddressInInputTree();

  m_IDVar_suffix = IDVar_suffix;

  // Loop over all tracks
  std::map<std::string, std::vector<void*> >::iterator itr_trkVar;
  for(itr_trkVar=m_TrackVars.begin(); itr_trkVar!=m_TrackVars.end(); ++itr_trkVar)
  {
    // Dynamically assign appropriate type to void pointer
    std::string VarType = getBranchType(m_InputRefTree, itr_trkVar->first+"_"+m_IDVar_suffix);
    if (VarType.compare("Float_t")==0) {
      itr_trkVar->second.push_back(new Float_t());
    }
    else if (VarType.compare("Double_t")==0) {
      itr_trkVar->second.push_back(new Double_t());
    }
    else if (VarType.compare("Int_t")==0) {
      itr_trkVar->second.push_back(new Int_t());
    }

    // Assign variable to associated branch address in input Tree
    cout<< "Setting address for variable " << itr_trkVar->first+"_"+m_IDVar_suffix
        << " of type " << getBranchType(m_InputRefTree, itr_trkVar->first+"_"+m_IDVar_suffix)
        << endl;

    m_InputRefTree->SetBranchAddress(TString(itr_trkVar->first+"_"+m_IDVar_suffix).Data(),
                                     itr_trkVar->second.back());

  }// Loop over all tracks

  m_IDVarBranchesSet = true;

}

//=============================================================================
// Create the branches to hold the per event PID effs, errs and bin IDs for
// each individual track
//=============================================================================
void MultiTrackCalibTool::CreateTrkPIDEffBranches()
{
  //cout<<"In MultiTrackCalibTool::CreateTrkPIDEffBranches()"<<endl;

  // Loop over all tracks
  std::map<std::string, TrkPIDParams >::iterator itr_trk;
  for(itr_trk=m_TrackPIDVals.begin(); itr_trk!=m_TrackPIDVals.end(); ++itr_trk)
  {

    TBranch * trk_PIDEffBranch = 0;
    trk_PIDEffBranch = m_OutputTree->Branch(TString(itr_trk->first+"_PIDCalibEff").Data(),
                                            &itr_trk->second.TrkPIDEff,
                                            TString(itr_trk->first+"_PIDCalibEff/F").Data());
    if (!trk_PIDEffBranch) {
      std::cout << "ERROR: Failed to create PID efficiency track branch"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    //m_OutputBranchList.Add(trk_PIDEffBranch);

    TBranch * trk_PIDErrBranch = 0;
    trk_PIDErrBranch = m_OutputTree->Branch(TString(itr_trk->first+"_PIDCalibEffError").Data(),
                                            &itr_trk->second.TrkPIDErr,
                                            TString(itr_trk->first+"_PIDCalibEffError/F").Data());

    if (!trk_PIDErrBranch) {
      std::cout << "ERROR: Failed to create PID efficiency error track branch"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    //m_OutputBranchList.Add(trk_PIDErrBranch);

    TBranch * trk_PIDEffWeightBranch = 0;
    trk_PIDEffWeightBranch = m_OutputTree->Branch(TString(itr_trk->first+"_PIDCalibEffWeight").Data(),
                                            &itr_trk->second.TrkPIDEffWeight,
                                            TString(itr_trk->first+"_PIDCalibEffWeight/F").Data());
    if (!trk_PIDEffWeightBranch) {
      std::cout << "ERROR: Failed to create PID efficiency weight track branch"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    //m_OutputBranchList.Add(trk_PIDEffWeightBranch);

    TBranch * trk_PIDErrWeightBranch = 0;
    trk_PIDErrWeightBranch = m_OutputTree->Branch(TString(itr_trk->first+"_PIDCalibEffErrorWeight").Data(),
                                            &itr_trk->second.TrkPIDErrWeight,
                                            TString(itr_trk->first+"_PIDCalibEffErrorWeight/F").Data());

    if (!trk_PIDErrWeightBranch) {
      std::cout << "ERROR: Failed to create PID efficiency error weight track branch"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    //m_OutputBranchList.Add(trk_PIDErrWeightBranch);

    TBranch * trk_BinNumBranch = 0;
    trk_BinNumBranch = m_OutputTree->Branch(TString(itr_trk->first+"_PIDCalibBinNumber").Data(),
                                            &itr_trk->second.TrkBinNumber,
                                            TString(itr_trk->first+"_PIDCalibBinNumber/I").Data());

    if (!trk_BinNumBranch) {
      std::cout << "ERROR: Failed to create bin number track branch"
                << std::endl;
      exit(EXIT_FAILURE);
    }
    //m_OutputBranchList.Add(trk_BinNumBranch);

  }// Loop over all tracks

  this->CreateTotPIDEffBranches(m_OutputTree);

}

//=============================================================================
//  Create the branches to hold the overall per event PID effs and errs
//=============================================================================
void MultiTrackCalibTool::CreateTotPIDEffBranches(TTree* tt)
{
  //cout<<"In MultiTrackCalibTool::CreateTotPIDEffBranches()"<<endl;

  TBranch * evt_PIDEffBranch = 0;
  evt_PIDEffBranch = tt->Branch("Event_PIDCalibEff",
                                &m_TotPIDEff,
                                "Event_PIDCalibEff/F");
  if (!evt_PIDEffBranch) {
    std::cout << "ERROR: Failed to create PID efficiency event branch"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  //m_OutputBranchList.Add(evt_PIDEffBranch);

  TBranch *evt_PIDErrBranch = 0;
  evt_PIDErrBranch = tt->Branch("Event_PIDCalibErr",
                                &m_TotPIDErr,
                                "Event_PIDCalibErr/F");

  if (!evt_PIDErrBranch) {
    std::cout << "ERROR: Failed to create PID efficiency error event branch"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  //m_OutputBranchList.Add(evt_PIDErrBranch);

  TBranch * evt_PIDEffWeightBranch = 0;
  evt_PIDEffWeightBranch = tt->Branch("Event_PIDCalibEffWeight",
                                &m_TotPIDEffWeight,
                                "Event_PIDCalibEffWeight/F");
  if (!evt_PIDEffWeightBranch) {
    std::cout << "ERROR: Failed to create PID weighted efficiency event branch"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  //m_OutputBranchList.Add(evt_PIDEffWeightBranch);

  TBranch *evt_PIDErrWeightBranch = 0;
  evt_PIDErrWeightBranch = tt->Branch("Event_PIDCalibErrWeight",
                                &m_TotPIDErrWeight,
                                "Event_PIDCalibErrWeight/F");

  if (!evt_PIDErrWeightBranch) {
    std::cout << "ERROR: Failed to create PID weighted efficiency error event branch"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  //m_OutputBranchList.Add(evt_PIDErrWeightBranch);

  TBranch *evt_WeightBranch = 0;
  evt_WeightBranch = tt->Branch("Event_Weight",
                                &m_TotWeight,
                                "Event_Weight/F");

  if (!evt_WeightBranch) {
    std::cout << "ERROR: Failed to create Weight event branch"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  //m_OutputBranchList.Add(evt_WeightBranch);

}

//=============================================================================
// Calcualte the individual track event-by-event efficiencies and associated
// errors. Simple errors calculed with NO statisical correlations considerd
//=============================================================================
void MultiTrackCalibTool::Calculate()
{
  cout<<"MultiTrackCalibTool::Calculate(): running" << endl;
  // Confirm performance histograms are valid
  this->CheckSanityOfPerfHists();
  cout<<"MultiTrackCalibTool::Calculate(): sane" << endl;

  // Obtain variable addresses in reference sample tree associated with the kinematic binning
  this->SetTrackKinVarBranchAddressInInputTree();
  cout<<"MultiTrackCalibTool::Calculate(): kin vars set" << endl;

  // Create the PID value branches in the output tree
  this->CreateTrkPIDEffBranches();
  cout<<"MultiTrackCalibTool::Calculate(): eff branches created" << endl;

  // Loop over all reference events
  this->Loop();
  cout<<"MultiTrackCalibTool::Calculate(): looped" << endl;

  // Delete dynamically declared kinematic variables
  this->DeleteHeapMemVars();
  cout<<"MultiTrackCalibTool::Calculate(): deleted" << endl;
}
//=============================================================================
// Calcualte the individual track event-by-event efficiencies and associated
// errors. Errors calculated consider cross terms amongst tracks of the same
// species (100% correlations assummed).
//=============================================================================
void MultiTrackCalibTool::Calculate(const std::string& IDVar_suffix)
{
  // Confirm performance histograms are valid
  this->CheckSanityOfPerfHists();

  // Obtain variable addresses in reference sample tree associated with the kinematic binning
  this->SetTrackKinVarBranchAddressInInputTree();

  // Assign the suffix associated with the ID variable for all tracks in the reference sample
  this->SetTrackIDVarBranchAddressInInputTree(IDVar_suffix);

  // Create the PID value branches in the output tree
  this->CreateTrkPIDEffBranches();

  // Loop over all reference events
  this->Loop();

  // Delete dynamically declared kinematic variables
  this->DeleteHeapMemVars();
}

//=============================================================================
// Once m_OutputTree has been filled, remove the dynamically allocated memory
// in m_TrackVars
//=============================================================================
void MultiTrackCalibTool::DeleteHeapMemVars()
{
  if(!m_BinningVectorSorted)
    exit(EXIT_FAILURE);

  // Loop over all tracks
  std::map<std::string, std::vector<void*> >::iterator itr_trk;
  for(itr_trk=m_TrackVars.begin(); itr_trk!=m_TrackVars.end(); ++itr_trk)
  {
    
    /*std::vector<void*>::iterator itr_v;
    for(itr_v=itr_trk->second.begin(); itr_v!=itr_trk->second.end(); ++itr_v)
    {
      // use global operate delete on a void pointer
       ::operator delete(*itr_v);
    }*/
    itr_trk->second.clear();

    // Loop over all kinematic variables
    std::vector<void*>::iterator itr_v;
    for(itr_v=itr_trk->second.begin(); itr_v!=itr_trk->second.end(); ++itr_v)
    {

      if ( (itr_trk!=m_TrackVars.begin())&&
          (m_BinningParam.at(distance(itr_trk->second.begin(),itr_v)).first=="nTRACKS" || 
           m_BinningParam.at(distance(itr_trk->second.begin(),itr_v)).first=="nSPDHITS" ||
           m_BinningParam.at(distance(itr_trk->second.begin(),itr_v)).first=="nVeloClusters") )
      {
         continue;
      }

      std::string branch_name = this->GetKinVarBranchName(itr_trk->first,
                                                          m_BinningParam.at(distance(itr_trk->second.begin(),
                                                                                     itr_v)).first,
                                                          m_BinningParam.at(distance(itr_trk->second.begin(),
                                                                                     itr_v)).second);
      std::string type=getBranchType(m_InputRefTree, branch_name);

      if (type.compare("Float_t")==0) {
        Float_t* ptr=static_cast<Float_t*>(*itr_v);
        assert(ptr);
        delete ptr;
      }
      else if (type.compare("Double_t")==0) {
        Double_t* ptr = static_cast<Double_t*>(*itr_v);
        assert(ptr);
        delete ptr;
      }
      else if (type.compare("Int_t")==0) {
        Int_t* ptr = static_cast<Int_t*>(*itr_v);
        assert(ptr);
        delete ptr;
      }
    }// Loop over all tracks

    itr_trk->second.clear();

  }// Loop over all kinematic variables

}


//=============================================================================
// For every reference event, loop over all tracks being considered
//=============================================================================
void MultiTrackCalibTool::Loop()
{
  // Loop over reference track events
  Long64_t nentries = !m_EntryList?m_InputRefTree->GetEntries():m_EntryList->GetN();
  cout<<"Number of entries: "<<nentries<<endl;
  for(UInt_t i = 0; i<nentries; i++)
  {
    Long64_t entryNumber = m_InputRefTree->GetEntryNumber(i);
    m_InputRefTree->GetEntry(entryNumber);

    Bool_t printEntry = m_verbose&&m_printFreq!=0&&((m_printFreq<0)||(i%m_printFreq==0));
    if (printEntry) cout << "**** Processing entry number " << i << " ****" << endl;

    // Get the SWeight
    m_TotWeight = 1;
    if(m_sWeight_name!="") {
      m_TotWeight = CastVoidPointerToDouble(m_sWeight_var, m_InputRefTree, m_sWeight_name);
      if (printEntry) cout << "m_TotWeight " << m_TotWeight << std::endl;
    }

    m_TotPIDEff = 1;
    m_TotPIDEffWeight = m_TotWeight;

    // Loop over all tracks
    std::map<std::string, std::vector<void*> >::iterator itr_trk;
    cout.setf(std::ios::fixed, std::ios::floatfield);
    cout.precision(3);
    for(itr_trk=m_TrackVars.begin(); itr_trk!=m_TrackVars.end(); ++itr_trk)
    {
      // Get the global bin number of the historgram that this event lies in
      Int_t track_bin = -100;

      std::string trkname = itr_trk->first;

      std::string xvarname = this->GetKinVarBranchName(trkname,
                                                       m_BinningParam.at(0).first,
                                                       m_BinningParam.at(0).second);
      Double_t xvar = CastVoidPointerToDouble(itr_trk->second.at(0),
                                              m_InputRefTree,
                                              xvarname);
      if (printEntry) cout << "Track " << trkname << " - " << xvarname << " = " << xvar;

      if (m_BinningDimensions==1) {
        track_bin = (m_TrackEffMap[itr_trk->first])->FindBin(xvar);
      }
      else {
        std::string yvarname = this->GetKinVarBranchName(trkname,
                                                         m_BinningParam.at(1).first,
                                                         m_BinningParam.at(1).second);

        Double_t yvar = CastVoidPointerToDouble(itr_trk->second.at(1),
                                                m_InputRefTree,
                                                yvarname);
        if (printEntry) {
        cout << ", " << yvarname << " = " << yvar;
        }
        if (m_BinningDimensions==2) {
          track_bin = (m_TrackEffMap[itr_trk->first])->FindBin(xvar,yvar);
        }
        else {
          std::string zvarname = this->GetKinVarBranchName(trkname,
                                                           m_BinningParam.at(2).first,
                                                           m_BinningParam.at(2).second);
    	  
          Double_t zvar = CastVoidPointerToDouble(itr_trk->second.at(2),
                                                  m_InputRefTree,
                                                  zvarname);
          if (printEntry) {
            cout << ", " << zvarname << " = " << zvar;
          }
          track_bin = (m_TrackEffMap[itr_trk->first])->FindBin(xvar,yvar,zvar);
        }
      }

      if (printEntry) cout << ": global bin number = " << track_bin << endl;

      //set the efficiency and error from this global bin number
      m_TrackPIDVals[itr_trk->first].TrkPIDEff    = m_TrackEffMap[itr_trk->first]->GetBinContent(track_bin);
      m_TrackPIDVals[itr_trk->first].TrkPIDErr    = m_TrackEffMap[itr_trk->first]->GetBinError(track_bin);

      m_TrackPIDVals[itr_trk->first].TrkPIDEffWeight    = m_TotWeight*m_TrackPIDVals[itr_trk->first].TrkPIDEff;
      m_TrackPIDVals[itr_trk->first].TrkPIDErrWeight    = m_TotWeight*m_TrackPIDVals[itr_trk->first].TrkPIDErr;

      m_TrackPIDVals[itr_trk->first].TrkBinNumber = track_bin;

      // Determine if this is a underflow or overflow bin. Not possible
      // if SetRefDataInPerfHistLimits() has been run prior to this!

      Int_t binX=-100, binY=-100, binZ=-100;
      m_TrackEffMap[itr_trk->first]->GetBinXYZ(track_bin, binX, binY,binZ);

      Int_t nX = m_TrackEffMap[itr_trk->first]->GetNbinsX(), nY =-100, nZ =-100;
      if (m_BinningDimensions==2) nY = m_TrackEffMap[itr_trk->first]->GetNbinsY();
      if (m_BinningDimensions==3) nZ = m_TrackEffMap[itr_trk->first]->GetNbinsZ();

      // Is this an underflow bin?
      if( (m_BinningDimensions==3 && binX*binY*binZ==0)||
          (m_BinningDimensions==2 && binX*binY==0)||
          (m_BinningDimensions==1 && binX==0) )
      {
        std::cout << "WARNING: Track in underflow bin. "
                  << "Consider using MultiTrackCalibTool::SetRefDataInPerfHistLimits()"
                  << std::endl;
        exit(EXIT_FAILURE);
      }
      // Is this an overflow bin?
      if (binX==(nX+1)||
          binY==(nY+1)||
          binZ==(nZ+1))
      {
        std::cout << "WARNING: Track in overflow bin. "
                  << "Consider using MultiTrackCalibTool::SetRefDataInPerfHistLimits()"
                  << std::endl;
        exit(EXIT_FAILURE);
      }

      // Append track efficiency to running event efficiency
      m_TotPIDEff *= m_TrackPIDVals[itr_trk->first].TrkPIDEff;
      m_TotPIDEffWeight *= m_TrackPIDVals[itr_trk->first].TrkPIDEff;
      // If ID variable declared, assign ID, else assign zero
      Int_t ID = (m_IDVarBranchesSet) ? CastVoidPointerToInt(itr_trk->second.at(m_BinningDimensions),
                                                             m_InputRefTree,
                                                             itr_trk->first+"_"+m_IDVar_suffix) : 0;

      // Ensure the error-over-eff fraction is physical
      Float_t ErrOverEff = 0;
      if(m_TrackPIDVals[itr_trk->first].TrkPIDEff!=0 && m_TrackPIDVals[itr_trk->first].TrkPIDErr!=0)
        ErrOverEff = m_TrackPIDVals[itr_trk->first].TrkPIDErr/m_TrackPIDVals[itr_trk->first].TrkPIDEff;

      // Push back this fraction along with the ID value of this track
      m_vTrckIDandFracErrs.push_back( std::make_pair(ID,
                                                     ErrOverEff) );
      // Ensure the error-over-eff fraction is physical Weight
      Float_t ErrOverEffWeight = 0;
      if(m_TrackPIDVals[itr_trk->first].TrkPIDEffWeight!=0 && m_TrackPIDVals[itr_trk->first].TrkPIDErrWeight!=0)
        ErrOverEffWeight = m_TrackPIDVals[itr_trk->first].TrkPIDErrWeight/m_TrackPIDVals[itr_trk->first].TrkPIDEffWeight;

      // Push back this fraction along with the ID value of this track for Weight
      m_vTrckIDandFracErrsWeight.push_back( std::make_pair(ID,
                                                     ErrOverEffWeight) );
      /*
      cout<<i
          <<'\t'<<CastVoidPointerToInt(itr_trk->second.at(m_BinningDimensions),
                                       m_InputRefTree,
                                       itr_trk->first+"_"+IDVar_suffix)
          <<'\t'<<m_TrackPIDVals[itr_trk->first].TrkPIDEff
          <<'\t'<<m_TrackPIDVals[itr_trk->first].TrkPIDErr
          <<'\t'<<m_TrackPIDVals[itr_trk->first].TrkPIDEffWeight
          <<'\t'<<m_TrackPIDVals[itr_trk->first].TrkPIDErrWeight
          <<'\t'<<m_TrackPIDVals[itr_trk->first].TrkBinNumber
          <<'\t'<<ErrOverEff
          <<endl;
      */

    }// Loop over all tracks

    // Calculate event efficiency
    m_TotPIDErr = fabs(m_TotPIDEff*this->CalcEventFracError(m_vTrckIDandFracErrs));
    m_TotPIDErrWeight = fabs(m_TotPIDEff*this->CalcEventFracError(m_vTrckIDandFracErrsWeight));

    //cout<<m_TotPIDEff<<'\t'<<m_TotPIDErr<<endl;

    m_NaiveAverageCounters.first += m_TotPIDEff;
    m_NaiveAverageCounters.second += pow(m_TotPIDErr,2);

    m_NaiveAverageWeightCounters.first += m_TotPIDEffWeight;
    m_NaiveAverageWeightCounters.second += pow(m_TotPIDErrWeight,2);

    //m_WeightedAverageCounters.first += m_TotPIDEff/pow(m_TotPIDErr,2);
    //m_WeightedAverageCounters.second += 1/pow(m_TotPIDErr,2);

    // Write to output tree
    m_OutputTree->Fill();

    // Clear the current contents of vTrckIDandFracErrs
    m_vTrckIDandFracErrs.clear();
    m_vTrckIDandFracErrsWeight.clear();

  }// Loop over events

  m_OutputTree->ResetBranchAddresses();
}


//=============================================================================
// Cast void pointer to appropriate type, and then return as a double
//=============================================================================
double MultiTrackCalibTool::CastVoidPointerToDouble(void *val, TTree* tt, std::string branchName)
{
  double v = 0;
  std::string type=getBranchType(tt, branchName);
  if (type.compare("Float_t")==0) {
    Float_t* ptr=static_cast<Float_t*>(val);
    assert(ptr);
    v=(double)(*ptr);
  }
  else if (type.compare("Double_t")==0) {
    Double_t* ptr = static_cast<Double_t*>(val);
    assert(ptr);
    v=(double)(*ptr);
  }
  else if (type.compare("Int_t")==0) {
    Int_t* ptr = static_cast<Int_t*>(val);
    assert(ptr);
    v=(double)(*ptr);
  }

  return v;
}

//=============================================================================
// Cast void pointer to appropriate type, and then return as a double
//=============================================================================
int MultiTrackCalibTool::CastVoidPointerToInt(void *val, TTree* tt, std::string branchName)
{
  int v = 0;
  std::string type=getBranchType(tt, branchName);
  if (type.compare("Float_t")==0) {
    Float_t* ptr=static_cast<Float_t*>(val);
    assert(ptr);
    v=(int)(*ptr);
  }
  else if (type.compare("Double_t")==0) {
    Double_t* ptr = static_cast<Double_t*>(val);
    assert(ptr);
    v=(int)(*ptr);
  }
  else if (type.compare("Int_t")==0) {
    Int_t* ptr = static_cast<Int_t*>(val);
    assert(ptr);
    v=(int)(*ptr);
  }

  return v;
}

//=============================================================================
// Ensure the reference sample tracks reside with the bin ranges of the
// declared PID performance histograms
//=============================================================================
void MultiTrackCalibTool::SetRefDataInPerfHistLimits()
{
  // Check the binning parameter vector has been sorted
  if(!m_BinningVectorSorted)
    this->ReOrderBinningVector();

  // string to hold full set of cuts
  std::string cuts = "";

  // Loop over all tracks
  std::map<std::string, TH1*>::iterator itr_trk;
  for(itr_trk=m_TrackEffMap.begin(); itr_trk!=m_TrackEffMap.end(); ++itr_trk)
  {
    // Loop over binning parameters
    std::vector<std::pair<std::string,std::string> >::iterator itr_par;
    for(itr_par=m_BinningParam.begin(); distance(m_BinningParam.begin(), itr_par) < m_BinningDimensions; ++itr_par)
      {
        std::string var_string = GetKinVarBranchName(itr_trk->first,
                                                     itr_par->first,
                                                     itr_par->second);

        double RangeMin, RangeMax;
        if(distance(m_BinningParam.begin(), itr_par)==0)
        {
          RangeMin = itr_trk->second->GetXaxis()->GetXmin();
          RangeMax = itr_trk->second->GetXaxis()->GetXmax();
        }
        else if(distance(m_BinningParam.begin(), itr_par)==1)
        {
          RangeMin = itr_trk->second->GetYaxis()->GetXmin();
          RangeMax = itr_trk->second->GetYaxis()->GetXmax();
        }
        else if(distance(m_BinningParam.begin(), itr_par)==2)
        {
          RangeMin = itr_trk->second->GetZaxis()->GetXmin();
          RangeMax = itr_trk->second->GetZaxis()->GetXmax();
        }
        else
        {
          cout<<"ERROR : Unphysical binning dimensions"<<endl;
          exit(EXIT_FAILURE);
        }

    
		//Check if the variable cut has already been added (don't append if it has)
		std::string::size_type pos = cuts.find(var_string);
		if(pos == std::string::npos){
		
			if(cuts!="") cuts += " && ";
        	cuts +=
          	boost::lexical_cast<std::string>(RangeMin)
          	+ " <= "
          	+ var_string
          	+ " && "
          	+ boost::lexical_cast<std::string>(RangeMax)
          	+ " > "
          	+ var_string;
		}
		
      }// Loop over binning parameters

  }// Loop over all tracks

  assert(cuts!="");
  cout<<cuts<<endl;
  cout<<m_InputRefTree<<endl;

  // Get TEntryList from reference tree
  TString entry_list_name, entry_list_draw_command(">>");
  entry_list_name.Form("elist_%s",m_name.c_str());
  entry_list_draw_command += entry_list_name;
  cout<<entry_list_name.Data()<<" "<<entry_list_draw_command.Data()<<endl;
  
  m_InputRefTree->Draw(entry_list_draw_command.Data(), cuts.c_str(), "entryList");
  m_EntryList = (TEntryList*)gDirectory->Get(entry_list_name.Data());
  cout<<m_EntryList<<endl;

  if (!m_EntryList)
  {
    cout << "Failed to retrieve TEntryList for cuts " << cuts << endl;
    exit(EXIT_FAILURE);
  }

  // Set EntryList for reference tree
  m_InputRefTree->SetEntryList(m_EntryList);

}

//=============================================================================
// Ensure that none of the performance histograms possess bins with
// efficiencies >1 or <0
//=============================================================================
void MultiTrackCalibTool::CheckSanityOfPerfHists()
{
  // Loop over all perf histograms
  std::map<std::string, TH1*>::iterator itr_m;
  for(itr_m=m_TrackEffMap.begin(); itr_m!=m_TrackEffMap.end(); ++itr_m)
  {
    cout << itr_m->second << endl;
    double val = itr_m->second->GetBinContent(itr_m->second->GetMaximumBin());
    double bin_num = itr_m->second->GetMaximumBin();
    double low_edge_x = itr_m->second->GetXaxis()->GetBinLowEdge(bin_num);
    double low_edge_y = 0;
    double low_edge_z = 0;
    
    if(m_BinningDimensions > 1){
      low_edge_y = itr_m->second->GetYaxis()->GetBinLowEdge(bin_num);
    }
    if(m_BinningDimensions > 2){
      low_edge_z = itr_m->second->GetZaxis()->GetBinLowEdge(bin_num);
    }

    if(val>1)
    {
      if(m_BinningDimensions == 1){
         cout<<"WARNING: Bin "<<bin_num<<" with low edge ("<<low_edge_x<<") in histogram "<<itr_m->second->GetName()
          <<" with value > 1 : "<<val<<endl;
      }
      else if(m_BinningDimensions == 2){
        cout<<"WARNING: Bin "<<bin_num<<" with low edge ("<<low_edge_x<<","<<low_edge_y<<") in histogram "<<itr_m->second->GetName()
          <<" with value > 1 : "<<val<<endl;
      }
      else if(m_BinningDimensions == 3){
        cout<<"WARNING: Bin "<<bin_num<<" with low edge ("<<low_edge_x<<","<<low_edge_y<<","<<low_edge_z<<") in histogram "<<itr_m->second->GetName()
          <<" with value > 1 : "<<val<<endl;
      }
      
      //exit(EXIT_FAILURE);
    }
    
    val = itr_m->second->GetBinContent(itr_m->second->GetMinimumBin());
    bin_num = itr_m->second->GetMinimumBin();
    low_edge_x = itr_m->second->GetXaxis()->GetBinLowEdge(bin_num);
    low_edge_y = 0;
    low_edge_z = 0;

    if(m_BinningDimensions > 1){
      low_edge_y = itr_m->second->GetYaxis()->GetBinLowEdge(bin_num);
    }
    if(m_BinningDimensions > 2){
      low_edge_z = itr_m->second->GetZaxis()->GetBinLowEdge(bin_num);
    }

    if(val<0)
    {
      if(m_BinningDimensions == 1){
         cout<<"WARNING: Bin "<<bin_num<<" with low edge ("<<low_edge_x<<") in histogram "<<itr_m->second->GetName()
          <<" with value < 1 : "<<val<<endl;
      }
      else if(m_BinningDimensions == 2){
        cout<<"WARNING: Bin "<<bin_num<<" with low edge ("<<low_edge_x<<","<<low_edge_y<<") in histogram "<<itr_m->second->GetName()
          <<" with value < 1 : "<<val<<endl;
      }
      else if(m_BinningDimensions == 3){
        cout<<"WARNING: Bin "<<bin_num<<" with low edge ("<<low_edge_x<<","<<low_edge_y<<","<<low_edge_z<<") in histogram "<<itr_m->second->GetName()
          <<" with value < 1 : "<<val<<endl;
      }
      
      //exit(EXIT_FAILURE);
    }
  }// Loop over all perf histograms
}
  

//=============================================================================
// Determine the per event fractional error (i.e. \frac{\sigma}{\eff} )
//=============================================================================
Float_t MultiTrackCalibTool::CalcEventFracError(std::vector<std::pair<Int_t,Float_t> > vTrckIDandFracErrs)
{
  Float_t evt_PIDErr = 0;

  //Loop over entries
  std::vector<std::pair<Int_t,Float_t> >::const_iterator itr;
  for(itr=vTrckIDandFracErrs.begin(); itr!=vTrckIDandFracErrs.end(); ++itr)
  {
    evt_PIDErr += pow(itr->second,2);
  }

  if(m_IDVarBranchesSet)
    evt_PIDErr = sqrt(evt_PIDErr + this->CalcPerEventErrorCrossTerm(vTrckIDandFracErrs));
  else
    evt_PIDErr = sqrt(evt_PIDErr);

  return evt_PIDErr;
}


//=============================================================================
// Determine the per event efficiency error cross term
//=============================================================================
Float_t MultiTrackCalibTool::CalcPerEventErrorCrossTerm(std::vector<std::pair<Int_t,Float_t> > vTrckIDandFracErrs)
{
  Float_t xTerms = 0;

  if(vTrckIDandFracErrs.size()<2)
    return xTerms;

  // 0) Let i=0
  // 1) Compare element i with elements j=i+1,i+2,...,N
  // 2) If i abs(ID) is equivalent to j abs(ID) then append relevant cross term
  // 3) ++i and jump back to 1)

  std::vector<std::pair<Int_t,Float_t> >::const_iterator i,j;

  for(i=vTrckIDandFracErrs.begin(); i!=vTrckIDandFracErrs.end(); ++i)
  {
    for(j=i+1; j!=vTrckIDandFracErrs.end(); ++j)
    {
      xTerms += ( abs(i->first)==abs(j->first) ) ? 2*(i->second)*(j->second) : 0;
    }//j

  }//i

  return xTerms;

}

//=============================================================================
// Calculate the naive event average
//=============================================================================
std::pair<Float_t, Float_t> MultiTrackCalibTool::CalculateNaiveAverage()
{
  Float_t av = m_NaiveAverageCounters.first/m_OutputTree->GetEntries();
  Float_t er = sqrt(m_NaiveAverageCounters.second)/m_OutputTree->GetEntries();

  return std::make_pair(av,er);
}

//=============================================================================
// Calculate the naive event average weighted
//=============================================================================
std::pair<Float_t, Float_t> MultiTrackCalibTool::CalculateNaiveWeightAverage()
{
  Float_t av = m_NaiveAverageWeightCounters.first/m_OutputTree->GetEntries();
  Float_t er = sqrt(m_NaiveAverageWeightCounters.second)/m_OutputTree->GetEntries();

  return std::make_pair(av,er);
}

//=============================================================================
// Get branch name of track kinematic variable
//=============================================================================
std::string MultiTrackCalibTool::GetKinVarBranchName(std::string trkName,
                                                     std::string binVarIntName,
                                                     std::string binVarName)
{
  std::string ret = "";
  if(binVarIntName!="nTRACKS" && binVarIntName!="nSPDHITS" && binVarIntName!="nVeloClusters") ret = trkName+"_"+binVarName; 
  else ret = binVarName;

  return ret;
  
}


//=============================================================================
