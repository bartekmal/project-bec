// $Id: $
#ifndef PIDPERFTOOLS_DATABINCUTS_H 
#define PIDPERFTOOLS_DATABINCUTS_H 1

// Include files
#include <map>
#include <list>
#include <string>

#include "TObject.h"
#include "TNamed.h"
#include "TCut.h"
#include "RooBinning.h"

/** @class DataBinCuts DataBinCuts.h PIDPerfTools/DataBinCuts.h
 *  
 *
 *  @author Andrew Powell (LHCb)04
 *  @date   2010-12-26
 */

class DataBinCuts : public TNamed
{

public: 

  /// Default Constructor
  DataBinCuts() { }

  /// Standard constructor
  DataBinCuts( const std::string& Name,
               const std::string& Title,
               std::list<std::pair<std::string, RooBinning*> > & BinList ); 

  virtual ~DataBinCuts( ); ///< Destructor

public:

  const TCut GetCuts( const std::list<int>& BinIndex );
  
  const TCut GetGlobalCuts( );

private:

  std::list<std::pair<std::string, RooBinning*> > m_BinDefs;

public:  
  
  ClassDef(DataBinCuts,1);
  
};
#endif // PIDPERFTOOLS_DATABINCUTS_H
