// $Id: $
// Include files
#include <algorithm>
#include <iomanip>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/foreach.hpp>
#include "RooErrorHandler.h"

// local
#include "PIDPerfTools/TrackDataSet.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackDataSet
//
// 2010-12-25 : Andrew Powell (LHCb)04
//-----------------------------------------------------------------------------

//ClassImp(TrackDataSet)

using std::cout;
using std::endl;

namespace PIDCalib 
{
  struct length 
  {
    bool operator() ( const std::string& a, const std::string& b )
    {
      return a.size() < b.size();   
    } 
  };

  struct strToDouble
  {
    double operator() ( const std::string& s )
    {
      return boost::lexical_cast<double>(s);
    }
  };
}

//=============================================================================
// Overloaded operators
//=============================================================================
void* TrackDataSet::operator new (size_t bytes)
{
  return RooDataSet::operator new(bytes);
}

void* TrackDataSet::operator new (size_t bytes, void *p)
{
  return TObject::operator new(bytes, p);
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackDataSet::TrackDataSet( )
  : RooDataSet( )
{

}

TrackDataSet::TrackDataSet( const std::string& TrackName,
                            const std::string& Charge,
                            RooDataSet* Data,
                            const RooArgSet& vars,
                            const std::string& P_Var,
                            const std::string& PT_Var,
                            const std::string& ETA_Var,
                            const std::string& DLLK_Var,
                            const std::string& DLLp_Var,
                            const std::string& Cuts,
                            const std::string& WgtVarName
                            ) 
  : RooDataSet( TrackName.c_str(),
                Data->GetTitle(),
                Data,
                vars,
                NULL,
                WgtVarName.empty() ? NULL : WgtVarName.c_str() ),
    m_Charge( Charge )
    
{
  SetTrackVar( P_Var,
               "P" );
  SetTrackVar( PT_Var,
               "PT" );
  SetTrackVar( ETA_Var,
               "ETA" );
  SetTrackVar( DLLK_Var,
               "DLLK" );
  SetTrackVar( DLLp_Var,
               "DLLp" );
  //  SetTrackVar_DLLpK( DLLp_Var,
  //                   DLLK_Var,
  //                   "DLLpK" );
  
  if (not Cuts.empty()) {
    _dstore = this->reduce(FormatCutList(Cuts).c_str())->store();
  }
  
}



TrackDataSet::TrackDataSet( const std::string& TrackName,
                            const std::string& Charge,
                            TrackDataSet* Data,
                            const RooArgSet& vars,
                            const std::string& Cuts ,
                            const std::string& WgtVarName
                            )
  : RooDataSet( TrackName.c_str(),
                Data->GetTitle(),
                Data,
                vars,
                NULL,
                WgtVarName.empty() ? NULL : WgtVarName.c_str() ),
    m_Charge( Charge )
{
  SetTrackVar( Data->Get_ParamName_P(),
               "P" );
  SetTrackVar( Data->Get_ParamName_PT(),
               "PT" );
  SetTrackVar( Data->Get_ParamName_ETA(),
               "ETA" );
  SetTrackVar( Data->Get_ParamName_DLLK(),
               "DLLK" );
  SetTrackVar( Data->Get_ParamName_DLLp(),
               "DLLp" ); 
  //SetTrackVar( Data->Get_ParamName_DLLpK(),
  //            "DLLpK" );
  
  if (not Cuts.empty()) {
    _dstore = this->reduce(FormatCutList(Cuts).c_str())->store();
  }
}


TrackDataSet::TrackDataSet( const std::string& TrackName, 
                            const std::string& Charge, 
                            TTree* Ntuple, 
                            const RooArgSet& Vars,
                            const std::string& P_Var,
                            const std::string& PT_Var,
                            const std::string& ETA_Var,
                            const std::string& DLLK_Var,
                            const std::string& DLLp_Var,
                            const std::string& Cuts, 
                            const std::string& WgtVarName
                            )
  : RooDataSet( TrackName.c_str(), 
                Charge.c_str(), 
                Ntuple, 
                Vars, 
                NULL, 
                WgtVarName.empty() ? NULL : WgtVarName.c_str() ),
    m_Charge( Charge ) 
{
  SetTrackVar( P_Var,
               "P" );
  SetTrackVar( PT_Var,
               "PT" );
  SetTrackVar( ETA_Var,
               "ETA" );
  SetTrackVar( DLLK_Var,
               "DLLK" );
  SetTrackVar( DLLp_Var,
               "DLLp" );
  //SetTrackVar_DLLpK( DLLp_Var,
  //                   DLLK_Var,
  //                   "DLLpK" ); 
  
  if (not Cuts.empty()) {
    _dstore = this->reduce(FormatCutList(Cuts).c_str())->store();
  }
}

TrackDataSet::TrackDataSet( const std::string& Name, 
                            const std::string& Title, 
                            TrackDataSet* Data, 
                            const RooArgSet& vars, 
                            const RooFormulaVar* cutVar, 
                            const std::string& cutRange, 
                            int nStart, 
                            int nStop, 
                            Bool_t copyCache, 
                            const std::string& wgtVarName)
  : RooDataSet(Name.c_str(),
               Title.c_str(),
               dynamic_cast<RooDataSet*>(Data),
               vars,
               cutVar,
               cutRange.c_str(),
               nStart,
               nStop,
               copyCache,
               wgtVarName.empty() ? NULL : wgtVarName.c_str() ),
    m_Charge( Data->m_Charge )
{
  SetTrackVar( Data->Get_ParamName_P(),
               "P");
  SetTrackVar( Data->Get_ParamName_PT(),
               "PT");
  SetTrackVar( Data->Get_ParamName_ETA(),
               "ETA");
  SetTrackVar( Data->Get_ParamName_DLLK(),
               "DLLK");
  SetTrackVar( Data->Get_ParamName_DLLp(),
               "DLLp");
  //SetTrackVar(Data->Get_ParamName_DLLpK(),
  //            "DLLpK" );
}

TrackDataSet::TrackDataSet( const std::string& Name,
                            const std::string& Charge,
                            TrackDataSet* Numerator,
                            TrackDataSet* Denominator,
                            const std::string& WgtVar,
                            const std::string& Cuts_Num,
                            const std::string& Cuts_Denom,
                            const std::string& WgtVar_Num, 
                            const std::string& WgtVar_Denom
                            )
  : RooDataSet(Name.c_str(), 
               Charge.c_str(),
               Denominator,
               *Denominator->get(),
               NULL,
               0),
    m_Charge( Charge ) 
{ 

  SetTrackVar( Denominator->Get_ParamName_P(),
               "P");
  SetTrackVar( Denominator->Get_ParamName_PT(),
               "PT");
  SetTrackVar( Denominator->Get_ParamName_ETA(),
               "ETA");
  SetTrackVar( Denominator->Get_ParamName_DLLK(),
               "DLLK");
  SetTrackVar( Denominator->Get_ParamName_DLLp(),
               "DLLp");
  // SetTrackVar_DLLpK( Denominator->Get_ParamName_DLLp(),
  //                  Denominator->Get_ParamName_DLLK(),
  //                  "DLLpK" );
  
  if (not Cuts_Denom.empty()) {
    _dstore = this->reduce(FormatCutList(Cuts_Denom).c_str())->store();
  }

  RooRealVar* Weight_Var = NULL;

  TrackDataSet* _NumAll = new TrackDataSet( Numerator->GetName(),
                                            Numerator->GetTitle(),
                                            Numerator,
                                            *Numerator->get(),
                                            0,
                                            WgtVar_Num
                                            );
  
  TrackDataSet* _DenomAll = new TrackDataSet( Denominator->GetName(),
                                              Denominator->GetTitle(),
                                              Denominator,
                                              *Denominator->get(),
                                              0,
                                              WgtVar_Denom
                                              );
  // Normalisation
  Double_t _NumTot   = _NumAll->sumEntries();
  Double_t _DenomTot = _DenomAll->sumEntries();
  
  delete _NumAll;
  delete _DenomAll;

  if ( !WgtVar_Denom.empty() )
  {
    Weight_Var = (RooRealVar*)(Denominator->get())->find(WgtVar_Denom.c_str());
    if(Weight_Var == NULL)
    {
      cout<<"**ERROR** "<<Weight_Var<<" is not in DataSet"<<endl;
      RooErrorHandler::softAbort();
    }
  }

  TrackDataSet* _Num = new TrackDataSet( Numerator->GetName(),
                                         Numerator->GetTitle(),
                                         Numerator,
                                         *Numerator->get(),
                                         FormatCutList(Cuts_Num),
                                         WgtVar_Num
                                         );
  
  TrackDataSet* _Denom = new TrackDataSet( Denominator->GetName(),
                                           Denominator->GetTitle(),
                                           Denominator,
                                           *Denominator->get(),
                                           FormatCutList(Cuts_Denom),
                                           WgtVar_Denom
                                           );
  
  Double_t ratio =  (_Num->sumEntries()/_Denom->sumEntries())*_DenomTot/_NumTot ;
  delete _Num;
  delete _Denom;

  RooRealVar wValue( "wValue", "", ratio );

  TString Formula = WgtVar_Denom.empty() ? "wValue" : TString(WgtVar_Denom.c_str())+"*wValue";
  
  RooFormulaVar wFunc( WgtVar.c_str(),
                       "event weight",
                       Formula.Data(),
                       RooArgList(*Weight_Var,
                                  wValue) );

  this->addColumn(wFunc) ;  
}

//=============================================================================
// Destructor
//=============================================================================
TrackDataSet::~TrackDataSet() {}

//=============================================================================
// Get track charge
//=============================================================================
const TString TrackDataSet::GetCharge() const
{
  return TString(m_Charge);  
}

//=============================================================================
// Define track variable
//=============================================================================
void TrackDataSet::SetTrackVar( const std::string& InputName,
                                std::string InternalName )
{
  // Get parameter list and pointer to parameter with name 'InputName'
  const RooArgSet* vars = this->get(); 
  
  if(vars->find(InputName.c_str()))
  {
    m_ParamMap.insert( make_pair(InternalName,
                                 (RooRealVar*)vars->find(InputName.c_str())) );
  }
  else
  {
    cout<<"WARNING: "<<InputName<<" is not in DataSet"<<endl;
//     RooErrorHandler::softAbort();
  }

}

/*
//=============================================================================
// Define track variable DLLpK and add to the DataSet
//=============================================================================
void TrackDataSet::SetTrackVar_DLLpK( const std::string& InputName_DLLp,
                                      const std::string& InputName_DLLK,
                                      std::string InternalName)
{
  // Get parameter list and pointer to parameter with name 'InputName'
  const RooArgSet* vars = this->get(); 
  
  try
  {  
    if(!vars->find(InputName_DLLp.c_str()) )
    {
      throw InputName_DLLp;
    }
    
    if(!vars->find(InputName_DLLK.c_str()) )
    {
      throw InputName_DLLK;
    }
  }
  catch( std::string& variable)
  {
    cout<<"***ERROR*** "<<variable<<" is not in DataSet"<<endl;
    RooErrorHandler::softAbort();
  }
  
  RooRealVar* DLLp = (RooRealVar*)vars->find(InputName_DLLp.c_str());
  RooRealVar* DLLK = (RooRealVar*)vars->find(InputName_DLLK.c_str());
  
  RooFormulaVar DLLpK("My_DLLpK",
                      "DLL(p - K)", 
                      "@0 - @1",
                      RooArgSet(*DLLp,*DLLK));
  
  if(!vars->find("My_DLLpK"))
  {
    this->addColumn(DLLpK);
  }

  m_ParamMap.insert( make_pair(InternalName,
                               (RooRealVar*)vars->find("My_DLLpK")) );
}
*/
//=============================================================================
// Append method for TrackDataSet
//=============================================================================
void TrackDataSet::append(TrackDataSet& Data)
{
  assert( Data.Get_ParamName_P() == Get_ParamName_P() );
  assert( Data.Get_ParamName_PT() == Get_ParamName_PT() );
  assert( Data.Get_ParamName_ETA() == Get_ParamName_ETA() );
  assert( Data.Get_ParamName_DLLK() == Get_ParamName_DLLK() );
  assert( Data.Get_ParamName_DLLp() == Get_ParamName_DLLp() );
  //assert( Data.Get_ParamName_DLLpK() == Get_ParamName_DLLpK() );

  RooDataSet::append(dynamic_cast<RooDataSet&>(Data));   
}

//=============================================================================
// Get Param Name
//=============================================================================
const std::string TrackDataSet::Get_ParamName(const std::string& Internal_Name) const 
{
  std::map<std::string, RooRealVar*>::const_iterator iter;

  iter = m_ParamMap.find(std::string(Internal_Name));

  if(iter==m_ParamMap.end())
  {
    cout << "**ERROR** In TrackDataSet::Get_ParamName:    " 
         << "internal parameter name '" << Internal_Name 
         << "' not known." << endl;
    RooErrorHandler::softAbort();
  }
  
  return iter->second->GetName();
}

//=============================================================================
// Get P Param Name
//=============================================================================
const std::string TrackDataSet::Get_ParamName_P() const 
{
  return Get_ParamName("P");
}

//=============================================================================
// Get PT Param Name
//=============================================================================
const std::string TrackDataSet::Get_ParamName_PT() const 
{
  return Get_ParamName("PT");
}

//=============================================================================
// Get ETA Param Name
//=============================================================================
const std::string TrackDataSet::Get_ParamName_ETA() const 
{
  return Get_ParamName("ETA");
}

//=============================================================================
// Get DLLK Param Name
//=============================================================================
const std::string TrackDataSet::Get_ParamName_DLLK() const 
{
  return Get_ParamName("DLLK");
}

//=============================================================================
// Get DLLp Param Name
//=============================================================================
const std::string TrackDataSet::Get_ParamName_DLLp() const 
{
  return Get_ParamName("DLLp");
}

//=============================================================================
// Get DLLpK Param Name
//=============================================================================
/*
const std::string TrackDataSet::Get_ParamName_DLLpK() const 
{
  return Get_ParamName("DLLpK");
}
*/

//=============================================================================
// Get Parameter
//=============================================================================
const RooRealVar* TrackDataSet::Get_Param(const std::string& Internal_Name) const 
{
  std::map<std::string, RooRealVar*>::const_iterator iter;

  iter = m_ParamMap.find(std::string(Internal_Name));

  if(iter==m_ParamMap.end())
  {
    cout<<"**ERROR** In TrackDataSet::Get_Param:    internal parameter name '"
        <<Internal_Name<<"' not known."<<endl;
    RooErrorHandler::softAbort();
  }
  
  return iter->second;
}

//=============================================================================
// Get P Parameter
//=============================================================================
const RooRealVar* TrackDataSet::Get_Param_P() const 
{
  return Get_Param("P");
}

//=============================================================================
// Get PT Param Name
//=============================================================================
const RooRealVar* TrackDataSet::Get_Param_PT() const 
{
  return Get_Param("PT");
}

//=============================================================================
// Get ETA Param Name
//=============================================================================
const RooRealVar* TrackDataSet::Get_Param_ETA() const 
{
  return Get_Param("ETA");
}

//=============================================================================
// Get DLLK Param Name
//=============================================================================
const RooRealVar* TrackDataSet::Get_Param_DLLK() const 
{
  return Get_Param("DLLK");
}

//=============================================================================
// Get DLLp Param Name
//=============================================================================
const RooRealVar* TrackDataSet::Get_Param_DLLp() const  
{
  return Get_Param("DLLp");
}

//=============================================================================
// Get DLLpK Param Name
//=============================================================================
/*
const RooRealVar* TrackDataSet::Get_Param_DLLpK() const  
{
  return Get_Param("DLLpK");
}
  
*/

//=============================================================================
// An additional reduce method that takes a vector of  RooBinnings and returns 
// all those events within the global acceptance of the Binning schema
//=============================================================================
RooAbsData* TrackDataSet::SetInBinSchema(const std::vector<RooBinning*>& BinSchema)
{
  std::vector<RooBinning*>::const_iterator itr_Binning;

  std::list<std::pair<std::string, RooBinning*> > BinDefs;

  for ( itr_Binning = BinSchema.begin(); itr_Binning != BinSchema.end(); ++itr_Binning)
  {
    const std::string name = this->Get_ParamName((*itr_Binning)->GetName());
    BinDefs.push_back( make_pair(name,*itr_Binning) );
  }
  
  DataBinCuts Data_Cuts("Data Cuts",
                        "",
                        BinDefs);

  RooFormulaVar cutVar(Data_Cuts.GetGlobalCuts(),
                       Data_Cuts.GetGlobalCuts(),
                       *get()) ;
  
  return reduceEng(*get(),
                   &cutVar,
                   "",
                   0,
                   2000000000,
                   kFALSE);

}

//=============================================================================
// Implementation of RooAbsData virtual method that drives the 
// RooAbsData::reduce() methods
//=============================================================================
RooAbsData* TrackDataSet::reduceEng(const RooArgSet& varSubset, 
                                    const RooFormulaVar* cutVar, 
                                    const std::string& cutRange,
                                    Int_t nStart, 
                                    Int_t nStop, 
                                    Bool_t copyCache)
{
  checkInit() ;

  RooArgSet tmp(varSubset) ;
  if (_wgtVar) {
    tmp.add(*_wgtVar) ;
  }

  // Really would like to pass cuts applied to the 'know' parameter names:
  // - P, PT, Eta, DLLK, DLLp 
  // Not clear how best to do this?
  
  TrackDataSet* ret =  new TrackDataSet(std::string(GetName()), 
                                        std::string(GetTitle()), 
                                        this, 
                                        tmp, 
                                        cutVar, 
                                        cutRange, 
                                        nStart, 
                                        nStop, 
                                        copyCache,
                                        _wgtVar ? _wgtVar->GetName() : "" ) ;
  
  // WVE - propagate optional weight variable
  //       check behaviour in plotting.
  //   if (_wgtVar) {
  //     ret->setWeightVar(_wgtVar->GetName()) ;
  //   }
  return ret ;
}

//=============================================================================



//=============================================================================
// Split composite cut string into individual cuts. Here we assumme the 
// individual cuts are separated by the Boolean logic operator '&&'.
//=============================================================================
void TrackDataSet::SplitCut( const std::string& Cut,
                                  std::vector< std::string >& SplitVec)
{
  boost::split( SplitVec, Cut, boost::is_any_of("&&") ); 
  
  // Above split command erroneously fills SplitVec with empty strings, in 
  // addition to the wanted sub strings. To correct for this, it is therefore 
  // necessary to remove these entries.

  SplitVec.erase( std::remove_if( SplitVec.begin(), SplitVec.end(), 
                                  boost::bind( &std::string::empty, _1 ) ), 
                  SplitVec.end() );
}

//=============================================================================
// Translate a cut on (P, PT, ETA, DLLK, DLLp, DLLKp) into a cut on the defined
// corresponding TrackDataSet variable
//=============================================================================
const std::string TrackDataSet::TranslateCut( const std::string& Cut )
{
  std::string s(Cut), sre, ret;
  boost::regex re;
  boost::cmatch matches;

  //sre = "\\s*(\\w+)\\s*([>=<!]{1,2})\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*";
  //sre = "\\s*(\\D+)\\s*([>=<!]{1,2})\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*";
  sre = "\\s*(.+)\\s*([>=<!]{1,2})\\s*([-+]?[0-9]*\\.?[0-9]+)\\s*";

  try
  {
    // Assignment and construction initialize the FSM used
    // for regexp parsing
    re = sre;
  }

  catch (boost::regex_error& e)
  {
    std::cout << sre << " is not a valid regular expression: \""
         << e.what() << "\"" << std::endl;
  }

  if (boost::regex_match(s.c_str(), matches, re))
  {
    //std::cout<<"Found a match"<<std::endl;
    // matches[0] contains the original string.  matches[n]
    // contains a sub_match object for each matching
    // subexpression
    for (unsigned int i = 1; i < matches.size(); i++)
    {
      // sub_match::first and sub_match::second are iterators that
      // refer to the first and one past the last chars of the
      // matching subexpression
      std::string match(matches[i].first, matches[i].second);

      //std::cout << "\tmatches[" << i << "] = " << match << std::endl;
    }
    
    //If all matches found, format the cut string
    if (matches.size()==4)
    {
      std::string A, B, C;
      A.assign(matches[1].first, matches[1].second);
      B.assign(matches[2].first, matches[2].second);
      C.assign(matches[3].first, matches[3].second);
      
      // Create a vector<string> storing the keys of m_ParamMap 
      std::pair<std::string, RooRealVar*> me;
      std::vector<std::string> vec_keys;      
      BOOST_FOREACH(me, this->m_ParamMap) 
      {  
        if(me.first!="P"){
          vec_keys.push_back(me.first);  
        }
      }
      // Sort strings with shortest -> longest
      sort( vec_keys.begin(), vec_keys.end(), PIDCalib::length() );

      // Declare string to store updated cut-variable string
      std::string newCut = A;

      std::vector<std::string>::iterator iter;
      for(iter=vec_keys.begin(); iter!=vec_keys.end(); ++iter)
      {
        //std::cout<<*iter<<'\t'<<"Current:" <<A;

        // Regular expression to find PID variable surrounded either side by non-word character
        std::string _regex = "\\b"+(*iter)+"\\b";
        boost::regex re2(_regex.c_str());
        
        std::string temp = boost::regex_replace(newCut, 
                                             re2,
                                             (this->m_ParamMap[*iter])->GetName()
                                             );
        newCut = temp;
        //std::cout<<'\t'<<"New: "<<A<<'\t'<<temp<<'\t'<<newCut<<std::endl; 
      }
      
      ret = newCut;
      ret += B;
      ret += C;

      //std::cout<<Cut<<" -->> "<<ret<<std::endl;

      return ret;
    }
    else
    {
      std::cout <<"Only "<<matches.size()<<" matches made in "<< s <<std::endl;
      return NULL;
    }

  }
  else
  {
    std::cout<<"The regexp \"" << re << "\" does not match \"" << s << "\"" <<std::endl;
    return NULL;
  }
}

//=============================================================================
// Format a given cut string, possibly composed of multiple cuts separated by 
// the Boolean '&&' operator, into a string to be passed to RooDataSet::reduce()
//=============================================================================
const std::string TrackDataSet::FormatCutList(const std::string& Cut)
{
  std::vector< std::string > CutSeries;

  // If there exist at least one instance of "&&" in the string, then pass it
  // to the SplitCut function for splitting into individual cuts, else simply 
  // pass cut into CutSeries vector
  if(boost::algorithm::find_first(Cut, "&&"))
  {
    SplitCut( Cut, CutSeries );    
  }
  else
  {
    CutSeries.push_back(Cut);
  }
  
  std::string TotCut = "";
  std::vector< std::string >::iterator cut_itr;
  for(cut_itr=CutSeries.begin(); cut_itr!=CutSeries.end(); ++cut_itr)
  {
    if(!TotCut.empty())
      TotCut += " && ";
    TotCut += TranslateCut(*cut_itr);
  }
  return TotCut;
}



