#include "../HBTAnalysis/Units.hpp"
#include "../HBTAnalysis/Utils.hpp"
#include "../utils/Styles.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"
#include "../HBTAnalysis/CoulombCorrection.hpp"

// use consts from Units
using namespace HBT::Units;

/*-------------- configuration -------------------*/

/*-------------- enf of configuration -------------*/

// provide custom style (based on the general ones)
void setStyleLocal(const unsigned int &flagStyle)
{
    HBT::Styles::setStyle(flagStyle);

    // local style settings
    gStyle->SetPadTopMargin(0.055);
    gStyle->SetPadLeftMargin(0.17);
    gStyle->SetPadRightMargin(0.115);

    gStyle->SetPalette(kRainBow);

    if (HBT::Styles::flagShowStats(flagStyle))
    {
        gStyle->SetStatW(0.13);
    }

    gROOT->ForceStyle();
}

void printDescription(const bool &flagMultBins, const bool &flagKtBins, const int &currentMultBin, const int &currentKtBin)
{
    // config / info
    const HBT::Units::FloatType commPosX = 0.05;
    const HBT::Units::FloatType commPosY = 0.85;

    const auto selection = SelectionClass();
    const auto binRangesMult = !flagKtBins ? selection.getBinsOfMultiplicityRangesAsStrings() : selection.getBinsOfMultiplicityForKtRangesAsStrings();
    const auto binRangesKt = selection.getBinsOfKtRangesAsStrings();

    // make description
    auto plotDescription = HBT::Styles::makeLhcbLabel(commPosX, commPosX + 0.25, commPosY - 0.075, commPosY);
    plotDescription->SetTextSize(plotDescription->GetTextSize() * 0.825); // ! make slighlty smaller
    auto binLabel = flagMultBins ? std::string("#font[12]{N}_{VELO} : " + binRangesMult[currentMultBin]) : std::string("");
    if (flagKtBins)
    {
        binLabel += std::string(";#font[12]{k}_{T} [GeV] : " + binRangesKt[currentKtBin]);
    }
    HBT::Utils::addMultilineText(binLabel.c_str(), plotDescription);
    plotDescription->Draw();
}

void drawHistogram1D(TH1D *h, const int color = HBT::Styles::getColorPrimary(), const Style_t marker = gStyle->GetMarkerStyle(), const TString drawOpts = "ESAME")
{
    //draw histogram
    h->SetMarkerStyle(marker);
    h->SetMarkerColor(color);
    h->SetLineColor(color);
    h->GetYaxis()->SetTitleOffset(1.2);

    h->Draw(drawOpts);
}

void drawHistogram2D(TH2D *h)
{
    //draw histogram
    // gPad->SetRightMargin(0.15);
    // gStyle->SetStatX(0.80);
    // gStyle->SetStatY(0.85);
    // h->UseCurrentStyle();

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

void drawHistogramsGeneric(const TString &inputFile, const TString &hMainNameBase, const TString &inputFileRef, const TString &hRefNameBase, const TString &hMainNameEnd, const int &nrBinsMult, const int &nrBinsKt, const TString &dataTypeMain, const TString &dataTypeRef, const bool &isHist1D, const bool &flagNormalisation, const bool &flagIntegration, const bool &isHistFullName, const TString &histTitle, const unsigned int &flagStyle)
{
    // set ROOT style
    setStyleLocal(flagStyle);

    // prepare settings for type of binning and histogram type
    const bool isNoBins = (nrBinsMult == 0) ? true : false;
    const bool isMultBinsOnly = (nrBinsKt == 0) ? true : false;
    const int nrBinsMultForLoops = isNoBins ? 1 : nrBinsMult;
    const int nrBinsKtForLoops = isMultBinsOnly ? 1 : nrBinsKt;

    const TString binsType = isNoBins ? TString("noBins") : isMultBinsOnly ? TString("mult")
                                                                           : TString("kT");
    const TString histMode = flagNormalisation ? TString("norm") : flagIntegration ? TString("integrated")
                                                                                   : TString("std");
    const TString title = hMainNameBase + "_" + histMode + "_" + binsType;

    // check if reference histogram is given (based on this setting, the relevant file / hist will be drawn, if available)
    const bool drawRef = !(!hRefNameBase.CompareTo(""));
    const TString descriptionDataType = drawRef ? "" : dataTypeMain;

    // prepare canvas
    const float scaleFactorDueToMargins = 1.1;
    const int canvasSizeHeight = HBT::Styles::defaultCanvasSizeY;
    const int canvasSizeWidth = isMultBinsOnly ? HBT::Styles::defaultCanvasSizeX * scaleFactorDueToMargins : nrBinsKtForLoops * HBT::Styles::defaultCanvasSizeX * scaleFactorDueToMargins;
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
            auto *tl = new TLegend(0.60, 0.55, 0.825, 0.75);

            TString hMainName = isHistFullName ? hMainNameBase : HBT::Utils::getHistogramName(hMainNameBase, hMainNameEnd, !isNoBins, !isMultBinsOnly, i, j);

            if (isHist1D)
            {
                TH1D *hMain = (TH1D *)fIn->Get(hMainName);
                // set custom plot title (labels for axes) if given
                // TODO in principle should be filled properly at the HBT alhorithms level
                if (histTitle.CompareTo("")) // ! if string not empty
                {
                    hMain->SetTitle(histTitle);
                }

                // process the histogram if required
                hMain = flagNormalisation ? makeNormalisedCopy(hMain) : flagIntegration ? makeIntegratedCopy(hMain)
                                                                                        : hMain;

                drawHistogram1D(hMain, HBT::Styles::getColorPrimary());
                tl->AddEntry(hMain, HBT::Utils::dataTypeAsString(dataTypeMain).c_str(), "pe");

                // draw the ref hist if available (and add proper descriptions)
                if (fInRef)
                {
                    auto *hRef = (TH1D *)fInRef->Get(isHistFullName ? hRefNameBase : HBT::Utils::getHistogramName(hRefNameBase, hMainNameEnd, !isNoBins, !isMultBinsOnly, i, j));

                    // process the histogram if required
                    hRef = flagNormalisation ? makeNormalisedCopy(hRef) : flagIntegration ? makeIntegratedCopy(hRef)
                                                                                          : hRef;

                    drawHistogram1D(hRef, HBT::Styles::getColorEmphasize(), HBT::Styles::getMarker(1));
                    tl->AddEntry(hRef, dataTypeRef, "pe");

                    // set the Y-axis range properly (main hist is drawn first)
                    hMain->GetYaxis()->SetRangeUser(0, 1.1 * std::max(hMain->GetMaximum(), hRef->GetMaximum()));

                    // do not show stats if histograms are compared
                    hMain->SetStats(0);
                    hRef->SetStats(0);
                }
            }
            else
            {
                TH2D *hMain = (TH2D *)fIn->Get(hMainName);
                drawHistogram2D(hMain);
            }

            // // add arrows (optional)
            // auto arrow = HBT::Styles::makeArrow();
            // arrow->DrawArrow(-160.0, 1e7, -160.0, 0.0);

            // add LHCb label
            auto lhcbLabel = HBT::Styles::makeLhcbLabel(0.05, 0.325, 0.87, 0.95);
            HBT::Utils::addMultilineText("LHCb preliminary;#font[12]{#sqrt{s_{#font[122]{NN}}}} = 5.02 TeV", lhcbLabel);
            lhcbLabel->Draw();
            tc->Update();

            // add legend
            tl->Draw("SAME");

            // add description
            printDescription(!isNoBins, !isMultBinsOnly, i, j);
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

void drawHistograms(const TString inputFile, const TString hMainNameBase, const TString inputFileRef, const TString hRefNameBase, TString hMainNameEnd, const TString dataTypeMain, const TString dataTypeRef, const bool isHist1D, const bool flagNormalisation, const bool flagIntegration, const int flagBins, const bool isHistFullName, const TString &histTitle = "", const unsigned int &flagStyle = HBT::Styles::defaultStyleFlag)
{
    // sanitise the flag for bins
    assert(flagBins >= 0 && flagBins <= 2);

    // get configuration
    auto selection = SelectionClass();

    auto nrBinsMult = selection.getNrOfBinsMult();
    auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    auto nrBinsKt = selection.getNrOfBinsKt();

    // call for no bins
    if (flagBins == 0)
        drawHistogramsGeneric(inputFile, hMainNameBase, inputFileRef, hRefNameBase, hMainNameEnd, 0, 0, dataTypeMain, dataTypeRef, isHist1D, flagNormalisation, flagIntegration, isHistFullName, histTitle, flagStyle);
    // call for mult bins only
    if (flagBins == 1)
        drawHistogramsGeneric(inputFile, hMainNameBase, inputFileRef, hRefNameBase, hMainNameEnd, nrBinsMult, 0, dataTypeMain, dataTypeRef, isHist1D, flagNormalisation, flagIntegration, isHistFullName, histTitle, flagStyle);
    // call for mult + kT bins
    // if (flagBins == 2)
    //     drawHistogramsGeneric(inputFile, hMainNameBase, inputFileRef, hRefNameBase, hMainNameEnd, nrBinsMultForKt, nrBinsKt, dataTypeMain, dataTypeRef, isHist1D, flagNormalisation, flagIntegration, isHistFullName, histTitle, flagStyle);
}
