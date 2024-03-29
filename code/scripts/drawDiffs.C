#include "../HBTAnalysis/Utils.hpp"
#include "../utils/Styles.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"

#include "fitModel.C"

#include "TStyle.h"
#include "TGraphErrors.h"

#include <memory>
#include <string>

/*-------------- configuration -------------------*/
Double_t maxForMult = 180.;
Double_t maxForKt = 1.0;
/*-------------- enf of configuration -------------*/

// provide custom style (based on the general ones)
void setStyleLocal(const unsigned int &flagStyle)
{
    HBT::Styles::setStyle(flagStyle);

    // local style settings
    HBT::Styles::setColorPalette();
    gROOT->ForceStyle();
}

// add an entry to the given graph, corresponding to a parameter values obtained from fits in the given bin (checks if the fit results are valid)
bool processSingleDiff(const TFitResult *fitResultMain, const TFitResult *fitResultRef, const std::pair<std::string, FitParam> &paramEntry, TGraphErrors &graph, const unsigned int &binNr, const HBT::Units::FloatType &binCentre, const HBT::Units::FloatType &binError)
{
    // validate fit results
    if (!(fitResultMain && fitResultMain->IsValid()) || !(fitResultRef && fitResultRef->IsValid()))
        return false;

    const auto &key = paramEntry.first;
    const auto &param = paramEntry.second;

    if (!key.compare("MinFcn"))
    {
        // get individual results
        const float minFncNdfMain = fitResultMain->MinFcnValue() / fitResultMain->Ndf();
        const float minFncNdfRef = fitResultRef->MinFcnValue() / fitResultRef->Ndf();

        // plot a ratio of the minFnc/ndf values (just to see how it changes)
        graph.SetPoint(binNr, binCentre, (float)(minFncNdfMain - minFncNdfRef) / minFncNdfRef * 100.0);
        graph.SetPointError(binNr, binError, 0.);
    }
    else if (!key.compare("amplBkgScaled"))
    {
        const auto fitParams = prepareFitParamsForTrends(); // not ideal
        const auto parIdAmplitudeBkg = fitParams.at("amplBkg").id();
        const auto parIdAmplitudeBkgScaleFactor = fitParams.at("scaleZ").id();

        // get individual results
        const float effAmplitudeMain = fitResultMain->Parameter(parIdAmplitudeBkg) * fitResultMain->Parameter(parIdAmplitudeBkgScaleFactor);
        const float effAmplitudeRef = fitResultRef->Parameter(parIdAmplitudeBkg) * fitResultRef->Parameter(parIdAmplitudeBkgScaleFactor);
        const float effAmplitudeMainError = fitResultMain->ParError(parIdAmplitudeBkg) * fitResultMain->Parameter(parIdAmplitudeBkgScaleFactor);
        const float effAmplitudeRefError = fitResultRef->ParError(parIdAmplitudeBkg) * fitResultRef->Parameter(parIdAmplitudeBkgScaleFactor);

        // get a difference wrt to the reference value [in %]
        const float diffValue = (float)(effAmplitudeMain - effAmplitudeRef) / effAmplitudeRef * 100.0;
        const float effAmplitudeRefErrorWeight = (float)effAmplitudeMain / effAmplitudeRef;
        const float diffError = (1. / effAmplitudeRef) * TMath::Sqrt(pow(effAmplitudeMainError, 2) + pow(effAmplitudeRefErrorWeight * effAmplitudeRefError, 2)) * 100.0;

        graph.SetPoint(binNr, binCentre, diffValue);
        // scale the error of the A_bkg by the scaleFactor used
        graph.SetPointError(binNr, binError, diffError);
    }
    else
    {
        // get individual results
        const float parMainError = fitResultMain->ParError(param.id());
        const float parRefError = fitResultRef->ParError(param.id());

        // get a difference wrt to the reference value [in %]
        const float diffValue = (float)(fitResultMain->Parameter(param.id()) - fitResultRef->Parameter(param.id())) / fitResultRef->Parameter(param.id()) * 100.0;
        const float parRefErrorWeight = (float)fitResultMain->Parameter(param.id()) / fitResultRef->Parameter(param.id());
        const float diffError = (1. / fitResultRef->Parameter(param.id())) * TMath::Sqrt(pow(parMainError, 2) + pow(parRefErrorWeight * parRefError, 2)) * 100.0;

        graph.SetPoint(binNr, binCentre, diffValue);
        graph.SetPointError(binNr, binError, diffError);
    }

    return true;
}

void drawDiffsGeneric(const TString inputFileMain, const TString hNameBaseMain, const TString fNameMain, const TString inputFileRef, const TString hNameBaseRef, const TString fNameRef, const bool doTrendMult, const bool flagIsUnlike, const TString dataType, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName, const unsigned int &flagStyle)
{

    // get configuration
    const bool isMultBinsOnly = (nrBinsKt == 0) ? true : false;

    const auto nrOfBinsInGraph = doTrendMult ? nrBinsMult : nrBinsKt;
    const auto nrOfGraphsToOverlap = doTrendMult ? (isMultBinsOnly ? 1 : nrBinsKt) : nrBinsMult;

    const auto selection = SelectionClass();
    const auto curBinCentres = doTrendMult ? (isMultBinsOnly ? selection.getBinsOfMultiplicityCentres() : selection.getBinsOfMultiplicityForKtCentres()) : selection.getBinsOfKtCentres();
    // const auto curBinErrors = doTrendMult ? (isMultBinsOnly ? selection.getBinsOfMultiplicityErrors() : selection.getBinsOfMultiplicityForKtErrors()) : selection.getBinsOfKtErrors();   // ! pass to processSingleDiff() to show the bin widths

    const auto maxValueForBins = doTrendMult ? maxForMult : maxForKt;
    const auto binsType = isMultBinsOnly ? TString("mult") : TString("kT");
    const auto trendType = doTrendMult ? TString("mult") : TString("kT");
    const auto trendTypeAsString = doTrendMult ? TString("#font[12]{N}_{VELO}") : TString("#font[12]{k}_{T} [GeV]");
    const auto overlappingGraphLabel = doTrendMult ? TString("#font[12]{k}_{T} [GeV]") : TString("#font[12]{N}_{VELO}");
    const auto binStringsForLabels = doTrendMult ? (isMultBinsOnly ? std::vector<std::string>(1, "full range") : selection.getBinsOfKtRangesAsStrings()) : selection.getBinsOfMultiplicityForKtRangesAsStrings();

    const auto histType = flagIsUnlike ? TString("UNLIKE") : TString("LIKE");

    // set ROOT style
    setStyleLocal(flagStyle);

    // prepare the PDF file
    const TString title = fNameMain + "_" + binsType + "_VS" + trendType + "_" + histType;
    const TString plotFile = title + ".pdf";
    auto tc = std::make_unique<TCanvas>(title, title, HBT::Styles::defaultCanvasSizeX, HBT::Styles::defaultCanvasSizeY);
    tc->SaveAs(plotFile + "[");

    // input file with the fit results
    auto fInMain = std::make_unique<TFile>(inputFileMain, "READ");
    auto fInRef = std::make_unique<TFile>(inputFileRef, "READ");

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
                // get the fit results in the given bin
                TString hNameMain = HBT::Utils::getHistogramName(hNameBaseMain, hCommonEndName, true, !isMultBinsOnly, doTrendMult ? j : i, doTrendMult ? i : j);
                TString hNameRef = HBT::Utils::getHistogramName(hNameBaseRef, hCommonEndName, true, !isMultBinsOnly, doTrendMult ? j : i, doTrendMult ? i : j);
                const auto fitResultMain = (TFitResult *)fInMain->Get(HBT::Utils::getFitResultName(hNameMain, fNameMain));
                const auto fitResultRef = (TFitResult *)fInRef->Get(HBT::Utils::getFitResultName(hNameRef, fNameRef));

                if (!processSingleDiff(fitResultMain, fitResultRef, *el, tGraphs[i], j, curBinCentres[j], 0.0))
                {
                    std::cout << "One of the fits is not valid: \n\t" << fInMain->GetName() << "\n\t" << hNameMain << "\n\t" << fNameMain << "\n\t" << fInRef->GetName() << "\n\t" << hNameRef << "\n\t" << fNameRef << std::endl;
                    continue;
                }
            }
        }

        // prepare canvas for the current parameter
        tc->SetGrid();
        auto frame = tc->DrawFrame(0., param.rangeDiffsMin(), maxValueForBins, param.rangeDiffsMax());
        frame->SetTitle(std::string(";" + trendTypeAsString + ";" + param.name().c_str()).c_str());

        // draw the graphs
        for (auto &graph : tGraphs)
        {
            graph.Draw("P PMC PLC");
        }

        // add LHCb label
        auto lhcbLabel = HBT::Styles::makeLhcbLabel(0.05,0.35,0.75,0.95);
        HBT::Utils::addMultilineText("LHCb preliminary;#font[12]{#sqrt{s_{#font[122]{NN}}}} = 5.02 TeV", lhcbLabel);
        const auto dataLabel = HBT::Utils::dataTypeAsString(dataType) + " fit (" + HBT::Utils::histTypeAsString(histType) + ");diff wrt reference [%]";
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

    fInMain->Close();
    fInRef->Close();
}

void drawDiffs(const TString inputFileMain, const TString hNameBaseMain, const TString fNameMain, const TString inputFileRef, const TString hNameBaseRef, const TString fNameRef, const bool flagIsUnlike = false, const TString dataType = "", TString hCommonEndNameForMult = "", TString hCommonEndNameForKt = "", const unsigned int &flagStyle = HBT::Styles::defaultStyleFlag)
{
    // get configuration
    const auto selection = SelectionClass();

    const auto nrBinsMult = selection.getNrOfBinsMult();
    const auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    const auto nrBinsKt = selection.getNrOfBinsKt();

    // call trend in mult (mult bins only)
    drawDiffsGeneric(inputFileMain, hNameBaseMain, fNameMain, inputFileRef, hNameBaseRef, fNameRef, true, flagIsUnlike, dataType, nrBinsMult, 0, hCommonEndNameForMult, flagStyle);
    // call trend in mult (mult + kT bins)
    // drawDiffsGeneric(inputFileMain, hNameBaseMain, fNameMain, inputFileRef, hNameBaseRef, fNameRef, true, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt, flagStyle);
    // call trend in kT (mult + kT bins)
    // drawDiffsGeneric(inputFileMain, hNameBaseMain, fNameMain, inputFileRef, hNameBaseRef, fNameRef, false, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt, flagStyle);
}
