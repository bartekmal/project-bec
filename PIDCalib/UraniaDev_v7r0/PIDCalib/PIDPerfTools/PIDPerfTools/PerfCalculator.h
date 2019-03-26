// $Id: $
#ifndef PIDPERFTOOLS_PERFCALCULATOR_H
#define PIDPERFTOOLS_PERFCALCULATOR_H 1

// Include files
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

/** @class PerfCalculator PerfCalculator.h PIDPerfTools/PerfCalculator.h
 *
 *
 *  @author Andrew Powell (LHCb)04
 *  @date   2010-12-20
 */
template <class T>
class PerfCalculator
{

public:

  /// Default Constructor
  PerfCalculator() : m_Data(NULL) { }

  /// Standard constructor from data
  PerfCalculator( T* Data )
    : m_Data( Data )
  {
    TH1::AddDirectory(kFALSE);
  }

  /// Copy Constructor
  PerfCalculator( const PerfCalculator<T> & perf )
    : m_Data( perf.data() ) { }

  /// Destructor
  virtual ~PerfCalculator( )  { }

  /// Access the data
  inline T* data() const { return m_Data; }

  //========================================================
  // Return PIDResult for given PID cut
  PIDResult Perf( const std::string& PIDCut,
                  Bool_t BayesErrors = kTRUE );
  //========================================================
  
  //========================================================
  // Return list of PIDResults for given list of PID cut
  PIDResult::Container Perf( const std::list<std::string>& PIDCuts,
                             Bool_t BayesErrors = kTRUE );
  //========================================================

  //========================================================
  // Return efficiency as a function of the  variable(s) given
  // in the vector of RooBinnings, for a given PID cut.
  // essentially just a wrapper for all the "Perf" functions
  // that return histograms of different dimension. This returns
  // a histogram of arbitrary dimension.
  TH1* Perf_Hist( const char * histName,
                  const std::string& pidCut,
                  std::vector<RooBinning*>& theBinSchema,
                  Bool_t rejectNonNormalBins = kFALSE,
                  Int_t minEntries=5,
                  Bool_t printVals=kFALSE);
  //========================================================
  

  //========================================================
  // Return efficiency as a function of the  variable
  // declared in the RooBinning name (Binominal Errors)
  TH1F* Perf( const std::string& PIDCut,
              RooBinning* BinScheme,
              Bool_t rejectNonNormalBins = kFALSE,
              Int_t minEntries=5,
              Bool_t printVals=kFALSE);
  //========================================================

  //========================================================
  // Return efficiency as a function of the  variable
  // declared in the RooBinning name. Errors returned
  // are from using Bayes' Theorem result (Ensures eff=[0,1])
  //
  // WARNING: When weighted histograms are used for the "pass"
  // and "total" histograms, the "effective entries" are used 
  // ( (sum_i(w_i))^2 / sum_i(w_i*w_i) ). As such, the efficiencies
  // are not guarenteed to be accurate with this method. If
  // it is known that the efficiencies will all be less than
  // 100%, then it is better to use binomial errors for 
  // weighted data sets
  TGraphAsymmErrors* Perf_BayesErrors( const std::string& PIDCut,
                                       RooBinning* );
  //========================================================

  //========================================================
  // Return efficiencies as a function of DLL Cut value. If 
  // multiple DLLs are used in the cut, then the plotted DLL
  // can be specified by the index argument (set by default
  // to the first sub-cut)
  TGraphAsymmErrors* Perf_Scan( const std::list<std::string>& Cuts,
                                Bool_t BayesErrors = kTRUE,
                                int index = 0 );
  //========================================================

  //========================================================
  // Return efficiency of this sample against efficiencies
  // from an external sample
  TGraphAsymmErrors* Perf_Scan( const PIDResult::Container& Ex_Results,
                                const std::list<std::string>& PIDCuts,
                                Bool_t BayesErrors = kTRUE );
  //========================================================

  //========================================================
  // Return efficiency of this sample against efficiencies 
  // from an external sample and save results to file
  TGraphAsymmErrors* Perf_Scan_ToFile( const PIDResult::Container& Ex_Results,
                                       const std::list<std::string>& PIDCuts,
                                       const std::string& FileName,
                                       Bool_t BayesErrors = kTRUE,
                                       int index = 0);
  //========================================================

  //========================================================
  // Return efficiency as a function of the  variables
  // declared in the RooBinning names
  TH2F* Perf( const std::string& PIDCut,
              RooBinning* BinSchema1,
              RooBinning* BinSchema2,
              Bool_t rejectNonNormalBins = kFALSE,
              Int_t minEntries=5,
              Bool_t printVals=kFALSE );
  //========================================================

  //========================================================
  // Return efficiency as a function of the  variables
  // declared in the RooBinning names
  TH3F* Perf( const std::string& PIDCut,
              RooBinning* BinSchema1,
              RooBinning* BinSchema2,
              RooBinning* BinSchema3,
              Bool_t rejectNonNormalBins = kFALSE,
              Int_t minEntries=5,
              Bool_t printVals=kFALSE );
  //========================================================

  //========================================================
  // Returns a TEfficiency object, which stores the "pass"
  // and "fail" histograms which are used to calculate the
  // efficiencies as a function of the variables declared in
  // the RooBinning names
  // The TEfficiency class provides several frequentist and
  // Bayesian methods of determining the confidence intervals
  // of the efficiencies for up to three binning variables.
  // 
  // WARNING: The TEfficiency class is not able to handle
  // weights, since it assumes that the efficiencies are
  // a parameter of a binomial distribution, and so the 
  // "pass" and "fail" entries must be integers.
  // A global weight can be set for each TEfficiency object,
  // but to use this correctly, it would be necessary to
  // produce a different TEfficiency object for each weight
  // and to merge the results. However, for continuous weights
  // (such as the signal sPlot weights), this is clearly not 
  // possible. One way around this would be to bin the weights,
  // or to use the "effective weights", as is done in the
  // TGraphAsymmErrors class (which can lead to incorrect results),
  // however, this is beyond the scope of this method.
  // As such, this method is only really useful for producing
  // efficiency estimated on truth matched Monte Carlo
  // calibration tracks
  TEfficiency* Perf_TEff( const std::string& PIDCut,
                          RooBinning* xBinning,
                          RooBinning* yBinning=NULL,
                          RooBinning* zBinning=NULL);
  //========================================================

  //========================================================
  // Return efficiencies within bins of the variables
  // declared in the RooBinning names
 
  // deprecated, should be removed at some point
  TH3F* Table( const std::string& PIDCut,
               RooBinning* BinSchema1,
               RooBinning* BinSchema2,
               RooBinning* BinSchema3,
               std::ostream& os,
               Bool_t rejectNonNormalBins = kFALSE,
               Int_t minEntries=5);

  PIDTable* Table( const std::string& PIDCut,
                   RooBinning* BinSchema1,
                   RooBinning* BinSchema2,
                   Bool_t rejectNonNormalBins = kFALSE,
                   Int_t minEntries=5);
               
  PIDTable* Table( const std::string& PIDCut,
                   RooBinning* BinSchema1,
                   RooBinning* BinSchema2,
                   RooBinning* BinSchema3,
                   Bool_t rejectNonNormalBins = kFALSE,
                   Int_t minEntries=5);
  PIDCrossTable* CrossTable( const std::string& PIDCut,
                             RooBinning* BinSchema1,
                             RooBinning* BinSchema2,
                             Bool_t rejectNonNormalBins = kFALSE,
                             Int_t minEntries=5);
    //========================================================
  
protected:

  //========================================================
  // Split composite cut string into individual cuts. Here 
  // we assumme the individual cuts are separated by the 
  // Boolean logic operator '&&'.
  void SplitCut( const std::string&,
                 std::vector< std::string >& );
  //========================================================

  //========================================================
  // Translate a cut on (P, PT, ETA, DLLK, DLLp, DLLKp) into 
  //a cut on the defined corresponding TrackDataSet variables
  const std::string TranslateCut( const std::string& Cut,
                                  std::string&,
                                  std::string&,
                                  std::string&);
  //========================================================
    
  //========================================================
  // Format a given cut string, possible composed of 
  // multiple cuts separated by the Boolean '&&' operator, 
  // into a string to be passed to RooDataSet::reduce()
  const std::string FormatCutList( const std::string&,
                                   std::vector<std::string>&,
                                   std::vector<std::string>&,
                                   std::vector<std::string>& );
  //========================================================

private:

  //Data
  T * m_Data;

};
#endif // PIDPERFTOOLS_PERFCALCULATOR_H
