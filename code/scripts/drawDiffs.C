#include "../HBTAnalysis/Utils.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"

/*-------------- configuration -------------------*/
Double_t maxForMult = 200.;
Double_t maxForKt = 1.0;
const int padSize = 1200;

struct FitParam
{
    int id;
    std::string name;
    float minValue;
    float maxValue;

    FitParam() : id(0), name(""), minValue(0.), maxValue(0.){};
    FitParam(int iId, std::string iName, float iMinValue, float iMaxValue) : id(iId), name(iName), minValue(iMinValue), maxValue(iMaxValue){};
};

std::vector<FitParam> initFitParams()
{
    std::vector<FitParam> fitParams{
        FitParam(0, "N", -60., 60.),
        FitParam(1, "lambda", -40., 40.),
        FitParam(2, "R", -40., 40.),
        // FitParam(3, "R_eff [fm]", -100., 100.),
        FitParam(4, "delta", -100., 100.),
        FitParam(5, "lambda_bkg", -40., 40.),
        FitParam(6, "R_bkg", -40., 40.),
        // FitParam(7, "alpha_bkg", -100., 100.),
        FitParam(8, "scaleFactor(lambda_bkg)", -40., 40.),
        FitParam(9, "effective(lambda_bkg)", -40., 40.),
        FitParam(10, "chi2", -60., 60.)};
    return fitParams;
};

const int parIdLambdaBkg = 5;
const int parIdLambdaBkgScaleFactor = 8;

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
bool processSingleDiff(const TFitResult *fitResultMain, const TFitResult *fitResultRef, const FitParam &param, TGraphErrors &graph, const unsigned int &binNr, const HBT::Units::FloatType &binCentre, const HBT::Units::FloatType &binError)
{
    // validate fit results
    if (!(fitResultMain && fitResultMain->IsValid()) || !(fitResultRef && fitResultRef->IsValid()))
        return false;

    if (!param.name.compare("chi2"))
    {
        // get individual results
        const float chi2ndfMain = fitResultMain->Chi2() / fitResultMain->Ndf();
        const float chi2ndfRef = fitResultRef->Chi2() / fitResultRef->Ndf();

        // plot a ratio of the chi2/ndf values (just to see how it changes)
        graph.SetPoint(binNr, binCentre, (float)(chi2ndfMain - chi2ndfRef) / chi2ndfRef * 100.0);
        graph.SetPointError(binNr, binError, 0.);
    }
    else if (!param.name.compare("effective(lambda_bkg)"))
    {
        // get individual results
        const float effLambdaMain = fitResultMain->Parameter(parIdLambdaBkg) * fitResultMain->Parameter(parIdLambdaBkgScaleFactor);
        const float effLambdaRef = fitResultRef->Parameter(parIdLambdaBkg) * fitResultRef->Parameter(parIdLambdaBkgScaleFactor);
        const float effLambdaMainError = fitResultMain->ParError(parIdLambdaBkg) * fitResultMain->Parameter(parIdLambdaBkgScaleFactor);
        const float effLambdaRefError = fitResultRef->ParError(parIdLambdaBkg) * fitResultRef->Parameter(parIdLambdaBkgScaleFactor);

        // get a difference wrt to the reference value [in %]
        const float diffValue = (float)(effLambdaMain - effLambdaRef) / effLambdaRef * 100.0;
        const float effLambdaRefErrorWeight = (float)effLambdaMain / effLambdaRef;
        const float diffError = (1. / effLambdaRef) * TMath::Sqrt(pow(effLambdaMainError, 2) + pow(effLambdaRefErrorWeight * effLambdaRefError, 2)) * 100.0;

        graph.SetPoint(binNr, binCentre, diffValue);
        // scale the error of the lambda_bkg by the scaleFactor used
        graph.SetPointError(binNr, binError, diffError);
    }
    else
    {
        // get individual results
        const float parMainError = fitResultMain->ParError(param.id);
        const float parRefError = fitResultRef->ParError(param.id);

        // get a difference wrt to the reference value [in %]
        const float diffValue = (float)(fitResultMain->Parameter(param.id) - fitResultRef->Parameter(param.id)) / fitResultRef->Parameter(param.id) * 100.0;
        const float parRefErrorWeight = (float)fitResultMain->Parameter(param.id) / fitResultRef->Parameter(param.id);
        const float diffError = (1. / fitResultRef->Parameter(param.id)) * TMath::Sqrt(pow(parMainError, 2) + pow(parRefErrorWeight * parRefError, 2)) * 100.0;

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

    // initialise a list of the fit params
    const auto fitParams = initFitParams();

    for (const auto &param : fitParams)
    {

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

                if (!processSingleDiff(fitResultMain, fitResultRef, param, tGraphs[i], j, curBinCentres[j], curBinErrors[j]))
                {
                    std::cout << "One of the fits is not valid: \n\t" << fInMain->GetName() << "\n\t" << hNameMain << "\n\t" << fNameMain << "\n\t" << fInRef->GetName() << "\n\t" << hNameRef << "\n\t" << fNameRef << std::endl;
                    continue;
                }
            }
        }

        // prepare canvas for the current parameter
        tc->SetGrid();
        tc->DrawFrame(0., param.minValue, maxValueForBins, param.maxValue);

        // draw the graphs
        for (auto &graph : tGraphs)
        {
            graph.Draw("P PMC PLC");
        }

        // add description
        tl->Draw("SAME");
        printDescription(dataType, param.name, binsType);

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
