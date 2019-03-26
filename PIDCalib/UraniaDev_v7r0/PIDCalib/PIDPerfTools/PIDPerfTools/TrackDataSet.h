// $Id: $
#ifndef PIDPERFTOOLS_TRACKDATASET_H 
#define PIDPERFTOOLS_TRACKDATASET_H 1

// Include files
#include <Rtypes.h>
#include "RooDataSet.h"
#include "RooArgList.h"
#include "RooRealVar.h"
#include "RooBinning.h"
#include <map>
#include <list>
#include <string>

#include "PIDPerfTools/DataBinCuts.h"
#include "PIDPerfTools/PerfCalculator.h"

/** @class TrackDataSet TrackDataSet.h PIDPerfTools/TrackDataSet.h
 *  
 *
 *  @author Andrew Powell (LHCb)04
 *  @date   2010-12-25
 */

class TrackDataSet : public RooDataSet
{
public:
  
  /// Overloaded operators (Necessary for ROOT versions >= v5.30)
  void* operator new(size_t sz);
  void* operator new(size_t sz, void* vp);

  /// Standard constructor
  TrackDataSet( );
  
  TrackDataSet( const std::string& TrackName,
                const std::string& Charge,
                RooDataSet* Data,
                const RooArgSet& vars,
                const std::string& P_Name,
                const std::string& PT_Name,
                const std::string& ETA_Name,
                const std::string& DLLK_Name,
                const std::string& DLLp_Name,
                const std::string& Cuts = "",
                const std::string& WgtVarName = ""
                );

  TrackDataSet( const std::string& TrackName,
                const std::string& Charge,
                TrackDataSet* Data,
                const RooArgSet& vars,
                const std::string& Cuts = "",
                const std::string& WgtVarName = ""
                );

  TrackDataSet( const std::string& name, 
                const std::string& title, 
                TTree* ntuple, 
                const RooArgSet& vars,
                const std::string& P_Name,
                const std::string& PT_Name,
                const std::string& ETA_Name,
                const std::string& DLLK_Name,
                const std::string& DLLp_Name,
                const std::string& cuts = "", 
                const std::string& wgtVarName = ""
                );

  TrackDataSet( const std::string& Name,
                const std::string& Charge,
                TrackDataSet* Numerator,
                TrackDataSet* Denominator,
                const std::string& WgtVar,
                const std::string& Cuts_Num = "",
                const std::string& Cuts_Denom = "",
                const std::string& WgtVar_Num = "", 
                const std::string& WgtVar_Denom = ""
                );  
  
  virtual ~TrackDataSet( ); ///< Destructor

  //========================================================
  // Get track charge
  const TString GetCharge() const;
  //========================================================

  //========================================================
  // Append a TrackDataSet
  void append(TrackDataSet& data);
  //========================================================
  
  //========================================================
  // Operator Overload (Assignment)
  //TrackDataSet& operator=(const TrackDataSet&);
  //========================================================

  //========================================================
  // Get track parameter names 
  virtual const std::string Get_ParamName(const std::string&) const;
  const std::string Get_ParamName_P() const;
  const std::string Get_ParamName_PT() const;
  const std::string Get_ParamName_ETA() const;
  const std::string Get_ParamName_DLLK() const;
  const std::string Get_ParamName_DLLp() const;
  //  const std::string Get_ParamName_DLLpK() const;
  //========================================================  

  //========================================================
  // Get track parameter  
  const RooRealVar* Get_Param(const std::string&) const;
  const RooRealVar* Get_Param_P() const;
  const RooRealVar* Get_Param_PT() const;
  const RooRealVar* Get_Param_ETA() const;
  const RooRealVar* Get_Param_DLLK() const;
  const RooRealVar* Get_Param_DLLp() const;
  //const RooRealVar* Get_Param_DLLpK() const;
  //========================================================

  //========================================================
  // An additional reduce method that takes a vector of
  // RooBinnings and returns all those events within the 
  // global acceptance of the Binning schema
  virtual RooAbsData* SetInBinSchema(const std::vector<RooBinning*>& BinSchema);
  //========================================================
  
  void SplitCut( const std::string& Cut, std::vector< std::string >& SplitVec );
  const std::string FormatCutList( const std::string& Cut );
  const std::string TranslateCut( const std::string& Cut );

protected:
  
  //========================================================
  //Protected constructor for internal use only
  TrackDataSet( const std::string& Name, 
                const std::string& Title, 
                TrackDataSet* Ntuple, 
                const RooArgSet& vars, 
                const RooFormulaVar* cutVar, 
                const std::string& cutRange, 
                int nStart, 
                int nStop, 
                Bool_t copyCache, 
                const std::string& wgtVarName = "" );

  //========================================================
  // Implementation of RooAbsData virtual method that drives 
  // the RooAbsData::reduce() methods
  RooAbsData* reduceEng(const RooArgSet& varSubset, 
                        const RooFormulaVar* cutVar, 
                        const std::string& cutRange=0,
                        Int_t nStart=0, 
                        Int_t nStop=2000000000, 
                        Bool_t copyCache=kTRUE);
  //========================================================

  //========================================================
  // Define track variable
  void SetTrackVar( const std::string& ,
                    std::string);
  //========================================================

  //========================================================
  // Define track variable DLLpK
  /* 
 void SetTrackVar_DLLpK( const std::string& ,
                          const std::string& ,
                          std::string);
  */ 
 //========================================================

  // Declare a friend class
	template <class T> friend class PerfCalculator;

private:
 
  //========================================================
  // STL Map
  // Mapping between DataSet variables and track parameters
  std::map< std::string, RooRealVar* > m_ParamMap;
  //========================================================
 
  //========================================================
  // Track charge propery
  const std::string m_Charge;
  //========================================================

public:

  ClassDef(TrackDataSet,1);

};
#endif // PIDPERFTOOLS_TRACKDATASET_H
