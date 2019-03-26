// $Id: $
#ifndef PIDPERFTOOLS_PIDRESULT_H 
#define PIDPERFTOOLS_PIDRESULT_H 1

#include <string>
#include <vector>

/** @class PIDResult PIDResult.h PIDPerfTools/PIDResult.h
 *  
 *
 *  @author Andrew Powell (LHCb)04
 *  @date   2011-02-01
 */

class PIDResult
{
public:
  
  double Eff;
  double Err_Lo;
  double Err_Hi;
  std::vector<std::string> DLL;
  std::vector<std::string> CutOp;
  std::vector<double> Cut;

public:

  /// Constructor with values
  PIDResult( double a = 0, 
             double b = 0,
             double c = 0,
             const std::vector<std::string>& d = std::vector<std::string>(),
             const std::vector<std::string>& e = std::vector<std::string>(),
             const std::vector<double>& f = std::vector<double>() )
    : Eff    ( a ),
      Err_Lo ( b ),
      Err_Hi ( c ),
      DLL    ( d ),
      CutOp  ( e ),
      Cut    ( f )
  { }
  

public:

  /// Container of PIDResults
  typedef std::vector<PIDResult> Container;
  
};

typedef PIDResult PIDRESULT;

#endif // PIDPERFTOOLS_PIDRESULT_H
