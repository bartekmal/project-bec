#ifndef USER_HBT_H 
#define USER_HBT_H 1
// Include files:
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>
#include <map>
// from Kernel
#include "Kernel/DaVinciAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>
#include "MCInterfaces/IForcedBDecayTool.h"
// from Gaudi
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/VectorMap.h"
// from Event 
#include "Event/RecHeader.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/MCParticle.h"
//#include "Kernel/IJetMaker.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "Kernel/DaVinciHistoAlgorithm.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"
// from Calo
#include "Event/RawEvent.h"
#include "Event/L0DUBase.h"
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"
#include "Event/RawEvent.h"
#include "Event/L0ProcessorData.h"
#include "L0Interfaces/IL0DUFromRawTool.h"
#include "CaloDAQ/ICaloDataProvider.h"
#include "GaudiKernel/IEventTimeDecoder.h"
#include "GaudiKernel/IDataProviderSvc.h"

/** @class HBT HBT.h 
 *  
 *  @author Marcin Kucharczyk
 *  @date   2013-04-08
 */

using namespace ROOT::Math;
using namespace Gaudi;
using namespace LHCb;
using namespace std;

typedef Gaudi::XYZVector EVector;
typedef Gaudi::XYZPoint EPoint;


class HBT : public DaVinciAlgorithm {

public: 
  /// Standard constructor
  HBT(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HBT( );                    ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  class chPart {
  public:
    int index;
    int pvIndex;
    int charge;
    double kDLL;
    double pDLL;
    double piNN;
    double kNN;
    double pNN;
    double ghostNN;
    double pvChi2;
    double xPV;
    double yPV;
    double zPV;
    double pxPV;
    double pyPV;
    double pzPV;
    double e;
    double p;
    double pt;
    double phi;
    double theta;
    double rapidity;
    double eta;
    double ip;
    double ipChi2;
    double trkChi2;
    double cloneDist;
    double chi2PV1;
    double chi2PV2;
    double chi2PV3;
    double chi2PV4;
    double shareVHits;
    int mcID;
    int isMuon;
    int save;
    Gaudi::LorentzVector lVec;
    Gaudi::LorentzVector lVecMC;
  };

  // Functions
  const LHCb::RecVertex* closestPV(const LHCb::Particle* part);
  bool writeEvent(std::vector<int>);
  double calculateCommonVeloHits(const LHCb::Track* track1, const LHCb::Track* track2);
  int fromMCVertex(const LHCb::MCParticle* mcParticle, const LHCb::MCVertex* mcVertex);
  double partMCIP(const LHCb::MCParticle* mcParticle, const LHCb::MCVertex* mcVertex);
  EVector partMCIPVector(const LHCb::MCParticle* mcParticle, const LHCb::MCVertex* mcVertex);

  // Variables  
  std::string m_inputPVsName;
  LHCb::RecVertices* m_inputPVs;
  std::string m_StrippLoc;
  std::string m_StrippingLine;
  int m_Sim;
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
  //  const IJetMaker* m_jetMaker;
  IParticle2MCAssociator* m_linker;
  const IDVAlgorithm* m_dva;
  const IDistanceCalculator* m_DOCA;
  CloneCheckerTool* m_cctool;
  ITriggerTisTos* m_tisTosToolL0;
  ITriggerTisTos* m_tisTosToolHlt1;
  ITriggerTisTos* m_tisTosToolHlt2;
  ICaloDataProvider* m_CaloDataProvider;
  std::vector<chPart> selParts;

  // nTuple 
  NTuple::Item<long>        m_Run;
  NTuple::Item<int>         m_nrPVs;
  NTuple::Item<int>         m_PVMult1;
  NTuple::Item<int>         m_PVMult2;
  NTuple::Item<int>         m_PVMult3;
  NTuple::Item<int>         m_PVMult4;
  NTuple::Item<int>         m_SPDhits;
  NTuple::Item<int>         m_multNoPreSel;
  NTuple::Item<int>         m_multPreSel;
  NTuple::Item<int>         m_N;
  NTuple::Array<int>        m_pvIndex;
  NTuple::Array<int>        m_charge;
  NTuple::Array<double>     m_kDLL;
  NTuple::Array<double>     m_pDLL;
  NTuple::Array<double>     m_piNN;
  NTuple::Array<double>     m_kNN;
  NTuple::Array<double>     m_pNN;
  NTuple::Array<double>     m_ghostNN;
  NTuple::Array<double>     m_chi2BestPV;
  NTuple::Array<double>     m_xBestPV;
  NTuple::Array<double>     m_yBestPV;
  NTuple::Array<double>     m_zBestPV;
  NTuple::Array<double>     m_pxBestPV;
  NTuple::Array<double>     m_pyBestPV;
  NTuple::Array<double>     m_pzBestPV;
  NTuple::Array<double>     m_e;
  NTuple::Array<double>     m_p;
  NTuple::Array<double>     m_pt;
  NTuple::Array<double>     m_phi;
  NTuple::Array<double>     m_theta;
  NTuple::Array<double>     m_rapidity;
  NTuple::Array<double>     m_eta;
  NTuple::Array<double>     m_ip;
  NTuple::Array<double>     m_ipChi2;
  NTuple::Array<double>     m_trkChi2;
  NTuple::Array<double>     m_cloneDist;
  NTuple::Array<double>     m_chi2PV1;
  NTuple::Array<double>     m_chi2PV2;
  NTuple::Array<double>     m_chi2PV3;
  NTuple::Array<double>     m_chi2PV4;
  NTuple::Array<double>     m_shareVHits;
  NTuple::Array<int>        m_MCID;
  NTuple::Array<double>     m_ipFromMCPV;
  NTuple::Array<int>        m_isClone;
  NTuple::Array<int>        m_isMuon;

};
//========================================================================//
#endif // USER_HBT_H
