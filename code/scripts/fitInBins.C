#include "../HBTAnalysis/Units.hpp"
#include "../HBTAnalysis/Utils.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"
#include "../HBTAnalysis/CoulombCorrection.hpp"

#include "fitModel.C"

#include "Fit/BinData.h"
#include "Fit/FitConfig.h"
#include "Fit/DataOptions.h"
#include "Fit/DataRange.h"
#include "Fit/Fitter.h"

#include "Math/WrappedMultiTF1.h"
#include "Math/MinimizerOptions.h"

// use consts from Units
using namespace HBT::Units;

/*-------------- configuration -------------------*/

const int padSize = 600;
const HBT::Units::FloatType lineWidth = 1.0;

const Double_t fitRangeLikeMin = 0.05;
const Double_t fitRangeUnlikeMin = 0.25;
const Double_t fitRangeMcMin = 0.25;
const Double_t fitRangeBkgScaleMin = 0.4;
const Double_t fitRangeMax = 2.0;

/*-------------- enf of configuration -------------*/

/*-------------- define fitter helper functions  -----------------*/

Double_t minimserFuncPartial(Double_t &a, Double_t &b, Double_t &c)
{
    return a * log(((1 + c) * a) / (c * (a + b + 2))) + (b + 2) * log(((1 + c) * (b + 2)) / (a + b + 2));
}
struct funcLogLikelihood
{
    funcLogLikelihood(const ROOT::Fit::BinData *dataSet1, const ROOT::Fit::BinData *dataSet2, TF1 *fitFunc, const unsigned int nrOfDataPoints) : m_dataSet1(dataSet1), m_dataSet2(dataSet2), m_fitFunc(fitFunc), m_nrOfDataPoints(nrOfDataPoints) {}

    double operator()(const double *par) const
    {
        // set the current parameters
        m_fitFunc->SetParameters(par);

        // prepare a tmp variable for the negative log-likelihood value
        Double_t tmp{0.};

        for (unsigned int i = 0; i < m_nrOfDataPoints; ++i)
        {
            // get values
            Double_t val1 = m_dataSet1->Value(i);
            Double_t val2 = m_dataSet2->Value(i);

            auto binCenter = *(m_dataSet1->Coords(i));
            Double_t funcVal = m_fitFunc->Eval(binCenter);

            // leave for debugging
            // std::cout << "Hello there! \t" << i << "\t" << binCenter << "\t" << val1 << "\t" << val2 << "\t" << val1 / val2 << "\t" << funcVal << "\t" << std::endl;

            tmp += minimserFuncPartial(val1, val2, funcVal);
        }

        // scale to make a negative log-likelihood
        return 2 * tmp;
    }

    const ROOT::Fit::BinData *m_dataSet1;
    const ROOT::Fit::BinData *m_dataSet2;
    TF1 *m_fitFunc;
    const unsigned int m_nrOfDataPoints;
};

/*-------------- end of fitter helper functions -----------------*/

Double_t getBkgScalingFactor(const TFitResult *fitResultMain, const TFitResult *fitResultRef, const int &parameterId)
{
    return float(fitResultMain->Parameter(parameterId)) / fitResultRef->Parameter(parameterId);
}

TFitResultPtr doFit(const ROOT::Fit::BinData *dataSet, const ROOT::Fit::BinData *dataSet1, const ROOT::Fit::BinData *dataSet2, TF1 *fitFunc, ROOT::Fit::Fitter &fitter, const TString &histName, const TString &funcName)
{
    // sanitise input
    const unsigned int nrOfDataPoints = dataSet1->NPoints();
    if (dataSet2->NPoints() != nrOfDataPoints)
    {
        std::cout << "Different number of bins in the histograms. Check input." << std::endl
                  << "\t" << histName << std::endl
                  << std::endl;
        return TFitResultPtr();
    }

    // 'regular' fitter usage
    // fitter.Fit(*dataSet); // LeastSquareFit (identical results as with the standard TF1 to TH1 fit)
    // fitter.LikelihoodFit(*dataSet); // LikelihoodFit (not suitable in general; huge errors)

    // use a custom function to minimise
    auto funcToMinimise = funcLogLikelihood(dataSet1, dataSet2, fitFunc, nrOfDataPoints);
    fitter.FitFCN(fitFunc->GetNpar(), funcToMinimise, 0, nrOfDataPoints, false); // 0 means to use the fit config defined before

    TFitResultPtr fitResult = new TFitResult(fitter.Result());

    // sanitize TFitResultPtr
    if (fitResult.Get() == nullptr)
        return TFitResultPtr();

    fitResult->Write(HBT::Utils::getFitResultName(histName, funcName));

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

void printFitInfo(const TFitResultPtr &fitResult)
{
    // prepare info
    std::stringstream myStr;
    myStr << "MinFcn / ndf : " << round(fitResult->MinFcnValue()) << " / " << fitResult->Ndf();

    // print
    const HBT::Units::FloatType commPosX = 0.50;
    const HBT::Units::FloatType commPosY = 0.35;
    TLatex comments;
    comments.SetNDC();
    comments.SetTextSize(0.03);
    comments.DrawLatex(commPosX, commPosY - 0.15, myStr.str().c_str());
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

void fitInBinsGeneric(const TString inputFile, const TString hMainNameBase, const TString hMainNameBase_1, const TString hMainNameBase_2, const bool flagIsMc, const bool flagIsUnlike, const TString dataType, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName, const bool flagDoFit, const TString inputFileRef, const TString hRefNameBase, const TString refType, const bool flagDrawRef, const bool flagUseBkgFromRef, const bool flagIsBkgScaling, const bool flagUseBkgScaling, const TString funcNameBkgScalingMain, const TString funcNameBkgScalingRef, const int &ignoreBinMultLower, const int &ignoreBinMultUpper, const int &ignoreBinKtLower, const int &ignoreBinKtUpper)
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

    // get info on analysis bins
    const auto selection = SelectionClass();
    const auto curMultValues = isMultBinsOnly ? selection.getBinsOfMultiplicityCentres() : selection.getBinsOfMultiplicityForKtCentres();
    const auto curKtValues = selection.getBinsOfKtCentres();

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

            TLegend *tl = new TLegend(0.30, 0.70, 0.60, 0.85);

            TPad *p1 = new TPad("p1", "p1", 0., 0.33, 1., 1.);
            TPad *p2 = new TPad("p2", "p2", 0., 0., 1., 0.33);
            p2->SetBottomMargin(0.15);
            p1->Draw();
            p2->Draw();

            // get histograms
            TString hMainName = HBT::Utils::getHistogramName(hMainNameBase, hCommonEndName, true, !isMultBinsOnly, i, j);
            TH1D *hMain = (TH1D *)fIn->Get(hMainName);
            TH1D *hMain_1 = (TH1D *)fIn->Get(HBT::Utils::getHistogramName(hMainNameBase_1, hCommonEndName, true, !isMultBinsOnly, i, j));
            TH1D *hMain_2 = (TH1D *)fIn->Get(HBT::Utils::getHistogramName(hMainNameBase_2, hCommonEndName, true, !isMultBinsOnly, i, j));

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
                // get fit configuration
                auto fitParams = prepareFitParams();

                // create a function relevant for the histogram type (from a generic form)
                const auto fitRangeMin = flagIsBkgScaling ? fitRangeBkgScaleMin : flagIsMc   ? fitRangeMcMin
                                                                              : flagIsUnlike ? fitRangeUnlikeMin
                                                                                             : fitRangeLikeMin;
                TF1 *funcMain = new TF1(funcName, flagIsUnlike ? funcFullUnlike : funcFullLike, fitRangeMin, fitRangeMax, fitParams.size());
                setParNames(funcMain, fitParams);

                // prepare data
                ROOT::Fit::DataOptions dataOpts;
                ROOT::Fit::DataRange dataRange(funcMain->GetXmin(), funcMain->GetXmax());
                ROOT::Fit::BinData dataMain(dataOpts, dataRange);
                ROOT::Fit::BinData dataMain_1(dataOpts, dataRange);
                ROOT::Fit::BinData dataMain_2(dataOpts, dataRange);
                ROOT::Fit::FillData(dataMain, hMain);
                ROOT::Fit::FillData(dataMain_1, hMain_1);
                ROOT::Fit::FillData(dataMain_2, hMain_2);

                // prepare fitter
                ROOT::Fit::Fitter fitterMain;
                ROOT::Math::WrappedMultiTF1 fitFuncMain(*funcMain, funcMain->GetNdim());
                fitterMain.SetFunction(fitFuncMain, false); // false to not pass the derivatives (to be calculated by the minimisation algorithm)

                // settings common for all histogram types
                setInitValues(fitterMain, fitParams);
                setLimits(fitterMain, fitParams);
                setFixed(fitterMain, fitParams, funcMain);
                setBinCentres(fitterMain, fitParams, curMultValues[i], curKtValues[j], isMultBinsOnly);

                // settings for the specific histogram types (keep an eye on 'priority': bkgScaling -> MC -> UNLIKE -> LIKE)
                // set the unused (fixed) parameters to some neutral value (e.g. 0)
                if (flagIsBkgScaling)
                {
                    fitterMain.Config().ParSettings(1).SetValue(0);
                    fitterMain.Config().ParSettings(1).Fix();
                    fitterMain.Config().ParSettings(2).SetValue(0);
                    fitterMain.Config().ParSettings(2).Fix();
                    fitterMain.Config().ParSettings(3).SetValue(0);
                    fitterMain.Config().ParSettings(3).Fix();
                }
                else if (flagIsMc)
                {
                    fitterMain.Config().ParSettings(1).SetValue(0);
                    fitterMain.Config().ParSettings(1).Fix();
                    fitterMain.Config().ParSettings(2).SetValue(0);
                    fitterMain.Config().ParSettings(2).Fix();
                    fitterMain.Config().ParSettings(3).SetValue(0);
                    fitterMain.Config().ParSettings(3).Fix();
                }
                else if (flagIsUnlike)
                {
                    const auto idScaleZ = fitParams.at("scaleZ").id();
                    fitterMain.Config().ParSettings(idScaleZ).Fix();
                    fitterMain.Config().ParSettings(idScaleZ).SetValue(1);
                    funcMain->FixParameter(idScaleZ, 1);

                    const auto idLambda = fitParams.at("lambda").id();
                    fitterMain.Config().ParSettings(idLambda).Fix();
                    fitterMain.Config().ParSettings(idLambda).SetValue(0);
                    funcMain->FixParameter(idLambda, 0);

                    const auto idRadius = fitParams.at("radius").id();
                    fitterMain.Config().ParSettings(idRadius).Fix();
                    fitterMain.Config().ParSettings(idRadius).SetValue(0);
                    funcMain->FixParameter(idRadius, 0);
                }

                // use the bkg parametrisation from a reference fit
                if (flagUseBkgFromRef)
                {
                    // use the fit only if it was valid
                    if (fitResultRef && fitResultRef->IsValid())
                    {
                        // use the bkg sigma from the reference fit
                        const std::vector<std::string> listForSigma{"sigmaBkgMult", "sigmaBkgKt", "sigmaBkgSig0", "sigmaBkgSig1", "sigmaBkgMult0", "sigmaBkgExpKt"};
                        for (const auto &paramName : listForSigma)
                        {
                            const auto paramId = fitParams.at(paramName).id();
                            fitterMain.Config().ParSettings(paramId).Fix();
                            fitterMain.Config().ParSettings(paramId).SetValue(fitResultRef->Parameter(paramId));
                            funcMain->FixParameter(paramId, fitResultRef->Parameter(paramId));
                        }

                        // use bkg amplitude from the reference fit
                        const std::vector<std::string> listForAmpl{"amplBkgMult", "amplBkgKt", "amplBkgAmpl0", "amplBkgKt0", "amplBkgExpMult"};
                        for (const auto &paramName : listForAmpl)
                        {
                            const auto paramId = fitParams.at(paramName).id();
                            fitterMain.Config().ParSettings(paramId).Fix();
                            fitterMain.Config().ParSettings(paramId).SetValue(fitResultRef->Parameter(paramId));
                            funcMain->FixParameter(paramId, fitResultRef->Parameter(paramId));
                        }

                        // scale the bkg amplitude if needed
                        if (flagUseBkgScaling)
                        {
                            // use a free z parameter for now (do not do anything)

                            // auto fitResultBkgScalingMain = (TFitResult *)fInRefFitResults->Get(HBT::Utils::getFitResultName(hMainName, funcNameBkgScalingMain));
                            // auto fitResultBkgScalingRef = (TFitResult *)fInRefFitResults->Get(HBT::Utils::getFitResultName(hRefName, funcNameBkgScalingRef));

                            // if (fitResultBkgScalingMain->IsValid() && fitResultBkgScalingRef->IsValid())
                            // {
                            //     // set z param
                            //     fitterMain.Config().ParSettings(fitParams.at("scaleZ").id()).SetValue(getBkgScalingFactor(fitResultBkgScalingMain, fitResultBkgScalingRef, fitParams.at("scaleZ").id()));
                            //     fitterMain.Config().ParSettings(fitParams.at("scaleZ").id()).Fix();
                            // }
                            // else
                            // {
                            //     std::cout << "One of fits for the bkg parameters scaling is not valid: \n\t" << fitResultBkgScalingMain << "\n\t" << fitResultBkgScalingRef << std::endl;
                            // }
                        }
                    }
                    else
                    {
                        std::cout << "Fit with the bkg parameters is not valid: \n\t" << hMain << "\n\t" << funcMain << std::endl;
                    }

                    fOut->cd();
                }

                // minimiser configuration
                fitterMain.Config().MinimizerOptions().SetPrintLevel(0);
                // fitterMain.Config().SetMinimizer("Minuit2", "Migrad");

                ROOT::Math::MinimizerOptions minimiserOpts;
                minimiserOpts.Print();

                // do a fit and draw if it is valid
                const auto fitResult = doFit(&dataMain, &dataMain_1, &dataMain_2, funcMain, fitterMain, hMain->GetName(), funcMain->GetName());
                fitterMain.Result().Print(std::cout, true);

                if ((fitResult.Get() != nullptr) && fitResult->IsValid())
                {
                    funcMain->SetFitResult(*fitResult);
                    funcMain->SetLineColor(kRed);
                    funcMain->SetLineWidth(lineWidth);

                    hMain->GetListOfFunctions()->Add(funcMain);
                    funcMain->Draw("SAME");
                    tl->AddEntry(funcMain, histType + " (fit)", "l");

                    // draw/print the fit info
                    printFitInfo(fitResult);

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

void fitInBins(const TString inputFile, const TString hMainNameBase, const TString hMainNameBase_1, const TString hMainNameBase_2, const bool flagIsMc, const bool flagIsUnlike, const TString dataType, TString hCommonEndNameForMult, TString hCommonEndNameForKt, const bool flagDoFit, const TString inputFileRef, const TString hRefNameBase, const TString refType, const bool flagDrawRef, const bool flagUseBkgFromRef, const bool flagIsBkgScaling, const bool flagUseBkgScaling, const TString funcNameBkgScalingMain, const TString funcNameBkgScalingRef, const int &ignoreBinMultLower, const int &ignoreBinMultUpper, const int &ignoreBinMultForKtLower, const int &ignoreBinMultForKtUpper, const int &ignoreBinKtLower, const int &ignoreBinKtUpper)
{
    // get configuration
    auto selection = SelectionClass();

    auto nrBinsMult = selection.getNrOfBinsMult();
    auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    auto nrBinsKt = selection.getNrOfBinsKt();

    // call for mult bins only
    fitInBinsGeneric(inputFile, hMainNameBase, hMainNameBase_1, hMainNameBase_2, flagIsMc, flagIsUnlike, dataType, nrBinsMult, 0, hCommonEndNameForMult, flagDoFit, inputFileRef, hRefNameBase, refType, flagDrawRef, flagUseBkgFromRef, flagIsBkgScaling, flagUseBkgScaling, funcNameBkgScalingMain, funcNameBkgScalingRef, ignoreBinMultLower, ignoreBinMultUpper, 0, 1);
    // call for mult + kT bins
    fitInBinsGeneric(inputFile, hMainNameBase, hMainNameBase_1, hMainNameBase_2, flagIsMc, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt, flagDoFit, inputFileRef, hRefNameBase, refType, flagDrawRef, flagUseBkgFromRef, flagIsBkgScaling, flagUseBkgScaling, funcNameBkgScalingMain, funcNameBkgScalingRef, ignoreBinMultForKtLower, ignoreBinMultForKtUpper, ignoreBinKtLower, ignoreBinKtUpper);
}
