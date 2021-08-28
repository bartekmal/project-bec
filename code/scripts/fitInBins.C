#include "../HBTAnalysis/Units.hpp"
#include "../HBTAnalysis/Utils.hpp"
#include "../utils/Styles.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"
#include "../HBTAnalysis/CoulombCorrection.hpp"

#include "fitModel.C"

#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TF3.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TLatex.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"

#include "Fit/BinData.h"
#include "Fit/FitConfig.h"
#include "Fit/DataOptions.h"
#include "Fit/DataRange.h"
#include "Fit/Fitter.h"

#include "Math/WrappedMultiTF1.h"
#include "Math/MinimizerOptions.h"

#include <memory>
#include <iostream>
#include <vector>

// use consts from Units
using namespace HBT::Units;

// define types for data structure
using DataStructureKt = std::vector<std::shared_ptr<ROOT::Fit::BinData>>;
using DataStructure = std::vector<DataStructureKt>;

using FuncStructureKt = std::vector<std::shared_ptr<TF3>>;
using FuncStructure = std::vector<FuncStructureKt>;

/*-------------- configuration -------------------*/

const Double_t fitRangeLikeMin = 0.05;
const Double_t fitRangeUnlikeMin = 0.25;
const Double_t fitRangeMcMin = 0.25;
const Double_t fitRangeMax = 2.0;

const HBT::Units::FloatType descriptionPosX = 0.425;
const HBT::Units::FloatType descriptionPosY = 0.25;

/*-------------- enf of configuration -------------*/

/*-------------- define fitter helper functions  -----------------*/

Double_t minimserFuncPartial(const Double_t &a, const Double_t &b, const Double_t &c)
{
    return a * log(((1 + c) * a) / (c * (a + b + 2))) + (b + 2) * log(((1 + c) * (b + 2)) / (a + b + 2));
}

struct funcLogLikelihood
{
    funcLogLikelihood(const ROOT::Fit::BinData *dataSet1, const ROOT::Fit::BinData *dataSet2, TF3 *fitFunc) : m_dataSet1(dataSet1), m_dataSet2(dataSet2), m_fitFunc(fitFunc) {}

    double operator()(const double *par) const
    {
        // set the current parameters
        m_fitFunc->SetParameters(par);

        // prepare a tmp variable for the negative log-likelihood value
        Double_t tmp{0.};

        for (unsigned int i = 0; i < m_dataSet1->NPoints(); ++i)
        {
            // get values
            const auto binCenter = m_dataSet1->Coords(i);
            const auto val1 = m_dataSet1->Value(i);
            const auto val2 = m_dataSet2->Value(i);

            const auto funcVal = m_fitFunc->Eval(binCenter[0], binCenter[1], binCenter[2]);

            // leave for debugging
            // std::cout << "Hello there! \t" << i << "\t" << binCenter[0] << "\t" << binCenter[1] << "\t" << binCenter[2] << "\t" << val1 << "\t" << val2 << "\t" << val1 / val2 << "\t" << funcVal << "\t" << std::endl;

            tmp += minimserFuncPartial(val1, val2, funcVal);
        }

        // scale to make a negative log-likelihood
        return 2 * tmp;
    }

    const ROOT::Fit::BinData *m_dataSet1;
    const ROOT::Fit::BinData *m_dataSet2;
    TF3 *m_fitFunc;
};

struct minimiserGlobal
{
    using MinimiserStructure = std::vector<funcLogLikelihood>;

    minimiserGlobal(const DataStructure &dataSet1, const DataStructure &dataSet2, FuncStructure &funcSet, const unsigned int &nParamsCommon, const unsigned int &nParamsIndividual) : m_nParamsCommon(nParamsCommon), m_nParamsIndividual(nParamsIndividual), m_nParamsSingle(nParamsCommon + nParamsIndividual)
    {
        // checked in the main code that all of the data/func structures are of equal size
        const unsigned int nrBinsMult = dataSet1.size();
        const unsigned int nrBinsKt = dataSet1.front().size();

        for (unsigned int i = 0; i < nrBinsMult; ++i)
        {
            for (unsigned int j = 0; j < nrBinsKt; ++j)
            {
                m_minimiserFuncs.emplace_back(dataSet1[i][j].get(), dataSet2[i][j].get(), funcSet[i][j].get());
            }
        }
    }

    double operator()(const double *par) const
    {
        // prepare a tmp variable for the minimiser value
        Double_t tmp{0.};

        for (unsigned int i = 0; i < m_minimiserFuncs.size(); ++i)
        {
            // offset/map parameters accordingly
            double curPars[m_nParamsSingle];
            for (unsigned int iCommon = 0; iCommon < m_nParamsCommon; ++iCommon)
            {
                curPars[iCommon] = par[iCommon];
            }
            for (unsigned int iInd = m_nParamsCommon; iInd < m_nParamsSingle; ++iInd)
            {
                curPars[iInd] = par[iInd + i * m_nParamsIndividual];
            }
            tmp += m_minimiserFuncs[i](curPars);
        }

        return tmp;
    }

public:
    inline unsigned int getNIndividualFits() const { return m_minimiserFuncs.size(); }

private:
    MinimiserStructure m_minimiserFuncs{};
    const unsigned int m_nParamsCommon{};
    const unsigned int m_nParamsIndividual{};
    const unsigned int m_nParamsSingle{};
};

/*-------------- end of fitter helper functions -----------------*/

TFitResultPtr doGlobalFit(const DataStructure &dataSet, const DataStructure &dataSet1, const DataStructure &dataSet2, FuncStructure &funcSet, ROOT::Fit::Fitter &fitter, const unsigned int &nParamsCommon, const unsigned int &nParamsIndividual, const unsigned int &nParamsGlobal, const unsigned int &nDataPoints, const TString &resultName)
{
    // use a custom function to minimise
    auto funcToMinimise = minimiserGlobal(dataSet1, dataSet2, funcSet, nParamsCommon, nParamsIndividual);

    std::cout << std::endl
              << "Performing a global fit (nIndividualFits nParamsCommon nParamsIndividual nParamsGlobal nDataPointsGlobal):\t" << funcToMinimise.getNIndividualFits() << "\t" << nParamsCommon << "\t" << nParamsIndividual << "\t" << nParamsGlobal << "\t" << nDataPoints << std::endl
              << std::endl;

    fitter.FitFCN(nParamsGlobal, funcToMinimise, 0, nDataPoints, false); // 0 means to use the fit config defined before

    TFitResultPtr fitResult = new TFitResult(fitter.Result());

    // sanitize TFitResultPtr
    if (fitResult.Get() == nullptr)
        return TFitResultPtr();

    fitResult->Write(resultName);

    return fitResult;
}

TFitResultPtr doFit(const ROOT::Fit::BinData *dataSet, const ROOT::Fit::BinData *dataSet1, const ROOT::Fit::BinData *dataSet2, TF3 *fitFunc, ROOT::Fit::Fitter &fitter, const TString &histName, const TString &funcName)
{
    const unsigned int nrOfDataPoints = dataSet1->NPoints();

    // 'regular' fitter usage
    // fitter.Fit(*dataSet); // LeastSquareFit (identical results as with the standard TF1 to TH1 fit)
    // fitter.LikelihoodFit(*dataSet); // LikelihoodFit (not suitable in general; huge errors)

    // use a custom function to minimise
    auto funcToMinimise = funcLogLikelihood(dataSet1, dataSet2, fitFunc);
    fitter.FitFCN(fitFunc->GetNpar(), funcToMinimise, 0, nrOfDataPoints, false); // 0 means to use the fit config defined before

    TFitResultPtr fitResult = new TFitResult(fitter.Result());

    // sanitize TFitResultPtr
    if (fitResult.Get() == nullptr)
        return TFitResultPtr();

    fitResult->Write(HBT::Utils::getFitResultName(histName, funcName));

    return fitResult;
}

void printDescription(const bool &isMultBinsOnly, const int &currentMultBin, const int &currentKtBin)
{
    // config / info
    const auto selection = SelectionClass();
    const auto binRangesMult = isMultBinsOnly ? selection.getBinsOfMultiplicityRangesAsStrings() : selection.getBinsOfMultiplicityForKtRangesAsStrings();
    const auto binRangesKt = selection.getBinsOfKtRangesAsStrings();

    // make description
    auto plotDescription = HBT::Styles::makeLhcbLabel(descriptionPosX, descriptionPosX + 0.25, descriptionPosY + 0.10, descriptionPosY + 0.20);
    plotDescription->SetTextSize(plotDescription->GetTextSize() * 0.825); // ! make slighlty smaller
    HBT::Utils::addMultilineText(std::string("#font[12]{N}_{VELO} : " + binRangesMult[currentMultBin]).c_str(), plotDescription);
    if (!isMultBinsOnly)
    {
        HBT::Utils::addMultilineText(std::string("#font[12]{k}_{T} [GeV] : " + binRangesKt[currentKtBin]).c_str(), plotDescription);
    }
    plotDescription->Draw();
}

void printFitInfo(const TFitResultPtr &fitResult)
{
    // prepare info
    std::stringstream myStr;
    myStr << "MinFcn / ndf : " << round(fitResult->MinFcnValue()) << " / " << fitResult->Ndf();

    // make description
    auto plotDescription = HBT::Styles::makeLhcbLabel(descriptionPosX, descriptionPosX + 0.25, descriptionPosY, descriptionPosY + 0.075);
    plotDescription->SetTextSize(plotDescription->GetTextSize() * 0.825); // ! make slighlty smaller
    HBT::Utils::addMultilineText(myStr.str().c_str(), plotDescription);

    plotDescription->Draw();
}

// provide custom style (based on the general ones)
void setStyleLocal(const unsigned int &flagStyle)
{
    HBT::Styles::setStyle(flagStyle);

    // local style settings
    gStyle->SetMarkerSize(gStyle->GetMarkerSize() * 0.75); // ! a lot of points - improve visibility
    if (HBT::Styles::flagShowStats(flagStyle))
    {
        gStyle->SetStatW(0.13);
    }
    else
    {
        gStyle->SetOptFit(0);
    }
    gROOT->ForceStyle();
}

void drawPull(const TH1D *hFit, const TF1 *fFit, const HBT::Units::FloatType &fitRangeMin, const TString drawOpts = "BSAME")
{
    // create a histogram with identical binning
    TH1D *hPull = (TH1D *)hFit->Clone(TString(hFit->GetName()) + "_pull");
    hPull->SetTitle("pull;#font[12]{Q} [GeV]; pull");
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
    hPull->SetFillColor(HBT::Styles::getColorPrimary());
    hPull->GetYaxis()->SetRangeUser(-6, 6);
    hPull->Draw(drawOpts);

    //draw lines
    auto lineUp = new TLine(0, 3, 2, 3);
    lineUp->SetLineColor(HBT::Styles::getColorEmphasize());
    lineUp->Draw("SAME");
    auto lineDown = new TLine(0, -3, 2, -3);
    lineDown->SetLineColor(HBT::Styles::getColorEmphasize());
    lineDown->Draw("SAME");
}

void drawHistogram(TH1D *h, const int color = kBlue, const Style_t marker = gStyle->GetMarkerStyle(), const TString drawOpts = "ESAME", const Double_t yMin = 0.5, const Double_t yMax = 1.6)
{

    //draw histogram
    h->GetYaxis()->SetRangeUser(yMin, yMax);
    h->SetMarkerStyle(marker);
    h->SetMarkerColor(color);
    h->SetLineColor(color);

    h->Draw(drawOpts);
}

void fillData(ROOT::Fit::BinData &data, const TH1D &hist, const float &multValue, const float &kTValue)
{
    for (unsigned int pointId = 1; pointId < hist.GetNbinsX() + 1; ++pointId)
    {
        const auto binCenter = hist.GetBinCenter(pointId);
        const auto binContent = hist.GetBinContent(pointId);

        // use only non-empty bins
        if (HBT::Utils::compareFloats(binContent, 0.))
        {
            continue;
        }

        // take data range into account (could be improved using the dedicated DataRange config)
        const auto range = data.Range().Ranges(0).at(0);
        if ((binCenter < range.first) || (binCenter > range.second))
        {
            continue;
        }

        double dataPoint[3];
        dataPoint[0] = binCenter;
        dataPoint[1] = multValue;
        dataPoint[2] = kTValue;
        data.Add(dataPoint, binContent, hist.GetBinError(pointId));
    }

    // for debugging
    // std::cout << "Hello there! (new)\t" << data.Size() << "\t" << data.Range().Ranges(0).at(0).first << std::endl;
}

template <class DataStruct2D>
bool checkDataStruct(DataStruct2D data, const unsigned int &sizeDim1, const unsigned int &sizeDim2)
{
    bool isOk = true;

    if (data.size() == sizeDim1)
    {
        for (const auto &el : data)
        {
            if (el.size() != sizeDim2)
            {
                isOk = false;
                break;
            }

            // check also if the stored pointers are valid
            for (const auto &ptr : el)
            {
                if (!ptr)
                {
                    isOk = false;
                    break;
                }
            }
        }
    }
    else
    {
        isOk = false;
    }

    return isOk;
}

bool checkBinDataSet(const DataStructure &binDataSet, const unsigned int &nPoints)
{
    for (const auto &elDim1 : binDataSet)
    {
        for (const auto &elDim2 : elDim1)
        {
            if (elDim2->NPoints() != nPoints)
            {
                return false;
            }
        }
    }
    return true;
}

std::string getBinCombinations(const DataStructure &binDataSet)
{
    std::stringstream ss;

    for (const auto &elDim1 : binDataSet)
    {
        // tab in new line
        ss << "\t";
        for (const auto &elDim2 : elDim1)
        {
            const auto coords = elDim2->Coords(0); // get coords of the first element in BinData (all mult/kT are the same)
            ss << coords[1] << " / " << coords[2] << "\t";
        }
        // each mult bin in new line
        ss << std::endl;
    }

    return ss.str();
}

void fitInBinsGeneric(const TString inputFile, const TString hMainNameBase, const TString hMainNameBase_1, const TString hMainNameBase_2, const bool flagIsMc, const bool flagIsUnlike, const TString dataType, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName, const int flagDoFit, const TString inputFileRef, const TString hRefNameBase, const TString refType, const bool flagDrawRef, const bool flagUseBkgFromRef, const bool flagUseBkgScaling, const int &ignoreBinMultLower, const int &ignoreBinMultUpper, const int &ignoreBinKtLower, const int &ignoreBinKtUpper, const unsigned int &flagStyle)
{
    // set ROOT style
    setStyleLocal(flagStyle);

    // prepare settings for type of binning and histogram type
    const bool isMultBinsOnly = (nrBinsKt == 0) ? true : false;
    const int nrBinsKtForLoops = isMultBinsOnly ? 1 : nrBinsKt;

    const bool doFitIndividual = (flagDoFit == 1);
    const bool doFitGlobal = (flagDoFit == 2);

    const TString binsType = isMultBinsOnly ? TString("mult") : TString("kT");
    const TString histType = flagIsUnlike ? TString("UNLIKE") : TString("LIKE");
    const TString funcName = TString("funcMain");
    const TString title = funcName + "_" + binsType + "_" + histType;
    const TString resName = binsType + "_globalFit";
    const TString funcBkgScalingName = "funcScaleZ";

    // get info on analysis bins
    const auto selection = SelectionClass();
    const auto curMultValues = isMultBinsOnly ? selection.getBinsOfMultiplicityCentres() : selection.getBinsOfMultiplicityForKtCentres();
    const auto curKtValues = isMultBinsOnly ? selection.getAverageKtForMultBins() : selection.getBinsOfKtCentres();
    const auto maxMultValue = curMultValues.back();
    const auto maxKtValue = curKtValues.back();

    // prepare canvas
    const int canvasSizeHeight = flagDoFit ? HBT::Styles::defaultCanvasSizeY * 1.5 : HBT::Styles::defaultCanvasSizeY;
    const int canvasSizeWidth = isMultBinsOnly ? HBT::Styles::defaultCanvasSizeX : nrBinsKtForLoops * HBT::Styles::defaultCanvasSizeX;
    auto tc = std::make_unique<TCanvas>(title, title, canvasSizeWidth, canvasSizeHeight);
    const TString plotFile = title + ".pdf";
    tc->SaveAs(plotFile + "[");

    // prepare I/O files
    auto fIn = std::make_unique<TFile>(inputFile, "READ");
    auto fOut = std::make_unique<TFile>("fitResults.root", "UPDATE");

    // optional file with reference histograms / fits
    auto fInRef = flagDrawRef ? std::make_unique<TFile>(inputFileRef, "READ") : nullptr;
    auto fInRefFitResults = flagUseBkgFromRef ? std::make_unique<TFile>(TString(inputFileRef).ReplaceAll("correlations.root", "fitResults.root").ReplaceAll("correlations", "fits"), "READ") : nullptr;
    auto fInRefBkgScaling = flagUseBkgScaling ? std::make_unique<TFile>(TString(inputFileRef).ReplaceAll("correlations.root", "trends.root").ReplaceAll("correlations", "trends"), "READ") : nullptr;

    // prepare a generic fit function (for the given histogram type)
    const auto fitParams = flagIsUnlike ? prepareParamsUnlike(isMultBinsOnly) : prepareParamsFull(isMultBinsOnly); // pass isMultBinsOnly to make kT terms neutral for mult bins
    const auto fitRangeMin = flagIsMc       ? fitRangeMcMin
                             : flagIsUnlike ? fitRangeUnlikeMin
                                            : fitRangeLikeMin;
    auto funcMain = std::make_unique<TF3>(funcName, flagIsUnlike ? funcFullUnlike : funcFullLike, fitRangeMin, fitRangeMax, 0, maxMultValue, 0, maxKtValue, fitParams.size());

    // create data structures
    ROOT::Fit::DataOptions dataOpts;
    ROOT::Fit::DataRange dataRange(funcMain->GetXmin(), funcMain->GetXmax(), funcMain->GetYmin(), funcMain->GetYmax(), funcMain->GetZmin(), funcMain->GetZmax());

    // init & fill
    DataStructure dataMain{};
    DataStructure dataMain_1{};
    DataStructure dataMain_2{};

    for (int i = 0; i < nrBinsMult; ++i)
    {
        // check if this bin should be ignored
        if (i < ignoreBinMultLower || i >= ignoreBinMultUpper)
            continue;

        dataMain.emplace_back();
        dataMain_1.emplace_back();
        dataMain_2.emplace_back();

        auto &curDataMainMult = dataMain.back();
        auto &curDataMainMult_1 = dataMain_1.back();
        auto &curDataMainMult_2 = dataMain_2.back();

        for (int j = 0; j < nrBinsKtForLoops; ++j)
        {
            // check if this bin should be ignored
            if (j < ignoreBinKtLower || j >= ignoreBinKtUpper)
                continue;

            // get histograms
            TString hMainName = HBT::Utils::getHistogramName(hMainNameBase, hCommonEndName, true, !isMultBinsOnly, i, j);
            TH1D *hMain = (TH1D *)fIn->Get(hMainName);
            TH1D *hMain_1 = (TH1D *)fIn->Get(HBT::Utils::getHistogramName(hMainNameBase_1, hCommonEndName, true, !isMultBinsOnly, i, j));
            TH1D *hMain_2 = (TH1D *)fIn->Get(HBT::Utils::getHistogramName(hMainNameBase_2, hCommonEndName, true, !isMultBinsOnly, i, j));

            const unsigned int nDataPointsMax = hMain->GetNbinsX();

            // create & fill data
            auto dataSmart = std::make_shared<ROOT::Fit::BinData>(dataOpts, dataRange, nDataPointsMax, 3);
            auto dataSmart_1 = std::make_shared<ROOT::Fit::BinData>(dataOpts, dataRange, nDataPointsMax, 3);
            auto dataSmart_2 = std::make_shared<ROOT::Fit::BinData>(dataOpts, dataRange, nDataPointsMax, 3);

            curDataMainMult.push_back(dataSmart);
            curDataMainMult_1.push_back(dataSmart_1);
            curDataMainMult_2.push_back(dataSmart_2);

            auto &curDataMainKt = curDataMainMult.back();
            auto &curDataMainKt_1 = curDataMainMult_1.back();
            auto &curDataMainKt_2 = curDataMainMult_2.back();

            fillData(*curDataMainKt, *hMain, curMultValues[i], curKtValues[j]);
            fillData(*curDataMainKt_1, *hMain_1, curMultValues[i], curKtValues[j]);
            fillData(*curDataMainKt_2, *hMain_2, curMultValues[i], curKtValues[j]);
        }
    }

    // prepare functions structure
    FuncStructure funcFit{};

    for (const auto &elMult : dataMain)
    {
        funcFit.emplace_back();
        auto &curFuncFitMult = funcFit.back();

        for (const auto &elKt : elMult)
        {
            auto funcSmart = std::make_shared<TF3>(*funcMain);
            curFuncFitMult.push_back(funcSmart);
        }
    }

    // sanitise data/func structure
    {
        // expected size (same for all data structures)
        const unsigned int nBinsMult = dataMain.size();
        const unsigned int nBinsKt = dataMain.front().size();

        // check if all structures are of the same size and have valid pointers
        assert(checkDataStruct(dataMain, nBinsMult, nBinsKt));
        assert(checkDataStruct(dataMain_1, nBinsMult, nBinsKt));
        assert(checkDataStruct(dataMain_2, nBinsMult, nBinsKt));
        assert(checkDataStruct(funcFit, nBinsMult, nBinsKt));

        // check if the BinData are filled properly
        const unsigned int nDataPoints = dataMain.front().front()->NPoints();
        assert(checkBinDataSet(dataMain, nDataPoints));
        assert(checkBinDataSet(dataMain_1, nDataPoints));
        assert(checkBinDataSet(dataMain_2, nDataPoints));

        std::cout << std::endl
                  << "Loaded valid data/func structures (nMult nKt nDataPoints):\t" << nBinsMult << "\t" << nBinsKt << "\t" << nDataPoints << "\t" << std::endl;
        std::cout << "Mult/kT mean values used in the fits are the following:" << std::endl
                  << getBinCombinations(dataMain) << std::endl
                  << std::endl;
    }

    // perform a global fit
    TFitResultPtr globalFitResult{};
    const auto paramsCommon = getListOfParams(fitParams, listOfParamsCommon());
    const auto paramsIndividual = getListOfParams(fitParams, listOfParamsIndividual());
    if (doFitGlobal)
    {
        // useful vars
        const unsigned int fitBinsMult = dataMain.size();
        const unsigned int fitBinsKt = dataMain.front().size();

        // const unsigned int nParamsSingle = fitParams.size();
        const unsigned int nParamsCommon = paramsCommon.size();
        const unsigned int nParamsIndividual = paramsIndividual.size();
        const unsigned int nParamsGlobal = nParamsCommon + nParamsIndividual * fitBinsMult * fitBinsKt;

        const unsigned int iParamsCommon = 0;
        const unsigned int iParamsIndividual = nParamsCommon;

        const unsigned int nDataPointsSingle = dataMain.front().front()->NPoints();
        const unsigned int nDataPointsGlobal = nDataPointsSingle * fitBinsMult * fitBinsKt;

        // prepare fitter
        ROOT::Fit::Fitter fitterMain;

        // init all params (global)
        fitterMain.Config().SetParamsSettings(nParamsGlobal, {});

        // fill common params
        configFromParamsListGlobal(paramsCommon, fitterMain, iParamsCommon);

        // configure params from each bin
        for (unsigned int i = 0; i < fitBinsMult; ++i)
        {
            for (unsigned int j = 0; j < fitBinsKt; ++j)
            {
                configFromParamsListGlobal(paramsIndividual, fitterMain, iParamsIndividual + (i * fitBinsKt + j) * nParamsIndividual, nParamsCommon);
            }
        }

        // single fit config
        std::cout << "Basic fit configuration (id name init fixed limits):" << std::endl;
        for (const auto &el : fitParams)
        {
            std::cout << "\t" << el.second << std::endl;
        }
        std::cout << std::endl;

        // minimiser configuration
        // fitterMain.Config().SetMinimizer("Minuit2", "Migrad");
        // fitterMain.Config().SetMinosErrors();
        // fitterMain.Config().MinimizerOptions().SetTolerance(0.001);       // default: 0.01
        // fitterMain.Config().MinimizerOptions().SetMaxIterations(1000000); // some fail up to ~50000
        fitterMain.Config().MinimizerOptions().SetPrintLevel(0);
        fitterMain.Config().MinimizerOptions().Print();

        // do a fit
        globalFitResult = doGlobalFit(dataMain, dataMain_1, dataMain_2, funcFit, fitterMain, nParamsCommon, nParamsIndividual, nParamsGlobal, nDataPointsGlobal, resName);
        fitterMain.Result().Print(std::cout, false); // true to show the correlation matrix etc
    }

    // loop over all bins of mult / kT
    for (int i = 0; i < nrBinsMult; ++i)
    {
        // check if this bin should be ignored
        if (i < ignoreBinMultLower || i >= ignoreBinMultUpper)
        {
            continue;
        }

        const unsigned int iFit = i - ignoreBinMultLower;
        if (!(iFit < dataMain.size()))
        {
            break;
        }

        // each mult bin is plotted on a single page, with subplot for kT bins
        tc->Clear();
        tc->Draw();
        tc->Divide(nrBinsKtForLoops);

        for (int j = 0; j < nrBinsKtForLoops; ++j)
        {
            // check if this bin should be ignored
            if (j < ignoreBinKtLower || j >= ignoreBinKtUpper)
            {
                continue;
            }

            const unsigned int jFit = j - ignoreBinKtLower;
            if (!(jFit < dataMain.front().size()))
            {
                break;
            }

            // prepare pads
            tc->cd(j + 1);

            auto tl = new TLegend(0.175, 0.225, 0.475, 0.40);

            auto p1 = new TPad("p1", "p1", 0., 0.33, 1., 1.);
            auto p2 = new TPad("p2", "p2", 0., 0., 1., 0.33);
            p1->Draw();
            p2->Draw();

            // get histograms
            TString hMainName = HBT::Utils::getHistogramName(hMainNameBase, hCommonEndName, true, !isMultBinsOnly, i, j);
            TH1D *hMain = (TH1D *)fIn->Get(hMainName);
            hMain->SetTitle(";#font[12]{Q} [GeV];#font[12]{C_{2}}(#font[12]{Q})");
            TH1D *hMain_1 = (TH1D *)fIn->Get(HBT::Utils::getHistogramName(hMainNameBase_1, hCommonEndName, true, !isMultBinsOnly, i, j));
            TH1D *hMain_2 = (TH1D *)fIn->Get(HBT::Utils::getHistogramName(hMainNameBase_2, hCommonEndName, true, !isMultBinsOnly, i, j));

            //get the reference histogram and fit result if available
            const auto hRefName = HBT::Utils::getHistogramName(hRefNameBase, hCommonEndName, true, !isMultBinsOnly, i, j);
            TH1D *hRef = flagDrawRef ? (TH1D *)fInRef->Get(hRefName) : nullptr;
            if (hRef)
            {
                hRef->SetTitle(";#font[12]{Q} [GeV];#font[12]{C_{2}}(#font[12]{Q})");
            }
            auto fitResultRef = flagUseBkgFromRef ? (TFitResult *)fInRefFitResults->Get(resName) : nullptr;
            fOut->cd();

            // draw histograms
            p1->cd();

            // draw a reference histogram if required
            if (flagDrawRef)
            {
                hRef->SetLineWidth(gStyle->GetLineWidth() / 2.0); // ! a lot of points - improve visibility
                drawHistogram(hRef, HBT::Styles::getColorSecondary(), HBT::Styles::getMarker(1));
                tl->AddEntry(hRef, refType, "pe");
            }
            hMain->SetLineWidth(gStyle->GetLineWidth() / 2.0); // ! a lot of points - improve visibility
            drawHistogram(hMain, HBT::Styles::getColorPrimary());
            const auto hMainLabel = HBT::Utils::dataTypeAsString(dataType) + " (" + HBT::Utils::histTypeAsString(histType) + ")";
            tl->AddEntry(hMain, hMainLabel.c_str(), "pe");

            // perform a fit in individual bins if required
            TFitResultPtr fitResult{};
            auto curFitParams = fitParams;
            if (doFitIndividual)
            {
                // prepare fitter
                ROOT::Fit::Fitter fitterMain;
                ROOT::Math::WrappedMultiTF1 fitFuncMain(*funcFit[iFit][jFit], funcFit[iFit][jFit]->GetNdim());
                fitterMain.SetFunction(fitFuncMain, false); // false to not pass the derivatives (to be calculated by the minimisation algorithm)

                // use the bkg parametrisation from a reference fit
                if (flagUseBkgFromRef)
                {
                    // use the fit only if it was valid
                    if (fitResultRef && fitResultRef->IsValid())
                    {
                        // use the common bkg params from the reference fit
                        for (const auto &paramName : listOfParamsCommon())
                        {
                            auto &curParam = curFitParams.at(paramName);
                            curParam.fixToValue(fitResultRef->Parameter(curParam.id()));
                        }

                        // scale the bkg amplitude if needed
                        if (flagUseBkgScaling)
                        {
                            // use the scaling obtained from trends of the free z parameter
                            auto fBkgScale = (TF1 *)fInRefBkgScaling->Get(HBT::Utils::getBkgScalingFuncName(funcBkgScalingName, isMultBinsOnly, j));

                            if (!fBkgScale)
                            {
                                std::cout << "Fit (for bkg scaling) is not valid: \n\t" << hMain << "\n\t" << funcFit[iFit][jFit] << std::endl;
                                continue;
                            }

                            curFitParams.at("scaleZ").fixToValue(fBkgScale->Eval(curMultValues[i]));
                        }
                    }
                    else
                    {
                        std::cout << "Fit with the bkg parameters is not valid: \n\t" << hMain << "\n\t" << funcFit[iFit][jFit] << std::endl;
                        continue;
                    }

                    fOut->cd();
                }

                // configure the fitter
                configFromParamsListGlobal(curFitParams, fitterMain);

                // minimiser configuration
                // fitterMain.Config().SetMinimizer("Minuit2", "Migrad");
                // fitterMain.Config().SetMinosErrors();
                // fitterMain.Config().MinimizerOptions().SetTolerance(0.001);       // default: 0.01
                // fitterMain.Config().MinimizerOptions().SetMaxIterations(1000000); // some fail up to ~50000
                // fitterMain.Config().MinimizerOptions().SetPrintLevel(0);
                // fitterMain.Config().MinimizerOptions().Print();

                // do a fit and draw if it is valid
                fitResult = doFit(dataMain[iFit][jFit].get(), dataMain_1[iFit][jFit].get(), dataMain_2[iFit][jFit].get(), funcFit[iFit][jFit].get(), fitterMain, hMain->GetName(), funcFit[iFit][jFit]->GetName());
                fitterMain.Result().Print(std::cout, false); // true to show the correlation matrix etc
            }

            // show results of the fit
            if (flagDoFit)
            {
                const auto &curFitResult = (doFitGlobal) ? globalFitResult : fitResult;

                if ((curFitResult.Get() != nullptr) && curFitResult->IsValid())
                {

                    // cast to a 1D function to show results
                    const auto &curFuncFit = *funcFit[iFit][jFit];
                    const auto &curMult = curMultValues[i];
                    const auto &curKt = curKtValues[j];

                    auto f = new TF1(
                        "f", [&](double *x, double *p)
                        { return curFuncFit.Eval(x[0], curMult, curKt); },
                        curFuncFit.GetXmin(), curFuncFit.GetXmax(), curFuncFit.GetNpar());
                    for (const auto &el : curFitParams)
                    {
                        const auto &param = el.second;
                        f->SetParName(param.id(), param.name().c_str());
                        if (param.isFixed())
                        {
                            f->FixParameter(param.id(), param.initValue());
                        }
                    }

                    // map the parameters from the global fitter if needed
                    if (doFitGlobal)
                    {
                        // useful vars
                        const int fitBinsKt = dataMain.front().size();
                        const int nParamsCommon = paramsCommon.size();
                        const int nParamsIndividual = paramsIndividual.size();
                        const int nParamsSingle = nParamsCommon + nParamsIndividual;

                        int paramIdsInGlobalFit[nParamsSingle];
                        for (int iCommon = 0; iCommon < nParamsCommon; ++iCommon)
                        {
                            paramIdsInGlobalFit[iCommon] = iCommon;
                        }
                        for (int iInd = nParamsCommon; iInd < nParamsSingle; ++iInd)
                        {
                            paramIdsInGlobalFit[iInd] = iInd + (iFit * fitBinsKt + jFit) * nParamsIndividual;
                        }

                        f->SetFitResult(*curFitResult, paramIdsInGlobalFit);
                    }
                    else
                    {
                        f->SetFitResult(*curFitResult);
                    }

                    f->SetLineColor(HBT::Styles::getColorEmphasize());
                    f->SetLineWidth(gStyle->GetLineWidth() * 3.0);

                    f->Draw("SAME");
                    const auto fLabel = std::string("fit (") + HBT::Utils::histTypeAsString(histType) + ")";
                    tl->AddEntry(f, fLabel.c_str(), "l");

                    // redraw the data points
                    if (flagDrawRef)
                    {
                        drawHistogram(hRef, HBT::Styles::getColorSecondary(), HBT::Styles::getMarker(1));
                    }
                    drawHistogram(hMain, HBT::Styles::getColorPrimary());

                    // draw/print the fit info
                    printFitInfo(curFitResult);

                    p2->cd();
                    drawPull(hMain, f, fitRangeMin);
                    p1->cd();
                }
                else
                {
                    const auto curStyle = gStyle->GetOptFit();
                    gStyle->SetOptFit(0);
                    hMain->UseCurrentStyle();
                    gStyle->SetOptFit(curStyle);
                    std::cout << "Fit is not valid: \n\t" << hMain << "\n\t" << funcFit[iFit][jFit] << std::endl;
                }
            }

            // add LHCb label
            auto lhcbLabel = HBT::Styles::makeLhcbLabel();
            HBT::Utils::addMultilineText("LHCb preliminary;#font[12]{#sqrt{s_{#font[122]{NN}}}} = 5.02 TeV", lhcbLabel);
            lhcbLabel->Draw();
            tc->Update();

            // add legend
            tl->Draw("SAME");

            // add description
            printDescription(isMultBinsOnly, i, j);
        }

        // plot each mult bin on a different page
        tc->Update();
        tc->Draw();
        tc->SaveAs(plotFile);
    }

    tc->SaveAs(plotFile + "]");

    // I/O close & cleanup
    fIn->Close();

    fOut->Close();

    if (fInRef != nullptr)
    {
        fInRef->Close();
    }
    if (fInRefFitResults != nullptr)
    {
        fInRefFitResults->Close();
    }
    if (fInRefBkgScaling != nullptr)
    {
        fInRefBkgScaling->Close();
    }
}

void fitInBins(const TString inputFile, const TString hMainNameBase, const TString hMainNameBase_1, const TString hMainNameBase_2, const bool flagIsMc, const bool flagIsUnlike, const TString dataType, TString hCommonEndNameForMult, TString hCommonEndNameForKt, const int flagDoFit, const TString inputFileRef, const TString hRefNameBase, const TString refType, const bool flagDrawRef, const bool flagUseBkgFromRef, const bool flagUseBkgScaling, const int &ignoreBinMultLower, const int &ignoreBinMultUpper, const int &ignoreBinMultForKtLower, const int &ignoreBinMultForKtUpper, const int &ignoreBinKtLower, const int &ignoreBinKtUpper, const unsigned int &flagStyle = HBT::Styles::defaultStyleFlag)
{
    // get configuration
    auto selection = SelectionClass();

    auto nrBinsMult = selection.getNrOfBinsMult();
    auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    auto nrBinsKt = selection.getNrOfBinsKt();

    // call for mult bins only
    fitInBinsGeneric(inputFile, hMainNameBase, hMainNameBase_1, hMainNameBase_2, flagIsMc, flagIsUnlike, dataType, nrBinsMult, 0, hCommonEndNameForMult, flagDoFit, inputFileRef, hRefNameBase, refType, flagDrawRef, flagUseBkgFromRef, flagUseBkgScaling, ignoreBinMultLower, ignoreBinMultUpper, 0, 1, flagStyle);
    // call for mult + kT bins
    // fitInBinsGeneric(inputFile, hMainNameBase, hMainNameBase_1, hMainNameBase_2, flagIsMc, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt, flagDoFit, inputFileRef, hRefNameBase, refType, flagDrawRef, flagUseBkgFromRef, flagUseBkgScaling, ignoreBinMultForKtLower, ignoreBinMultForKtUpper, ignoreBinKtLower, ignoreBinKtUpper, flagStyle);
}
