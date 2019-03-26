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
#include "RooFit.h"
#include "RooCmdArg.h"
#include "RooRealVar.h"
#include "RooErrorHandler.h"

// local
#include "PIDPerfTools/GenericDataSet.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenericDataSet
//
// 2010-12-25 : Andrew Powell (LHCb)04
//-----------------------------------------------------------------------------

//ClassImp(GenericDataSet)

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
void* GenericDataSet::operator new (size_t bytes)
{
  return RooDataSet::operator new(bytes);
}

void* GenericDataSet::operator new (size_t bytes, void *p)
{
  return TObject::operator new(bytes, p);
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GenericDataSet::GenericDataSet( )
  : RooDataSet( )
{

}

GenericDataSet::GenericDataSet( const std::string& TrackName,
                            RooDataSet* Data,
                            const RooArgSet& vars,
                            const VectorOfStringPairs& VarAliases,
                            const std::string& Cuts,
                            const std::string& WgtVarName
                            )
//   : RooDataSet( TrackName.c_str(),
//                 Data->GetTitle(),
//                 Data,
//                 vars,
//                 ReplaceVarNamesInCuts(Cuts,VarAliases).c_str(),
//                 WgtVarName.empty() ? NULL : WgtVarName.c_str() ),
  : RooDataSet( TrackName.c_str(),
                Data->GetTitle(),
                vars,
                RooFit::Import(*Data),
                RooFit::Cut(ReplaceVarNamesInCuts(Cuts,VarAliases).c_str()),
                WgtVarName.empty() ? RooCmdArg::none() : RooFit::WeightVar(WgtVarName.c_str()) ),
    m_Aliases( VarAliases )

{
  SetTrackVars( VarAliases );

//   if (not Cuts.empty()) {
// //     RooDataSet* dset = this->reduce(ReplaceVarNamesInCuts(Cuts).c_str());
//
//     _dstore = new RooTreeDataStore(fName.Data(),fTitle.Data(),_vars,*_dstore,ReplaceVarNamesInCuts(Cuts).c_str(),WgtVarName.c_str()) ;
//
//     appendToDir(this,kTRUE) ;
//
//     if (WgtVarName.c_str()) {
//       // Use the supplied weight column
//       initialize(WgtVarName.c_str()) ;
//     } else {
//       if (_wgtVar && vars.find(_wgtVar->GetName())) {
//         // Use the weight column of the source data set
//         initialize(_wgtVar->GetName()) ;
//       } else {
//         initialize(0) ;
//       }
//     }
//   }

}



GenericDataSet::GenericDataSet( const std::string& TrackName,
                            GenericDataSet* Data,
                            const RooArgSet& vars,
                            const std::string& Cuts ,
                            const std::string& WgtVarName
                            )
//   : RooDataSet( TrackName.c_str(),
//                 Data->GetTitle(),
//                 Data,
//                 vars,
//                 ReplaceVarNamesInCuts(Cuts,*Data->Get_Aliases()).c_str(),
//                 WgtVarName.empty() ? NULL : WgtVarName.c_str() ),
  : RooDataSet( TrackName.c_str(),
                Data->GetTitle(),
                vars,
                RooFit::Import(*Data),
                RooFit::Cut(ReplaceVarNamesInCuts(Cuts,*Data->Get_Aliases()).c_str()),
                WgtVarName.empty() ? RooCmdArg::none() : RooFit::WeightVar(WgtVarName.c_str()) ),
    m_Aliases( *Data->Get_Aliases() )
{
  SetTrackVars( *(Data->Get_Aliases()) );

//   if (not Cuts.empty()) {
// //     RooDataSet* dset = this->reduce(ReplaceVarNamesInCuts(Cuts).c_str());
//
//     _dstore = new RooTreeDataStore(fName.Data(),fTitle.Data(),_vars,*_dstore,ReplaceVarNamesInCuts(Cuts).c_str(),WgtVarName.c_str()) ;
//
//     appendToDir(this,kTRUE) ;
//
//     if (WgtVarName.c_str()) {
//       // Use the supplied weight column
//       initialize(WgtVarName.c_str()) ;
//     } else {
//       if (_wgtVar && vars.find(_wgtVar->GetName())) {
//         // Use the weight column of the source data set
//         initialize(_wgtVar->GetName()) ;
//       } else {
//         initialize(0) ;
//       }
//     }
//   }

}

GenericDataSet::GenericDataSet( const std::string& Name,
                            const std::string& Title,
                            GenericDataSet* Data,
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
    m_Aliases( *Data->Get_Aliases() )
{
  SetTrackVars( *(Data->Get_Aliases()) );
}

//=============================================================================
// Destructor
//=============================================================================
GenericDataSet::~GenericDataSet() {}

//=============================================================================
// Get track charge
//=============================================================================
const VectorOfStringPairs* GenericDataSet::Get_Aliases() const
{
  return &m_Aliases;
}

//=============================================================================
// Define track variable
//=============================================================================
void GenericDataSet::SetTrackVars( const VectorOfStringPairs& VarAliases )
{
  std::pair<std::string,std::string> alias;
  BOOST_FOREACH(alias, VarAliases) {
    SetTrackVar(alias.second, alias.first);
  }
  //SetTrackVar_DLLpK();
  
}

//=============================================================================
// Define track variable
//=============================================================================
void GenericDataSet::SetTrackVar( const std::string& InputName,
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
    //cout<<"WARNING: "<<InputName<<" is not in DataSet"<<endl;
//     RooErrorHandler::softAbort();
  }

}

//=============================================================================
// Define track variable DLLpK and add to the DataSet
//=============================================================================
// void GenericDataSet::SetTrackVar_DLLpK()
// {
//   // Get parameter list and pointer to parameter with name 'InputName'
//   const RooArgSet* vars = this->get();
// 
//   RooRealVar* DLLp = (RooRealVar*)this->Get_Param("DLLp");
//   RooRealVar* DLLK = (RooRealVar*)this->Get_Param("DLLK");
//   
//   if ( not DLLp or not DLLK ) {
//     if ( not DLLp ) cout<<"WARNING: DLLp is not in DataSet"<<endl;
//     if ( not DLLK ) cout<<"WARNING: DLLK is not in DataSet"<<endl;
//     cout<<"WARNING: Not creating My_DLLpK"<<endl;
//     return;
//   }
// 
//   RooFormulaVar DLLpK("My_DLLpK",
//                       "DLL(p - K)",
//                       "@0 - @1",
//                       RooArgSet(*DLLp,*DLLK));
// 
//   if(!vars->find("My_DLLpK"))
//   {
//     this->addColumn(DLLpK);
//   }
// 
//   m_ParamMap.insert( make_pair(std::string("DLLpK"),
//                                (RooRealVar*)vars->find("My_DLLpK")) );
// }

//=============================================================================
// Append method for GenericDataSet
//=============================================================================
void GenericDataSet::append(GenericDataSet& Data)
{
  const VectorOfStringPairs* Data_Aliases = Data.Get_Aliases();

  assert(m_Aliases.size() == Data_Aliases->size());

  std::pair<std::string,std::string> Data_Pair, Local_Pair;
  BOOST_FOREACH(Data_Pair, *Data_Aliases) {
    bool Var_Found = false;
    BOOST_FOREACH(Local_Pair, m_Aliases) {
      if (Data_Pair.first == Local_Pair.first) {
        Var_Found = true;
        assert(Data_Pair.second == Local_Pair.second);
      }
    }
    assert(Var_Found);
    _unused(Var_Found);
  }

  RooDataSet::append(dynamic_cast<RooDataSet&>(Data));
}

//=============================================================================
// Get Param Name
//=============================================================================
const std::string GenericDataSet::Get_ParamName(const std::string& Internal_Name) const
{
  std::map<std::string, RooRealVar*>::const_iterator iter;

  iter = m_ParamMap.find(std::string(Internal_Name));

  if(iter==m_ParamMap.end())
  {
    cout << "**ERROR** In GenericDataSet::Get_ParamName:    "
         << "internal parameter name '" << Internal_Name
         << "' not known." << endl;
    RooErrorHandler::softAbort();
  }

  return iter->second->GetName();
}


//=============================================================================
// Get Parameter
//=============================================================================
const RooRealVar* GenericDataSet::Get_Param(const std::string& Internal_Name) const
{
  std::map<std::string, RooRealVar*>::const_iterator iter;

  iter = m_ParamMap.find(std::string(Internal_Name));

  if(iter==m_ParamMap.end())
  {
    cout<<"**ERROR** In GenericDataSet::Get_Param:    internal parameter name '"
        <<Internal_Name<<"' not known."<<endl;
    RooErrorHandler::softAbort();
  }

  return iter->second;
}


//=============================================================================
// An additional reduce method that takes a vector of  RooBinnings and returns
// all those events within the global acceptance of the Binning schema
//=============================================================================
RooAbsData* GenericDataSet::SetInBinSchema(const std::vector<RooBinning*>& BinSchema)
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


RooAbsData* GenericDataSet::reduce(const char* cut)
{
  // Create a subset of the data set by applying the given cut on the data points.
  // The cut expression can refer to any variable in the data set. For cuts involving
  // other variables, such as intermediate formula objects, use the equivalent
  // reduce method specifying the as a RooFormulVar reference.

  std::string newcut("1");
  if (cut != 0x0 and strlen(cut) != 0) {
    newcut = ReplaceVarNamesInCuts(cut);
  }

  RooFormulaVar cutVar(newcut.c_str(),newcut.c_str(),*get()) ;
  return reduceEng(*get(),&cutVar,"",0,2000000000,kFALSE) ;
}


//=============================================================================
// Implementation of RooAbsData virtual method that drives the
// RooAbsData::reduce() methods
//=============================================================================
RooAbsData* GenericDataSet::reduceEng(const RooArgSet& varSubset,
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

  GenericDataSet* ret =  new GenericDataSet(std::string(GetName()),
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
// Search through a cut string and replace any varible names with those found
// in the dataset, for example DLLmu -> Mu_CombDLLmu
//=============================================================================
const std::string GenericDataSet::ReplaceVarNamesInCuts(const std::string& Cut, const VectorOfStringPairs VarAliases)
{

  // make a vector of variable names which are to be replaced
  std::vector<std::string> names_to_replace;

  if(m_ParamMap.size() == 0) {
    if (VarAliases.size() == 0) {
      std::cerr << "GenericDataSet::ReplaceVarNamesInCuts(): ERROR: function called with no varible name map." << std::endl;
      return Cut;
    } else {
      std::pair<std::string, std::string> i;
      BOOST_FOREACH(i, VarAliases) {
        names_to_replace.push_back(i.first);
      }
    }
  } else {
    if (VarAliases.size() == 0) {
      std::pair<std::string, RooRealVar*> i;
      BOOST_FOREACH(i, this->m_ParamMap) {
        names_to_replace.push_back(i.first);
      }
    } else {
      std::pair<std::string, std::string> i;
      BOOST_FOREACH(i, VarAliases) {
        names_to_replace.push_back(i.first);
      }
    }
  }
  // Sort strings with shortest -> longest
  sort( names_to_replace.begin(), names_to_replace.end(), PIDCalib::length() );

  std::vector< std::string > split_cut;

  std::string sre, s(Cut);
  boost::regex re;
  boost::cmatch matches;

  // Search for non-mathematical charactors, defined as anything other than:
  // /*-+()<>!&|=
  // The non-mathematical charactors can optionally be between two sets of
  // mathematical charactors.
  sre = "\\s*([-=|&\\(\\)\\+\\*/<>\\[\\]!\\s]*)\\s*([^-=|&\\(\\)\\+\\*/<>\\[\\]!\\s]+)\\s*([-=|&\\(\\)\\+\\*/<>\\[\\]!\\s]*).*";

  try
  { // Assignment and construction initialize the FSM used for regexp parsing
    re = sre;
  } catch (boost::regex_error& e)  {
    std::cout << sre << " is not a valid regular expression: \""
              << e.what() << "\"" << std::endl;
  }

  // continue to search along the sting until there are no more groups of
  // non-mathematical charactors
  while (boost::regex_match(s.c_str(), matches, re)) {
    int matched_length(0);
    for (unsigned int i = 1; i < matches.size(); i++)
    {
      std::string match(matches[i].first, matches[i].second);
      matched_length += match.length();
//       std::cout << "\tmatches[" << i << "] = " << match << std::endl;
      split_cut.push_back(match);
    }
    // removed any matched charactors from the beginning of the search string
    s.assign(matches[matches.size()-1].second,s.length()-matched_length);
  }

//   BOOST_FOREACH( std::string cut_piece, split_cut ) {
//     std::cout << cut_piece.c_str();
//   }
//   std::cout << std::endl;

  std::string ret("");

  // search through the split cut string for varible names to replace and
  // replace them with the name of the varible in the dataset, for
  // example DLLmu -> Mu_CombDLLmu. Then join original cut string back together.
  BOOST_FOREACH( std::string cut_piece, split_cut ) {
    bool found = false;
    BOOST_FOREACH(std::string var_name, names_to_replace) {
      if (cut_piece.compare(var_name)==0) {
        found = true;
        if (VarAliases.size() == 0) {
          ret += (this->m_ParamMap[var_name])->GetName();
        } else {
          std::pair<std::string, std::string> i;
          BOOST_FOREACH(i, VarAliases) {
            if (i.first.compare(var_name)==0) {
              ret += i.second;
              break;
            }
          }
        }
        //std::cout << cut_piece.c_str() << "==" << var_name.c_str() << std::endl;
      } /*else {
        std::cout << cut_piece.c_str() << "!=" << var_name.c_str() << " " << cut_piece.compare(var_name) << std::endl;
      } */
    }
    if (not found) {
      ret += cut_piece;
    }
  }

//   std::cout << ret << std::endl;

  return ret;

//   std::cout << "TBIRD!" << std::endl;
//
//   exit(0);
//
//   std::vector< std::string > CutSeries;
//   // If there exist at least one instance of "&&" in the string, then pass it
//   // to the SplitCut function for splitting into individual cuts, else simply
//   // pass cut into CutSeries vector
//   if(boost::algorithm::find_first(Cut, "&&"))
//   {
//     SplitCut( Cut, CutSeries );
//   }
//   else
//   {
//     CutSeries.push_back(Cut);
//   }
//
//   std::string TotCut = "";
//   std::vector< std::string >::iterator cut_itr;
//   for(cut_itr=CutSeries.begin(); cut_itr!=CutSeries.end(); ++cut_itr)
//   {
//     if(!TotCut.empty())
//       TotCut += " && ";
//     TotCut += TranslateCut(*cut_itr);
//   }
//   return TotCut;
}



TTree* GenericDataSet::tree() {
  if (_dstore) {
    TString tree_ds_name;
    tree_ds_name.Form("%s_TTree",_dstore->GetName());
    RooTreeDataStore* tree_ds = new RooTreeDataStore(fName.Data(), fTitle.Data(), _vars,*_dstore,0,_wgtVar->GetName()) ;
    return &(tree_ds->tree());
  } 
  return 0;
}
