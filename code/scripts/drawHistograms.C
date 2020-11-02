#include "../HBTAnalysis/Units.hpp"
#include "../HBTAnalysis/Utils.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"
#include "../HBTAnalysis/CoulombCorrection.hpp"

// use consts from Units
using namespace HBT::Units;

/*-------------- configuration -------------------*/

const int padSize = 600;
const HBT::Units::FloatType lineWidth = 1.0;

/*-------------- enf of configuration -------------*/

// provide custom style (based on the general one)
void setStyleLocal()
{
    HBT::Utils::setStyle();
    gStyle->SetOptStat(1111);
    gStyle->SetMarkerSize(0.6);
}

void printDescription(const TString &dataType, const bool &flagMultBins, const bool &flagKtBins, const int &currentMultBin, const int &currentKtBin)
{
    const auto selection = SelectionClass();
    const auto binRangesMult = !flagKtBins ? selection.getBinsOfMultiplicityRangesAsStrings() : selection.getBinsOfMultiplicityForKtRangesAsStrings();
    const auto binRangesKt = selection.getBinsOfKtRangesAsStrings();

    const HBT::Units::FloatType commPosX = 0.25;
    const HBT::Units::FloatType commPosY = 0.30;
    TLatex comments;
    comments.SetNDC();
    comments.SetTextSize(0.035);
    comments.DrawLatex(commPosX, commPosY, dataType);

    // print only info relevant for the given bins
    if (flagMultBins)
    {
        comments.DrawLatex(commPosX, commPosY - 0.05, TString("Multiplicity : ") + binRangesMult[currentMultBin].c_str());
    }
    if (flagKtBins)
    {
        comments.DrawLatex(commPosX, commPosY - 0.10, TString("k_{T} [GeV]: ") + binRangesKt[currentKtBin].c_str());
    }
}

void drawHistogram1D(TH1D *h, const int color = kBlue, const TString drawOpts = "ESAME")
{
    //draw histogram
    h->SetMarkerColor(color);
    h->SetLineColor(color);
    h->SetLineWidth(lineWidth);
    h->GetYaxis()->SetTitleOffset(1.2);

    h->Draw(drawOpts);
}

void drawHistogram2D(TH2D *h)
{
    //draw histogram
    gPad->SetRightMargin(0.15);
    gStyle->SetStatX(0.80);
    gStyle->SetStatY(0.85);
    h->UseCurrentStyle();

    h->Draw("COLZ");
}

TH1D *makeNormalisedCopy(const TH1D *hSrc)
{
    auto *h = (TH1D *)hSrc->Clone(TString(hSrc->GetName()) + "_norm");
    if (h->GetSumw2N() == 0)
        h->Sumw2();

    h->Scale(1. / h->GetEntries());

    return h;
}

TH1D *makeIntegratedCopy(const TH1D *hSrc)
{
    // get a normalised histogram first
    auto *h = makeNormalisedCopy(hSrc);

    const auto nrOfBins = h->GetNbinsX();

    // for each bin (bin numbering starts from 1)
    for (int i = 1; i < nrOfBins + 1; i++)
    {
        // include overflow in the integral
        const auto tmpIntegral = h->Integral(i, nrOfBins + 1);

        h->SetBinContent(i, tmpIntegral);
    }

    return h;
}

void drawHistogramsGeneric(const TString &inputFile, const TString &hMainNameBase, const TString &inputFileRef, const TString &hRefNameBase, const TString &hMainNameEnd, const int &nrBinsMult, const int &nrBinsKt, const TString &dataTypeMain, const TString &dataTypeRef, const bool &isHist1D, const bool &flagNormalisation, const bool &flagIntegration, const bool &isHistFullName)
{
    // set ROOT style
    setStyleLocal();

    // prepare settings for type of binning and histogram type
    const bool isNoBins = (nrBinsMult == 0) ? true : false;
    const bool isMultBinsOnly = (nrBinsKt == 0) ? true : false;
    const int nrBinsMultForLoops = isNoBins ? 1 : nrBinsMult;
    const int nrBinsKtForLoops = isMultBinsOnly ? 1 : nrBinsKt;

    const TString binsType = isNoBins ? TString("noBins") : isMultBinsOnly ? TString("mult") : TString("kT");
    const TString histMode = flagNormalisation ? TString("norm") : flagIntegration ? TString("integrated") : TString("std");
    const TString title = hMainNameBase + "_" + histMode + "_" + binsType;

    // check if reference histogram is given (based on this setting, the relevant file / hist will be drawn, if available)
    const bool drawRef = !(!hRefNameBase.CompareTo(""));
    const TString descriptionDataType = drawRef ? "" : dataTypeMain;

    // prepare canvas
    const HBT::Units::FloatType scaleFactorDueToMargins = 1.2;
    const int canvasSizeHeight = padSize;
    const int canvasSizeWidth = isMultBinsOnly ? padSize : nrBinsKtForLoops * padSize * scaleFactorDueToMargins;
    TCanvas *tc = new TCanvas(title, title, canvasSizeWidth, canvasSizeHeight);
    const TString plotFile = title + ".pdf";
    tc->SaveAs(plotFile + "[");

    // prepare I/O files
    TFile *fIn = new TFile(inputFile, "READ");
    TFile *fInRef = drawRef ? new TFile(inputFileRef, "READ") : nullptr;

    // loop over all bins of mult / kT
    for (int i = 0; i < nrBinsMultForLoops; ++i)
    {
        // each mult bin is plotted on a single page, with subplot for kT bins
        tc->Clear();
        tc->Draw();
        tc->Divide(nrBinsKtForLoops);

        for (int j = 0; j < nrBinsKtForLoops; ++j)
        {
            // prepare pads
            tc->cd(j + 1);
            gPad->Draw();

            TString hMainName = isHistFullName ? hMainNameBase : HBT::Utils::getHistogramName(hMainNameBase, hMainNameEnd, !isNoBins, !isMultBinsOnly, i, j);

            if (isHist1D)
            {
                TH1D *hMain = (TH1D *)fIn->Get(hMainName);

                // process the histogram if required
                hMain = flagNormalisation ? makeNormalisedCopy(hMain) : flagIntegration ? makeIntegratedCopy(hMain) : hMain;

                drawHistogram1D(hMain, kBlue);

                // draw the ref hist if available (and add proper descriptions)
                if (fInRef)
                {
                    auto *hRef = (TH1D *)fInRef->Get(isHistFullName ? hRefNameBase : HBT::Utils::getHistogramName(hRefNameBase, hMainNameEnd, !isNoBins, !isMultBinsOnly, i, j));

                    // process the histogram if required
                    hRef = flagNormalisation ? makeNormalisedCopy(hRef) : flagIntegration ? makeIntegratedCopy(hRef) : hRef;

                    drawHistogram1D(hRef, kGreen);

                    // set the Y-axis range properly (main hist is drawn first)
                    hMain->GetYaxis()->SetRangeUser(0, 1.05 * std::max(hMain->GetMaximum(), hRef->GetMaximum()));

                    // do not show stats if histograms are compared
                    hMain->SetStats(0);
                    hRef->SetStats(0);

                    // add description
                    auto *tl = new TLegend(0.55, 0.50, 0.85, 0.65);
                    tl->AddEntry(hMain, hMainNameBase + " " + dataTypeMain, "pe");
                    tl->AddEntry(hRef, hRefNameBase + " " + dataTypeRef, "pe");
                    tl->Draw("SAME");
                }
            }
            else
            {
                TH2D *hMain = (TH2D *)fIn->Get(hMainName);
                drawHistogram2D(hMain);
            }

            // add description / legend
            printDescription(descriptionDataType, !isNoBins, !isMultBinsOnly, i, j);
        }

        // plot each mult bin on a different page
        tc->Update();
        tc->Draw();
        tc->SaveAs(plotFile);
    }

    tc->SaveAs(plotFile + "]");
    delete tc;

    // I/O close & cleanup
    fIn->Close();
    delete fIn;

    if (fInRef)
    {
        fInRef->Close();
        delete fInRef;
    }
}

void drawHistograms(const TString inputFile, const TString hMainNameBase, const TString inputFileRef, const TString hRefNameBase, TString hMainNameEnd, const TString dataTypeMain, const TString dataTypeRef, const bool isHist1D, const bool flagNormalisation, const bool flagIntegration, const int flagBins, const bool isHistFullName)
{
    // sanitise the flag for bins
    assert(flagBins >= 0 && flagBins <= 2);

    // get configuration
    auto selection = SelectionClass();

    auto nrBinsMult = selection.getNrOfBinsMult();
    auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    auto nrBinsKt = selection.getNrOfBinsKt();

    // call for no bins
    if (flagBins >= 0)
        drawHistogramsGeneric(inputFile, hMainNameBase, inputFileRef, hRefNameBase, hMainNameEnd, 0, 0, dataTypeMain, dataTypeRef, isHist1D, flagNormalisation, flagIntegration, isHistFullName);
    // call for mult bins only
    if (flagBins >= 1)
        drawHistogramsGeneric(inputFile, hMainNameBase, inputFileRef, hRefNameBase, hMainNameEnd, nrBinsMult, 0, dataTypeMain, dataTypeRef, isHist1D, flagNormalisation, flagIntegration, isHistFullName);
    // call for mult + kT bins
    if (flagBins >= 2)
        drawHistogramsGeneric(inputFile, hMainNameBase, inputFileRef, hRefNameBase, hMainNameEnd, nrBinsMultForKt, nrBinsKt, dataTypeMain, dataTypeRef, isHist1D, flagNormalisation, flagIntegration, isHistFullName);
}
