#include "../HBTAnalysis/Units.hpp"
#include "../HBTAnalysis/Utils.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"
#include "../HBTAnalysis/CoulombCorrection.hpp"

// use consts from Units
using namespace HBT::Units;

/*-------------- configuration -------------------*/

const int padSize = 600;
const HBT::Units::FloatType lineWidth = 1.0;

const HBT::Units::FloatType particleMass = HBT::Units::MassPion;
const Double_t R_eff = HBT::Coulomb::REffForPions;

const Double_t fitRangeLikeMin = 0.05;
const Double_t fitRangeUnlikeMin = 0.25;
const Double_t fitRangeMcMin = 0.25;
const Double_t fitRangeBkgScaleMin = 0.4;
const Double_t fitRangeMax = 2.0;
const Int_t fitNrOfParams = 9;

/*-------------- enf of configuration -------------*/

/*-------------- define partial fit functions -------------------*/

Double_t funcBkg(Double_t *x, Double_t *par)
{
    return (1 + par[0] * x[0] + par[1] * par[4] * exp(-pow(par[2] * x[0], par[3])));
}

Double_t funcBEC(Double_t *x, Double_t *par)
{
    return (1 + exp(-x[0] * par[0]));
}

Double_t funcCoulombLike(Double_t *x, Double_t *par)
{
    return HBT::Coulomb::BS(x[0], true, par[0], particleMass);
}

Double_t funcCoulombUnlike(Double_t *x, Double_t *par)
{
    return HBT::Coulomb::BS(x[0], false, par[0], particleMass);
}

/*-------------- define final fit functions -------------------*/

Double_t funcLevy(Double_t *x, Double_t *par)
{
    return par[0] * (1 + par[1] * exp(-(x[0] * par[2]))) * (1 + x[0] * par[3]);
}

Double_t funcFullLike(Double_t *x, Double_t *par)
{
    return par[0] * (1 - par[1] + par[1] * funcCoulombLike(x, &par[3]) * funcBEC(x, &par[2])) * funcBkg(x, &par[4]);
}

Double_t funcFullUnlike(Double_t *x, Double_t *par)
{
    return par[0] * (1 - par[1] + par[1] * funcCoulombUnlike(x, &par[3])) * funcBkg(x, &par[4]);
}

/*----------------------------------------------------------------*/

Double_t getBkgScalingFactor(const TFitResult *fitResultMain, const TFitResult *fitResultRef, const int &parameterId)
{
    return float(fitResultMain->Parameter(parameterId)) / fitResultRef->Parameter(parameterId);
}

TFitResultPtr doFit(TH1D *h, TF1 *f, const TString fitOpts = "S0R")
{
    TFitResultPtr fitResult = h->Fit(f->GetName(), fitOpts);

    // sanitize TFitResultPtr
    if (fitResult.Get() == nullptr)
        return TFitResultPtr();

    fitResult->Write(HBT::Utils::getFitResultName(h->GetName(), f->GetName()));

    return fitResult;
}

void printDescription(const TString &dataType, const bool &isMultBinsOnly, const int &currentMultBin, const int &currentKtBin)
{
    const auto selection = SelectionClass();
    const auto binRangesMult = isMultBinsOnly ? selection.getBinsOfMultiplicityRangesAsStrings() : selection.getBinsOfMultiplicityForKtRangesAsStrings();
    const auto binRangesKt = selection.getBinsOfKtRangesAsStrings();

    const HBT::Units::FloatType commPosX = 0.50;
    const HBT::Units::FloatType commPosY = 0.35;
    TLatex comments;
    comments.SetNDC();
    comments.SetTextSize(0.035);
    comments.DrawLatex(commPosX, commPosY, dataType);

    comments.DrawLatex(commPosX, commPosY - 0.05, TString("Multiplicity : ") + binRangesMult[currentMultBin].c_str());

    if (!isMultBinsOnly)
        comments.DrawLatex(commPosX, commPosY - 0.10, TString("k_{T} [GeV]: ") + binRangesKt[currentKtBin].c_str());
}

void drawPull(const TH1D *hFit, const TF1 *fFit, const HBT::Units::FloatType &fitRangeMin, const TString drawOpts = "BSAME")
{
    // create a histogram with identical binning
    TH1D *hPull = (TH1D *)hFit->Clone(TString(hFit->GetName()) + "_pull");
    hPull->SetTitle("pull;Q [GeV]; pull");
    hPull->Reset();

    //make pull
    for (int i = 1; i < hFit->GetNbinsX() + 1; ++i)
    {
        const HBT::Units::FloatType binCenter = hFit->GetBinCenter(i);

        // draw only pull across the fit range
        if (binCenter < fitRangeMin)
            continue;

        const HBT::Units::FloatType expected = fFit->Eval(binCenter);
        const HBT::Units::FloatType observed = hFit->GetBinContent(i);
        const HBT::Units::FloatType sigma = hFit->GetBinError(i);

        const HBT::Units::FloatType pullValue = (sigma != 0.) ? (observed - expected) / sigma : 0.;

        hPull->SetBinContent(i, pullValue);
    }

    //draw
    hPull->SetLabelSize(0.06, "x"); // X numbers label size ()
    hPull->SetLabelSize(0.06, "y"); // Y numbers label size ()
    hPull->SetTitleSize(0.07, "x"); // X title size ()
    hPull->SetTitleSize(0.07, "y"); // Y title size ()
    hPull->GetYaxis()->SetTitleOffset(0.5);
    //hPull->SetBarWidth( 0.1 );
    hPull->SetFillColor(kBlue);
    hPull->SetLineWidth(lineWidth);
    hPull->GetYaxis()->SetRangeUser(-6, 6);
    hPull->Draw(drawOpts);

    //draw lines
    TLine *lineUp = new TLine(0, 3, 2, 3);
    lineUp->SetLineColor(kRed);
    lineUp->SetLineWidth(lineWidth);
    lineUp->Draw("SAME");
    TLine *lineDown = new TLine(0, -3, 2, -3);
    lineDown->SetLineColor(kRed);
    lineDown->SetLineWidth(lineWidth);
    lineDown->Draw("SAME");
}

void drawHistogram(TH1D *h, const int color = kBlue, const TString drawOpts = "ESAME", const Double_t yMin = 0.5, const Double_t yMax = 1.6)
{

    //draw histogram
    h->GetYaxis()->SetRangeUser(yMin, yMax);
    h->SetMarkerColor(color);
    h->SetLineColor(color);
    h->SetLineWidth(lineWidth);
    h->GetYaxis()->SetTitleOffset(1.2);

    h->Draw(drawOpts);
}

void fitInBinsGeneric(const TString inputFile, const TString hMainNameBase, const bool flagIsMc, const bool flagIsUnlike, const TString dataType, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName, const bool flagDoFit, const TString inputFileRef, const TString hRefNameBase, const TString refType, const bool flagDrawRef, const bool flagUseBkgFromRef, const bool flagIsBkgScaling, const bool flagUseBkgScaling, const TString funcNameBkgScalingMain, const TString funcNameBkgScalingRef, const int &ignoreBinMultLower, const int &ignoreBinMultUpper, const int &ignoreBinKtLower, const int &ignoreBinKtUpper)
{
    // set ROOT style
    HBT::Utils::setStyle();

    // prepare settings for type of binning and histogram type
    const bool isMultBinsOnly = (nrBinsKt == 0) ? true : false;
    const int nrBinsKtForLoops = isMultBinsOnly ? 1 : nrBinsKt;

    const TString binsType = isMultBinsOnly ? TString("mult") : TString("kT");
    const TString histType = flagIsUnlike ? TString("UNLIKE") : TString("LIKE");
    const TString funcName = flagIsBkgScaling ? TString("funcBkgScaling") : TString("funcMain");
    const TString title = funcName + "_" + binsType + "_" + histType;

    // prepare canvas
    const HBT::Units::FloatType scaleFactorDueToMargins = 1.2;
    const int canvasSizeHeight = flagDoFit ? padSize * 1.5 : padSize;
    const int canvasSizeWidth = isMultBinsOnly ? padSize : nrBinsKtForLoops * padSize * scaleFactorDueToMargins;
    TCanvas *tc = new TCanvas(title, title, canvasSizeWidth, canvasSizeHeight);
    const TString plotFile = title + ".pdf";
    tc->SaveAs(plotFile + "[");

    // prepare I/O files
    TFile *fIn = new TFile(inputFile, "READ");
    TFile *fOut = new TFile("fitResults.root", "UPDATE");

    // optional file with reference histograms / fits
    TFile *fInRef = flagDrawRef ? new TFile(inputFileRef, "READ") : nullptr;
    TFile *fInRefFitResults = flagUseBkgFromRef ? new TFile(TString(inputFileRef).ReplaceAll("correlations.root", "fitResults.root").ReplaceAll("correlations", "fits"), "READ") : nullptr;

    // loop over all bins of mult / kT
    for (int i = 0; i < nrBinsMult; ++i)
    {
        // check if this bin should be ignored
        if (i < ignoreBinMultLower || i >= ignoreBinMultUpper)
            continue;

        // each mult bin is plotted on a single page, with subplot for kT bins
        tc->Clear();
        tc->Draw();
        tc->Divide(nrBinsKtForLoops);

        for (int j = 0; j < nrBinsKtForLoops; ++j)
        {
            // check if this bin should be ignored
            if (j < ignoreBinKtLower || j >= ignoreBinKtUpper)
                continue;

            // prepare pads
            tc->cd(j + 1);

            TLegend *tl = new TLegend(0.55, 0.50, 0.85, 0.65);

            TPad *p1 = new TPad("p1", "p1", 0., 0.33, 1., 1.);
            TPad *p2 = new TPad("p2", "p2", 0., 0., 1., 0.33);
            p2->SetBottomMargin(0.15);
            p1->Draw();
            p2->Draw();

            // get histograms
            TString hMainName = HBT::Utils::getHistogramName(hMainNameBase, hCommonEndName, true, !isMultBinsOnly, i, j);
            TH1D *hMain = (TH1D *)fIn->Get(hMainName);

            //get the reference histogram and fit result if available
            const auto hRefName = HBT::Utils::getHistogramName(hRefNameBase, hCommonEndName, true, !isMultBinsOnly, i, j);
            TH1D *hRef = flagDrawRef ? (TH1D *)fInRef->Get(hRefName) : nullptr;
            auto fitResultRef = flagUseBkgFromRef ? (TFitResult *)fInRefFitResults->Get(HBT::Utils::getFitResultName(hRefName, "funcMain")) : nullptr;
            fOut->cd();

            // draw histograms
            p1->cd();
            drawHistogram(hMain, kBlue);
            tl->AddEntry(hMain, histType, "pe");

            // draw a reference histogram if required
            if (flagDrawRef)
            {
                drawHistogram(hRef, kGreen);
                tl->AddEntry(hRef, refType, "pe");
            }

            // perform a fit if required
            if (flagDoFit)
            {
                // create a function relevant for the histogram type (from a generic form)
                const auto fitRangeMin = flagIsBkgScaling ? fitRangeBkgScaleMin : flagIsMc ? fitRangeMcMin : flagIsUnlike ? fitRangeUnlikeMin : fitRangeLikeMin;
                TF1 *funcMain = new TF1(funcName, flagIsUnlike ? funcFullUnlike : funcFullLike, fitRangeMin, fitRangeMax, fitNrOfParams);

                // settings common for all histogram types
                funcMain->SetParNames("N", "#lambda", "R", "R_{eff} [fm]", "#delta", "#lambda_{bkg}", "R_bkg", "alpha_bkg", "scaleFactor(#lambda_{bkg})");
                funcMain->SetParameters(1.0, 0.5, 7., R_eff, 0.1, 0.1, 1.5, 2.0, 1.0);
                funcMain->FixParameter(3, R_eff);
                funcMain->FixParameter(7, 2);
                funcMain->FixParameter(8, 1.0);

                funcMain->SetParLimits(0, 0., 2.0);
                funcMain->SetParLimits(1, 0., 2.0);
                funcMain->SetParLimits(2, 0., 50.);
                funcMain->SetParLimits(4, -1.0, 1.0);
                funcMain->SetParLimits(5, 0., 10.);
                funcMain->SetParLimits(6, 0., 50.);

                // settings for the specific histogram types (keep an eye on 'priority': bkgScaling -> MC -> UNLIKE -> LIKE)
                // set the unused (fixed) parameters to some neutral value (e.g. 0)
                if (flagIsBkgScaling)
                {
                    funcMain->FixParameter(1, 0);
                    funcMain->FixParameter(2, 0);
                    funcMain->FixParameter(3, 0);
                }
                else if (flagIsMc)
                {
                    funcMain->FixParameter(1, 0);
                    funcMain->FixParameter(2, 0);
                    funcMain->FixParameter(3, 0);
                }
                else if (flagIsUnlike)
                {
                    // works also with the following settings, but is more unstable (lambda is at 0 limit anyway)
                    // funcMain->SetParameter(1, 0.0);
                    // funcMain->SetParLimits(1, 0., 1.0);
                    funcMain->FixParameter(1, 0);
                    funcMain->FixParameter(2, 0);
                }

                // use the bkg parametrisation from a reference fit
                if (flagUseBkgFromRef)
                {
                    // use the fit only if it was valid
                    if (fitResultRef && fitResultRef->IsValid())
                    {
                        // scale the bkg parameters if needed
                        if (flagUseBkgScaling)
                        {
                            auto fitResultBkgScalingMain = (TFitResult *)fInRefFitResults->Get(HBT::Utils::getFitResultName(hMainName, funcNameBkgScalingMain));
                            auto fitResultBkgScalingRef = (TFitResult *)fInRefFitResults->Get(HBT::Utils::getFitResultName(hRefName, funcNameBkgScalingRef));

                            if (fitResultBkgScalingMain->IsValid() && fitResultBkgScalingRef->IsValid())
                            {
                                // use the lambda_bkg parameter from the reference fit (scaled by the difference between LIKE/UNLIKE)
                                funcMain->FixParameter(5, fitResultRef->Parameter(5));
                                funcMain->FixParameter(8, getBkgScalingFactor(fitResultBkgScalingMain, fitResultBkgScalingRef, 5));
                            }
                            else
                            {
                                std::cout << "One of fits for the bkg parameters scaling is not valid: \n\t" << fitResultBkgScalingMain << "\n\t" << fitResultBkgScalingRef << std::endl;
                            }
                        }

                        // use the R_bkg parameter from the reference fit
                        funcMain->FixParameter(6, fitResultRef->Parameter(6));
                    }
                    else
                    {
                        std::cout << "Fit with the bkg parameters is not valid: \n\t" << hMain << "\n\t" << funcMain << std::endl;
                    }

                    fOut->cd();
                }

                // do a fit and draw if it is valid
                const auto fitResult = doFit(hMain, funcMain);
                if ((fitResult.Get() != nullptr) && fitResult->IsValid())
                {
                    funcMain->SetLineColor(kRed);
                    funcMain->SetLineWidth(lineWidth);
                    funcMain->Draw("SAME");
                    tl->AddEntry(funcMain, histType + " (fit)", "l");

                    // draw the pull distribution
                    p2->cd();
                    drawPull(hMain, funcMain, fitRangeMin);
                    p1->cd();
                }
                else
                {
                    const auto curStyle = gStyle->GetOptFit();
                    gStyle->SetOptFit(0);
                    hMain->UseCurrentStyle();
                    gStyle->SetOptFit(curStyle);
                    std::cout << "Fit is not valid: \n\t" << hMain << "\n\t" << funcMain << std::endl;
                }
            }

            // add description / legend
            printDescription(dataType, isMultBinsOnly, i, j);
            tl->Draw("SAME");
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

    fOut->Close();
    delete fOut;

    if (fInRef != nullptr)
    {
        fInRef->Close();
        delete fInRef;
    }
    if (fInRefFitResults != nullptr)
    {
        fInRefFitResults->Close();
        delete fInRefFitResults;
    }
}

void fitInBins(const TString inputFile, const TString hMainNameBase, const bool flagIsMc, const bool flagIsUnlike, const TString dataType, TString hCommonEndNameForMult, TString hCommonEndNameForKt, const bool flagDoFit, const TString inputFileRef, const TString hRefNameBase, const TString refType, const bool flagDrawRef, const bool flagUseBkgFromRef, const bool flagIsBkgScaling, const bool flagUseBkgScaling, const TString funcNameBkgScalingMain, const TString funcNameBkgScalingRef, const int &ignoreBinMultLower, const int &ignoreBinMultUpper, const int &ignoreBinMultForKtLower, const int &ignoreBinMultForKtUpper, const int &ignoreBinKtLower, const int &ignoreBinKtUpper)
{
    // get configuration
    auto selection = SelectionClass();

    auto nrBinsMult = selection.getNrOfBinsMult();
    auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    auto nrBinsKt = selection.getNrOfBinsKt();

    // call for mult bins only
    fitInBinsGeneric(inputFile, hMainNameBase, flagIsMc, flagIsUnlike, dataType, nrBinsMult, 0, hCommonEndNameForMult, flagDoFit, inputFileRef, hRefNameBase, refType, flagDrawRef, flagUseBkgFromRef, flagIsBkgScaling, flagUseBkgScaling, funcNameBkgScalingMain, funcNameBkgScalingRef, ignoreBinMultLower, ignoreBinMultUpper, 0, 1);
    // call for mult + kT bins
    fitInBinsGeneric(inputFile, hMainNameBase, flagIsMc, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt, flagDoFit, inputFileRef, hRefNameBase, refType, flagDrawRef, flagUseBkgFromRef, flagIsBkgScaling, flagUseBkgScaling, funcNameBkgScalingMain, funcNameBkgScalingRef, ignoreBinMultForKtLower, ignoreBinMultForKtUpper, ignoreBinKtLower, ignoreBinKtUpper);
}
