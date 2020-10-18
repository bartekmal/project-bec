#ifndef HBTPARTICLECLASSES_H
#define HBTPARTICLECLASSES_H

#include <vector>
#include <string>

#include <TLorentzVector.h>

#include "Units.hpp"

//using namespace std;
using std::cout;
using std::endl;

namespace HBT
{

  class Particle
  {
  public:
    int m_pvIndex;
    int m_chargedParticleMultiplicity;
    int m_charge;
    int m_isClone;
    int m_MCID;
    HBT::Units::FloatType m_kDLL;
    HBT::Units::FloatType m_pDLL;
    HBT::Units::FloatType m_piNN;
    HBT::Units::FloatType m_kNN;
    HBT::Units::FloatType m_pNN;
    HBT::Units::FloatType m_ghostNN;
    HBT::Units::FloatType m_xBestPV;
    HBT::Units::FloatType m_yBestPV;
    HBT::Units::FloatType m_zBestPV;
    HBT::Units::FloatType m_pxBestPV;
    HBT::Units::FloatType m_pyBestPV;
    HBT::Units::FloatType m_pzBestPV;
    HBT::Units::FloatType m_chi2BestPV;
    HBT::Units::FloatType m_e;
    HBT::Units::FloatType m_p;
    HBT::Units::FloatType m_pt;
    HBT::Units::FloatType m_phi;
    HBT::Units::FloatType m_theta;
    HBT::Units::FloatType m_rapidity;
    HBT::Units::FloatType m_eta;
    HBT::Units::FloatType m_ip;
    HBT::Units::FloatType m_ipChi2;
    HBT::Units::FloatType m_trkChi2;
    HBT::Units::FloatType m_cloneDist;
    HBT::Units::FloatType m_chi2PV1;
    HBT::Units::FloatType m_chi2PV2;
    HBT::Units::FloatType m_chi2PV3;
    HBT::Units::FloatType m_chi2PV4;

    int m_PID;
    HBT::Units::FloatType m_mass;
    TLorentzVector m_fourVec;
    TLorentzVector m_transverse4Vec;

    //methods
    TLorentzVector build4V(const HBT::Units::FloatType &p, const HBT::Units::FloatType &pt, const HBT::Units::FloatType &phi, const HBT::Units::FloatType &m);
    TLorentzVector buildTrans4V(const HBT::Units::FloatType &pt, const HBT::Units::FloatType &phi);

    //constructor
    Particle(const int &part_i, const bool &isMC, const int &particlePIDCode, const HBT::Units::FloatType &particleMass) : m_pvIndex(pvIndex[part_i]),
                                                                                                                           m_chargedParticleMultiplicity(PVMult1),
                                                                                                                           m_charge(charge[part_i]),
                                                                                                                           m_isClone(isClone[part_i]),
                                                                                                                           m_MCID(MCID[part_i]),
                                                                                                                           m_kDLL(kDLL[part_i]),
                                                                                                                           m_pDLL(pDLL[part_i]),
                                                                                                                           m_piNN(isMC ? fabs(piNNcor[part_i]) : piNN[part_i]),
                                                                                                                           m_kNN(isMC ? fabs(kNNcor[part_i]) : kNN[part_i]),
                                                                                                                           m_pNN(isMC ? fabs(pNNcor[part_i]) : pNN[part_i]),
                                                                                                                           m_ghostNN(ghostNN[part_i]),
                                                                                                                           m_xBestPV(xBestPV[part_i]),
                                                                                                                           m_yBestPV(yBestPV[part_i]),
                                                                                                                           m_zBestPV(zBestPV[part_i]),
                                                                                                                           m_pxBestPV(pxBestPV[part_i]),
                                                                                                                           m_pyBestPV(pyBestPV[part_i]),
                                                                                                                           m_pzBestPV(pzBestPV[part_i]),
                                                                                                                           m_chi2BestPV(chi2BestPV[part_i]),
                                                                                                                           m_e(e[part_i]),
                                                                                                                           m_p(p[part_i]),
                                                                                                                           m_pt(pt[part_i]),
                                                                                                                           m_phi(phi[part_i]),
                                                                                                                           m_theta(theta[part_i]),
                                                                                                                           m_rapidity(rapidity[part_i]),
                                                                                                                           m_eta(eta[part_i]),
                                                                                                                           m_ip(ip[part_i]),
                                                                                                                           m_ipChi2(ipChi2[part_i]),
                                                                                                                           m_trkChi2(trkChi2[part_i]),
                                                                                                                           m_cloneDist(cloneDist[part_i]),
                                                                                                                           m_chi2PV1(chi2PV1[part_i]),
                                                                                                                           m_chi2PV2(chi2PV2[part_i]),
                                                                                                                           m_chi2PV3(chi2PV3[part_i]),
                                                                                                                           m_chi2PV4(chi2PV4[part_i]),

                                                                                                                           m_PID(particlePIDCode),
                                                                                                                           m_mass(particleMass),
                                                                                                                           m_fourVec(build4V(m_p, m_pt, m_phi, m_mass)),
                                                                                                                           m_transverse4Vec(buildTrans4V(m_pt, m_phi))

    {
    }
  }; // end of class Particle

  class ParticlePair
  {
  public:
    int m_pvIndexParticle1;
    int m_pvIndexParticle2;
    int m_chargeParticle1;
    int m_chargeParticle2;
    HBT::Units::FloatType m_pt1;
    HBT::Units::FloatType m_pt2;

    int m_pairMCID;
    HBT::Units::FloatType m_invariantMass;
    HBT::Units::FloatType m_chargedParticleMultiplicity;
    HBT::Units::FloatType m_kt;
    HBT::Units::FloatType m_slopeDiffX;
    HBT::Units::FloatType m_slopeDiffY;

    HBT::Units::FloatType m_Q_LAB;
    HBT::Units::FloatType m_deltaPhi_LAB;
    HBT::Units::FloatType m_deltaRapidity_LAB;
    HBT::Units::FloatType m_deltaPt_LAB;
    HBT::Units::FloatType m_Q_LCMS;
    HBT::Units::FloatType m_Q_long;
    HBT::Units::FloatType m_Q_side;
    HBT::Units::FloatType m_Q_out;
    HBT::Units::FloatType m_deltaPhi_LCMS;
    HBT::Units::FloatType m_deltaRapidity_LCMS;
    HBT::Units::FloatType m_deltaPt_LCMS;

    //methods
    //assign particle MCID to a pair if both partcles have same MCID (fabs); otherwise assign '0'
    int assignPairMCID(const HBT::Particle &part1, const HBT::Particle &part2);
    HBT::Units::FloatType calculateInvariantMass(const HBT::Particle &part1, const HBT::Particle &part2);
    HBT::Units::FloatType calculateAveragePairTransverseMomentum(const HBT::Particle &part1, const HBT::Particle &part2);
    inline HBT::Units::FloatType getSlopeDifferenceX(const HBT::Particle &p1, const HBT::Particle &p2);
    inline HBT::Units::FloatType getSlopeDifferenceY(const HBT::Particle &p1, const HBT::Particle &p2);
    void buildLAB(const HBT::Particle &part1, const HBT::Particle &part2);
    void buildLCMS(const HBT::Particle &part1, const HBT::Particle &part2);

    //constructor
    ParticlePair(const HBT::Particle &part1, const HBT::Particle &part2) : m_pvIndexParticle1(part1.m_pvIndex),
                                                                           m_pvIndexParticle2(part2.m_pvIndex),
                                                                           m_chargeParticle1(part1.m_charge),
                                                                           m_chargeParticle2(part2.m_charge),
                                                                           m_pt1(part1.m_pt),
                                                                           m_pt2(part2.m_pt),
                                                                           m_pairMCID(assignPairMCID(part1, part2)),
                                                                           m_invariantMass(calculateInvariantMass(part1, part2)),
                                                                           m_chargedParticleMultiplicity(part1.m_chargedParticleMultiplicity),
                                                                           m_kt(calculateAveragePairTransverseMomentum(part1, part2)),
                                                                           m_slopeDiffX(getSlopeDifferenceX(part1, part2)),
                                                                           m_slopeDiffY(getSlopeDifferenceY(part1, part2))

    {
      buildLAB(part1, part2);
      buildLCMS(part1, part2);
    }

  }; // end of class ParticlePair
} // namespace HBT

// helper functions definitions
inline TLorentzVector HBT::Particle::build4V(const HBT::Units::FloatType &p, const HBT::Units::FloatType &pt, const HBT::Units::FloatType &phi, const HBT::Units::FloatType &m)
{

  const HBT::Units::FloatType a = p * p - pt * pt;
  if (a < 0)
  {
    cout << "Error in build4V: negative value detected!" << endl;
    exit(1);
  }
  TLorentzVector v(pt * cos(phi), pt * sin(phi), sqrt(a), sqrt(p * p + m * m));
  return v;
}

inline TLorentzVector HBT::Particle::buildTrans4V(const HBT::Units::FloatType &pt, const HBT::Units::FloatType &phi)
{
  TLorentzVector v(pt * cos(phi), pt * sin(phi), 0.0, pt);
  return v;
}

/*
Get interesting variables in the LAB frame ( delta Phi, Rapidity, Pt to be revised before use in analysis )
*/
inline void HBT::ParticlePair::buildLAB(const HBT::Particle &part1, const HBT::Particle &part2)
{

  TLorentzVector LVm = part1.m_fourVec - part2.m_fourVec;
  m_Q_LAB = sqrt(-LVm.M2());
  m_deltaPhi_LAB = fabs(part1.m_phi - part2.m_phi);
  if (m_deltaPhi_LAB > HBT::Units::Pi)
    m_deltaPhi_LAB = HBT::Units::TwoPi - m_deltaPhi_LAB;
  m_deltaRapidity_LAB = fabs(part1.m_fourVec.Rapidity() - part2.m_fourVec.Rapidity());
  m_deltaPt_LAB = fabs(part1.m_pt - part2.m_pt);
}

inline int HBT::ParticlePair::assignPairMCID(const HBT::Particle &part1, const HBT::Particle &part2)
{
  return fabs(part1.m_MCID) == fabs(part2.m_MCID) ? fabs(part1.m_MCID) : 0;
}

inline HBT::Units::FloatType HBT::ParticlePair::calculateInvariantMass(const HBT::Particle &part1, const HBT::Particle &part2)
{
  const TLorentzVector LVp = part1.m_fourVec + part2.m_fourVec;
  return sqrt(LVp.M2());
}

inline HBT::Units::FloatType HBT::ParticlePair::calculateAveragePairTransverseMomentum(const HBT::Particle &part1, const HBT::Particle &part2)
{

  const auto vec1 = part1.m_fourVec.Vect();
  const auto vec2 = part2.m_fourVec.Vect();

  return fabs((vec1 + vec2).Perp()) / 2.0;
}

inline HBT::Units::FloatType HBT::ParticlePair::getSlopeDifferenceX(const HBT::Particle &p1, const HBT::Particle &p2)
{
  const HBT::Units::FloatType t1 = p1.m_fourVec.Px() / p1.m_fourVec.Pz();
  const HBT::Units::FloatType t2 = p2.m_fourVec.Px() / p2.m_fourVec.Pz();
  return fabs(t1 - t2);
}

inline HBT::Units::FloatType HBT::ParticlePair::getSlopeDifferenceY(const HBT::Particle &p1, const HBT::Particle &p2)
{
  const HBT::Units::FloatType t1 = p1.m_fourVec.Py() / p1.m_fourVec.Pz();
  const HBT::Units::FloatType t2 = p2.m_fourVec.Py() / p2.m_fourVec.Pz();
  return fabs(t1 - t2);
}

/*
Get interesting variables in the LCMS frame ( to be revised before use in analysis )
*/
inline void HBT::ParticlePair::buildLCMS(const HBT::Particle &part1, const HBT::Particle &part2)
{

  //Construct LCMS system ( z-axis as a reference axis )
  TVector3 direction = TVector3(0.0, 0.0, 1.0);
  direction *= (1.0 / direction.Mag());
  TLorentzVector Qp = part1.m_fourVec + part2.m_fourVec;
  TVector3 boost;
  boost.SetX(-1.0 * direction.X() * Qp.X() / Qp.E());
  boost.SetY(-1.0 * direction.Y() * Qp.Y() / Qp.E());
  boost.SetZ(-1.0 * direction.Z() * Qp.Z() / Qp.E());
  TLorentzVector q1Boosted = part1.m_fourVec;
  TLorentzVector q2Boosted = part2.m_fourVec;
  q1Boosted.Boost(boost);
  q2Boosted.Boost(boost);
  TLorentzVector QpBoosted = q1Boosted + q2Boosted;
  TLorentzVector QmBoosted = q1Boosted - q2Boosted;
  TVector3 Q3p = QpBoosted.Vect();
  TVector3 Q3m = QmBoosted.Vect();

  //Get OUT, LONG, SIDE directions
  TVector3 V3Out = Q3p;
  V3Out *= (1.0 / V3Out.Mag()); // out versor
  double vOut = Q3m.Dot(V3Out);
  V3Out *= vOut;

  TVector3 V3Long = direction;
  V3Long *= (1.0 / V3Long.Mag()); // long versor
  double vLong = V3Long.Dot(Q3m);
  V3Long *= vLong;

  TVector3 V3Side = V3Long.Cross(V3Out);
  V3Side *= (1.0 / V3Side.Mag()); // side versor
  double vSide = V3Side.Dot(Q3m);
  V3Side *= vSide;

  //Fill interesting variables in LCMS frame
  m_Q_LCMS = sqrt(QpBoosted.M2() - 4 * q1Boosted.M() * q2Boosted.M());
  m_Q_long = (V3Long.Theta() != 0.0) ? -1.0 * V3Long.Mag() : V3Long.Mag();
  m_Q_side = (V3Side.Theta() != 0.0) ? -1.0 * V3Side.Mag() : V3Side.Mag();
  m_Q_out = (V3Out.Theta() != 0.0) ? -1.0 * V3Out.Mag() : V3Out.Mag();
  m_deltaPhi_LCMS = fabs(q1Boosted.Phi() - q2Boosted.Phi());
  if (m_deltaPhi_LCMS > HBT::Units::Pi)
    m_deltaPhi_LCMS = HBT::Units::TwoPi - m_deltaPhi_LCMS;
  m_deltaRapidity_LCMS = fabs(q1Boosted.Rapidity() - q2Boosted.Rapidity());
  m_deltaPt_LCMS = fabs(q1Boosted.Perp() - q1Boosted.Perp());
}

#endif /* !HBTPARTICLECLASSES_H */
