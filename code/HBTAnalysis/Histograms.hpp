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
    void FillHistogramSeries1000InBins(std::vector<std::vector<std::vector<HistDef>>> &histogramSet, const int &part_i);
    void FillHistogramSeries1000InBins(std::vector<std::vector<std::vector<HistDef>>> &histogramSet, const int &part_i, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity);
    void FillHistogramSetWithParticlePairLevelInfo(std::vector<HBT::Units::TH1FloatType> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC);
    void FillHistogramSeries200(std::vector<HBT::Units::TH2FloatType> &histogramSet);
    void Fill2DHistogramSeries1000(std::vector<HBT::Units::TH2FloatType> &histogramSet, const int &part_i);
    void FillHistogramSeries2000(std::vector<HBT::Units::TH2FloatType> &histogramSet, const HBT::ParticlePair &pair);
    void FillHistogramSeries3000InBins(std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC);
    void FillHistogramSeries3000InBins(std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity);
    void FillHistogramSeries3000InBins(std::vector<std::vector<std::vector<HBT::Units::TH1FloatType>>> &histogramSet, const HBT::ParticlePair &pair, const bool &isMC, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity, const std::vector<HBT::Units::FloatType> &binsOfKt);

    //create histograms
    std::vector<HistDef> CreateHistogramSeries100(std::string setHeader = std::string("10"));
    std::vector<HBT::Units::TH2FloatType> CreateHistogramSeries200(const std::string &setHeader = std::string("20"));
    std::vector<HistDef> CreateHistogramSeries1000(std::string setHeader = std::string("10"), const int &currentMultiplicityBin = 0, const int &currentKtBin = 0);
    std::vector<std::vector<std::vector<HistDef>>> CreateHistogramSeries1000InBins(const std::string &setHeader, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity = std::vector<HBT::Units::FloatType>(), const std::vector<HBT::Units::FloatType> &binsOfKt = std::vector<HBT::Units::FloatType>());
    std::vector<HBT::Units::TH2FloatType> Create2DHistogramSeries1000(const std::string &setHeader = std::string("2D_10"));
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

inline void HBT::Histograms::FillHistogramSeries1000InBins(std::vector<std::vector<std::vector<HistDef>>> &histogramSet, const int &part_i)
{
  HBT::Histograms::FillHistogramSetWithParticleLevelInfo(histogramSet[0][0], part_i);
}

inline void HBT::Histograms::FillHistogramSeries1000InBins(std::vector<std::vector<std::vector<HistDef>>> &histogramSet, const int &part_i, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity)
{
  //useful consts (assume valid bins definition - it is checked at histogram series creation)
  const auto nrOfMultiplicityBins = binsOfMultiplicity.empty() ? 0 : binsOfMultiplicity.size() - 1;
  const auto curMultiplicity = PVMult1;

  //fill histograms
  for (unsigned int iMultiplicityBins = 0; iMultiplicityBins < nrOfMultiplicityBins; iMultiplicityBins++)
  {

    //find the multiplicity bin of the pair
    if (curMultiplicity >= binsOfMultiplicity[iMultiplicityBins] && curMultiplicity < binsOfMultiplicity[iMultiplicityBins + 1])
    {

      HBT::Histograms::FillHistogramSetWithParticleLevelInfo(histogramSet[iMultiplicityBins][0], part_i);

      //break the loop once the bin is found
      break;
    }
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
  // special cases
  const HBT::Units::FloatType restrictedQFractions = 1.0f;

  const HBT::Units::FloatType slopeCloseRegion = 0.0005f; //rad
  const HBT::Units::FloatType restrictedQSlopes = 0.1f;

  //general
  histogramSet[0].Fill(pair.m_invariantMass);
  histogramSet[1].Fill(pair.m_chargedParticleMultiplicity);
  histogramSet[2].Fill(pair.m_kt);
  histogramSet[3].Fill(pair.m_zPv);

  //LAB frame
  histogramSet[4].Fill(pair.m_Q_LAB);
  histogramSet[5].Fill(pair.m_Q_LAB);
  histogramSet[6].Fill(pair.m_Q_LAB);

  histogramSet[7].Fill(pair.m_deltaPhi_LAB);
  histogramSet[8].Fill(pair.m_deltaRapidity_LAB);
  histogramSet[9].Fill(pair.m_deltaPt_LAB);

  // //LCMS frame
  // histogramSet[15].Fill( pair.m_Q_LCMS );
  // histogramSet[16].Fill( pair.m_Q_long );
  // histogramSet[17].Fill( pair.m_Q_side );
  // histogramSet[18].Fill( pair.m_Q_out );

  // histogramSet[19].Fill( pair.m_deltaPhi_LCMS );
  // histogramSet[20].Fill( pair.m_deltaRapidity_LCMS );
  // histogramSet[21].Fill( pair.m_deltaPt_LCMS );

  // ghost / clone / MCID checks
  if (isMC)
  {
    histogramSet[10].Fill(pair.m_pairMCID);
    histogramSet[11].Fill(pair.m_hasClone);
    histogramSet[12].Fill(pair.m_hasGhost);

    // signal Q region
    if (pair.m_Q_LAB < restrictedQFractions)
    {
      histogramSet[13].Fill(pair.m_pairMCID);
      histogramSet[14].Fill(pair.m_hasClone);
      histogramSet[15].Fill(pair.m_hasGhost);
    }

    // Q distribution / correlation functions for pairs with fake tracks
    if (pair.m_hasClone)
    {
      histogramSet[20].Fill(pair.m_Q_LAB);
    }
    if (pair.m_hasGhost)
    {
      histogramSet[21].Fill(pair.m_Q_LAB);
    }
    if (pair.m_hasClone || pair.m_hasGhost)
    {
      histogramSet[22].Fill(pair.m_Q_LAB);
    }
  }

  // slopes (ghost / clones; aimed for data)
  {
    // get slope diffs [rad]
    const HBT::Units::FloatType slopeDiffX = pair.m_slopeDiffX;
    const HBT::Units::FloatType slopeDiffY = pair.m_slopeDiffY;

    if (slopeDiffY < slopeCloseRegion)
    {
      histogramSet[16].Fill(slopeDiffX);
      if (pair.m_Q_LAB < restrictedQSlopes)
      {
        histogramSet[17].Fill(slopeDiffX);
      }
    }
    if (slopeDiffX < slopeCloseRegion)
    {
      histogramSet[18].Fill(slopeDiffY);
      if (pair.m_Q_LAB < restrictedQSlopes)
      {
        histogramSet[19].Fill(slopeDiffY);
      }
    }
  }
}

inline void HBT::Histograms::FillHistogramSeries200(std::vector<HBT::Units::TH2FloatType> &histogramSet)
{
  histogramSet[0].Fill(zBestPV[0], PVMult1);
}

inline void HBT::Histograms::Fill2DHistogramSeries1000(std::vector<HBT::Units::TH2FloatType> &histogramSet, const int &part_i)
{
  histogramSet[0].Fill(zBestPV[part_i], PVMult1);
}

inline void HBT::Histograms::FillHistogramSeries2000(std::vector<HBT::Units::TH2FloatType> &histogramSet, const HBT::ParticlePair &pair)
{
  //general
  histogramSet[0].Fill(pair.m_chargedParticleMultiplicity, pair.m_Q_LAB);
  histogramSet[1].Fill(pair.m_kt, pair.m_Q_LAB);
  histogramSet[2].Fill(pair.m_chargedParticleMultiplicity, pair.m_kt);
  histogramSet[3].Fill(pair.m_zPv, pair.m_chargedParticleMultiplicity);
  histogramSet[4].Fill(pair.m_deltaPhi_LAB, pair.m_Q_LAB);
  histogramSet[5].Fill(pair.m_deltaPhi_LAB, pair.m_kt);
  histogramSet[6].Fill(pair.m_deltaRapidity_LAB, pair.m_Q_LAB);
  histogramSet[7].Fill(pair.m_deltaRapidity_LAB, pair.m_kt);
  histogramSet[8].Fill(pair.m_deltaPhi_LAB, pair.m_deltaRapidity_LAB);

  histogramSet[9].Fill(pair.m_pt1, pair.m_Q_LAB);
  histogramSet[9].Fill(pair.m_pt2, pair.m_Q_LAB);
}

inline std::vector<HBT::Histograms::HistDef> HBT::Histograms::CreateHistogramSeries100(std::string setHeader)
{

  std::vector<HistDef> hSet;

  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "0").c_str(), "trkMult", 200, 0.0, 200.0), &multNoPreSel));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "1").c_str(), "PVMult1", 150, 0.0, 150.0), &PVMult1));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "2").c_str(), "SPDhits", 65, 0.0, 650.0), &SPDhits));

  return hSet;
}

inline std::vector<HBT::Units::TH2FloatType> HBT::Histograms::CreateHistogramSeries200(const std::string &setHeader)
{

  const HBT::Units::FloatType chargedParticleMultiplicityMax = 150.0;

  const int histogram2DBinning = 100;
  const int histogram2DBinningForMultiplicity = chargedParticleMultiplicityMax / 2.;

  std::vector<HBT::Units::TH2FloatType> hSet;

  //h2yy
  //2D event-level histograms
  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "0").c_str(), std::string("PVMult1 vs z_{PV}; z_{PV} [mm]; PVMult1").c_str(), histogram2DBinning, -200.0, 200.0, histogram2DBinningForMultiplicity, 0., chargedParticleMultiplicityMax));

  return hSet;
}

inline std::vector<HBT::Histograms::HistDef> HBT::Histograms::CreateHistogramSeries1000(std::string setHeader, const int &currentMultiplicityBin, const int &currentKtBin)
{

  //helpful constants
  const auto strMultiplicity = std::to_string(currentMultiplicityBin);
  const auto strKt = std::to_string(currentKtBin);

  std::vector<HistDef> hSet;

  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"01" + "_" + strMultiplicity + "_" + strKt).c_str(),"pvIndex", 100, 0.0, 100.0), (int*) pvIndex ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"02" + "_" + strMultiplicity + "_" + strKt).c_str(),"charge", 100, -5.0, 5.0), (int*) charge ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"03" + "_" + strMultiplicity + "_" + strKt).c_str(),"kDLL", 100, -25.0, 25.0), (HBT::Units::FloatType*) kDLL ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"04" + "_" + strMultiplicity + "_" + strKt).c_str(),"pDLL", 100, -25.0, 25.0), (HBT::Units::FloatType*) pDLL ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "05" + "_" + strMultiplicity + "_" + strKt).c_str(), "piNN", 100, 0.0, 1.0), (HBT::Units::FloatType *)piNN));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "06" + "_" + strMultiplicity + "_" + strKt).c_str(), "kNN", 100, 0.0, 1.0), (HBT::Units::FloatType *)kNN));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "07" + "_" + strMultiplicity + "_" + strKt).c_str(), "pNN", 100, 0.0, 1.0), (HBT::Units::FloatType *)pNN));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "08" + "_" + strMultiplicity + "_" + strKt).c_str(), "ghostNN", 100, 0., 1.0), (HBT::Units::FloatType *)ghostNN));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "09" + "_" + strMultiplicity + "_" + strKt).c_str(), "xBestPV", 100, -2.0, 2.0), (HBT::Units::FloatType *)xBestPV));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "10" + "_" + strMultiplicity + "_" + strKt).c_str(), "yBestPV", 100, -2.0, 2.0), (HBT::Units::FloatType *)yBestPV));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "11" + "_" + strMultiplicity + "_" + strKt).c_str(), "zBestPV", 100, -200, 200), (HBT::Units::FloatType *)zBestPV));
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"12" + "_" + strMultiplicity + "_" + strKt).c_str(),"pxBestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) pxBestPV ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"13" + "_" + strMultiplicity + "_" + strKt).c_str(),"pyBestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) pyBestPV ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"14" + "_" + strMultiplicity + "_" + strKt).c_str(),"pzBestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) pzBestPV ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"15" + "_" + strMultiplicity + "_" + strKt).c_str(),"chi2BestPV", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2BestPV ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"16" + "_" + strMultiplicity + "_" + strKt).c_str(),"e", 100, 0.0, 100.0), (HBT::Units::FloatType*) e ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "17" + "_" + strMultiplicity + "_" + strKt).c_str(), "p", 100, 0.0, 100.0), (HBT::Units::FloatType *)p));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "18" + "_" + strMultiplicity + "_" + strKt).c_str(), "pt", 100, 0.0, 3.0), (HBT::Units::FloatType *)pt));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "19" + "_" + strMultiplicity + "_" + strKt).c_str(), "phi [rad]", 100, -HBT::Units::Pi, HBT::Units::Pi), (HBT::Units::FloatType *)phi));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "20" + "_" + strMultiplicity + "_" + strKt).c_str(), "theta", 100, 0.0, 0.5), (HBT::Units::FloatType *)theta));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "21" + "_" + strMultiplicity + "_" + strKt).c_str(), "rapidity", 200, -1000.0, 1000.0), (HBT::Units::FloatType *)rapidity));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "22" + "_" + strMultiplicity + "_" + strKt).c_str(), "eta", 100, 1.5, 5.5), (HBT::Units::FloatType *)eta));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "23" + "_" + strMultiplicity + "_" + strKt).c_str(), "ip", 100, 0.0, 0.5), (HBT::Units::FloatType *)ip));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "24" + "_" + strMultiplicity + "_" + strKt).c_str(), "ipChi2", 100, 0.0, 5.0), (HBT::Units::FloatType *)ipChi2));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "25" + "_" + strMultiplicity + "_" + strKt).c_str(), "trkChi2", 100, 0.0, 3.0), (HBT::Units::FloatType *)trkChi2));
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"26" + "_" + strMultiplicity + "_" + strKt).c_str(),"chi2PV1", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2PV1 ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"27" + "_" + strMultiplicity + "_" + strKt).c_str(),"chi2PV2", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2PV2 ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"28" + "_" + strMultiplicity + "_" + strKt).c_str(),"chi2PV3", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2PV3 ) );
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"29" + "_" + strMultiplicity + "_" + strKt).c_str(),"chi2PV4", 100, 0.0, 1.0), (HBT::Units::FloatType*) chi2PV4 ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "30" + "_" + strMultiplicity + "_" + strKt).c_str(), "isClone", 2, 0., 2.), (int *)isClone));
  // hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "31" + "_" + strMultiplicity + "_" + strKt).c_str(), "cloneDist", 100, 0.0, 2e4), (HBT::Units::FloatType *)cloneDist));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "32" + "_" + strMultiplicity + "_" + strKt).c_str(), "piNNcorr", 200, -1.0, 1.0), (HBT::Units::FloatType *)piNNcor));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "33" + "_" + strMultiplicity + "_" + strKt).c_str(), "kNNcorr", 200, -1.0, 1.0), (HBT::Units::FloatType *)kNNcor));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "34" + "_" + strMultiplicity + "_" + strKt).c_str(), "pNNcorr", 200, -1.0, 1.0), (HBT::Units::FloatType *)pNNcor));
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "35" + "_" + strMultiplicity + "_" + strKt).c_str(), "shareVHits", 200, -1.0, 1.0), (HBT::Units::FloatType *)shareVHits));
  //hSet.push_back( HistDef( HBT::Units::TH1FloatType( std::string("h"+setHeader+"36" + "_" + strMultiplicity + "_" + strKt).c_str(),"isMuon", 4, -2.0, 2.0), (HBT::Units::FloatType*) isMuon ) );
  hSet.push_back(HistDef(HBT::Units::TH1FloatType(std::string("h" + setHeader + "37" + "_" + strMultiplicity + "_" + strKt).c_str(), "MCID", 20000, -10000., 10000.), (int *)MCID));

  return hSet;
}

inline std::vector<std::vector<std::vector<HBT::Histograms::HistDef>>> HBT::Histograms::CreateHistogramSeries1000InBins(const std::string &setHeader, const std::vector<HBT::Units::FloatType> &binsOfMultiplicity, const std::vector<HBT::Units::FloatType> &binsOfKt)
{

  std::vector<std::vector<std::vector<HistDef>>> hSet;

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

    hSet.assign(1, std::vector<std::vector<HistDef>>());
    hSet[0].push_back(HBT::Histograms::CreateHistogramSeries1000(setHeader, 0, 0));
  }
  else
  {

    hSet.assign(nrOfMultiplicityBins, std::vector<std::vector<HistDef>>());

    for (unsigned int iMultiplicity = 0; iMultiplicity < nrOfMultiplicityBins; ++iMultiplicity)
    {

      //option for no bins in kT
      if (binsOfKt.empty())
      {
        hSet[iMultiplicity].push_back(HBT::Histograms::CreateHistogramSeries1000(setHeader, iMultiplicity + 1, 0));
      }
      else
      {

        for (unsigned int iKt = 0; iKt < nrOfKtBins; ++iKt)
        {

          hSet[iMultiplicity].push_back(HBT::Histograms::CreateHistogramSeries1000(setHeader, iMultiplicity + 1, iKt + 1));
        }
      }
    }
  }

  return hSet;
}

inline std::vector<HBT::Units::TH2FloatType> HBT::Histograms::Create2DHistogramSeries1000(const std::string &setHeader)
{

  const HBT::Units::FloatType chargedParticleMultiplicityMax = 150.0;

  const int histogram2DBinning = 100;
  const int histogram2DBinningForMultiplicity = chargedParticleMultiplicityMax / 2.;

  std::vector<HBT::Units::TH2FloatType> hSet;

  //h2yy_2D
  //2D particle-level histograms
  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "50_2D").c_str(), std::string("N_{ch} vs z_{PV}; z_{PV} [mm]; N_{ch}").c_str(), histogram2DBinning, -200.0, 200.0, histogram2DBinningForMultiplicity, 0., chargedParticleMultiplicityMax));

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
  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "02" + "_" + "0" + "_" + "0").c_str(), std::string("k_{T} vs N_{ch}" + tmpPairType + "; N_{ch}; k_{T} [GeV]").c_str(), histogram2DBinningForMultiplicity, 0., chargedParticleMultiplicityMax, histogram2DBinning, 0., averagePairTransverseMomentumMax));
  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "03" + "_" + "0" + "_" + "0").c_str(), std::string("N_{ch} vs z_{PV}" + tmpPairType + "; z_{PV} [mm]; N_{ch}").c_str(), histogram2DBinning, -200.0, 200.0, histogram2DBinningForMultiplicity, 0., chargedParticleMultiplicityMax));

  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "04" + "_" + "0" + "_" + "0").c_str(), std::string("Q vs #Delta#phi" + tmpPairType + "; #Delta#phi [rad]; Q [GeV]").c_str(), histogram2DBinning, 0.0, HBT::Units::Pi, histogram2DBinning, 0., qRangeMax));
  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "05" + "_" + "0" + "_" + "0").c_str(), std::string("k_{T} vs #Delta#phi" + tmpPairType + "; #Delta#phi [rad]; k_{T} [GeV]").c_str(), histogram2DBinning, 0.0, HBT::Units::Pi, histogram2DBinning, 0., averagePairTransverseMomentumMax));

  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "06" + "_" + "0" + "_" + "0").c_str(), std::string("Q vs #Deltay" + tmpPairType + "; #Deltay; Q [GeV]").c_str(), histogram2DBinning, 0.0, 5.0, histogram2DBinning, 0., qRangeMax));
  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "07" + "_" + "0" + "_" + "0").c_str(), std::string("k_{T} vs #Deltay" + tmpPairType + "; #Deltay; k_{T} [GeV]").c_str(), histogram2DBinning, 0.0, 5.0, histogram2DBinning, 0., averagePairTransverseMomentumMax));

  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "08" + "_" + "0" + "_" + "0").c_str(), std::string("#Delta#phi vs  #Deltay" + tmpPairType + "; #Deltay; #Delta#phi [rad]").c_str(), histogram2DBinning, 0.0, 3.0, histogram2DBinning, 0., HBT::Units::Pi));

  hSet.push_back(HBT::Units::TH2FloatType(std::string("h" + setHeader + "09" + "_" + "0" + "_" + "0").c_str(), std::string("Q vs p_{T}" + tmpPairType + "; p_{T} [GeV]; Q [GeV]").c_str(), histogram2DBinning, 0., 3., histogram2DBinning, qRangeMin, qRangeMax));

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
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "02" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("pair charged particle multiplicity" + tmpPairType + ";N_{ch}; dN/d(N_{ch})").c_str(), chargedParticleMultiplicityMax, 0.0, chargedParticleMultiplicityMax));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "03" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("average pair transverse momentum" + tmpPairType + ";k_{T} [GeV]; dN/d(k_{T})").c_str(), 100, 0.0, averagePairTransverseMomentumMax));

  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "04" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("z_{PV}" + tmpPairType + ";z_{PV} [mm]; dN/d(z_{PV})").c_str(), 100, -200.0, 200.0));

  //h3x10 - h3x19
  //Q distributions in LAB frame (different binnings; extended Q range)
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "10" + "_" + strMultiplicity + "_" + strKt + "_" + "400").c_str(), std::string("Q_LAB" + tmpPairType + ";Q [GeV]; dN/dQ").c_str(), qBinning, qRangeMin, qRangeMax));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "10" + "_" + strMultiplicity + "_" + strKt + "_" + "200").c_str(), std::string("Q_LAB" + tmpPairType + ";Q [GeV]; dN/dQ").c_str(), qBinning / 2.0, qRangeMin, qRangeMax));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "11" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("Q_LAB" + tmpPairType + ";Q [GeV]; dN/dQ").c_str(), qExtendedBinning, qRangeMin, qExtendedRangeMax));
  //h3x20 - h3x29
  //dY, dPhi, dpT in LAB frame
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "20" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("#Delta#phi_LAB" + tmpPairType + ";#Delta#phi [rad]; dN/d(#Delta#phi)").c_str(), 100, 0.0, HBT::Units::Pi));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "21" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("#Deltay_LAB" + tmpPairType + ";#Deltay; dN/d(#Deltay)").c_str(), 100, 0.0, 5.0));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "22" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("#Deltap_{T}_LAB" + tmpPairType + ";#Deltap_{T} [GeV]; dN/d(#Deltap_{T})").c_str(), 100, 0.0, 3.0));
  //h3x30 - h3x39
  //Q distributions in LCMS frame (full Q + long/side/out components)
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"30" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string( "Q_LCMS" + tmpPairType  + ";Q [GeV]; dN/dQ").c_str(), qBinning, qRangeMin, qRangeMax ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"31" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string( "Q_LCMS_long" + tmpPairType  + ";Q [GeV]; dN/dQ").c_str(), qBinning, -1.0 * qLCMSRangeMax, qLCMSRangeMax ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"32" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string( "Q_LCMS_side" + tmpPairType  + ";Q [GeV]; dN/dQ").c_str(), qBinning, -1.0 * qLCMSRangeMax, qLCMSRangeMax ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"33" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string( "Q_LCMS_out" + tmpPairType  + ";Q [GeV]; dN/dQ").c_str(), qBinning, -1.0 * qLCMSRangeMax, qLCMSRangeMax ) );
  //h3x40 - h3x49
  //dY, dPhi, dpT in LCMS frame
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"40" + "_" + strMultiplicity + "_" + strKt ).c_str(), std::string( "#Deltaphi_LCMS" + tmpPairType  + ";#Delta#phi [rad]; dN/d(#Delta#phi)").c_str(), 100, 0.0, HBT::Units::Pi ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"41" + "_" + strMultiplicity + "_" + strKt ).c_str(), std::string( "#Deltay_LCMS" + tmpPairType  + ";#Deltay; dN/d(#Deltay)").c_str(), 100, 0.0, 5.0 ) );
  // hSet.push_back( HBT::Units::TH1FloatType( std::string( "h"+setHeader+"42" + "_" + strMultiplicity + "_" + strKt ).c_str(), std::string( "#Deltap_{T}_LCMS" + tmpPairType  + ";#Deltap_{T} [GeV]; dN/d(#Deltap_{T})").c_str(), 100, 0.0, 3.0 ) );

  //h3x50 - h3x59
  // ghost / clone / MCID checks
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "50" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("pair by MCID" + tmpPairType + "; pair MCID; dN/d(pair MCID)").c_str(), 20000, -10000., 10000.));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "51" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("hasClone" + tmpPairType + ";hasClone; dN/d(hasClone)").c_str(), 2, 0, 2.0));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "52" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("hasGhost" + tmpPairType + ";hasGhost; dN/d(hasGhost)").c_str(), 2, 0, 2.0));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "50_restrictedQ" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("pair by MCID (restricted Q)" + tmpPairType + "; pair MCID; dN/d(pair MCID)").c_str(), 20000, -10000., 10000.));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "51_restrictedQ" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("hasClone (restricted Q)" + tmpPairType + ";hasClone; dN/d(hasClone)").c_str(), 2, 0, 2.0));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "52_restrictedQ" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("hasGhost (restricted Q)" + tmpPairType + ";hasGhost; dN/d(hasGhost)").c_str(), 2, 0, 2.0));
  // slopes
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "53" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("#Deltat_{x} (#Deltat_{y} < 0.5)" + tmpPairType + ";#Deltat_{x}; dN/d(#Deltat_{x})").c_str(), 100, -0.0005, 0.0005));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "53_restrictedQ" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("#Deltat_{x} (#Deltat_{y} < 0.5) (restricted Q)" + tmpPairType + ";#Deltat_{x}; dN/d(#Deltat_{x})").c_str(), 100, -0.0005, 0.0005));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "54" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("#Deltat_{y} (#Deltat_{x} < 0.5)" + tmpPairType + ";#Deltat_{y}; dN/d(#Deltat_{y})").c_str(), 100, -0.0005, 0.0005));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "54_restrictedQ" + "_" + strMultiplicity + "_" + strKt).c_str(), std::string("#Deltat_{y} (#Deltat_{x} < 0.5) (restricted Q)" + tmpPairType + ";#Deltat_{y}; dN/d(#Deltat_{y})").c_str(), 100, -0.0005, 0.0005));
  // Q distribution
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "55" + "_" + strMultiplicity + "_" + strKt + "_" + "400").c_str(), std::string("Q_LAB (hasClone)" + tmpPairType + ";Q [GeV]; dN/dQ").c_str(), qBinning, qRangeMin, qRangeMax));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "56" + "_" + strMultiplicity + "_" + strKt + "_" + "400").c_str(), std::string("Q_LAB (hasGhost)" + tmpPairType + ";Q [GeV]; dN/dQ").c_str(), qBinning, qRangeMin, qRangeMax));
  hSet.push_back(HBT::Units::TH1FloatType(std::string("h" + setHeader + "57" + "_" + strMultiplicity + "_" + strKt + "_" + "400").c_str(), std::string("Q_LAB (hasClone/hasGhost)" + tmpPairType + ";Q [GeV]; dN/dQ").c_str(), qBinning, qRangeMin, qRangeMax));

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
