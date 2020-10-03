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
        FitParam(0, "N", 0., 1.),
        FitParam(1, "lambda", 0., 1.),
        FitParam(2, "R", 0., 30.),
        // FitParam(3, "R_eff [fm]", 0., 5.),
        FitParam(4, "delta", 0., 1.),
        FitParam(5, "lambda_bkg", 0., 1.),
        FitParam(6, "R_bkg", 0., 3.),
        // FitParam(7, "alpha_bkg", 0., 3.),
        FitParam(8, "scaleFactor(lambda_bkg)", 0., 2.),
        FitParam(9, "effective(lambda_bkg)", 0., 0.5),
        FitParam(10, "chi2", 0., 7.)};
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
}

void drawTrendsGeneric(const TString inputFile, const TString hNameBase, const TString fName, const bool flagIsUnlike, const TString dataType, const int nrBinsMult, const int nrBinsKt, TString hCommonEndName)
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
    const TString title = fName + "_" + binsType + "_" + histType;
    const TString pdfFile = title + ".pdf";
    TCanvas *tc = new TCanvas(title, title, padSize, padSize);
    tc->SaveAs(pdfFile + "[");

    // input file with the fit results
    TFile *fIn = new TFile(inputFile, "READ");

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
                TString hName = isMultBinsOnly ? HBT::Utils::getHistogramName(hNameBase, hCommonEndName, isMultBinsOnly, i) : HBT::Utils::getHistogramName(hNameBase, hCommonEndName, isMultBinsOnly, i, j);
                const auto fitResult = (TFitResult *)fIn->Get(HBT::Utils::getFitResultName(hName, fName));
                if (!(fitResult && fitResult->IsValid()))
                {
                    std::cout << "The given fit is not valid: \n\t" << fIn->GetName() << "\n\t" << hName << "\n\t" << fName << std::endl;
                    continue;
                }

                if (!param.name.compare("chi2"))
                {
                    tGraphs[j].SetPoint(i, curBinsMult[i], fitResult->Chi2() / fitResult->Ndf());
                    tGraphs[j].SetPointError(i, curBinsMultError[i], 0.);
                }
                else if (!param.name.compare("effective(lambda_bkg)"))
                {
                    tGraphs[j].SetPoint(i, curBinsMult[i], fitResult->Parameter(parIdLambdaBkg) * fitResult->Parameter(parIdLambdaBkgScaleFactor));
                    // scale the error of the lambda_bkg by the scaleFactor used
                    tGraphs[j].SetPointError(i, curBinsMultError[i], fitResult->ParError(parIdLambdaBkg) * fitResult->Parameter(parIdLambdaBkgScaleFactor));
                }
                else
                {
                    tGraphs[j].SetPoint(i, curBinsMult[i], fitResult->Parameter(param.id));
                    tGraphs[j].SetPointError(i, curBinsMultError[i], fitResult->ParError(param.id));
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

    fIn->Close();
    delete fIn;
}

void drawTrends(const TString inputFile, const TString hNameBase, const TString fName, const bool flagIsUnlike = false, const TString dataType = "", TString hCommonEndNameForMult = "", TString hCommonEndNameForKt = "")
{
    // get configuration
    auto selection = SelectionClass();

    auto nrBinsMult = selection.getNrOfBinsMult();
    auto nrBinsMultForKt = selection.getNrOfBinsMultForKt();
    auto nrBinsKt = selection.getNrOfBinsKt();

    // call for mult bins only
    drawTrendsGeneric(inputFile, hNameBase, fName, flagIsUnlike, dataType, nrBinsMult, 0, hCommonEndNameForMult);
    // call for mult + kT bins
    drawTrendsGeneric(inputFile, hNameBase, fName, flagIsUnlike, dataType, nrBinsMultForKt, nrBinsKt, hCommonEndNameForKt);
}
