// $Id: $
#ifndef PIDPERFTOOLS_WEIGHTDATASETTOOL_H 
#define PIDPERFTOOLS_WEIGHTDATASETTOOL_H 1

// Include files
#include "RooAbsData.h"
#include "RooBinning.h"
#include <list>
#include <vector>
#include <string>

#include "PIDPerfTools/DataBinCuts.h"

/** @class WeightDataSetTool WeightDataSetTool.h PIDPerfTools/WeightDataSetTool.h
 *  
 *
 *  @author Andrew Powell (LHCb)04
 *  @date   2011-09-19
 */
template <class T>
class WeightDataSetTool {
public: 
  
  /// Default Constructor
  WeightDataSetTool() 
    : m_RefData(NULL),
      m_CalData(NULL),
      m_BinSchema(0),
      m_EmptyBinFlag(false)
  {}

  /// Standard constructor
  WeightDataSetTool( T* Ref,
                     T* Cal,
                     std::vector<RooBinning*>& Bin)
    : m_RefData( Ref ),
      m_CalData( Cal ),
      m_BinSchema( Bin ),
      m_EmptyBinFlag(false)
  {}
    
  /// Copy Constructor
  WeightDataSetTool( const WeightDataSetTool<T> & tool )
    : m_RefData( tool.ref_data() ),
      m_CalData( tool.cal_data() ),
      m_BinSchema( tool.bin_schema() ),
      m_EmptyBinFlag(false)
  { }
  
  /// Desctructor
  virtual ~WeightDataSetTool( ) {}; ///< Destructor

  /// Access Reference DataSet
  inline T* ref_data() const 
  {
    return m_RefData;
  }
  
  /// Access Calibration DataSet
  inline T* cal_data() const
  {  
    return m_CalData;
  }

  /// Access Bin Schema
  inline std::vector<RooBinning*> bin_schema() const
  {
    return m_BinSchema;
  }

  //========================================================
  // Get a DataSet that contains all those events in 
  // this DataSet which reside in bins where the 
  // Reference DataSet also contains events
  virtual RooAbsData* ANDInBins( const std::string& WgtVar = "",
                                 const std::string& WgtVar_Ref = ""
                                 );
  //========================================================

  //========================================================
  // Produce a 'copy' of this DataSet but weighted wrt
  // the 'Numerator' DataSet in bins defined in 
  // BinSchema
  virtual RooAbsData* WeightInBins( const std::string& WgtVar,
                                    Bool_t NormToSumEnties,
                                    const std::string& WgtVar_Ref = "", 
                                    const std::string& WgtVar_Cal = ""
                                    );
  //========================================================

protected:

  //========================================================
  // Protected method for internal use only. Caller takes 
  // ownership of the returned DataSet
  virtual RooAbsData* WeightInBin( const std::string& WgtVar,
                                   Double_t Norm,
                                   const std::string& Cuts_Ref = "",
                                   const std::string& Cuts_Cal = "",
                                   const std::string& WgtVar_Ref = "", 
                                   const std::string& WgtVar_Cal = ""
                                   );
  //========================================================

  //========================================================
  // Protected method for internal use only. Caller takes 
  // ownership of the returned DataSet
  virtual RooAbsData* ANDInBin( const std::string& Cuts_Cal = "",
                                const std::string& Cuts_Ref = "",
                                const std::string& WgtVar = "",
                                const std::string& WgtVar_Ref = ""
                                );
  //========================================================

  //========================================================
  // Protected method for internal use only. Construct 
  // DataBinCut objects for two DataSets given a 
  // Binning Schema (vector<RooBinning*>). Caller takes 
  // ownership of the DataBinCut instances.
  void MakeDataBinCuts(DataBinCuts*& Cal_Cuts,
                       DataBinCuts*& Ref_Cuts,
                       int& BinDimensions);
  //========================================================
  
  //========================================================
  // Protected method for internal use only. Determine the 
  // bin indices for a given Binning Schema (vector<RooBinning*>)
  // and a given flat index value
  std::list<int> GetBinIndicesFromFlatIndex(int FlatIndex);
  //========================================================
  
private:

  //Data
  T * m_RefData;
  T * m_CalData;
  std::vector<RooBinning*> m_BinSchema;
  bool m_EmptyBinFlag;
  

};
#endif // PIDPERFTOOLS_WEIGHTDATASETTOOL_H
