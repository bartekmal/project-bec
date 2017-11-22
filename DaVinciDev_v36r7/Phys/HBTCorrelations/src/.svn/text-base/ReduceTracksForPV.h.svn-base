#ifndef REDUCETRACKSFORPV_H 
#define REDUCETRACKSFORPV_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class ReduceTracksForPV ReduceTracksForPV.h
 *   
 *
 *  @author Mariusz Witek
 *  @date   2014-04-12
 */
class ReduceTracksForPV : public GaudiAlgorithm {
public: 
  /// Standard constructor
  ReduceTracksForPV( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ReduceTracksForPV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  double m_trackReduceRate;
  std::string m_inputTracks;
  std::string m_outputTracks;

  mutable Rndm::Numbers m_rndm;

};
#endif // REDUCETRACKSFORPV_H
