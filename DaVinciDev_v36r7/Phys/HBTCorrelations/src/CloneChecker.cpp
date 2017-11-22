// Include files:
// STL
#include <string>
#include <vector>
// GSL
#include "gsl/gsl_math.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IAlgTool.h"
// from Kernel
#include <Kernel/GetIDVAlgorithm.h>
#include "Kernel/DaVinciAlgorithm.h"
// from Event 
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/RecHeader.h"
#include "DaVinciUtils/Functions.h"
// Local
#include "CloneCheckerTool.h"
#include "CloneChecker.h"

using namespace LHCb;
using namespace Gaudi::Units;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : CloneChecker
// 2014-02-10 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(CloneChecker);
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CloneChecker::CloneChecker(const std::string& name, ISvcLocator* pSvcLocator) 
  : DaVinciAlgorithm(name,pSvcLocator)
{
  declareProperty("InputPVs", m_inputPVsName = 
                  LHCb::RecVertexLocation::Primary);
  declareProperty("TrkChi2",      m_trkChi2Cut       = 3.0);
  declareProperty("IP",           m_ipCut            = 0.3);
  declareProperty("IPChi2",       m_ipChi2Cut        = 3.0);
  declareProperty("piNN",         m_piNNCut          = 0.2);
  declareProperty("pNN",          m_pNNCut           = 0.1);
  declareProperty("kNN",          m_kNNCut           = 0.2);
  declareProperty("ghostNN",      m_ghostNNCut       = 0.3);
  declareProperty("partP",        m_partPCut         = 0.0); 
  declareProperty("partPt",       m_partPtCut        = 0.1);
  declareProperty("jetCPartPt",   m_chargedPartPtCut = 0.1);
  declareProperty("jetNPartE",    m_neutralPartECut  = 2.0);
  declareProperty("partsToCheck", m_partsToCheck     = 1);
  declareProperty("MinChargedProtoPMCAssWeight",  m_minAssWeightCh   = 0.0);
  declareProperty("MinNeutralProtoPMCAssWeight",  m_minAssWeightNu   = 0.0);
  declareProperty("MinCompositePartMCAssWeight",  m_minAssWeightComp = 0.0);
  declareProperty("UseReversePToMCPCharged",      m_useRMCPcharged  = false);
  declareProperty("UseReversePToMCPNeutral",      m_useRMCPneutral  = true);
  m_dva = 0;
  m_DOCA = 0;
  m_cctool = 0;
}

//=============================================================================
// Destructor
//=============================================================================
CloneChecker::~CloneChecker() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CloneChecker::initialize() 
{
  StatusCode sc = DaVinciAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  // Track selector
  m_trSel = tool<ITrackSelector>("TrackSelector","TrackSelector",this);
  // Parent DVAlgorithm
  m_dva = Gaudi::Utils::getIDVAlgorithm(contextSvc());
  if(!m_dva) {
    error() << "Couldn't get parent DVAlgorithm" << endreq;
    return StatusCode::FAILURE;
  }
  // DOCA distance calculator
  m_DOCA = m_dva->distanceCalculator();
  if(!m_DOCA) {
    error() << "Unable to retrieve DistanceCalculator tool" << endreq;
    return StatusCode::FAILURE;
  }
  m_cctool = tool<CloneCheckerTool>("CloneCheckerTool","CloneCheckerTool");
  if(!m_cctool) {
    error() << "Unable to retrieve CloneChecker tool" << endreq;
    return StatusCode::FAILURE;
  }

  m_cctClones = 0;
  m_nSelParts = 0;
  m_nClones = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode CloneChecker::execute()
{

  setFilterPassed(false);
  
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS;
  // Primary vertices
  if(!exist<RecVertices>(m_inputPVsName)) return StatusCode::SUCCESS;
  m_inputPVs = get<LHCb::RecVertices>(m_inputPVsName);
  if(!m_inputPVs->size()) return StatusCode::SUCCESS;
  if(m_inputPVs->size() > 4) return StatusCode::SUCCESS;
  // Particles
  Particle::ConstVector inputParts = this->i_particles();
  std::vector<const LHCb::Particle*> parts;
  for(Particle::ConstVector::const_iterator ip = inputParts.begin();
      ip != inputParts.end(); ip++) {
    parts.push_back(*ip);
  }
  ////////// Preselection
  std::vector<const LHCb::Particle*> pions;
  std::vector<const LHCb::Particle*> kaons;
  std::vector<const LHCb::Particle*> protons;
  std::vector<const LHCb::Particle*> allparts;
  for(std::vector<const LHCb::Particle*>::const_iterator ip = parts.begin(); 
      ip != parts.end(); ip++) {
    const LHCb::Particle* part = *ip;
    if(part->charge() == 0) continue;
    if(part->proto() == NULL) continue;
    if(part->proto()->track() == NULL) continue;
    if(part->proto()->track()->type() != Track::Long) continue;
    if(part->proto()->track()->chi2PerDoF() > m_trkChi2Cut) continue;   
    if(part->p() < m_partPCut) continue;
    if(part->pt() < m_partPtCut) continue;
    const ProtoParticle* proto = part->proto();
    const LHCb::RecVertex* bestPV = closestPV(part);
    if(!bestPV) continue;
    double impPar = -1.0;
    double ipChi2 = -1.0;
    sc = m_DOCA->distance(part,bestPV,impPar,ipChi2);
    if(impPar > m_ipCut) continue;
    if(ipChi2 > m_ipChi2Cut) continue;
    double piNN     = proto->info(ProtoParticle::ProbNNpi,   -1000.0);
    double kNN      = proto->info(ProtoParticle::ProbNNk,    -1000.0);
    double pNN      = proto->info(ProtoParticle::ProbNNp,    -1000.0);
    double ghostNN  = proto->info(ProtoParticle::ProbNNghost,-1000.0);
    if(ghostNN > m_ghostNNCut) continue;  
    // Pions, kaons, protons by ProbNN
    if(piNN > m_piNNCut) pions.push_back(part);
    if(kNN > m_kNNCut) kaons.push_back(part);
    if(pNN > m_pNNCut) protons.push_back(part);
    allparts.push_back(part);
  }
  /////////// CHECK FOR CLONES
  //std::cout << "Nr of preselected pions: "   << pions.size() << std::endl;
  //std::cout << "Nr of preselected kaons: "   << kaons.size() << std::endl;
  //std::cout << "Nr of preselected protons: " << protons.size() << std::endl;
  std::vector<const LHCb::Particle*> partsTC;
  if(m_partsToCheck == 1) partsTC = pions;
  if(m_partsToCheck == 2) partsTC = kaons;
  if(m_partsToCheck == 3) partsTC = protons;
  if(!partsTC.size()) return StatusCode::SUCCESS;
  // Clear the maps
  m_partProtoMap.clear();
  m_np2mcp.clear();
  m_cp2mcp.clear();
  // Fill the maps
  m_nSelParts = m_nSelParts + partsTC.size();
  for(std::vector<const LHCb::Particle*>::const_iterator ip = partsTC.begin(); 
      ip != partsTC.end(); ip++) {
    const LHCb::Particle* part = *ip;
    const std::string loc = objectLocation(*ip);
    //std::cout << "Found input Particle at: " << loc << std::endl;
    addParticle(part,part);
  }
  typedef std::set<std::string> Locations;
  // Proto TES locations found
  Locations protoLocations;
  // MCParticle locations found
  Locations mcPLocations;
  // Already used MCParticles for each each ProtoParticle location
  std::map<const std::string,std::set<const LHCb::MCParticle*> > usedMCPs;
  // Loop over the final list of protos and associated particles
  for(ParticleProtoMap::const_iterator iPM = m_partProtoMap.begin();
      iPM != m_partProtoMap.end(); ++iPM) {
    if (!selectProto((*iPM).first)) continue;
    const std::string& protoTesLoc = objectLocation((*iPM).first);
    protoLocations.insert(protoTesLoc);
    const LHCb::MCParticle* mcPart = mcParticle((*iPM).first);
    if(mcPart) mcPLocations.insert(objectLocation(mcPart));
    for(ParticleHistory::Map::const_iterator iPart = (*iPM).second.begin();
        iPart != (*iPM).second.end(); ++iPart) {
      const std::string& tesLoc = objectLocation((*iPart).first.firstParticle);
      // Check if it is a clone
      const std::string cloneKey(tesLoc+protoTesLoc);
      const bool isClone = (usedMCPs[cloneKey].find(mcPart) 
                            != usedMCPs[cloneKey].end());
      // Add to list of seen MCPs
      if(isClone) {
        //std::cout << "Clone found " << std::endl;
        m_nClones++;
      }
      usedMCPs[cloneKey].insert(mcPart);
    }
  }
  // Use clone checker tool
  for(std::vector<const LHCb::Particle*>::const_iterator ip = partsTC.begin(); 
      ip != partsTC.end(); ip++) {
    if(m_cctool->hasClone(*ip,pions)) {
      m_cctClones++;
    }
  }
  // Final printing
  //std::cout << "Nr of preselected parts: " << m_nSelParts << std::endl;
  //std::cout << "Nr of found ghosts: " << m_nClones << std::endl;

  return StatusCode::SUCCESS;
}


//=============================================================================
// Finalization   
//=============================================================================
StatusCode CloneChecker::finalize() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  double nSelParts  = m_nSelParts;
  double nClones    = m_nClones;
  double cloneFract = 0.0;
  if(nSelParts > 0.0) cloneFract = nClones / nSelParts;
  info() << "=====================================================" << endmsg;
  info() << "FRACTION OF CLONES"                                    << endmsg;
  info() << "=====================================================" << endmsg;
  info() << "Nr of clones / Nr of selected particles: " 
         << nClones << " / " << m_nSelParts << " = " 
         << cloneFract << endmsg;
  info() << "Clones from CloneCheckerTool (Each clone from the clone pair enters) = " << m_cctClones << endmsg;
  info() << "=====================================================" << endmsg;

  return DaVinciAlgorithm::finalize();
}


//=============================================================================
// Add particle info  
//=============================================================================
void CloneChecker::addParticle(const LHCb::Particle* particle,
                               const LHCb::Particle* firstParticle,
                               const unsigned int recurCount,
                               const std::string & history,
                               const bool toplevel) const {
  if(recurCount > 99999) {
    Warning("Recursive limit. Aborting Particle tree scan" ).ignore();
    return;
  }
  // Get the history for the particle
  const LHCb::ParticleProperty * prop = partProp(particle->particleID());
  if(!prop) { 
    return;
  }
  // Does this particle have a ProtoParticle. i.e. it's stable
  const LHCb::ProtoParticle * proto = particle->proto();
  // If yes, add to map
  if(proto) {
    ++(m_partProtoMap[proto])[ParticleHistory(particle,firstParticle,
                                              history + prop->particle(),
                                              prop, toplevel)];
  } else if(!particle->isBasicParticle()) {
    // Loop over descendants and to add them instead
    const LHCb::Particle::ConstVector& daughters = particle->daughtersVector();
    for(LHCb::Particle::ConstVector::const_iterator iP = daughters.begin();
        iP != daughters.end(); ++iP) {
      addParticle(*iP, firstParticle, recurCount+1,
                  history + prop->particle() + " -> ", false);
    }
  }
}


//=============================================================================
// ParticleProperty object for a given ParticleID
//=============================================================================
const LHCb::ParticleProperty* CloneChecker::partProp(
                              const LHCb::ParticleID& id) const {
  const LHCb::ParticleProperty * prop = ppSvc()->find(id);
  if(NULL == prop) {
    std::ostringstream mess;
    mess << "LHCb::ParticleProperty missing for ParticleID " << id;
    Warning(mess.str(), StatusCode::SUCCESS,1);
  }
  return prop;
}


//=============================================================================
// ProtoParticle selection
//=============================================================================
bool CloneChecker::selectProto(const LHCb::ProtoParticle* proto) const {
  bool OK(true);
  if(0 != proto->charge()) {
    // Charged proto selection
    const LHCb::Track* track = proto->track();
    if(!track || !m_trSel->accept(*track)) OK = false;
  }
  return OK;
}


//=============================================================================
// MCParticle for a given ProtoParticle (using best forward MCP -> P assoc.)
//=============================================================================
const LHCb::MCParticle* CloneChecker::mcParticle_forward(
                        const LHCb::ProtoParticle* proto) const {
  const LHCb::MCParticle * mcP(NULL);
  if(proto->track()) {
    // For charged tracks use the RICH tool due to problems with ProtoLinkers
    mcP = truthTool()->mcParticle(proto->track(),minMCWeight(proto));
  } else {
    // Get the appropriate linker
    ProtoParticle2MCLinker* links = protoLinker(proto);
    if(links) {
      double bestWeight(-999999);
      const LHCb::MCParticle * tmpMCP(links->firstMCP(proto));
      while(NULL != tmpMCP) {
        if(links->weight() > bestWeight &&
           links->weight() > minMCWeight(proto)) {
          bestWeight = links->weight();
          mcP = tmpMCP;
        }
        tmpMCP = links->next();
      }
    }
  }
  return mcP;
}


//=============================================================================
// MCParticle for a given ProtoParticle (using best reverse MCP -> P assoc.)
//=============================================================================
const LHCb::MCParticle* CloneChecker::mcParticle_reverse(
                        const LHCb::ProtoParticle* proto) const {
  // Correct map
  P2MCP* pmap = (0 != proto->charge() ? &m_cp2mcp : &m_np2mcp);
  if(pmap->empty()) {
    // Get the appropriate linker
    ProtoParticle2MCLinker* links = protoLinker(proto);
    if(links) {
      // Loop over all MCPs
      const LHCb::MCParticles* mcps = 
                  get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
      for(LHCb::MCParticles::const_iterator iMCP = mcps->begin();
          iMCP != mcps->end(); ++iMCP) {
        // Check charge
        const int charge = (*iMCP)->particleID().threeCharge();
        if((0 == proto->charge() && 0 == charge) ||
           (0 != proto->charge() && 0 != charge)) {
          if(m_mcSel->accept(*iMCP)) {
            double bestWeight(-999999);
            const LHCb::ProtoParticle *P(NULL), *tmpP(links->firstP(*iMCP));
            while(NULL != tmpP) {
              if(links->weightP() > bestWeight &&
                 links->weightP() > minMCWeight(proto)) {
                bestWeight = links->weightP();
                P = tmpP;
              }
              tmpP = links->nextP();
            }
            // Store final best association
            (*pmap)[P] = *iMCP;
          }
        }
      }
    }
  }
  P2MCP::const_iterator p = pmap->find(proto);
  return (p == pmap->end() ? NULL : p->second);
}


//=============================================================================
// MCParticle for a given ProtoParticle
//=============================================================================
const LHCb::MCParticle* CloneChecker::mcParticle(const LHCb::Particle* part)
                                                 const {
  if(part->isBasicParticle()) {
    return mcParticle(part->proto());
  } else {
    // Get the appropriate linker
    Particle2MCLinker* links = particleLinker(part);
    double bestWeight(-999999);
    const LHCb::MCParticle *mcP(NULL), *tmpMCP(links->firstMCP(part));
    while(NULL != tmpMCP) {
      if(links->weight() > bestWeight &&
         links->weight() > minMCWeight(part)) {
        bestWeight = links->weight();
        mcP = tmpMCP;
      }
      tmpMCP = links->next();
    }
    return mcP;
  }
}


//=============================================================================
// Access the ProtoParticle Linker appropriate for the given ProtoParticle
//=============================================================================
ProtoParticle2MCLinker * CloneChecker::protoLinker(
                         const LHCb::ProtoParticle* proto) const {
  if(!proto) return NULL;
  const std::string& loc = objectLocation(proto);
  ProtoParticle2MCLinker *& linker = m_protoLinker[loc];
  if(!linker) {
    linker = new ProtoParticle2MCLinker(this,(0 != proto->charge() ?
                                              Particle2MCMethod::ChargedPP : 
                                              Particle2MCMethod::NeutralPP),
                                        std::vector<std::string>(1,loc));
  }
  return linker;
}


//=============================================================================
// Access the Particle Linker appropriate for the given Particle
//=============================================================================
Particle2MCLinker * CloneChecker::particleLinker(const LHCb::Particle*) const {
  if(!m_particleLinker) {
    m_particleLinker = new Particle2MCLinker (this,
                                              Particle2MCMethod::Composite,
                                              std::vector<std::string>(1,""));
  }
  return m_particleLinker;
}


//=============================================================================
// Find the PV with smallest IP chi2
//=============================================================================
const LHCb::RecVertex* CloneChecker::closestPV(const LHCb::Particle* part)
{
  const LHCb::RecVertex* bestPV = 0;
  double smallest = 999999.0;
  LHCb::RecVertices* PVs =
                     get<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary);
  for(LHCb::RecVertices::const_iterator ipv = PVs->begin();
      ipv != PVs->end(); ipv++ ) {
    const LHCb::RecVertex* tmppv = *ipv;
    double tmpip = 9999.0;
    double tmpipchi2 = 1000.0;
    StatusCode sc = distanceCalculator()->distance(part,tmppv,tmpip,tmpipchi2);
    if(sc.isSuccess()) {
      if(tmpipchi2 < smallest) {
        bestPV = tmppv;
        smallest = tmpipchi2;
      }
    }
  }
  return bestPV;
}
