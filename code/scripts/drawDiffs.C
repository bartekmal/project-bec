#include "../HBTAnalysis/Utils.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"

/*-------------- configuration -------------------*/
Color_t binsKtColor[] = {kRed, kOrange, kGreen, kViolet, kAzure, kBlue, kMagenta, kCyan, kYellow, kGray};
Double_t multMax = 200.;

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

void printDescription(const TString dataType, const TString paramName)
{
    Double_t commPosX = 0.20;
    Double_t commPosY = 0.80;
    TLatex comments;
    comments.SetNDC();
    comments.SetTextSize(0.035);
    comments.DrawLatex(commPosX, commPosY, dataType);
    comments.DrawLatex(commPosX, commPosY - 0.05, paramName + " vs Nch");
    comments.DrawLatex(commPosX, commPosY - 0.10, "relative diff wrt reference [%]");
}

void drawDiffsGeneric(const TString inputFileMain, const TString hNameBaseMain, const TString fNameMain, const TString inputFileRef, const TString hNameBaseRef, const TString fNameRef, const bool flagIsUnlike, const TString dataType, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName)
{
    // get configuration
    auto selection = SelectionClass();

    auto binsMult = selection.getBinsOfMultiplicityCentres();
    auto binsMultError = selection.getBinsOfMultiplicityErrors();
    auto binsMultForKt = selection.getBinsOfMultiplicityForKtCentres();
    auto binsMultForKtError = selection.getBinsOfMultiplicityForKtErrors();
    auto binsKt = selection.getBinsOfKtCentres();
    auto binsKtError = selection.getBinsOfKtErrors();

    // set ROOT style
    HBT::Utils::setStyle();

    // prepare settings for type of binning
    const bool isMultBinsOnly = (nrBinsKt == 0) ? true : false;

    const int nrBinsKtForLoops = isMultBinsOnly ? 1 : nrBinsKt;
    auto curBinsMult = isMultBinsOnly ? binsMult : binsMultForKt;
    auto curBinsMultError = isMultBinsOnly ? binsMultError : binsMultForKtError;
    const TString binsType = isMultBinsOnly ? TString("mult") : TString("kT");
    const TString histType = flagIsUnlike ? TString("UNLIKE") : TString("LIKE");

    // prepare the PDF file
    const int padSize = 1200;
    const TString title = fNameMain + "_" + binsType + "_" + histType;
    const TString pdfFile = title + ".pdf";
    TCanvas *tc = new TCanvas(title, title, padSize, padSize);
    tc->SaveAs(pdfFile + "[");

    // input file with the fit results
    TFile *fInMain = new TFile(inputFileMain, "READ");
    TFile *fInRef = new TFile(inputFileRef, "READ");

    // initialise a list of the fit params
    const auto fitParams = initFitParams();

    for (const auto &param : fitParams)
    {

        // prepare graphs
        std::vector<TGraphErrors> tGraphs = std::vector<TGraphErrors>(nrBinsKtForLoops, TGraphErrors());
        TLegend *tl = new TLegend(0.80, 0.85, 0.95, 0.95);
        for (int i = 0; i < tGraphs.size(); ++i)
        {
            tGraphs[i].SetMarkerColor(binsKtColor[i]);
            tl->AddEntry(&tGraphs[i], TString("kT bin: ") += (i + 1), "pe");
        }

        for (int i = 0; i < nrBinsMult; ++i)
        {
            for (int j = 0; j < nrBinsKtForLoops; ++j)
            {
                // get the fit result in the given bin
                TString hNameMain = isMultBinsOnly ? HBT::Utils::getHistogramName(hNameBaseMain, hCommonEndName, isMultBinsOnly, i) : HBT::Utils::getHistogramName(hNameBaseMain, hCommonEndName, isMultBinsOnly, i, j);
                TString hNameRef = isMultBinsOnly ? HBT::Utils::getHistogramName(hNameBaseRef, hCommonEndName, isMultBinsOnly, i) : HBT::Utils::getHistogramName(hNameBaseRef, hCommonEndName, isMultBinsOnly, i, j);
                const auto fitResultMain = (TFitResult *)fInMain->Get(HBT::Utils::getFitResultName(hNameMain, fNameMain));
                const auto fitResultRef = (TFitResult *)fInRef->Get(HBT::Utils::getFitResultName(hNameRef, fNameRef));
                if (!(fitResultMain && fitResultMain->IsValid()) || !(fitResultRef && fitResultRef->IsValid()))
                {
                    std::cout << "One of the fits is not valid: \n\t" << fInMain->GetName() << "\n\t" << hNameMain << "\n\t" << fNameMain << "\n\t" << fInRef->GetName() << "\n\t" << hNameRef << "\n\t" << fNameRef << std::endl;
                    continue;
                }

                if (!param.name.compare("chi2"))
                {
                    // get individual results
                    const float chi2ndfMain = fitResultMain->Chi2() / fitResultMain->Ndf();
                    const float chi2ndfRef = fitResultRef->Chi2() / fitResultRef->Ndf();

                    // plot a ratio of the chi2/ndf values (just to see how it changes)
                    tGraphs[j].SetPoint(i, curBinsMult[i], (float)(chi2ndfMain - chi2ndfRef) / chi2ndfRef * 100.0);
                    tGraphs[j].SetPointError(i, curBinsMultError[i], 0.);
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

                    tGraphs[j].SetPoint(i, curBinsMult[i], diffValue);
                    tGraphs[j].SetPointError(i, curBinsMultError[i], diffError);
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

                    tGraphs[j].SetPoint(i, curBinsMult[i], diffValue);
                    tGraphs[j].SetPointError(i, curBinsMultError[i], diffError);
                }
            }
        }

        // prepare canvas for the current parameter
        tc->SetGrid();
        tc->DrawFrame(0., param.minValue, multMax, param.maxValue);

        // draw the graphs
        for (auto &graph : tGraphs)
        {
            graph.Draw("P");
        }

        // add description
        tl->Draw("SAME");
        printDescription(dataType, param.name);

        // save the current plot
        tc->SaveAs(pdfFile);
    }

    // close the PDF file
    tc->SaveAs(pdfFile + "]");
    delete tc;

    fInMain->Close();
    delete fInMain;
    fInRef->Close();
    delete fInRef;
}

void drawDiffs(const TString inputFileMain, const TString hNameBaseMain, const TString fNameMain, const TString inputFileRef, const TString hNameBaseRef, const TString fNameRef, const bool flagIsUnlike = false, const TString dataType = "", TString hCommonEndNameForMult = "", TString hCommonEndNameForKt = "")
{
    // get configuration
    auto selection = SelectionClass();

    auto nrBinsMult = selection.getNrOfBinsMult();
    auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    auto nrBinsKt = selection.getNrOfBinsKt();

    // call for mult bins only
    drawDiffsGeneric(inputFileMain, hNameBaseMain, fNameMain, inputFileRef, hNameBaseRef, fNameRef, flagIsUnlike, dataType, nrBinsMult, 0, hCommonEndNameForMult);
    // call for mult + kT bins
    drawDiffsGeneric(inputFileMain, hNameBaseMain, fNameMain, inputFileRef, hNameBaseRef, fNameRef, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt);
}
