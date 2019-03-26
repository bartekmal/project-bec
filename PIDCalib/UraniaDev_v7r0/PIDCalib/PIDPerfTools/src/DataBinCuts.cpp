// STL
#include <iostream>

// ROOT
#include "TROOT.h"
#include "RooErrorHandler.h"

// Boost
#include <boost/lexical_cast.hpp>

// local
#include "PIDPerfTools/DataBinCuts.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DataBinCuts
//
// 2010-12-26 : Andrew Powell (LHCb)04
//-----------------------------------------------------------------------------

//ClassImp(DataBinCuts);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DataBinCuts::DataBinCuts( const std::string& Name,
                          const std::string& Title,
                          std::list<std::pair<std::string,RooBinning*> >& BinList)
  : TNamed    ( Name.c_str(), Title.c_str() ),
    m_BinDefs ( BinList     ) { }

//=============================================================================
// Destructor
//=============================================================================
DataBinCuts::~DataBinCuts() {}

const TCut DataBinCuts::GetCuts( const std::list<int>& BinIndex)
{
  if( BinIndex.size() != m_BinDefs.size() )
  {
    std::cout<<"Bin index list differs in size from Bin List: "<<
      BinIndex.size()<<'\t'<<
      m_BinDefs.size()<<std::endl;
    RooErrorHandler::softAbort();
  }

  std::string AllCuts = "";
  Bool_t first(kTRUE);
  std::list<std::pair<std::string, RooBinning*> >::iterator liter = m_BinDefs.begin();
  std::list<int>::const_iterator biter = BinIndex.begin();

  for( ; liter != m_BinDefs.end() ; ++liter, ++biter )
  {
    if(first)
    {
      first=kFALSE ;
      AllCuts += ( liter->first +
                   " >= " +
                   boost::lexical_cast<std::string>(liter->second->binLow(*biter)) );
      AllCuts += ( " && " + liter->first +
                   " < " +
                   boost::lexical_cast<std::string>(liter->second->binHigh(*biter)) );
    }
    else
    {
      AllCuts += ( " && " + liter->first +
                   " >= " +
                   boost::lexical_cast<std::string>(liter->second->binLow(*biter)) );
      AllCuts += ( " && " + liter->first +
                   " < " +
                   boost::lexical_cast<std::string>(liter->second->binHigh(*biter)) );
    }

  }//for

  return TCut(AllCuts.c_str());

}

const TCut DataBinCuts::GetGlobalCuts( )
{
  std::string AllCuts = "";
  Bool_t first(kTRUE);
  std::list<std::pair<std::string, RooBinning*> >::iterator liter = m_BinDefs.begin();
  for( ; liter != m_BinDefs.end() ; ++liter )
  {
    if(first)
    {
      first=kFALSE ;
      AllCuts += ( liter->first +
                   " >= " +
                   boost::lexical_cast<std::string>(liter->second->lowBound()) );
      AllCuts += ( " && " + liter->first +
                   " < " +
                   boost::lexical_cast<std::string>(liter->second->highBound()) );
    }
    else
    {
      AllCuts += ( " && " + liter->first +
                   " >= " +
                   boost::lexical_cast<std::string>(liter->second->lowBound()) );
      AllCuts += ( " && " + liter->first +
                   " < " +
                   boost::lexical_cast<std::string>(liter->second->highBound()) );
    }

  }

  return TCut(AllCuts.c_str());
}

//=============================================================================
