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
#include "Event/State.h"
#include "Linker/LinkedTo.h"
#include "Event/MCTrackInfo.h"
#include "MCInterfaces/IMCReconstructible.h"
// Local
#include "PVChecker.h"

using namespace LHCb;
using namespace Gaudi::Units;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : PVChecker
// 2014-03-12 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(PVChecker);
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVChecker::PVChecker(const std::string& name, ISvcLocator* pSvcLocator) 
  : DaVinciAlgorithm(name,pSvcLocator)
{
  declareProperty("InputPVs", m_inputPVsName = 
                  LHCb::RecVertexLocation::Primary);
  declareProperty("InputTracks", m_inputTracksName = 
                  LHCb::TrackLocation::Default);
  declareProperty("Presel",       m_presel           = 0);
  declareProperty("TrkChi2",      m_trkChi2Cut       = 3.0);
  declareProperty("IP",           m_ipCut            = 0.3);
  declareProperty("IPChi2",       m_ipChi2Cut        = 3.0);
  declareProperty("piNN",         m_piNNCut          = 0.2);
  declareProperty("pNN",          m_pNNCut           = 0.1);
  declareProperty("kNN",          m_kNNCut           = 0.2);
  declareProperty("ghostNN",      m_ghostNNCut       = 0.3);
  declareProperty("partP",        m_partPCut         = 0.0); 
  declareProperty("partPt",       m_partPtCut        = 0.1);
  m_dva = 0;
  m_DOCA = 0;
}

//=============================================================================
// Destructor
//=============================================================================
PVChecker::~PVChecker() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PVChecker::initialize() 
{
  StatusCode sc = DaVinciAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
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
  m_recPV1       = m_recPV2       = m_recPV3       = m_recPV4       = 0.0;
  m_MCvisPV1     = m_MCvisPV2     = m_MCvisPV3     = m_MCvisPV4     = 0.0;
  m_visPV1       = m_visPV2       = m_visPV3       = m_visPV4       = 0.0;
  m_visPVM1      = m_visPVM2      = m_visPVM3      = m_visPVM4      = 0.0;
  m_visPVP1      = m_visPVP2      = m_visPVP3      = m_visPVP4      = 0.0;
  m_visPVPClose1 = m_visPVPClose2 = m_visPVPClose3 = m_visPVPClose4 = 0.0;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode PVChecker::execute()
{
  setFilterPassed(false); 
  // EventSvc
  IDataProviderSvc* m_EventSvc;
  if(serviceLocator())
    serviceLocator()->service("EventDataSvc",m_EventSvc,true);
  if(!m_EventSvc) {
    error() << "Unable to find EventDataSvc" << endreq;
  }
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS;
  // Get reconstructed primary vertices
  if(!exist<RecVertices>(m_inputPVsName)) return StatusCode::SUCCESS;
  m_inputPVs = get<LHCb::RecVertices>(m_inputPVsName);
  if(!m_inputPVs->size()) return StatusCode::SUCCESS;
  if(m_inputPVs->size() > 4) return StatusCode::SUCCESS;
  // Get tracks 
  m_inputTracks = get<LHCb::Tracks>(m_inputTracksName);
  // MC particles assigned to hasVelo tracks
  MCTrackInfo trInfo(m_EventSvc, msgSvc());
  LinkedTo<LHCb::MCParticle> trackMClink(m_EventSvc,msgSvc(),
					 LHCb::TrackLocation::Default);
  std::vector<const LHCb::MCParticle*> mcParts;
  for(LHCb::Track::Container::const_iterator itr = m_inputTracks->begin(); 
      m_inputTracks->end() != itr; itr++) {
    const LHCb::Track* trk = *itr;
    if(trk->hasVelo()) {
      LHCb::MCParticle* mcPart = trackMClink.first(trk->key());
      if(mcPart) mcParts.push_back(mcPart);
    }
  }
  // Visible MC PV's
  LHCb::MCVertices* mcVertices = 
                    get<LHCb::MCVertices>(LHCb::MCVertexLocation::Default);
  std::vector<const LHCb::MCVertex*> vtcsMCPV;
  for(LHCb::MCVertices::const_iterator itMCV = mcVertices->begin();
      mcVertices->end() != itMCV; itMCV++) {
    const LHCb::MCParticle* motherPart = (*itMCV)->mother();
    if(0 == motherPart) {
      if((*itMCV)->type() == LHCb::MCVertex::ppCollision) {
	const LHCb::MCVertex* mcVtx = *itMCV;
	int isfromMCPV = 0;
	for(std::vector<const LHCb::MCParticle*>::iterator
	      itP = mcParts.begin(); itP != mcParts.end(); itP++) {
	  const LHCb::MCParticle* mcPart = *itP;
	  isfromMCPV = isfromMCPV + fromMCVertex(mcPart,mcVtx);
	}
	if(isfromMCPV > 4) vtcsMCPV.push_back(*itMCV);
      }
    }
  }
  // Particles
  Particle::ConstVector inputParts = this->i_particles();
  std::vector<const LHCb::Particle*> parts;
  for(Particle::ConstVector::const_iterator ip = inputParts.begin();
      ip != inputParts.end(); ip++) {
    parts.push_back(*ip);
  }
  ////////// Preselection
  selParts.clear();
  std::vector<int> kaons;
  std::vector<int> pions;
  std::vector<int> protons;
  int index = -1;
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
    int pvIndex = 0;
    int pvNumber = 0;
    for(std::vector<LHCb::RecVertex*>::const_iterator ipv = m_inputPVs->begin();
        ipv != m_inputPVs->end(); ++ipv) {
      pvNumber++;
      if(bestPV->key() == (*ipv)->key()) pvIndex = pvNumber;
    }
    double piNN     = proto->info(ProtoParticle::ProbNNpi,   -1000.0);
    double kNN      = proto->info(ProtoParticle::ProbNNk,    -1000.0);
    double pNN      = proto->info(ProtoParticle::ProbNNp,    -1000.0);
    double ghostNN  = proto->info(ProtoParticle::ProbNNghost,-1000.0);
    chPart tmpPart;
    tmpPart.pvIndex  = pvIndex;
    tmpPart.save     = 0;
    if(ghostNN > m_ghostNNCut) continue;  
    // Pions, kaons, protons by ProbNN
    bool writeEvt = false;
    // Pions by ProbNN
    if(piNN > m_piNNCut) {
      writeEvt = true;
      pions.push_back(index + 1);
    }
    // Kaons by ProbNN 
    if(kNN > m_kNNCut) {
      writeEvt = true;
      kaons.push_back(index + 1);
    }
    // Protons by NN
    if(pNN > m_pNNCut) {
      writeEvt = true;
      protons.push_back(index + 1);
    }
    if(writeEvt) {
      index++;
      selParts.push_back(tmpPart);
    } 
  }
  if(m_presel == 1) {
    if(!selParts.size()) return StatusCode::SUCCESS;
    bool decPions   = writeEvent(pions);
    bool decKaons   = writeEvent(kaons);
    bool decProtons = writeEvent(protons);
    if((!decPions) && (!decKaons) && (!decProtons)) return StatusCode::SUCCESS;
  }
  //////////
  bool twoMoreClose = false; 
  for(std::vector<LHCb::RecVertex*>::const_iterator ipv = m_inputPVs->begin();
      ipv != m_inputPVs->end(); ++ipv) {
    double recPVx = (*ipv)->position().x() / Gaudi::Units::mm;
    double recPVy = (*ipv)->position().y() / Gaudi::Units::mm;
    double recPVz = (*ipv)->position().z() / Gaudi::Units::mm;
    int nrClose = 0;
    for(std::vector<const LHCb::MCVertex*>::iterator
        it = vtcsMCPV.begin(); it != vtcsMCPV.end(); it++) {
      double xMCPV = (*it)->position().x() / Gaudi::Units::mm;
      double yMCPV = (*it)->position().y() / Gaudi::Units::mm;
      double zMCPV = (*it)->position().z() / Gaudi::Units::mm;
      double dR = sqrt((xMCPV -  recPVx) * (xMCPV -  recPVx) + 
		       (yMCPV -  recPVy) * (yMCPV -  recPVy) +
		       (zMCPV -  recPVz) * (zMCPV -  recPVz));
      if(dR < 3.0) nrClose++;
    }
    if(nrClose > 1) twoMoreClose = true;
  }
  //////////
  if(m_inputPVs->size() == 1) m_recPV1++;
  if(m_inputPVs->size() == 2) m_recPV2++;
  if(m_inputPVs->size() == 3) m_recPV3++;
  if(m_inputPVs->size() == 4) m_recPV4++;
  if(vtcsMCPV.size() == 1) m_MCvisPV1++;
  if(vtcsMCPV.size() == 2) m_MCvisPV2++;
  if(vtcsMCPV.size() == 3) m_MCvisPV3++;
  if(vtcsMCPV.size() == 4) m_MCvisPV4++;
  if((m_inputPVs->size() == 1) && (vtcsMCPV.size() == 1)) m_visPV1++;
  if((m_inputPVs->size() == 2) && (vtcsMCPV.size() == 2)) m_visPV2++;
  if((m_inputPVs->size() == 3) && (vtcsMCPV.size() == 3)) m_visPV3++;
  if((m_inputPVs->size() == 4) && (vtcsMCPV.size() == 4)) m_visPV4++;
  if((m_inputPVs->size() == 1) && (vtcsMCPV.size() < 1)) m_visPVM1++;
  if((m_inputPVs->size() == 2) && (vtcsMCPV.size() < 2)) m_visPVM2++;
  if((m_inputPVs->size() == 3) && (vtcsMCPV.size() < 3)) m_visPVM3++;
  if((m_inputPVs->size() == 4) && (vtcsMCPV.size() < 4)) m_visPVM4++;
  if((m_inputPVs->size() == 1) && (vtcsMCPV.size() > 1)) m_visPVP1++;
  if((m_inputPVs->size() == 2) && (vtcsMCPV.size() > 2)) m_visPVP2++;
  if((m_inputPVs->size() == 3) && (vtcsMCPV.size() > 3)) m_visPVP3++;
  if((m_inputPVs->size() == 4) && (vtcsMCPV.size() > 4)) m_visPVP4++;
  // Nr of recPV's < nr of vis MCPV's
  if((m_inputPVs->size() == 1) && (vtcsMCPV.size() > 1) && twoMoreClose) {
    m_visPVPClose1++;
  }
  if((m_inputPVs->size() == 2) && (vtcsMCPV.size() > 2) && twoMoreClose) {
    m_visPVPClose2++;
  }
  if((m_inputPVs->size() == 3) && (vtcsMCPV.size() > 3) && twoMoreClose) {
    m_visPVPClose3++;
  }
  if((m_inputPVs->size() == 4) && (vtcsMCPV.size() > 4) && twoMoreClose) {
    m_visPVPClose4++;
  }
  return StatusCode::SUCCESS;
}


//=============================================================================
// Finalization   
//=============================================================================
StatusCode PVChecker::finalize() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  info() << "=====================================================" << endmsg;
  info() << "PRIMARY VERTICES"                                      << endmsg;
  info() << "=====================================================" << endmsg;
  info() << "Nr of evts with 1 rec. PV  : " << m_recPV1 << endmsg;
  info() << "Nr of evts with 2 rec. PV's: " << m_recPV2 << endmsg;
  info() << "Nr of evts with 3 rec. PV's: " << m_recPV3 << endmsg;
  info() << "Nr of evts with 4 rec. PV's: " << m_recPV4 << endmsg;
  info() << "=====================================================" << endmsg;
  info() << "Nr of evts with 1 MC vis. PV  : " << m_MCvisPV1 << endmsg;
  info() << "Nr of evts with 2 MC vis. PV's: " << m_MCvisPV2 << endmsg;
  info() << "Nr of evts with 3 MC vis. PV's: " << m_MCvisPV3 << endmsg;
  info() << "Nr of evts with 4 MC vis. PV's: " << m_MCvisPV4 << endmsg;
  info() << "=====================================================" << endmsg;
  info() << "Nr of evts with 1 rec. PV and 1 vis. MCPV      : " 
	 << m_visPV1 << endmsg;
  info() << "Nr of evts with 2 rec. PV's and 2 vis. MCPV's  : " 
	 << m_visPV2 << endmsg;
  info() << "Nr of evts with 3 rec. PV's and 3 vis. MCPV's  : " 
	 << m_visPV3 << endmsg;
  info() << "Nr of evts with 4 rec. PV's and 4 vis. MCPV's  : " 
	 << m_visPV4 << endmsg;
  info() << "=====================================================" << endmsg;
  info() << "Nr of evts with 1 rec. PV and < 1 vis. MCPV      : " 
	 << m_visPVM1 << endmsg;
  info() << "Nr of evts with 2 rec. PV's and < 2 vis. MCPV's  : " 
	 << m_visPVM2 << endmsg;
  info() << "Nr of evts with 3 rec. PV's and < 3 vis. MCPV's  : " 
	 << m_visPVM3 << endmsg;
  info() << "Nr of evts with 4 rec. PV's and < 4 vis. MCPV's  : " 
	 << m_visPVM4 << endmsg;
  info() << "=====================================================" << endmsg;
  info() << "Nr of evts with 1 rec. PV and > 1 vis. MCPV      : " 
	 << m_visPVP1 << endmsg;
  info() << "Nr of evts with 2 rec. PV's and > 2 vis. MCPV's  : " 
	 << m_visPVP2 << endmsg;
  info() << "Nr of evts with 3 rec. PV's and > 3 vis. MCPV's  : " 
	 << m_visPVP3 << endmsg;
  info() << "Nr of evts with 4 rec. PV's and > 4 vis. MCPV's  : " 
	 << m_visPVP4 << endmsg;
  info() << "=====================================================" << endmsg;
  info() << "Nr of evts with 1 rec. PV and > 1 vis. MCPV (close)     : " 
	 << m_visPVPClose1 << endmsg;
  info() << "Nr of evts with 2 rec. PV's and > 2 vis. MCPV's (close) : " 
	 << m_visPVPClose2 << endmsg;
  info() << "Nr of evts with 3 rec. PV's and > 3 vis. MCPV's (close) : " 
	 << m_visPVPClose3 << endmsg;
  info() << "Nr of evts with 4 rec. PV's and > 4 vis. MCPV's (close) : " 
	 << m_visPVPClose4 << endmsg;
  info() << "=====================================================" << endmsg;

  return DaVinciAlgorithm::finalize();
}


//=============================================================================
// MC particle from MC visible PV 
//=============================================================================
int PVChecker::fromMCVertex(const LHCb::MCParticle* mcParticle,
                            const LHCb::MCVertex* mcVertex)
{
  int isDaugh = 0;
  const LHCb::MCVertex* mcVtx = 0;
  const LHCb::MCParticle* motherMC = mcParticle->mother();
  while(motherMC) {
    mcVtx = motherMC->originVertex();
    motherMC = motherMC->mother();
  }
  if(mcVertex == mcVtx) isDaugh = 1;
  return isDaugh;
}


//=============================================================================
// Find the PV with smallest IP chi2
//=============================================================================
const LHCb::RecVertex* PVChecker::closestPV(const LHCb::Particle* part)
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


//=============================================================================
// Write the event  
//=============================================================================
bool PVChecker::writeEvent(std::vector<int> particles)
{
  bool dec = false;
  for(std::vector<int>::iterator part1 = particles.begin(); 
      part1 != particles.end(); part1++) {
    for(std::vector<int>::iterator part2 = particles.begin(); 
        part2 != particles.end(); part2++) {
      if(part1 == part2) continue;
      if(selParts[*part1].pvIndex == selParts[*part2].pvIndex) {
        selParts[*part1].save = 1;
        selParts[*part2].save = 1;
        dec = true;
      }
    }
  }
  if(dec) return true;
  return false;
}
