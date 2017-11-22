#ifndef USER_CloneChecker_H 
#define USER_CloneChecker_H 1
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

/** @class CloneChecker CloneChecker.h
 *  
 *  @author Marcin Kucharczyk
 *  @date   2014-02-10
 */

using namespace LHCb;

class CloneChecker : public DaVinciAlgorithm {

public: 
  /// Standard constructor
  CloneChecker(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CloneChecker();            ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  class ParticleHistory {
  public:
    ParticleHistory(const LHCb::Particle*         _part,
                    const LHCb::Particle*         _prim_part,
                    const std::string&            _history,
                    const LHCb::ParticleProperty* _prop,
                    const bool                    _orig)
      : particle       (_part),
        firstParticle  (_prim_part),
        history        (_history),
        properties     (_prop),
        topLevel       (_orig) {}
  public:
    const LHCb::Particle * particle;
    const LHCb::Particle * firstParticle;
    std::string history;
    const LHCb::ParticleProperty * properties;
    bool topLevel;
  public:
    typedef std::map<ParticleHistory,unsigned int> Map;
  public:
    friend inline std::ostream& operator << (std::ostream& os,
                                             const ParticleHistory& hist) {
      return os << "Particle=" << hist.particle << " history=" << hist.history
                << " topLevel=" << hist.topLevel;
    }
  public:
    inline bool operator== (const ParticleHistory& hist) const {
      return (this->particle == hist.particle &&
              this->history == hist.history);
    }
    inline bool operator!= (const ParticleHistory& hist) const {
      return ! this->operator==(hist);
    }
    inline bool operator<  (const ParticleHistory& hist) const {
      return ( boost::lexical_cast<std::string>(this->particle)+this->history <
               boost::lexical_cast<std::string>(hist.particle)+hist.history   );
    }
  };
  
  typedef std::map<const LHCb::ProtoParticle*,ParticleHistory::Map>
               ParticleProtoMap;
  typedef std::map<const LHCb::ProtoParticle*,const LHCb::MCParticle*> P2MCP;
  typedef std::map<std::string,ProtoParticle2MCLinker *> ProtoLinkerTESMap;

private:

  // Functions
  void addParticle(const LHCb::Particle* particle,
                   const LHCb::Particle* firstParticle,
                   const unsigned int recurCount = 0,
                   const std::string & history= "",
                   const bool toplevel = true) const;
  const LHCb::ParticleProperty* partProp(const LHCb::ParticleID& id) const;
  bool selectProto(const LHCb::ProtoParticle* proto) const;
  ProtoParticle2MCLinker* protoLinker(const LHCb::ProtoParticle* proto) const;
  Particle2MCLinker* particleLinker(const LHCb::Particle*) const;
  const LHCb::MCParticle* mcParticle(const LHCb::Particle* part) const;
  const LHCb::MCParticle* mcParticle_forward(const LHCb::ProtoParticle* proto) 
                          const;
  const LHCb::MCParticle* mcParticle_reverse(const LHCb::ProtoParticle* proto) 
                          const;
  inline const LHCb::MCParticle* mcParticle(const LHCb::ProtoParticle* proto)
                                 const {
    return((0 != proto->charge() && m_useRMCPcharged) ||
           (0 == proto->charge() && m_useRMCPneutral) ?
           mcParticle_reverse(proto) : mcParticle_forward(proto));
  }
  inline std::string objectLocation(const DataObject* pObj) const {
    return(!pObj ? "Null DataObject !" : (pObj->registry() ? 
                                          pObj->registry()->identifier() : 
                                          "UnRegistered"));
  }
  inline std::string objectLocation(const ContainedObject* pObj) const {
    return(pObj ? objectLocation(pObj->parent()) : "Null ContainedObject !");
  }
  inline double minMCWeight(const LHCb::Particle* part) const {
    return(part->isBasicParticle() ? m_minAssWeightComp : 
           minMCWeight(part->proto()));
  }
  inline double minMCWeight(const LHCb::ProtoParticle* proto) const {
    return(proto == NULL ? 1.0 :
           (0 != proto->charge() ? m_minAssWeightCh : m_minAssWeightNu));
  }
  const Rich::Rec::MC::IMCTruthTool * truthTool() const {
    if(!m_truthTool) {
      m_truthTool = 
        tool<Rich::Rec::MC::IMCTruthTool>("Rich::Rec::MC::MCTruthTool",
                                          "RichRecMCTruth");
    }
    return m_truthTool;
  }
  const LHCb::RecVertex* closestPV(const LHCb::Particle* part);

  // Variables 
  std::string m_inputPVsName;
  LHCb::RecVertices* m_inputPVs;
  double m_trkChi2Cut;
  double m_ipCut;
  double m_ipChi2Cut;
  double m_piNNCut;
  double m_pNNCut;
  double m_kNNCut;
  double m_ghostNNCut;
  double m_partPCut;
  double m_partPtCut;
  double m_chargedPartPtCut;
  double m_neutralPartECut;
  int m_partsToCheck;
  const IDVAlgorithm* m_dva;
  const IDistanceCalculator* m_DOCA;
  mutable const Rich::Rec::MC::IMCTruthTool* m_truthTool;
  IMCParticleSelector* m_mcSel;
  ITrackSelector* m_trSel;
  mutable Particle2MCLinker* m_particleLinker;
  mutable ParticleProtoMap m_partProtoMap;
  mutable ProtoLinkerTESMap m_protoLinker;
  IMCReconstructible* m_mcRec;
  bool m_useRMCPcharged;
  bool m_useRMCPneutral;
  double m_minAssWeightCh;
  mutable P2MCP m_cp2mcp;
  mutable P2MCP m_np2mcp;
  double m_minAssWeightNu;
  double m_minAssWeightComp;
  int m_nSelParts;
  int m_nClones;
  int m_cctClones;
  CloneCheckerTool* m_cctool;
};
//========================================================================//
#endif // USER_CloneChecker_H
