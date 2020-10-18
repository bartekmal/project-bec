#ifndef HISTOGRAMS_H
#define HISTOGRAMS_H

#include <vector>
#include <string>
#include <cstdio>

#include "Units.hpp"
#include "TreeData.hpp"
#include "HBTParticleClasses.hpp"
#include "CoulombCorrection.hpp"

namespace HBT
{

  namespace Histograms
  {

    struct InputVariable
    {
      enum
      {
        typeInt,
        typeFloat
      } type;
      union
      {
        int *i;
        HBT::Units::FloatType *f;
      } value;

      InputVariable(int *var) : type(typeInt) { value.i = var; };
      InputVariable(HBT::Units::FloatType *var) : type(typeFloat) { value.f = var; };
    };

    struct HistDef
    {
      HBT::Units::TH1FloatType hist;
      InputVariable var;

      HistDef(HBT::Units::TH1FloatType h, int *v) : hist(h), var(InputVariable(v)){};
      HistDef(HBT::Units::TH1FloatType h, HBT::Units::FloatType *v) : hist(h), var(InputVariable(v)){};
    };

    //fill histograms
    void FillHistogramSetWithEventLevelInfo(std::vector<HistDef> &histogramSet);
    void FillHistogramSetWithParticleLevelInfo(std::vector<HistDef> &histogramSet, const int &part_i);
    void FillHistogramSetWithParticlePairLevelInfo(std::vector<HBT::Units::TH1FloatType> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC);
    void FillHistogramSeries2000(std::vector<HBT::Units::TH2FloatType> &histogramSet, const HBT::ParticlePair &pair);
    void FillHistogramSeries3000InBins(std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC);
    void FillHistogramSeries3000InBins(std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity);
    void FillHistogramSeries3000InBins(std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity, const std::vector<HBT::Units::FloatType> &binsOfKt);

    //create histograms
    std::vector<HistDef> CreateHistogramSeries100(std::string setHeader = std::string("10"));
    std::vector<HistDef> CreateHistogramSeries1000(std::string setHeader = std::string("10"));
    std::vector<HBT::Units::TH2FloatType> CreateHistogramSeries2000(const std::string &setHeader = std::string("20"), const std::string &pairType = "all pairs");
    std::vector<HBT::Units::TH1FloatType> CreateHistogramSeries3000(const std::string &setHeader, const std::string &pairType, const int &currentMultiplicityBin = 0, const int &currentKtBin = 0);
    std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> CreateHistogramSeries3000InBins(const std::string &setHeader, const std::string &pairType, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity = std::vector<HBT::Units::FloatType>(), const std::vector<HBT::Units::FloatType> &binsOfKt = std::vector<HBT::Units::FloatType>());
  } // namespace Histograms

} // namespace HBT

/*------------------------ definitions -------------------------*/

inline void HBT::Histograms::FillHistogramSetWithEventLevelInfo(std::vector<HistDef> &histogramSet)
{
  for (auto &el : histogramSet)
  {
    el.hist.Fill(el.var.type == HBT::Histograms::InputVariable::typeInt ? (HBT::Units::FloatType) * (el.var.value.i) : (HBT::Units::FloatType) * (el.var.value.f));
  }
}

inline void HBT::Histograms::FillHistogramSetWithParticleLevelInfo(std::vector<HistDef> &histogramSet, const int &part_i)
{
  for (auto &el : histogramSet)
  {
    el.hist.Fill(el.var.type == HBT::Histograms::InputVariable::typeInt ? (HBT::Units::FloatType)el.var.value.i[part_i] : (HBT::Units::FloatType)el.var.value.f[part_i]);
  }
}

inline void HBT::Histograms::FillHistogramSeries3000InBins(std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC)
{

  HBT::Histograms::FillHistogramSetWithParticlePairLevelInfo(histogramSet[0][0], pair, isMC);
}

inline void HBT::Histograms::FillHistogramSeries3000InBins(std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity)
{

  //useful consts (assume valid bins definition - it is checked at histogram series creation)
  const auto nrOfMultiplicityBins = binsOfMultiplicity.empty() ? 0 : binsOfMultiplicity.size() - 1;
  const auto pairMultiplicity = pair.m_chargedParticleMultiplicity;

  //fill histograms
  for (unsigned int iMultiplicityBins = 0; iMultiplicityBins < nrOfMultiplicityBins; iMultiplicityBins++)
  {

    //find the multiplicity bin of the pair
    if (pairMultiplicity >= binsOfMultiplicity[iMultiplicityBins] && pairMultiplicity < binsOfMultiplicity[iMultiplicityBins + 1])
    {

      HBT::Histograms::FillHistogramSetWithParticlePairLevelInfo(histogramSet[iMultiplicityBins][0], pair, isMC);

      //break the loop once the bin is found
      break;
    }
  }
}

inline void HBT::Histograms::FillHistogramSeries3000InBins(std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity, const std::vector<HBT::Units::FloatType> &binsOfKt)
{

  //useful consts (assume valid bins definition - it is checked at histogram series creation)
  const auto nrOfMultiplicityBins = binsOfMultiplicity.empty() ? 0 : binsOfMultiplicity.size() - 1;
  const auto nrOfKtBins = binsOfKt.empty() ? 0 : binsOfKt.size() - 1;

  const auto pairMultiplicity = pair.m_chargedParticleMultiplicity;
  const auto pairKt = pair.m_kt;

  //fill histograms
  for (unsigned int iMultiplicityBins = 0; iMultiplicityBins < nrOfMultiplicityBins; iMultiplicityBins++)
  {

    //find the multiplicity bin of the pair
    if (pairMultiplicity >= binsOfMultiplicity[iMultiplicityBins] && pairMultiplicity < binsOfMultiplicity[iMultiplicityBins + 1])
    {

      for (unsigned int iKtBins = 0; iKtBins < nrOfKtBins; iKtBins++)
      {

        //find the kT bin of the pair
        if (pairKt >= binsOfKt[iKtBins] && pairKt < binsOfKt[iKtBins + 1])
        {

          HBT::Histograms::FillHistogramSetWithParticlePairLevelInfo(histogramSet[iMultiplicityBins][iKtBins], pair, isMC);

          //break the loop once the bin is found
          break;
        }
      }

      //break the loop once the bin is found
      break;
    }
  }
}

inline void HBT::Histograms::FillHistogramSetWithParticlePairLevelInfo(std::vector<HBT::Units::TH1FloatType> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC)
{

  //general
  histogramSet[0].Fill(pair.m_invariantMass);
  if (isMC)
    histogramSet[1].Fill(pair.m_pairMCID);
  histogramSet[2].Fill(pair.m_chargedParticleMultiplicity);
  histogramSet[3].Fill(pair.m_kt);

  //LAB frame
  histogramSet[4].Fill(pair.m_Q_LAB);
  histogramSet[5].Fill(pair.m_Q_LAB);
  histogramSet[6].Fill(pair.m_Q_LAB);

  // histogramSet[12].Fill( pair.m_deltaPhi_LAB );
  // histogramSet[13].Fill( pair.m_deltaRapidity_LAB );
  // histogramSet[14].Fill( pair.m_deltaPt_LAB );

  // //LCMS frame
  // histogramSet[15].Fill( pair.m_Q_LCMS );
  // histogramSet[16].Fill( pair.m_Q_long );
  // histogramSet[17].Fill( pair.m_Q_side );
  // histogramSet[18].Fill( pair.m_Q_out );

  // histogramSet[19].Fill( pair.m_deltaPhi_LCMS );
  // histogramSet[20].Fill( pair.m_deltaRapidity_LCMS );
  // histogramSet[21].Fill( pair.m_deltaPt_LCMS );
}

inline void HBT::Histograms::FillHistogramSeries2000(std::vector<HBT::Units::TH2FloatType> &histogramSet, const HBT::ParticlePair &pair)
{

  //general
  histogramSet[0].Fill(pair.m_chargedParticleMultiplicity, pair.m_Q_LAB);
  histogramSet[1].Fill(pair.m_kt, pair.m_Q_LAB);
}

inline std::vector<HBT::Histograms::HistDef> HBT::Histograms::CreateHistogramSeries100(std::string setHeader)
{

  std::vector<HistDef> hSet;

  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "0").c_str(), "trkMult", 200, 0.0, 200.0), &multNoPreSel));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "1").c_str(), "PVMult1", 150, 0.0, 150.0), &PVMult1));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "2").c_str(), "SPDhits", 65, 0.0, 650.0), &SPDhits));

  return hSet;
}

inline std::vector<HBT::Histograms::HistDef> HBT::Histograms::CreateHistogramSeries1000(std::string setHeader)
{

  std::vector<HistDef> hSet;

  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"01").c_str(),"pvIndex", 100, 0.0, 100.0), (int*) pvIndex ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"02").c_str(),"charge", 100, -5.0, 5.0), (int*) charge ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"03").c_str(),"kDLL", 100, -25.0, 25.0), (HBT::Units::FloatType*) kDLL ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"04").c_str(),"pDLL", 100, -25.0, 25.0), (HBT::Units::FloatType*) pDLL ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "05").c_str(), "piNN", 100, 0.0, 1.0), (HBT::Units::FloatType *)piNN));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "06").c_str(), "kNN", 100, 0.0, 1.0), (HBT::Units::FloatType *)kNN));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "07").c_str(), "pNN", 100, 0.0, 1.0), (HBT::Units::FloatType *)pNN));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "08").c_str(), "ghostNN", 100, 0., 1.0), (HBT::Units::FloatType *)ghostNN));
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"09").c_str(),"xBestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) xBestPV ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"10").c_str(),"yBestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) yBestPV ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "11").c_str(), "zBestPV", 100, -200, 200), (HBT::Units::FloatType *)zBestPV));
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"12").c_str(),"pxBestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) pxBestPV ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"13").c_str(),"pyBestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) pyBestPV ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"14").c_str(),"pzBestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) pzBestPV ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"15").c_str(),"chi2BestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2BestPV ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"16").c_str(),"e", 100, 0.0, 100.0), (HBT::Units::FloatType*) e ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "17").c_str(), "p", 100, 0.0, 100.0), (HBT::Units::FloatType *)p));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "18").c_str(), "pt", 100, 0.0, 10.0), (HBT::Units::FloatType *)pt));
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"19").c_str(),"phi; [rad]", 100, -HBT::Units::Pi, HBT::Units::Pi), (HBT::Units::FloatType*) phi  ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"20").c_str(),"theta", 100, 0.0, 1.0), (HBT::Units::FloatType*) theta ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"21").c_str(),"rapidity", 100, 0.0, 1.0), (HBT::Units::FloatType*) rapidity ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "22").c_str(), "eta", 100, 1.5, 5.5), (HBT::Units::FloatType *)eta));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "23").c_str(), "ip", 100, 0.0, 1.0), (HBT::Units::FloatType *)ip));
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"24").c_str(),"ipChi2", 100, 0.0, 5.0), (HBT::Units::FloatType*) ipChi2 ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "25").c_str(), "trkChi2", 100, 0.0, 3.0), (HBT::Units::FloatType *)trkChi2));
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"26").c_str(),"chi2PV1", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2PV1 ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"27").c_str(),"chi2PV2", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2PV2 ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"28").c_str(),"chi2PV3", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2PV3 ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"29").c_str(),"chi2PV4", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2PV4 ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"30").c_str(),"isClone", 20, -10.0, 10.0), (HBT::Units::FloatType*) isClone ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"31").c_str(),"cloneDist", 100, -10000.0, 10000.0), (HBT::Units::FloatType*) cloneDist ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "32").c_str(), "piNNcorr", 200, -1.0, 1.0), (HBT::Units::FloatType *)piNNcor));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "33").c_str(), "kNNcorr", 200, -1.0, 1.0), (HBT::Units::FloatType *)kNNcor));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "34").c_str(), "pNNcorr", 200, -1.0, 1.0), (HBT::Units::FloatType *)pNNcor));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "35").c_str(), "shareVHits", 200, -1.0, 1.0), (HBT::Units::FloatType *)shareVHits));
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"36").c_str(),"isMuon", 4, -2.0, 2.0), (HBT::Units::FloatType*) isMuon ) );

  return hSet;
}

inline std::vector<HBT::Units::TH2FloatType> HBT::Histograms::CreateHistogramSeries2000(const std::string &setHeader, const std::string &pairType)
{

  const HBT::Units::FloatType qRangeMin = 0.0;
  const HBT::Units::FloatType qRangeMax = 2.0;
  const HBT::Units::FloatType chargedParticleMultiplicityMax = 150.0;
  const HBT::Units::FloatType averagePairTransverseMomentumMax = 2.0;

  const int histogram2DBinning = 100;
  const int histogram2DBinningForMultiplicity = chargedParticleMultiplicityMax / 2.;

  const std::string tmpPairType = std::string(" ( " + pairType + " )").c_str();

  std::vector<HBT::Units::TH2FloatType> hSet;

  //h2xyy
  //2D 'control' histograms (Q vs Nch, kT etc)
  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "00" + "_" + "0" + "_" + "0").c_str(), std::string("Q vs N_{ch}" + tmpPairType + "; N_{ch}; Q [GeV]").c_str(), histogram2DBinningForMultiplicity, 0., chargedParticleMultiplicityMax, histogram2DBinning, qRangeMin, qRangeMax));
  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "01" + "_" + "0" + "_" + "0").c_str(), std::string("Q vs k_{T}" + tmpPairType + "; k_{T} [GeV]; Q [GeV]").c_str(), histogram2DBinning, 0., averagePairTransverseMomentumMax, histogram2DBinning, qRangeMin, qRangeMax));

  return hSet;
}

inline std::vector<HBT::Units::TH1FloatType> HBT::Histograms::CreateHistogramSeries3000(const std::string &setHeader, const std::string &pairType, const int &currentMultiplicityBin, const int &currentKtBin)
{

  //helpful constants
  const HBT::Units::FloatType qRangeMin = 0.0;
  const HBT::Units::FloatType qRangeMax = 2.0;
  const HBT::Units::FloatType qExtendedRangeMax = 5.0;
  // const HBT::Units::FloatType qLCMSRangeMax = 1.0;
  const int qBinning = 400;
  const int qExtendedBinning = qBinning / 2 * (qExtendedRangeMax / qRangeMax);

  const HBT::Units::FloatType chargedParticleMultiplicityMax = 200.0;
  const HBT::Units::FloatType averagePairTransverseMomentumMax = 2.0;

  const auto tmpPairType = std::string(" ( " + pairType + " )");
  const auto strMultiplicity = std::to_string(currentMultiplicityBin);
  const auto strKt = std::to_string(currentKtBin);

  std::vector<HBT::Units::TH1FloatType> hSet;

  //h3x00 - h3x09
  //general pair variables (kT, pair MCID, invariant mass)
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "00" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("pair invariant mass" + tmpPairType + ";m_{inv} [GeV]; dN/d(m_{inv})").c_str(), qBinning * 2, qRangeMin, qRangeMax * 2));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "01" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("pair by MCID" + tmpPairType + "; pair MCID; dN/d(pair MCID)").c_str(), 20000, -10000., 10000.));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "02" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("pair charged particle multiplicity" + tmpPairType + ";N_{ch}; dN/d(N_{ch})").c_str(), chargedParticleMultiplicityMax, 0.0, chargedParticleMultiplicityMax));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "03" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("average pair transverse momentum" + tmpPairType + ";k_{T} [GeV]; dN/d(k_{T})").c_str(), 100, 0.0, averagePairTransverseMomentumMax));

  //h3x10 - h3x19
  //Q distributions in LAB frame (different binnings; extended Q range)
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "10" + "_" + strMultiplicity + "_" + strKt + "_" + "400").c_str(), std::string("Q_LAB" + tmpPairType + ";Q [GeV]; dN/dQ").c_str(), qBinning, qRangeMin, qRangeMax));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "10" + "_" + strMultiplicity + "_" + strKt + "_" + "200").c_str(), std::string("Q_LAB" + tmpPairType + ";Q [GeV]; dN/dQ").c_str(), qBinning / 2.0, qRangeMin, qRangeMax));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "11" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("Q_LAB" + tmpPairType + ";Q [GeV]; dN/dQ").c_str(), qExtendedBinning, qRangeMin, qExtendedRangeMax));
  //h3x20 - h3x29
  //dY, dPhi, dpT in LAB frame
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"20" + "_" + strMultiplicity + "_" + strKt ).c_str(), std::string( "#Deltaphi_LAB" + tmpPairType  + ";#Delta#phi [rad]; dN/d(#Delta#phi)").c_str(), 100, 0.0, HBT::Units::Pi ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"21" + "_" + strMultiplicity + "_" + strKt ).c_str(), std::string( "#Deltay_LAB" + tmpPairType  + ";#Deltay; dN/d(#Deltay)").c_str(), 100, 0.0, 5.0 ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"22" + "_" + strMultiplicity + "_" + strKt ).c_str(), std::string( "#Deltap_{T}_LAB" + tmpPairType  + ";#Deltap_{T} [GeV]; dN/d(#Deltap_{T})").c_str(), 100, 0.0, 3.0 ) );
  //h3x30 - h3x39
  //Q distributions in LCMS frame (full Q + long/side/out components)
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"30" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string( "Q_LCMS" + tmpPairType  + ";Q [GeV]; dN/dQ").c_str(), qBinning, qRangeMin, qRangeMax ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"31" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string( "Q_LCMS_long" + tmpPairType  + ";Q [GeV]; dN/dQ").c_str(), qBinning, -1.0 * qLCMSRangeMax, qLCMSRangeMax ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"32" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string( "Q_LCMS_side" + tmpPairType  + ";Q [GeV]; dN/dQ").c_str(), qBinning, -1.0 * qLCMSRangeMax, qLCMSRangeMax ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"33" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string( "Q_LCMS_out" + tmpPairType  + ";Q [GeV]; dN/dQ").c_str(), qBinning, -1.0 * qLCMSRangeMax, qLCMSRangeMax ) );
  //h3x40 - h3x49
  //dY, dPhi, dpT in LCMS frame
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"40" + "_" + strMultiplicity + "_" + strKt ).c_str(), std::string( "#Deltaphi_LAB" + tmpPairType  + ";#Delta#phi [rad]; dN/d(#Delta#phi)").c_str(), 100, 0.0, HBT::Units::Pi ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"41" + "_" + strMultiplicity + "_" + strKt ).c_str(), std::string( "#Deltay_LAB" + tmpPairType  + ";#Deltay; dN/d(#Deltay)").c_str(), 100, 0.0, 5.0 ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"42" + "_" + strMultiplicity + "_" + strKt ).c_str(), std::string( "#Deltap_{T}_LAB" + tmpPairType  + ";#Deltap_{T} [GeV]; dN/d(#Deltap_{T})").c_str(), 100, 0.0, 3.0 ) );

  return hSet;
}

inline std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> HBT::Histograms::CreateHistogramSeries3000InBins(const std::string &setHeader, const std::string &pairType, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity, const std::vector<HBT::Units::FloatType> &binsOfKt)
{

  std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> hSet;

  //check arguments validity

  const auto nrOfElementsInMultiplicityList = binsOfMultiplicity.size();
  const auto nrOfElementsInKtList = binsOfKt.size();

  if (nrOfElementsInMultiplicityList == 1)
  {

    printf("Invalid bins difinition: multiplicity bins list must be empty or have at least 2 elements");
    return hSet;
  }

  if (nrOfElementsInKtList == 1)
  {

    printf("Invalid bins difinition: kT bins list must be empty or have at least 2 elements");
    return hSet;
  }

  const auto nrOfMultiplicityBins = binsOfMultiplicity.empty() ? 0 : nrOfElementsInMultiplicityList - 1;
  const auto nrOfKtBins = binsOfKt.empty() ? 0 : nrOfElementsInKtList - 1;

  //option for no bins in multiplicity
  if (binsOfMultiplicity.empty())
  {

    hSet.assign(1, std::vector<std::vector<HBT::Units::TH1FloatType>>());
    hSet[0].push_back(HBT::Histograms::CreateHistogramSeries3000(setHeader, pairType, 0, 0));
  }
  else
  {

    hSet.assign(nrOfMultiplicityBins, std::vector<std::vector<HBT::Units::TH1FloatType>>());

    for (unsigned int iMultiplicity = 0; iMultiplicity < nrOfMultiplicityBins; ++iMultiplicity)
    {

      //option for no bins in kT
      if (binsOfKt.empty())
      {
        hSet[iMultiplicity].push_back(HBT::Histograms::CreateHistogramSeries3000(setHeader, pairType, iMultiplicity + 1, 0));
      }
      else
      {

        for (unsigned int iKt = 0; iKt < nrOfKtBins; ++iKt)
        {

          hSet[iMultiplicity].push_back(HBT::Histograms::CreateHistogramSeries3000(setHeader, pairType, iMultiplicity + 1, iKt + 1));
        }
      }
    }
  }

  return hSet;
}

#endif /* !HISTOGRAMS_H */
