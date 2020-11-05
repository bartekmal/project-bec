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
}

// provide custom style (based on the general one)
void setStyleLocal()
{
    HBT::Utils::setStyle();
    gStyle->SetMarkerSize(1.4);
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

void drawTrendsGeneric(const TString inputFile, const TString hNameBase, const TString fName, const bool flagIsUnlike, const TString dataType, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName)
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
    const TString title = fName + "_" + binsType + "_" + histType;
    const TString plotFile = title + ".pdf";
    TCanvas *tc = new TCanvas(title, title, padSize, padSize);
    tc->SaveAs(plotFile + "[");

    // input file with the fit results
    TFile *fIn = new TFile(inputFile, "READ");

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
                // get the fit result in the given bin
                TString hName = HBT::Utils::getHistogramName(hNameBase, hCommonEndName, true, !isMultBinsOnly, isMultBinsOnly ? j : i, isMultBinsOnly ? i : j);
                const auto fitResult = (TFitResult *)fIn->Get(HBT::Utils::getFitResultName(hName, fName));

                if (!processSingleResult(fitResult, *el, tGraphs[i], j, curBinCentres[j], curBinErrors[j]))
                {
                    std::cout << "The given fit is not valid: \n\t" << fIn->GetName() << "\n\t" << hName << "\n\t" << fName << std::endl;
                    continue;
                }
            }
        }

        // prepare canvas for the current parameter
        tc->SetGrid();
        tc->DrawFrame(0., param.rangeTrendsMin(), maxValueForBins, param.rangeTrendsMax());

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

    fIn->Close();
    delete fIn;
}

void drawTrends(const TString inputFile, const TString hNameBase, const TString fName, const bool flagIsUnlike = false, const TString dataType = "", TString hCommonEndNameForMult = "", TString hCommonEndNameForKt = "")
{
    // get configuration
    const auto selection = SelectionClass();

    const auto nrBinsMult = selection.getNrOfBinsMult();
    const auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    const auto nrBinsKt = selection.getNrOfBinsKt();

    // call for mult bins only
    drawTrendsGeneric(inputFile, hNameBase, fName, flagIsUnlike, dataType, nrBinsMult, 0, hCommonEndNameForMult);
    // call for mult + kT bins
    drawTrendsGeneric(inputFile, hNameBase, fName, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt);
}
