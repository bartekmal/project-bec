#include "../HBTAnalysis/Utils.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"

#include "fitModel.C"

/*-------------- configuration -------------------*/
Double_t maxForMult = 200.;
Double_t maxForKt = 1.0;
const int padSize = 1200;

/*-------------- enf of configuration -------------*/

void printDescription(const TString &dataType, const TString &paramName, const TString &binsType)
{
    Double_t commPosX = 0.20;
    Double_t commPosY = 0.80;
    TLatex comments;
    comments.SetNDC();
    comments.SetTextSize(0.035);
    comments.DrawLatex(commPosX, commPosY, dataType);
    comments.DrawLatex(commPosX, commPosY - 0.05, paramName + " vs " + binsType);
    comments.DrawLatex(commPosX, commPosY - 0.10, "relative diff wrt reference [%]");
}

// provide custom style (based on the general one)
void setStyleLocal()
{
    HBT::Utils::setStyle();
    gStyle->SetMarkerSize(1.4);
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

void drawDiffsGeneric(const TString inputFileMain, const TString hNameBaseMain, const TString fNameMain, const TString inputFileRef, const TString hNameBaseRef, const TString fNameRef, const bool flagIsUnlike, const TString dataType, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName)
{

    // get configuration
    const bool isMultBinsOnly = (nrBinsKt == 0) ? true : false;

    const auto nrOfBinsInGraph = isMultBinsOnly ? nrBinsMult : nrBinsKt;
    const auto nrOfGraphsToOverlap = isMultBinsOnly ? 1 : nrBinsMult;

    const auto selection = SelectionClass();
    const auto curBinCentres = isMultBinsOnly ? selection.getBinsOfMultiplicityCentres() : selection.getBinsOfKtCentres();
    const auto curBinErrors = isMultBinsOnly ? selection.getBinsOfMultiplicityErrors() : selection.getBinsOfKtErrors();

    const auto maxValueForBins = isMultBinsOnly ? maxForMult : maxForKt;
    const auto binsType = isMultBinsOnly ? TString("mult") : TString("kT");
    const auto overlappingGraphLabel = isMultBinsOnly ? TString("kT") : TString("mult");

    const auto histType = flagIsUnlike ? TString("UNLIKE") : TString("LIKE");

    // set ROOT style
    setStyleLocal();

    // prepare the PDF file
    const TString title = fNameMain + "_" + binsType + "_" + histType;
    const TString plotFile = title + ".pdf";
    TCanvas *tc = new TCanvas(title, title, padSize, padSize);
    tc->SaveAs(plotFile + "[");

    // input file with the fit results
    TFile *fInMain = new TFile(inputFileMain, "READ");
    TFile *fInRef = new TFile(inputFileRef, "READ");

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
            if (!isMultBinsOnly)
                tl->AddEntry(&tGraphs[i], selection.getBinsOfMultiplicityForKtRangesAsStrings()[i].c_str(), "pe");
            else
                tl->AddEntry(&tGraphs[i], "full range", "pe");

            // loop over #entries (points) in the graph
            for (int j = 0; j < nrOfBinsInGraph; ++j)
            {
                // get the fit results in the given bin
                TString hNameMain = HBT::Utils::getHistogramName(hNameBaseMain, hCommonEndName, true, !isMultBinsOnly, isMultBinsOnly ? j : i, isMultBinsOnly ? i : j);
                TString hNameRef = HBT::Utils::getHistogramName(hNameBaseRef, hCommonEndName, true, !isMultBinsOnly, isMultBinsOnly ? j : i, isMultBinsOnly ? i : j);
                const auto fitResultMain = (TFitResult *)fInMain->Get(HBT::Utils::getFitResultName(hNameMain, fNameMain));
                const auto fitResultRef = (TFitResult *)fInRef->Get(HBT::Utils::getFitResultName(hNameRef, fNameRef));

                if (!processSingleDiff(fitResultMain, fitResultRef, *el, tGraphs[i], j, curBinCentres[j], curBinErrors[j]))
                {
                    std::cout << "One of the fits is not valid: \n\t" << fInMain->GetName() << "\n\t" << hNameMain << "\n\t" << fNameMain << "\n\t" << fInRef->GetName() << "\n\t" << hNameRef << "\n\t" << fNameRef << std::endl;
                    continue;
                }
            }
        }

        // prepare canvas for the current parameter
        tc->SetGrid();
        tc->DrawFrame(0., param.rangeDiffsMin(), maxValueForBins, param.rangeDiffsMax());

        // draw the graphs
        for (auto &graph : tGraphs)
        {
            graph.Draw("P PMC PLC");
        }

        // add description
        tl->Draw("SAME");
        printDescription(dataType, param.name(), binsType);

        // save the current plot
        tc->SaveAs(plotFile);
    }

    // close the PDF file
    tc->SaveAs(plotFile + "]");
    delete tc;

    fInMain->Close();
    delete fInMain;
    fInRef->Close();
    delete fInRef;
}

void drawDiffs(const TString inputFileMain, const TString hNameBaseMain, const TString fNameMain, const TString inputFileRef, const TString hNameBaseRef, const TString fNameRef, const bool flagIsUnlike = false, const TString dataType = "", TString hCommonEndNameForMult = "", TString hCommonEndNameForKt = "")
{
    // get configuration
    const auto selection = SelectionClass();

    const auto nrBinsMult = selection.getNrOfBinsMult();
    const auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    const auto nrBinsKt = selection.getNrOfBinsKt();

    // call for mult bins only
    drawDiffsGeneric(inputFileMain, hNameBaseMain, fNameMain, inputFileRef, hNameBaseRef, fNameRef, flagIsUnlike, dataType, nrBinsMult, 0, hCommonEndNameForMult);
    // call for mult + kT bins
    drawDiffsGeneric(inputFileMain, hNameBaseMain, fNameMain, inputFileRef, hNameBaseRef, fNameRef, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt);
}
