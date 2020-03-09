const Double_t Pi = TMath::Pi();
const Double_t TwoPi = 2 * TMath::Pi();
const Double_t MassPion = 0.140;
const Double_t FineStructureConstant = 1.0 / 137.0;

Double_t gamovFactorForPionPair(const Double_t &Q, const bool &sameSign)
{

  const int sign = sameSign ? 1 : -1;

  const Double_t tmpEtaTwoPi = TwoPi * MassPion * FineStructureConstant / Q;

  return sign * tmpEtaTwoPi / (exp(sign * tmpEtaTwoPi) - 1);
}

Double_t fullCoulombCorrectionForPionPair(const Double_t &Q, const bool &sameSign, const Double_t assumedR = 2.)
{

  const int sign = sameSign ? 1 : -1;

  const Double_t tmpDzeta = MassPion * FineStructureConstant / Q;

  const Double_t weight = (1 + sign * Pi * tmpDzeta * Q * assumedR / (1.26 + Q * assumedR));

  return gamovFactorForPionPair(Q, sameSign) * weight;
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
    hOut->Divide(h1, h2, h2->Integral(), h1->Integral());
  else
    hOut->Divide(h1, h2);

  // calculate errors by hand
  if (!isDR)
  {

    // get histograms normalisation
    const float n1 = h1->Integral();
    const float n2 = h2->Integral();

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

// get a full histogram name in the given bin
TString getFullHistogramName(const TString baseNameBegin, const TString baseNameEnd, const bool multBinsOnly, const int binNrMult, const int binNrKt = 0)
{
  return multBinsOnly ? TString(TString(TString(baseNameBegin + "_") += (binNrMult + 1)) + "_0") + baseNameEnd : TString(TString(TString(TString(baseNameBegin + "_") += (binNrMult + 1)) + "_") += (binNrKt + 1)) + baseNameEnd;
}

void divideHistogramsInBins(const TString file1, const TString h1BaseName, const TString file2, const TString h2BaseName, const TString fileOut, const TString hOutBaseName, const int isDR = 0, const int nrBinsMult = 6, const int nrBinsKt = 0, TString hCommonEndName = "", const int flagCorrectCoulomb = 0, const int isLikePair = 0)
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

      TString h1Name = isMultBinsOnly ? getFullHistogramName(h1BaseName, hCommonEndName, isMultBinsOnly, i) : getFullHistogramName(h1BaseName, hCommonEndName, isMultBinsOnly, i, j);
      TString h2Name = isMultBinsOnly ? getFullHistogramName(h2BaseName, hCommonEndName, isMultBinsOnly, i) : getFullHistogramName(h2BaseName, hCommonEndName, isMultBinsOnly, i, j);
      TString hOutName = isMultBinsOnly ? getFullHistogramName(hOutBaseName, hCommonEndName, isMultBinsOnly, i) : getFullHistogramName(hOutBaseName, hCommonEndName, isMultBinsOnly, i, j);

      TH1D *h1 = (TH1D *)f1->Get(h1Name);
      TH1D *h2 = (TH1D *)f2->Get(h2Name);

      auto hOut = divideHistograms(h1, h2, hOutName, isDR);

      if (flagCorrectCoulomb)
      {

        hOut->Sumw2();

        for (int i = 1; i < hOut->GetNbinsX() + 1; i++)
        {

          const auto currentBinContent = hOut->GetBinContent(i);
          const auto currentCorrectionFactor = 1. / fullCoulombCorrectionForPionPair(hOut->GetBinCenter(i), isLikePair);
          hOut->SetBinContent(i, currentBinContent * currentCorrectionFactor);
        }
      }

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