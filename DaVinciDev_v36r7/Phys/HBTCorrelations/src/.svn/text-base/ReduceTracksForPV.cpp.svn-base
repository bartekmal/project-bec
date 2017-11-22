// Include files 

 // from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
// local
#include "ReduceTracksForPV.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ReduceTracksForPV
//
// 2014-04-12 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ReduceTracksForPV )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReduceTracksForPV::ReduceTracksForPV( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputTracks"       , m_inputTracks);
  declareProperty("OutputTracks"      , m_outputTracks);  
  declareProperty("TrackReduceRate" , m_trackReduceRate = 0.0 );  
}
//=============================================================================
// Destructor
//=============================================================================
ReduceTracksForPV::~ReduceTracksForPV() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ReduceTracksForPV::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // randomn number generator                                                                                           
  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) )
    {
      return Error( "Unable to create Random generator" );
    }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ReduceTracksForPV::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::Track::Container* out_tracks = new LHCb::Track::Container() ;
  put( out_tracks, m_outputTracks);

  LHCb::Tracks* in_tracks = get<LHCb::Tracks>(m_inputTracks);
  if(!in_tracks) {
    warning() << "Tracks not found at location: " << m_inputTracks << endmsg;
    return StatusCode::SUCCESS;
  }
  for(LHCb::Tracks::iterator itrack = in_tracks->begin();
      in_tracks->end() != itrack; itrack++) {
    const LHCb::Track* itr = (*itrack);
    if ( itr->hasVelo() && m_rndm() > m_trackReduceRate ) {
      LHCb::Track* otr = itr->clone();
      out_tracks->insert(otr);
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ReduceTracksForPV::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
