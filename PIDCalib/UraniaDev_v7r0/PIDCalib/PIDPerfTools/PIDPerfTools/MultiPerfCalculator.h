// $Id: $
#ifndef PIDPERFTOOLS_MULTIPERFCALCULATOR_H 
#define PIDPERFTOOLS_MULTIPERFCALCULATOR_H 1

// Include files
#include "PerfCalculator.h"
#include <Rtypes.h>

#include <list>
#include <string>
#include "PIDResult.h"
#include "PIDPerfTools/PIDTable.h"
#include "PIDPerfTools/PIDCrossTable.h"
#include "TrackDataSet.h"
#include "RooBinning.h"
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TEfficiency.h"

/** @class MultiPerfCalculator MultiPerfCalculator.h PIDPerfTools/MultiPerfCalculator.h
 *  
 *
 *  @author Edmund Smith (LHCb 2010) 
 *  @date   2012-05-24
 */

template<class T>
class MultiPerfCalculator : public PerfCalculator<T> {
public: 
  /// Standard constructor
  MultiPerfCalculator(T* Data) : PerfCalculator<T>(Data)
  {
  
  };

  //=============================================================================
  // Returns a range of cuts (at values plus or minus Range) for the cut string 
  // in theCut. If theCut is a composite cut, the vector contains individual cuts
  // at plus or minus the Range for each in there. And the original cut.
  //=============================================================================
  std::vector<std::string> RangeOfCuts(const std::string theCut, int Range);

  //=============================================================================
  // Returns a vector of doubles of cut values for range of cuts (at values plus or minus Range) 
  // for the cut string in theCut. Doesn't work for a composite cut.
  //=============================================================================
  std::vector<double> DLLsForRangeOfCuts(const std::string theCut, int Range);
  
  //=============================================================================
  // Calls the Perf_Hist function for as many cuts as there are in the vector
  // returns a vector of TH1s adds the cut to the hist name to make them unique.
  //=============================================================================
  std::vector<TH1*> Many_Perf_Hists(std::string histName,
                                    std::vector<std::string> pidCuts,
                                    std::vector<RooBinning*>& theBinSchema,
                                    Bool_t rejectNonNormalBins,
                                    Int_t minEntries);
  
  virtual ~MultiPerfCalculator( ){}; ///< Destructor

protected:

private:

};
#endif // PIDPERFTOOLS_MULTIPERFCALCULATOR_H
