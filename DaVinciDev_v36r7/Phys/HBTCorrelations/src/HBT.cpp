	// Include files:
	// STL
	#include <string>
	#include <vector>
	// GSL
	#include "gsl/gsl_math.h"
	//from Gaudi
	#include "GaudiKernel/AlgFactory.h"
	#include "GaudiKernel/ToolFactory.h"
	#include "GaudiKernel/SystemOfUnits.h"
	#include "GaudiKernel/IDataProviderSvc.h"
	#include "GaudiKernel/GaudiException.h"
	#include "GaudiKernel/DeclareFactoryEntries.h"
	#include "GaudiKernel/IAlgTool.h"
	//from Kernel
	#include <Kernel/GetIDVAlgorithm.h>
	#include "Kernel/DaVinciAlgorithm.h"
	// #include "Kernel/IJetMaker.h"
	// from Event 
	#include "Event/Track.h"
	#include "Event/RecVertex.h"
	#include "Event/RecHeader.h"
	#include "DaVinciUtils/Functions.h"
	#include "Linker/LinkedTo.h"
	#include "Event/MCTrackInfo.h"
	//Trigger decisions
	#include "Event/L0DUReport.h"
	#include "Event/L0DUBase.h"
	#include "Event/HltDecReport.h"
	#include "Event/HltDecReports.h"
	#include "Kernel/ITriggerTisTos.h"
	#include "CaloDAQ/ICaloDataProvider.h"
	// Local
	#include "CloneCheckerTool.h"
	#include "CloneChecker.h"
	#include "HBT.h"

	using namespace LHCb;
	using namespace Gaudi::Units;
	using namespace std;

	//-----------------------------------------------------------------------------
	// Implementation file for class : HBTCorrelations
	// 2013-04-08 : Marcin Kucharczyk
	//-----------------------------------------------------------------------------

	DECLARE_ALGORITHM_FACTORY(HBT);
	//=============================================================================
	// Standard constructor, initializes variables
	//=============================================================================
	HBT::HBT(const std::string& name, ISvcLocator* pSvcLocator) 
	  : DaVinciAlgorithm(name,pSvcLocator)
	{
	  declareProperty("InputPVs", m_inputPVsName = 
					  LHCb::RecVertexLocation::Primary);
	  declareProperty("Sim",          m_Sim              = 1);
	  declareProperty("TrkChi2",      m_trkChi2Cut       = 3.0);
	  declareProperty("IP",           m_ipCut            = 0.3);
	  declareProperty("IPChi2",       m_ipChi2Cut        = 3.0);
	  declareProperty("piNN",         m_piNNCut          = 0.2);
	  declareProperty("pNN",          m_pNNCut           = 0.1);
	  declareProperty("kNN",          m_kNNCut           = 0.2);
	  declareProperty("ghostNN",      m_ghostNNCut       = 0.3);
	  declareProperty("partP",        m_partPCut         = 0.0); 
	  declareProperty("partPt",       m_partPtCut        = 0.1);
    declareProperty("StrippLoc",      m_StrippLoc = "Strip/Phys/DecReports"); 
    declareProperty("StrippingLine", m_StrippingLine = "StrippingIonProtonMagDownMinBiasLineDecision"); 
	  m_linker = 0;
	  m_dva = 0;
	  m_DOCA = 0;
	}

	//=============================================================================
	// Destructor
	//=============================================================================
	HBT::~HBT() {}; 

	//=============================================================================
	// Initialization
	//=============================================================================
	StatusCode HBT::initialize() 
	{
	  StatusCode sc = DaVinciAlgorithm::initialize();
	  if (!sc) return sc;
	  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
	  // MCParticle to Particle associator
	  m_linker = tool<IParticle2MCAssociator>("MCMatchObjP2MCRelator", this);
	  if(!m_linker) {
		error() << "Unable to retrieve Link Associator tool"<<endreq;
		return StatusCode::FAILURE;
	  }
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
	  // TisTosTool for L0
	  m_tisTosToolL0 = tool<ITriggerTisTos>("L0TriggerTisTos",this);
	  if(!m_tisTosToolL0) {
		fatal() << "Unable to retrieve L0TriggerTisTos"<< endreq;
		return StatusCode::FAILURE;
	  }
	  // TisTosTool for Hlt1
	  m_tisTosToolHlt1 = tool<ITriggerTisTos>("TriggerTisTos", this);
	  if(!m_tisTosToolHlt1) {
		fatal() << "Unable to retrieve TriggerTisTos"<< endreq;
		return StatusCode::FAILURE;
	  }
	  // TisTosTool for Hlt2
	  m_tisTosToolHlt2 = tool<ITriggerTisTos>("TriggerTisTos", this);
	  if(!m_tisTosToolHlt2) {
		fatal() << "Unable to retrieve TriggerTisTos"<< endreq;
		return StatusCode::FAILURE;
	  }
	  // SPD hits
	  m_CaloDataProvider = tool<ICaloDataProvider>("CaloDataProvider","SpdReadoutTool", this);
	  if(!m_CaloDataProvider) {
		fatal() << "Unable to retrieve SpdReadoutTool"<< endreq;
		return StatusCode::FAILURE;
	  }
	  // Ntuple booking -------------------
	  NTuplePtr nt(ntupleSvc(),"FILE1/HBT");
	  if(!nt) {
		nt = ntupleSvc()->book("FILE1/HBT",CLID_ColumnWiseTuple,"HBT");
		if(!nt) {
		  error() << "Unable to book Ntuple" << endreq;
		  return StatusCode::FAILURE;
		}
		// Event
		nt->addItem ("Run",          m_Run);
		nt->addItem ("nrPVs",        m_nrPVs);
		nt->addItem ("PVMult1",      m_PVMult1);
		nt->addItem ("PVMult2",      m_PVMult2);
		nt->addItem ("PVMult3",      m_PVMult3);
		nt->addItem ("PVMult4",      m_PVMult4);
		nt->addItem ("SPDhits",      m_SPDhits);
		nt->addItem ("multNoPreSel", m_multNoPreSel);
		nt->addItem ("multPreSel",   m_multPreSel);
		// Particles
		nt->addItem ("N",            m_N, 0, 2000);
		nt->addItem ("pvIndex",      m_N, m_pvIndex);
		nt->addItem ("charge",       m_N, m_charge);
		nt->addItem ("kDLL",         m_N, m_kDLL);
		nt->addItem ("pDLL",         m_N, m_pDLL);
		nt->addItem ("piNN",         m_N, m_piNN);
		nt->addItem ("kNN",          m_N, m_kNN);
		nt->addItem ("pNN",          m_N, m_pNN);
		nt->addItem ("ghostNN",      m_N, m_ghostNN);
		nt->addItem ("xBestPV",      m_N, m_xBestPV);
		nt->addItem ("yBestPV",      m_N, m_yBestPV);
		nt->addItem ("zBestPV",      m_N, m_zBestPV);
		nt->addItem ("pxBestPV",     m_N, m_pxBestPV);
		nt->addItem ("pyBestPV",     m_N, m_pyBestPV);
		nt->addItem ("pzBestPV",     m_N, m_pzBestPV);
		nt->addItem ("chi2BestPV",   m_N, m_chi2BestPV);
		nt->addItem ("e",            m_N, m_e);
		nt->addItem ("p",            m_N, m_p);
		nt->addItem ("pt",           m_N, m_pt);
		nt->addItem ("phi",          m_N, m_phi);
		nt->addItem ("theta",        m_N, m_theta);
		nt->addItem ("rapidity",     m_N, m_rapidity);
		nt->addItem ("eta",          m_N, m_eta);
		nt->addItem ("ip",           m_N, m_ip);
		nt->addItem ("ipChi2",       m_N, m_ipChi2);
		nt->addItem ("trkChi2",      m_N, m_trkChi2);
		nt->addItem ("cloneDist",    m_N, m_cloneDist);
		nt->addItem ("chi2PV1",      m_N, m_chi2PV1);
		nt->addItem ("chi2PV2",      m_N, m_chi2PV2);
		nt->addItem ("chi2PV3",      m_N, m_chi2PV3);
		nt->addItem ("chi2PV4",      m_N, m_chi2PV4);
		nt->addItem ("shareVHits",   m_N, m_shareVHits);
		nt->addItem ("MCID",         m_N, m_MCID);
		nt->addItem ("isClone",      m_N, m_isClone);
		nt->addItem ("isMuon",       m_N, m_isMuon);
	  } else {
		error() << "Ntuple was already booked" << endreq;
		return StatusCode::FAILURE;
	  }
	  // -------------------- 

	  return StatusCode::SUCCESS;
	}

	//=============================================================================
	// Execution
	//=============================================================================
	StatusCode HBT::execute()
	{
	  setFilterPassed(false);
	  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
	  StatusCode sc = StatusCode::SUCCESS;
	  NTuplePtr nt(ntupleSvc(),"FILE1/HBT");
	  if(!nt) {
		error() << "Unable to book nTuple" << endreq;
		return StatusCode::FAILURE;
	  }
	  // Run number
	  m_Run = -1;
	  if(!m_Sim) {
		RecHeader* recHdr = get<RecHeader>(RecHeaderLocation::Default);
		if(!recHdr) {
		  error() << "Unable to retrieve RecHeader" << endreq;
		  return StatusCode::FAILURE;
		} else {
		  m_Run = recHdr->runNumber();
		}
	  }
	  // Primary vertices
	  if(!exist<RecVertices>(m_inputPVsName)) return StatusCode::SUCCESS;
	  m_inputPVs = get<LHCb::RecVertices>(m_inputPVsName);
	  if(!m_inputPVs->size()) return StatusCode::SUCCESS;
	  if(m_inputPVs->size() != 1) return StatusCode::SUCCESS;
	  m_nrPVs = m_inputPVs->size();
	  // MC PV's
	  std::vector<const LHCb::MCVertex*> vtcsMCPV;
	  if(m_Sim) {
		LHCb::MCVertices* mcVertices =
		  get<LHCb::MCVertices>(LHCb::MCVertexLocation::Default);
		for(LHCb::MCVertices::const_iterator itMCV = mcVertices->begin();
		mcVertices->end() != itMCV; itMCV++) {
		  const LHCb::MCParticle* motherPart = (*itMCV)->mother();
		  if(0 == motherPart) {
		if((*itMCV)->type() == LHCb::MCVertex::ppCollision) {
		  vtcsMCPV.push_back(*itMCV);
		}
		  }
		}
	  }
	  // SPD hits
	  if(!m_CaloDataProvider->getBanks()) return StatusCode::SUCCESS;    
	  const CaloVector<LHCb::CaloAdc>& adcs = m_CaloDataProvider->adcs();
	  m_SPDhits = adcs.size();
	  //Particles
	  Particle::ConstVector inputParts = this->i_particles();
	  // Velo tracks multiplicity bins
	  m_PVMult1 = 0;
	  m_PVMult2 = 0;
	  m_PVMult3 = 0;
	  m_PVMult4 = 0;
	  int pvIndx = 0;
    for(std::vector<LHCb::RecVertex*>::const_iterator
          ipv = m_inputPVs->begin(); ipv != m_inputPVs->end(); ++ipv) {
			pvIndx++;
			for(Particle::ConstVector::const_iterator ip = inputParts.begin();
          ip != inputParts.end(); ip++) {
			  const LHCb::Particle* part = *ip;
			  if(part->charge() == 0) continue;
			  if(part->proto() == NULL) continue;
			  if(part->proto()->track() == NULL) continue;
			  if(!(part->proto()->track()->hasVelo())) continue;
			  const LHCb::RecVertex* bestPV = closestPV(part);
			  if(!bestPV) continue;
			  double impPar = -1.0;
			  double ipChi2 = -1.0;
			  sc = m_DOCA->distance(part,bestPV,impPar,ipChi2);
			  if(bestPV->key() == (*ipv)->key()) {
          if(impPar < 0.5) {
            if(pvIndx == 1) m_PVMult1++;
            if(pvIndx == 2) m_PVMult2++;
            if(pvIndx == 3) m_PVMult3++;
            if(pvIndx == 4) m_PVMult4++;
          }
			  }
			}
    }
		  
	  //take only visible PVs
	  if(m_PVMult1 < 5) return StatusCode::SUCCESS; 
	  
	  //microBias events from minBias line
	  
	  if(!m_Sim) {
      //select minimumBias line
      if(exist<LHCb::HltDecReports>(m_StrippLoc)) {
        LHCb::HltDecReports* reports = get<LHCb::HltDecReports>(m_StrippLoc);
        // All the lines from the stream
        /* 
        std::vector<std::string> names = reports->decisionNames();
        for(std::vector<std::string>::const_iterator s = names.begin();
            s != names.end() ; ++s) {
          std::cout << "Stripp line: " << " " << *s << std::endl;
        }
        */
        int strippMinimumBias = 0;
        const HltDecReport* minimumBiasDown = reports->decReport(m_StrippingLine);      
        if(minimumBiasDown) {
          if(minimumBiasDown->decision()) strippMinimumBias = 1;
        }
        
        if(!strippMinimumBias) return StatusCode::SUCCESS; 
      } else return StatusCode::SUCCESS; 
      
      
      //check microBias decision
      int acceptedMicroBias = 0;
      bool decHlt1MicroBias, tisHlt1MicroBias, tosHlt1MicroBias;
      m_tisTosToolHlt1->triggerTisTos("Hlt1MBMicroBiasVeloDecision",decHlt1MicroBias,tisHlt1MicroBias,tosHlt1MicroBias);
      if(decHlt1MicroBias) acceptedMicroBias = 1;
      if(!acceptedMicroBias) return StatusCode::SUCCESS; 
      
	  }
 

	  // Preselect charged particles with associated long track
	  std::vector<const LHCb::Particle*> parts;
	  for(Particle::ConstVector::const_iterator ip = inputParts.begin();
		  ip != inputParts.end(); ip++) {
		const LHCb::Particle* part = *ip;
		if(part->charge() == 0) continue;
		if(part->proto() == NULL) continue;
		if(part->proto()->track() == NULL) continue;
		if(part->proto()->track()->type() != Track::Long) continue;
		parts.push_back(part);
	  }
	  // For pair of particles calculate rate of common VELO LHCBIDs 
	  std::map<const LHCb::Particle*,double> common_rates;
	  int nparts = parts.size();
	  if (nparts > 1) {
		for(int i = 0; i < nparts - 1; i++) { 
		  const LHCb::Particle* part1 = parts[i];
		  const LHCb::Track* track1 = part1->proto()->track();
		  if(common_rates.count(part1) > 0) {
		if(common_rates.at(part1) < 0.0) continue;
		  }
		  for(int j = i+1; j < nparts; j++) { 
		if(common_rates.count(part1) > 0) {
		  if( common_rates.at(part1) < 0.0) break;
		}
		const LHCb::Particle* part2 = parts[j];
		const LHCb::Track* track2 = part2->proto()->track();
		if(common_rates.count(part2) > 0) {
		  if(common_rates.at(part2) < 0.0) continue;
		}
		double rate = calculateCommonVeloHits(track1,track2);
		if(rate > 0.0) {
		  double rate1, rate2;
		  if(track1->chi2PerDoF() < track2->chi2PerDoF()) {
			rate1 = rate;
			rate2 = -1.0 * rate;
		  } else {
			rate1 = -1.0 * rate;
			rate2 = rate;
		  }
		  common_rates[part1] = rate1;

		  common_rates[part2] = rate2;
		  continue;
		}
		  }
		}
	  }
	  // Preselection
	  selParts.clear();
	  std::vector<const LHCb::Particle*> finalParts;
	  std::vector<int> kaons;
	  std::vector<int> pions;
	  std::vector<int> protons;
	  m_multNoPreSel = 0;
	  m_multPreSel = 0;
	  int index = -1;
	  for(std::vector<const LHCb::Particle*>::const_iterator ip = parts.begin(); 
		  ip != parts.end(); ip++) {
		const LHCb::Particle* part = *ip;
		m_multNoPreSel++;  
		if(part->proto()->track()->chi2PerDoF() > m_trkChi2Cut) continue;   
		if((part->p() / Gaudi::Units::GeV) < m_partPCut) continue;
		if((part->pt() / Gaudi::Units::GeV) < m_partPtCut) continue;
		const ProtoParticle* proto = part->proto();
		const LHCb::RecVertex* bestPV = closestPV(part);
		if(!bestPV) continue;
		// Calculate momentum for bestPV
		Gaudi::XYZVector bestPVMom(0.0,0.0,0.0);
		SmartRefVector<LHCb::Track> pvtracks = bestPV->tracks();
		for(SmartRefVector<LHCb::Track>::iterator trIt = pvtracks.begin();
			pvtracks.end() != trIt; trIt++) {
		  LHCb::Track* track = *trIt;
		  if(!track) continue;
		  if(track->p()) bestPVMom += track->momentum();
		}
		double impPar = -1.0;
		double ipChi2 = -1.0;
		sc = m_DOCA->distance(part,bestPV,impPar,ipChi2);
		if(impPar > m_ipCut) continue;
		if(ipChi2 > m_ipChi2Cut) continue;
		int pvIndex = 0;
		int pvNumber = 0;
		// Take all PV's chi2's
		std::vector<double> pvChi2s;
		double xip = -1.0;
		double xipchi2 = -1.0;
		for(std::vector<LHCb::RecVertex*>::const_iterator 
		ipv = m_inputPVs->begin(); ipv != m_inputPVs->end(); ++ipv) {
		  pvNumber++;
		  if(bestPV->key() == (*ipv)->key()) pvIndex = pvNumber;
		  const LHCb::RecVertex* pv = *ipv;
		  StatusCode scPV = distanceCalculator()->distance(part,pv,xip,xipchi2);
		  if(scPV.isSuccess()) pvChi2s.push_back(xipchi2);
		}
		double chi2PV1 = -1.0;
		double chi2PV2 = -1.0;
		double chi2PV3 = -1.0;
		double chi2PV4 = -1.0;
		if(pvChi2s[0]) chi2PV1 = pvChi2s[0];
		if(pvChi2s[1]) chi2PV2 = pvChi2s[1];
		if(pvChi2s[2]) chi2PV3 = pvChi2s[2];
		if(pvChi2s[3]) chi2PV4 = pvChi2s[3];
		double shareVHits = 0.;
		if(common_rates.find(part) != common_rates.end() ) {
		  shareVHits = common_rates.at(part);
		}
		int mcID = 0;
		Gaudi::LorentzVector lVecMC;
		if(m_Sim) {
		  const MCParticle* mcp =
							m_linker->relatedMCP(part,LHCb::MCParticleLocation::Default);
		  if(mcp) {
		mcID = mcp->particleID().pid();
		lVecMC = mcp->momentum();
		  }
		}
		// Load particles
		chPart tmpPart;
		tmpPart.pvIndex    = pvIndex;
		tmpPart.charge     = part->charge();
		tmpPart.kDLL       = proto->info(ProtoParticle::CombDLLk,   -1000.0);
		tmpPart.pDLL       = proto->info(ProtoParticle::CombDLLp,   -1000.0);
		tmpPart.piNN       = proto->info(ProtoParticle::ProbNNpi,   -1000.0);
		tmpPart.kNN        = proto->info(ProtoParticle::ProbNNk,    -1000.0);
		tmpPart.pNN        = proto->info(ProtoParticle::ProbNNp,    -1000.0);
		tmpPart.ghostNN    = proto->info(ProtoParticle::ProbNNghost,-1000.0);
		tmpPart.xPV        = bestPV->position().x() / Gaudi::Units::mm;
		tmpPart.yPV        = bestPV->position().y() / Gaudi::Units::mm;
		tmpPart.zPV        = bestPV->position().z() / Gaudi::Units::mm;
		tmpPart.pxPV       = bestPVMom.x() / Gaudi::Units::GeV;
		tmpPart.pyPV       = bestPVMom.y() / Gaudi::Units::GeV;
		tmpPart.pzPV       = bestPVMom.z() / Gaudi::Units::GeV;
		tmpPart.pvChi2     = bestPV->chi2PerDoF();
		tmpPart.e          = part->momentum().e() / Gaudi::Units::GeV;
		tmpPart.p          = part->p()            / Gaudi::Units::GeV;
		tmpPart.pt         = part->pt()           / Gaudi::Units::GeV;
		tmpPart.phi        = part->momentum().phi();
		tmpPart.theta      = part->momentum().theta();
		tmpPart.rapidity   = part->momentum().y();
		tmpPart.eta        = part->momentum().eta();
		tmpPart.ip         = impPar;
		tmpPart.ipChi2     = ipChi2;
		tmpPart.trkChi2    = part->proto()->track()->chi2PerDoF();
		tmpPart.cloneDist  = part->proto()->track()->info(LHCb::Track::CloneDist,9999.0);
		tmpPart.chi2PV1    = chi2PV1;
		tmpPart.chi2PV2    = chi2PV2;
		tmpPart.chi2PV3    = chi2PV3;
		tmpPart.chi2PV4    = chi2PV4;
		tmpPart.shareVHits = shareVHits;
		tmpPart.mcID       = mcID;
		int isMuon = 0;
		if(part->proto()->muonPID()) {
		  if(part->proto()->muonPID()->IsMuon()) isMuon = 1;
		}
		tmpPart.isMuon     = isMuon;
		tmpPart.save       = 0;
		tmpPart.lVec = part->momentum();
		tmpPart.lVecMC = lVecMC; 
		//////////
		if(tmpPart.ghostNN > m_ghostNNCut) continue;  
		if(proto->info(ProtoParticle::CombDLLpi,-1000.0) != 0) continue;
		m_multPreSel++;
		//////////
		bool writeEvt = false;
		// Pions by ProbNN
		if(tmpPart.piNN > m_piNNCut) {
		  writeEvt = true;
		  pions.push_back(index + 1);
		}
		// Kaons by ProbNN 
		if(tmpPart.kNN > m_kNNCut) {
		  writeEvt = true;
		  kaons.push_back(index + 1);
		}
		// Protons by NN
		if(tmpPart.pNN > m_pNNCut) {
		  writeEvt = true;
		  protons.push_back(index + 1);
		}
		if(writeEvt) {
		  index++;
		  tmpPart.index = index;
		  selParts.push_back(tmpPart);
		  finalParts.push_back(part);
		} 
	  }
	  if(!selParts.size()) return StatusCode::SUCCESS;
	  bool decPions   = writeEvent(pions);
	  bool decKaons   = writeEvent(kaons);
	  bool decProtons = writeEvent(protons);
	  //////////
	  if((!decPions) && (!decKaons) && (!decProtons)) return StatusCode::SUCCESS;
	  // Fill nTuple --------------------  
	  m_N = 0;
	  for(std::vector<chPart>::iterator ips = selParts.begin(); 
		  ips != selParts.end(); ips++) {
		chPart part = *ips;
		if(part.save) {
		  // Use clone checker tool
		  int isClone = 0;
		  if(m_Sim) {
		if(m_cctool->hasClone(finalParts[m_N],finalParts)) isClone = 1;
		  }
		  m_pvIndex[m_N]    = part.pvIndex;
		  m_charge[m_N]     = part.charge;
		  m_kDLL[m_N]       = part.kDLL;
		  m_pDLL[m_N]       = part.pDLL;
		  m_piNN[m_N]       = part.piNN;
		  m_kNN[m_N]        = part.kNN;
		  m_pNN[m_N]        = part.pNN;
		  m_ghostNN[m_N]    = part.ghostNN;
		  m_xBestPV[m_N]    = part.xPV;
		  m_yBestPV[m_N]    = part.yPV;
		  m_zBestPV[m_N]    = part.zPV;
		  m_pxBestPV[m_N]   = part.pxPV;
		  m_pyBestPV[m_N]   = part.pyPV;
		  m_pzBestPV[m_N]   = part.pzPV;
		  m_chi2BestPV[m_N] = part.pvChi2;
		  m_e[m_N]          = part.e;
		  m_p[m_N]          = part.p;
		  m_pt[m_N]         = part.pt;
		  m_phi[m_N]        = part.phi;
		  m_theta[m_N]      = part.theta;
		  m_rapidity[m_N]   = part.rapidity;
		  m_eta[m_N]        = part.eta;
		  m_ip[m_N]         = part.ip;
		  m_ipChi2[m_N]     = part.ipChi2;
		  m_trkChi2[m_N]    = part.trkChi2;
		  m_cloneDist[m_N]  = part.cloneDist;
		  m_chi2PV1[m_N]    = part.chi2PV1;
		  m_chi2PV2[m_N]    = part.chi2PV2;
		  m_chi2PV3[m_N]    = part.chi2PV3;
		  m_chi2PV4[m_N]    = part.chi2PV4;
		  m_shareVHits[m_N] = part.shareVHits;
		  m_MCID[m_N]       = part.mcID;
		  m_isClone[m_N]    = isClone;
		  m_isMuon[m_N]     = part.isMuon;
		  if(m_N < 2000) m_N++;
		} 
	  }

	  sc = nt->write();
	  if(!sc) err() << "Cannot fill nTuple" << endreq;
	  
	  return StatusCode::SUCCESS;
	}


	//=============================================================================
	// Finalization   
	//=============================================================================
	StatusCode HBT::finalize() {
	  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
	  return DaVinciAlgorithm::finalize();
	}


	//=============================================================================
	// Find the PV with smallest IP chi2
	//=============================================================================
	const LHCb::RecVertex* HBT::closestPV(const LHCb::Particle* part)
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
	bool HBT::writeEvent(std::vector<int> particles)
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


	//=============================================================================
	// Calculate common VELO hits 
	//=============================================================================
	double HBT::calculateCommonVeloHits(const LHCb::Track* ptr1, 
						const LHCb::Track* ptr2)
	{
	  const std::vector< LHCb::LHCbID > & ids1 = ptr1->lhcbIDs();
	  const std::vector< LHCb::LHCbID > & ids2 = ptr2->lhcbIDs();
	  std::vector< LHCb::LHCbID >::const_iterator ih1, ih2;
	  int nhit1 = 0;
	  int nhit2 = 0;
	  int nCommon = 0;
	  for(ih1 = ids1.begin(); ih1 != ids1.end(); ++ih1) {
		if(ih1->isVelo()) nhit1++;
	  }
	  for(ih2 = ids2.begin(); ih2 != ids2.end(); ++ih2) {
		if(ih2->isVelo()) nhit2++;
	  }
	  // Common hits
	  for(ih2 = ids2.begin(); ih2 != ids2.end(); ++ih2) {
		if(!ih2->isVelo()) continue;
		for(ih1 = ids1.begin(); ih1 != ids1.end(); ++ih1) {
		  if(!ih1->isVelo()) continue;
		  if(*ih1 == *ih2) {
		++nCommon;
		break;
		  }
		}
	  }
	  if(nCommon < 1) return 0.0;
	  double rate1 = 0.0;
	  double rate2 = 0.0;
	  if(nhit1 > 0) rate1 = 1.0 * nCommon / nhit1;
	  if(nhit2 > 0) rate2 = 1.0 * nCommon / nhit2;
	  return 0.5 * (rate1 + rate2);
	}


	//=============================================================================
	// MC particle from MC PV
	//=============================================================================
	int HBT::fromMCVertex(const LHCb::MCParticle* mcParticle,
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
	// MC particle IP wrt MC PV
	//=============================================================================
	double HBT::partMCIP(const LHCb::MCParticle* mcParticle,
				 const LHCb::MCVertex* mcVertex)
	{
	  EVector ipVector = partMCIPVector(mcParticle,mcVertex);
	  double ip = sqrt(ipVector.mag2());
	  if(ipVector.z() < 0) ip = -ip;
	  return ip;
	}


	//=============================================================================
	// MC particle IP vector wrt MC PV
	//=============================================================================
	EVector HBT::partMCIPVector(const LHCb::MCParticle* mcParticle,
					const LHCb::MCVertex* mcVertex)
	{
	  EVector direction(mcParticle->momentum().x(),
						mcParticle->momentum().y(),
						mcParticle->momentum().z());
	  EVector dirVersor = direction.unit();
	  EPoint posMCPart(mcParticle->originVertex()->position().x(),
					   mcParticle->originVertex()->position().y(),
					   mcParticle->originVertex()->position().z());
	  EVector distance = posMCPart - mcVertex->position();
	  return dirVersor.Cross(distance.Cross(dirVersor));
	}


	//-----------------------------------------------------------------------------
	// Q in LAB frame                                                       
	//-----------------------------------------------------------------------------
	/*
	void LAB(chPart part1, chPart part2, double& Qp, double& Qm, double& dPhi,
			 double& dY, double& dPt) {
	  TLorentzVector LVp = part1.fourVec + part2.fourVec;
	  TLorentzVector LVm = part1.fourVec - part2.fourVec;
	  Qp = sqrt(LVp.M2() - 4 * part1.mass * part2.mass);
	  Qm = sqrt((-1.0) * LVm.M2());
	  dPhi = fabs(part1.phi - part2.phi);
	  if(dPhi > 3.1415) dPhi = 6.283 - dPhi;
	  dPhi = dPhi * 57.29;
	  dY = fabs(part1.fourVec.Rapidity() - part2.fourVec.Rapidity());
	  dPt = fabs(part1.pt - part2.pt);
	}
	*/
