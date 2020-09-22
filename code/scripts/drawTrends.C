/*-------------- configure plots -------------------*/
Double_t binsMult[] = {7.5, 12.5, 17.5, 22.5, 27.5, 32.5, 37.5, 42.5, 47.5, 52.5, 57.5, 62.5, 72.5, 85.0, 95.0, 107.5, 127.5, 160.0};
Double_t binsMultError[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
Double_t binsMultForKt[] = {10.0, 25.0, 42.5, 65.0, 90.0, 120.0};
Double_t binsMultForKtError[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
Double_t binsKt[] = {0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 0.85, 0.95};
Double_t binsKtError[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
Color_t binsKtColor[] = {kRed, kOrange, kGreen, kViolet, kAzure, kBlue, kMagenta, kCyan, kYellow, kGray};

Double_t multMax = 200.;

/*-------------- configure fit params -------------------*/

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

void setStyle()
{
    //style
    gROOT->Reset();                  // Reset options
    gStyle->SetPadGridX(kFALSE);     // No grid in x (0)
    gStyle->SetPadGridY(kFALSE);     // No grid in y (0)
    gStyle->SetOptTitle(kFALSE);     // No title
    gStyle->SetStatBorderSize(0);    // Stats box shadow size (2)
    gStyle->SetStatColor(18);        // Stats box fill color (0)
    gStyle->SetStatFont(62);         // Stats font style (62)
    gStyle->SetStatH(0.1);           // Stats box height
    gStyle->SetStatW(0.15);          // Stats box width
    gStyle->SetStatX(0.91);          // Stats box x coordinate
    gStyle->SetStatY(0.91);          // Stats box y coordinate
    gStyle->SetStatStyle(1001);      // Stat box fill style
    gStyle->SetStatTextColor(1);     // Stat text color (1)
    gStyle->SetOptStat(0);           // No statistics (0) (1000001110)
    gStyle->SetOptFit(111);          // No fit box (0) (111)
    gStyle->SetFrameBorderMode(0);   // No red box
    gStyle->SetHistFillColor(0);     // Histogram fill color (0) (18)
    gStyle->SetHistFillStyle(1001);  // Histogram fill style (1001) (0)
    gStyle->SetHistLineColor(kBlue); // Histogram line color (1)
    gStyle->SetHistLineStyle(0);     // Histogram line style (0)
    gStyle->SetHistLineWidth(1);     // Histogram line width (1.0)
    gStyle->SetMarkerStyle(21);      // Marker style (0)
    gStyle->SetMarkerColor(kBlack);  // Marker color (1)
    gStyle->SetMarkerSize(1.2);      // Marker size ()
    gStyle->SetLineColor(kBlack);    // Line color (1)
    gStyle->SetLineWidth(1.0);       // Line width (1.0)
    gStyle->SetTextSize(0.07);       // Text size (1.0)
    gStyle->SetLabelSize(0.03, "x"); // X numbers label size ()
    gStyle->SetLabelSize(0.03, "y"); // Y numbers label size ()
    gStyle->SetTitleSize(0.04, "x"); // X title size ()
    gStyle->SetTitleSize(0.04, "y"); // Y title size ()
    gStyle->SetErrorX(0);            // No errors along x
    gROOT->ForceStyle();
}

// get a full histogram name in the given bin
TString getFullHistogramName(const TString baseNameBegin, const TString baseNameEnd, const bool multBinsOnly, const int binNrMult, const int binNrKt = 0)
{
    return multBinsOnly ? TString(TString(TString(baseNameBegin + "_") += (binNrMult + 1)) + "_0") + baseNameEnd : TString(TString(TString(TString(baseNameBegin + "_") += (binNrMult + 1)) + "_") += (binNrKt + 1)) + baseNameEnd;
}

// get a name of the fit result
TString getFitResultName(const TString hName, const TString fName)
{
    return hName + TString("_") + fName;
}

void drawTrends(const TString inputFile, const TString hNameBase, const TString fName, const bool flagIsUnlike = false, const TString dataType = "", const int nrBinsMult = 6, const int nrBinsKt = 0, TString hCommonEndName = "")
{
    setStyle();

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
                TString hName = isMultBinsOnly ? getFullHistogramName(hNameBase, hCommonEndName, isMultBinsOnly, i) : getFullHistogramName(hNameBase, hCommonEndName, isMultBinsOnly, i, j);
                const auto fitResult = (TFitResult *)fIn->Get(getFitResultName(hName, fName));
                if (!(fitResult && fitResult->IsValid()))
                {
                    std::cout << "The given fit is not valid: \n\t" << fIn->GetName() << "\n\t" << hName << "\n\t" << fName << std::endl;
                    continue;
                }

                if (!param.name.compare("chi2"))
                {
                    tGraphs[j].SetPoint(i, curBinsMult[i], fitResult->Chi2() / fitResult->Ndf());
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
