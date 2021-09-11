#include "../HBTAnalysis/Units.hpp"
#include "../HBTAnalysis/Utils.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"
#include "../HBTAnalysis/CoulombCorrection.hpp"

// use consts from Units
using namespace HBT::Units;

/*-------------- configuration -------------------*/

const HBT::Units::FloatType R_eff = HBT::Coulomb::REffForPions;
const HBT::Units::FloatType particleMass = HBT::Units::MassPion;

/*-------------- enf of configuration -------------*/

void removeResonances(TH1D *&h)
{
  const HBT::Units::FloatType binWidth = h->GetBinWidth(1);
  const HBT::Units::FloatType histQMin = h->GetXaxis()->GetBinLowEdge(1);

  for (auto const &resonance : HBT::Units::Resonances)
  {
    const unsigned int minBin = HBT::Utils::getBinIndex(resonance.second.getQRangeMin(), histQMin, binWidth);
    const unsigned int maxBin = HBT::Utils::getBinIndex(resonance.second.getQRangeMax(), histQMin, binWidth);
    for (unsigned int i = minBin; i < maxBin; i++)
    {
      h->SetBinContent(i, 0.);
      h->SetBinError(i, 0.);
    }
  }
}

TH1D *divideHistograms(TH1D *h1, TH1D *h2, const TString hOutName = "", const bool isDR = false)
{

  // create a histogram with identical binning
  TH1D *hOut = (TH1D *)h1->Clone(hOutName);
  hOut->SetTitle(isDR ? TString(TString(hOut->GetName()) + "; Q [GeV]; DR(Q)") : TString(TString(hOut->GetName()) + "; Q [GeV]; C(Q)"));
  hOut->Reset();

  // deal with histogram errors before dividing
  if (h1->GetSumw2N() == 0)
  {
    h1->Sumw2();
  }

  if (h2->GetSumw2N() == 0)
  {
    h2->Sumw2();
  }

  // divide
  if (!isDR)
    hOut->Divide(h1, h2, h2->GetEntries(), h1->GetEntries());
  else
    hOut->Divide(h1, h2);

  // calculate errors by hand
  if (!isDR)
  {

    // get histograms normalisation
    const float n1 = h1->GetEntries();
    const float n2 = h2->GetEntries();

    for (int i = 1; i < hOut->GetNbinsX() + 1; i++)
    {
      // get bin contents and errors
      const float q1 = h1->GetBinContent(i);
      const float q2 = h2->GetBinContent(i);
      const float e1 = h1->GetBinError(i);
      const float e2 = h2->GetBinError(i);

      // calculate error
      const float eOut = n2 / n1 * (1. / q2) * TMath::Sqrt(TMath::Power(e1, 2) + TMath::Power(q1 / q2 * e2, 2));

      // check for differences
      if (fabs(eOut - hOut->GetBinError(i)) > 0.00001)
      {
        std::cout << "Hello there!: errors propagation in the correlation function gives unexpected results." << std::endl;
        std::cout << eOut << "\t" << hOut->GetBinError(i) << std::endl;
      }

      // hOut->SetBinError(i, eOut);
    }
  }

  return hOut;
}

void divideHistogramsInBinsGeneric(const TString file1, const TString h1BaseName, const TString file2, const TString h2BaseName, const TString fileOut, const TString hOutBaseName, const int isDR, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName, const int flagCorrectCoulomb, const int isLike, const bool flagRemoveResonances)
{

  // prepare settings for type of binning
  const bool isMultBinsOnly = (nrBinsKt == 0) ? true : false;

  const int nrBinsKtForLoops = isMultBinsOnly ? 1 : nrBinsKt;

  //prepare input/output files
  auto f1 = new TFile(TString(file1), "READ");
  auto f2 = new TFile(TString(file2), "READ");
  auto fOut = new TFile(TString(fileOut), "UPDATE");

  for (int i = 0; i < nrBinsMult; ++i)
  {

    for (int j = 0; j < nrBinsKtForLoops; ++j)
    {

      TString h1Name = HBT::Utils::getHistogramName(h1BaseName, hCommonEndName, true, !isMultBinsOnly, i, j);
      TString h2Name = HBT::Utils::getHistogramName(h2BaseName, hCommonEndName, true, !isMultBinsOnly, i, j);
      TString hOutName = HBT::Utils::getHistogramName(hOutBaseName, hCommonEndName, true, !isMultBinsOnly, i, j);

      TH1D *h1 = (TH1D *)f1->Get(h1Name);
      TH1D *h2 = (TH1D *)f2->Get(h2Name);

      auto hOut = divideHistograms(h1, h2, hOutName, isDR);

      if (flagCorrectCoulomb)
      {

        hOut->Sumw2();

        for (int i = 1; i < hOut->GetNbinsX() + 1; i++)
        {

          const auto currentBinContent = hOut->GetBinContent(i);
          const auto currentCorrectionFactor = 1. / HBT::Coulomb::BS(hOut->GetBinCenter(i), isLike, R_eff, particleMass);
          hOut->SetBinContent(i, currentBinContent * currentCorrectionFactor);
        }
      }

      // process also the basic histograms
      // normalise the ref histogram to the main one
      h2->Scale(double(h1->GetEntries()) / h2->GetEntries());

      // remove resonances from the correlation function for UNLIKE pairs
      // if (flagRemoveResonances && !isLike)
      // {
      //   removeResonances(h1);
      //   removeResonances(h2);
      //   removeResonances(hOut);
      // }

      h1->Write();
      h2->Write();
      hOut->Write();

      delete h1;
      delete h2;
      delete hOut;
    }
  }

  //cleanup
  f1->Close();
  f2->Close();
  fOut->Close();

  delete f1;
  delete f2;
  delete fOut;
}

void divideHistogramsInBins(const TString file1, const TString h1BaseName, const TString file2, const TString h2BaseName, const TString fileOut, const TString hOutBaseName, TString hCommonEndNameForMult = "", TString hCommonEndNameForKt = "", const int isDR = 0, const int flagCorrectCoulomb = 0, const int isLike = 0, const bool flagRemoveResonances = true)
{
  // get configuration
  auto selection = SelectionClass();

  auto nrBinsMult = selection.getNrOfBinsMult();
  auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
  auto nrBinsKt = selection.getNrOfBinsKt();

  // call for mult bins only
  divideHistogramsInBinsGeneric(file1, h1BaseName, file2, h2BaseName, fileOut, hOutBaseName, isDR, nrBinsMult, 0, hCommonEndNameForMult, flagCorrectCoulomb, isLike, flagRemoveResonances);
  // call for mult + kT bins
  // divideHistogramsInBinsGeneric(file1, h1BaseName, file2, h2BaseName, fileOut, hOutBaseName, isDR, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt, flagCorrectCoulomb, isLike, flagRemoveResonances);
}