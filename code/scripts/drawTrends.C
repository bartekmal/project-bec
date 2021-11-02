#include "../HBTAnalysis/Utils.hpp"
#include "../utils/Styles.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"

#include "fitModel.C"

#include "TF1.h"
#include "TStyle.h"
#include "TGraphErrors.h"

#include <memory>
#include <string>

/*-------------- configuration -------------------*/
Double_t maxForMult = 180.;
Double_t maxForKt = 1.0;
const float bkgScalingBandWidth = 0.15f;

/*-------------- enf of configuration -------------*/

// provide custom style (based on the general ones)
void setStyleLocal(const unsigned int &flagStyle)
{
    HBT::Styles::setStyle(flagStyle);

    // local style settings
    HBT::Styles::setColorPalette();
    gROOT->ForceStyle();
}

// add base function with an offset to the given graph
void addFunction(TGraphErrors &graph, const TF1 &funcBase, const float &offset, const unsigned int &offsetParamId)
{
    const auto func = new TF1(funcBase);
    func->FixParameter(offsetParamId, offset);
    func->SetLineStyle(9);
    func->SetLineColor(HBT::Styles::getColorEmphasize());
    graph.GetListOfFunctions()->Add(func);
}

// add an entry to the given graph, corresponding to a parameter value obtained from a fit in the given bin (checks if the fit result is valid)
bool processSingleResult(const TFitResult *fitResult, const std::pair<std::string, FitParam> &paramEntry, TGraphErrors &graph, const unsigned int &binNr, const HBT::Units::FloatType &binCentre, const HBT::Units::FloatType &binError)
{
    // validate fit result
    if (!(fitResult && fitResult->IsValid()))
        return false;

    const auto &key = paramEntry.first;
    const auto &param = paramEntry.second;

    if (!key.compare("MinFcn"))
    {
        graph.SetPoint(binNr, binCentre, fitResult->MinFcnValue() / fitResult->Ndf());
        graph.SetPointError(binNr, binError, 0.);
    }
    else if (!key.compare("amplBkgScaled"))
    {
        const auto fitParams = prepareFitParamsForTrends(); // not ideal
        const auto parIdAmplitudeBkg = fitParams.at("amplBkg").id();
        const auto parIdAmplitudeBkgScaleFactor = fitParams.at("scaleZ").id();

        graph.SetPoint(binNr, binCentre, fitResult->Parameter(parIdAmplitudeBkg) * fitResult->Parameter(parIdAmplitudeBkgScaleFactor));
        // scale the error of the A_bkg by the scaleFactor used
        graph.SetPointError(binNr, binError, fitResult->ParError(parIdAmplitudeBkg) * fitResult->Parameter(parIdAmplitudeBkgScaleFactor));
    }
    else
    {
        graph.SetPoint(binNr, binCentre, fitResult->Parameter(param.id()));
        graph.SetPointError(binNr, binError, fitResult->ParError(param.id()));
    }

    return true;
}

void drawTrendsGeneric(const TString inputFile, const TString hNameBase, const TString fName, const bool doTrendMult, const bool flagIsUnlike, const TString dataType, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName, const unsigned int &flagStyle)
{
    // get configuration
    const bool isMultBinsOnly = (nrBinsKt == 0) ? true : false;

    const auto nrOfBinsInGraph = doTrendMult ? nrBinsMult : nrBinsKt;
    const auto nrOfGraphsToOverlap = doTrendMult ? (isMultBinsOnly ? 1 : nrBinsKt) : nrBinsMult;

    const auto selection = SelectionClass();
    const auto curBinCentres = doTrendMult ? (isMultBinsOnly ? selection.getBinsOfMultiplicityCentres() : selection.getBinsOfMultiplicityForKtCentres()) : selection.getBinsOfKtCentres();
    // const auto curBinErrors = doTrendMult ? (isMultBinsOnly ? selection.getBinsOfMultiplicityErrors() : selection.getBinsOfMultiplicityForKtErrors()) : selection.getBinsOfKtErrors(); // ! pass to processSingleResult() to show the bin widths

    const auto maxValueForBins = doTrendMult ? maxForMult : maxForKt;
    const auto binsType = isMultBinsOnly ? TString("mult") : TString("kT");
    const auto trendType = doTrendMult ? TString("mult") : TString("kT");
    const auto trendTypeAsString = doTrendMult ? TString("#font[12]{N}_{VELO}") : TString("#font[12]{k}_{T} [GeV]");
    const auto overlappingGraphLabel = doTrendMult ? TString("#font[12]{k}_{T} [GeV]") : TString("#font[12]{N}_{VELO}");
    const auto binStringsForLabels = doTrendMult ? (isMultBinsOnly ? std::vector<std::string>(1, "full range") : selection.getBinsOfKtRangesAsStrings()) : selection.getBinsOfMultiplicityForKtRangesAsStrings();

    const auto histType = flagIsUnlike ? TString("UNLIKE") : TString("LIKE");
    const TString funcScaleZName = TString("funcScaleZ");

    // set ROOT style
    setStyleLocal(flagStyle);

    // prepare the PDF file
    const TString title = fName + "_" + binsType + "_VS" + trendType + "_" + histType;
    const TString plotFile = title + ".pdf";
    auto tc = std::make_unique<TCanvas>(title, title, HBT::Styles::defaultCanvasSizeX, HBT::Styles::defaultCanvasSizeY);
    tc->SaveAs(plotFile + "[");

    // prepare I/O files
    auto fIn = std::make_unique<TFile>(inputFile, "READ");
    auto fOut = std::make_unique<TFile>("trends.root", "UPDATE");

    // process all fit parameters
    const auto fitParams = prepareFitParamsForTrends();
    const auto fitParamKeys = prepareKeyListForTrends();

    for (const auto &curKey : fitParamKeys)
    {
        // check if the key exists (and get iterator)
        const auto el = fitParams.find(curKey);
        if (el == fitParams.end())
        {
            continue;
        }

        const auto &param = fitParams.at(curKey);

        // do not draw the fixed params
        if (param.isFixed())
        {
            continue;
        }

        // prepare graphs for the given param
        std::vector<TGraphErrors> tGraphs = std::vector<TGraphErrors>(nrOfGraphsToOverlap, TGraphErrors());
        TLegend *tl = new TLegend(0.80, 0.75, 0.95, 0.95);
        tl->SetHeader(overlappingGraphLabel, "C");
        for (int i = 0; i < nrOfGraphsToOverlap; ++i)
        {
            tl->AddEntry(&tGraphs[i], binStringsForLabels[i].c_str(), "pe");

            // loop over #entries (points) in the graph
            for (int j = 0; j < nrOfBinsInGraph; ++j)
            {
                // get the fit result in the given bin
                TString hName = HBT::Utils::getHistogramName(hNameBase, hCommonEndName, true, !isMultBinsOnly, doTrendMult ? j : i, doTrendMult ? i : j);
                const auto fitResult = (TFitResult *)fIn->Get(HBT::Utils::getFitResultName(hName, fName));

                if (!processSingleResult(fitResult, *el, tGraphs[i], j, curBinCentres[j], 0.0))
                {
                    std::cout << "The given fit is not valid: \n\t" << fIn->GetName() << "\n\t" << hName << "\n\t" << fName << std::endl;
                    continue;
                }
            }

            // make fit for bkg scaling
            if ((!curKey.compare("scaleZ") && doTrendMult && !flagIsUnlike))
            {
                auto &curGraph = tGraphs[i];

                // configure fit
                const auto fTrend = std::make_unique<TF1>(funcScaleZName, "([0] * x + [1]) / (1 + [0] * x + [1]) + [2]", 1., maxForMult); // make sure not to take points at 0,0 (they should be not there in principle)
                fTrend->SetParNames("a", "b", "offset");
                fTrend->SetParameters(0.5, 0.5, 0.0);
                fTrend->SetParLimits(0, 0, 10);
                fTrend->SetParLimits(1, 0, 10);
                fTrend->FixParameter(2, 0.0);
                fTrend->SetLineColor(HBT::Styles::getColorEmphasize());

                // do fit and save if valid
                const TFitResultPtr fitResult = curGraph.Fit(fTrend.get(), "SREX0"); // ! do not use errors in X for fits
                if ((fitResult.Get() != nullptr) && fitResult->IsValid())
                {
                    fTrend->Write(HBT::Utils::getBkgScalingFuncName(funcScaleZName, isMultBinsOnly, i));

                    // plot results only for the mult bins only
                    if (isMultBinsOnly)
                    {
                        // show fit results
                        // const auto curStyle = gStyle->GetOptFit();
                        // gStyle->SetOptFit(111);
                        // curGraph.UseCurrentStyle();
                        // gStyle->SetOptFit(curStyle);

                        // plot a band around the main trend (for systematics)
                        addFunction(curGraph, *(fTrend.get()), -1.0 * bkgScalingBandWidth, 2);
                        addFunction(curGraph, *(fTrend.get()), 1.0 * bkgScalingBandWidth, 2);
                    }
                }
                else
                {
                    curGraph.GetListOfFunctions()->Remove(fTrend.get());
                    std::cout << "Fit (scaleZ) is not valid: \n\t" << &curGraph << "\n\t" << fTrend.get() << std::endl;
                }
            }
        }

        // prepare canvas for the current parameter
        tc->SetGrid();
        auto frame = tc->DrawFrame(0., param.rangeTrendsMin(), maxValueForBins, param.rangeTrendsMax());
        frame->SetTitle(std::string(";" + trendTypeAsString + ";" + param.name().c_str()).c_str());

        // draw the graphs
        for (auto &graph : tGraphs)
        {
            graph.Draw("P PMC PLC");
        }

        // add LHCb label
        auto lhcbLabel = HBT::Styles::makeLhcbLabel(0.05,0.35,0.80,0.95);
        HBT::Utils::addMultilineText("LHCb Unofficial;#font[12]{#sqrt{s_{#font[122]{NN}}}} = 5.02 TeV", lhcbLabel);
        const auto dataLabel = HBT::Utils::dataTypeAsString(dataType) + " fit (" + HBT::Utils::histTypeAsString(histType) + ")";
        HBT::Utils::addMultilineText((";"+dataLabel).c_str(), lhcbLabel); // ? rm this line
        lhcbLabel->Draw();
        tc->Update();

        // add legend
        if (!doTrendMult){
            tl->Draw("SAME");
        }

        // save the current plot
        tc->SaveAs(plotFile);
    }

    // close the PDF file
    tc->SaveAs(plotFile + "]");

    fIn->Close();
    fOut->Close();
}

void drawTrends(const TString inputFile, const TString hNameBase, const TString fName, const bool flagIsUnlike = false, const TString dataType = "", TString hCommonEndNameForMult = "", TString hCommonEndNameForKt = "", const unsigned int &flagStyle = HBT::Styles::defaultStyleFlag)
{
    // get configuration
    const auto selection = SelectionClass();

    const auto nrBinsMult = selection.getNrOfBinsMult();
    const auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    const auto nrBinsKt = selection.getNrOfBinsKt();

    // call trend in mult (mult bins only)
    drawTrendsGeneric(inputFile, hNameBase, fName, true, flagIsUnlike, dataType, nrBinsMult, 0, hCommonEndNameForMult, flagStyle);
    // call trend in mult (mult + kT bins)
    // drawTrendsGeneric(inputFile, hNameBase, fName, true, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt, flagStyle);
    // call trend in kT (mult + kT bins)
    // drawTrendsGeneric(inputFile, hNameBase, fName, false, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt, flagStyle);
}
