#ifndef USER_PVChecker_H 
#define USER_PVChecker_H 1
// std
#include <sstream>
#include <memory>
#include <map>
#include <set>
#include <iostream>
#include <algorithm>
#include <vector>
// Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "GaudiKernel/VectorMap.h"
// Base class
#include "Kernel/DaVinciHistoAlgorithm.h"
// Interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "MCInterfaces/IMCReconstructible.h"
#include "MCInterfaces/IMCParticleSelector.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
// Kernel
#include "Kernel/DaVinciAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>
#include "RichKernel/RichPoissonEffFunctor.h"
// MC association
#include "Kernel/Particle2MCLinker.h"
// Event
#include "Event/RecHeader.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/MCParticle.h"
#include "Kernel/IJetMaker.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "Kernel/DaVinciHistoAlgorithm.h"
// Boost
#include "boost/lexical_cast.hpp"

/** @class PVChecker PVChecker.h
 *  
 *  @author Marcin Kucharczyk
 *  @date   2014-03-12
 */

using namespace LHCb;

class PVChecker : public DaVinciAlgorithm {

public: 
  /// Standard constructor
  PVChecker(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~PVChecker();            ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  class chPart {
  public:
    int pvIndex;
    int save;
  };

  int fromMCVertex(const LHCb::MCParticle*,const LHCb::MCVertex*);
  const LHCb::RecVertex* closestPV(const LHCb::Particle* part);
  bool writeEvent(std::vector<int>);
  std::string m_inputPVsName;
  LHCb::RecVertices* m_inputPVs;
  std::string m_inputTracksName;
  LHCb::Tracks* m_inputTracks;
  int m_recPV1,  m_recPV2,  m_recPV3,  m_recPV4;
  int m_MCvisPV1,  m_MCvisPV2,  m_MCvisPV3,  m_MCvisPV4;
  int m_visPV1,  m_visPV2,  m_visPV3,  m_visPV4;
  int m_visPVM1, m_visPVM2, m_visPVM3, m_visPVM4;
  int m_visPVP1, m_visPVP2, m_visPVP3, m_visPVP4;
  int m_visPVPClose1, m_visPVPClose2, m_visPVPClose3, m_visPVPClose4;
  int m_presel;
  double m_trkChi2Cut;
  double m_ipCut;
  double m_ipChi2Cut;
  double m_piNNCut;
  double m_pNNCut;
  double m_kNNCut;
  double m_ghostNNCut;
  double m_partPCut;
  double m_partPtCut;
  const IDVAlgorithm* m_dva;
  const IDistanceCalculator* m_DOCA;
  std::vector<chPart> selParts;
};
//========================================================================//
#endif // USER_PVChecker_H
