#include "../../HBTAnalysis/Units.hpp"
#include "../../HBTAnalysis/Utils.hpp"
#include "../../HBTAnalysis/SelectionClass.hpp"

#include "../fitModel.C"

#include "TF1.h"
#include "TStyle.h"
#include "TGraphErrors.h"

#include <memory>
#include <string>

/*-------------- global config -------------------*/
const int padSize = 1200;

const FloatType errorXForPlots{1.5f};
/*-------------- end of global config -------------*/

/*-------------- helpers -------------------*/

using Range = std::pair<FloatType, FloatType>;
struct PlotConfig
{
    const std::string plotName{};
    const std::string plotTitle{};
    const std::string plotDescription{};
    const Range axisRangeX{};
    const Range axisRangeY{};

    const std::string paramKey{};
    const std::vector<std::string> inputPaths{};
    const std::vector<std::string> inputLabels{};
    const std::vector<bool> flagScaleRUnit{};
    const unsigned int flagFit{0};
    const bool flagScaleMultAxis{false};

    PlotConfig() = default;
    PlotConfig(const std::string &plotName, const std::string &plotTitle, const std::string &plotDescription, const Range &axisRangeX, const Range &axisRangeY, const std::string &paramKey, const std::vector<std::string> &inputPaths, const std::vector<std::string> &inputLabels, const std::vector<bool> &flagScaleRUnit, const unsigned int &flagFit = 0, const bool &flagScaleMultAxis = false) : plotName(plotName), plotTitle(plotTitle), plotDescription(plotDescription), axisRangeX(axisRangeX), axisRangeY(axisRangeY), paramKey(paramKey), inputPaths(inputPaths), inputLabels(inputLabels), flagScaleRUnit(flagScaleRUnit), flagFit(flagFit), flagScaleMultAxis(flagScaleMultAxis){};
};

/*-------------- end of helpers -------------------*/

void printDescription(const std::string &description)
{
    Double_t commPosX = 0.20;
    Double_t commPosY = 0.80;
    TLatex comments;
    comments.SetNDC();
    comments.SetTextSize(0.035);
    comments.DrawLatex(commPosX, commPosY, description.c_str());
}

// provide custom style (based on the general one)
void setStyleLocal()
{
    HBT::Utils::setStyle();
    gStyle->SetMarkerSize(1);
    gStyle->SetOptFit(111);
    gStyle->SetStatX(0.875);
    gStyle->SetStatY(0.3);
}

void discussionGeneric(const PlotConfig &plot)
{
    // global config
    const std::string overlappingGraphLabel{"Data sample"};

    // set ROOT style
    setStyleLocal();

    // prepare I/O files
    auto fOut = std::make_unique<TFile>("discussion.root", "UPDATE");

    // prepare canvas for the current parameter
    auto tc = std::make_unique<TCanvas>(plot.plotName.c_str(), plot.plotTitle.c_str(), padSize, padSize);
    tc->SetGrid();
    tc->DrawFrame(plot.axisRangeX.first, plot.axisRangeY.first, plot.axisRangeX.second, plot.axisRangeY.second);

    // read graphs from csv files
    std::vector<std::map<std::string, TGraphErrors>> tGraphs{};
    for (auto i = 0; i < plot.inputPaths.size(); ++i)
    {
        tGraphs.emplace_back();
        auto &graphs = tGraphs.back();
        graphs["stat"] = TGraphErrors(plot.inputPaths[i].c_str(), "%lg %lg %lg %*lg");
        graphs["syst"] = TGraphErrors(plot.inputPaths[i].c_str(), "%lg %lg %*lg %lg");

        // scale to fm if needed
        if (plot.flagScaleRUnit[i])
        {
            for (auto &el : graphs)
            {
                auto &graph = el.second;
                for (auto i = 0; i < graph.GetN(); ++i)
                {
                    graph.SetPointY(i, GevInvToFm(graph.GetPointY(i)));
                    graph.SetPointError(i, graph.GetErrorX(i), GevInvToFm(graph.GetErrorY(i)));
                }
            }
        }
    }

    // process data in graphs
    for (auto &graphs : tGraphs)
    {
        // set errors in X for plotting
        {
            auto &graph = graphs["syst"];
            for (auto i = 0; i < graph.GetN(); ++i)
            {
                graph.SetPointError(i, errorXForPlots, graph.GetErrorY(i));
            }
        }

        if (plot.flagScaleMultAxis)
        {
            for (auto &el : graphs)
            {
                auto &graph = el.second;
                for (auto i = 0; i < graph.GetN(); ++i)
                {
                    graph.SetPointX(i, pow(graph.GetPointX(i), 1. / 3.));
                }
            }
        }

        // // offset points slighty for plots clarity
        // graph.MovePoints(0.05,0.00);
    }

    // drawing and fits
    TLegend *tl = new TLegend(0.80, 0.75, 0.95, 0.95);
    tl->SetHeader(overlappingGraphLabel.c_str(), "C");
    for (auto i = 0; i < tGraphs.size(); ++i)
    {
        auto &graphs = tGraphs[i];
        const auto color = gStyle->GetColorPalette(0 + i * 200);

        graphs["syst"].SetFillStyle(0);
        graphs["syst"].SetLineColor(color);
        graphs["syst"].Draw("5");

        graphs["stat"].SetFillStyle(0);
        graphs["stat"].SetLineColor(color);
        graphs["stat"].SetMarkerColor(color);
        graphs["stat"].Draw("P");

        tl->AddEntry(&graphs["stat"], plot.inputLabels[i].c_str(), "pe");

        // make fits to the graph with statistical error only (optional)
        if (plot.flagFit)
        {
            auto &graph = graphs["stat"];

            // configure fit
            const auto funcName = TString("funcMult");

            auto fTrend = std::make_unique<TF1>(funcName, (plot.flagFit == 1) ? "[0] + [1]*pow(x,1./3.)" : "[0] + [1]*x", plot.axisRangeX.first, plot.axisRangeX.second);
            fTrend->SetParNames("a", "b");
            fTrend->SetParameters(0.0, 1.0);
            fTrend->SetParLimits(0, -10.0, 10.0);
            fTrend->SetParLimits(1, 0, 10);
            fTrend->SetLineColor(color);

            // do fit and save if valid
            const TFitResultPtr fitResult = graph.Fit(fTrend.get(), "SREX0");
            if ((fitResult.Get() != nullptr) && fitResult->IsValid())
            {
                fTrend->Write(funcName);
            }
            else
            {
                graph.GetListOfFunctions()->Remove(fTrend.get());
                const auto curStyle = gStyle->GetOptFit();
                gStyle->SetOptFit(0);
                graph.UseCurrentStyle();
                gStyle->SetOptFit(curStyle);
                std::cout << "Fit (" << funcName << ") is not valid: \n\t" << &graph << "\n\t" << fTrend.get() << std::endl;
            }
        }
    }

    // add description
    tl->Draw("SAME");
    printDescription(plot.plotDescription);

    // save the current plot
    tc->SaveAs(".pdf");

    fOut->Close();
}

void discussion(const std::string basePath)
{
    // // keep to acces the param configuration (eg name in latex etc)
    // const auto fitParams = prepareFitParamsForTrends();
    // const auto el = fitParams.find(plot.paramKey);
    // if (el == fitParams.end())
    // {
    //     return;
    // }
    // const auto &param = fitParams.at(plot.paramKey);

    // global config
    const std::string externalPath{basePath + "/../external"};
    const FloatType radiusMax = 5.0; // fm
    const FloatType lambdaMax = 1.0;
    const FloatType multMax = 180.0;

    const std::vector<PlotConfig> plots{
        // basic mult dependence
        PlotConfig("lambda_mult_pPb", ";mult;#lambda", "#lambda : mult (pPb)", std::make_pair(0., multMax), std::make_pair(0., lambdaMax), "lambda", {basePath + "/main/output/systematics/RD_pPb/lambda_multReconstructed.csv"}, {"pPb"}, {false}),
        PlotConfig("lambda_mult_Pbp", ";mult;#lambda", "#lambda : mult (Pbp)", std::make_pair(0., multMax), std::make_pair(0., lambdaMax), "lambda", {basePath + "/main/output/systematics/RD_Pbp/lambda_multReconstructed.csv"}, {"Pbp"}, {false}),
        PlotConfig("radius_mult_pPb", ";mult;R [fm]", "R : mult (pPb)", std::make_pair(0., multMax), std::make_pair(0.0, radiusMax), "radius", {basePath + "/main/output/systematics/RD_pPb/radius_multReconstructed.csv"}, {"pPb"}, {true}, 1),
        PlotConfig("radius_mult_Pbp", ";mult;R [fm]", "R : mult (Pbp)", std::make_pair(0., multMax), std::make_pair(0.0, radiusMax), "radius", {basePath + "/main/output/systematics/RD_Pbp/radius_multReconstructed.csv"}, {"Pbp"}, {true}, 1),
        // compare data samples
        PlotConfig("radius_mult_compareSystems", ";mult;R [fm]", "R : data samples", std::make_pair(0., multMax), std::make_pair(0.0, radiusMax), "radius", {basePath + "/main/output/systematics/RD_pPb/radius_multReconstructed.csv", basePath + "/main/output/systematics/RD_Pbp/radius_multReconstructed.csv", externalPath + "/pp_lhcb_radius_multReconstructed.csv"}, {"pPb", "Pbp", "pp"}, {true, true, false}, 1),
        PlotConfig("lambda_mult_compareSystems", ";mult;#lambda", "#lambda : data samples", std::make_pair(0., multMax), std::make_pair(0., lambdaMax), "lambda", {basePath + "/main/output/systematics/RD_pPb/lambda_multReconstructed.csv", basePath + "/main/output/systematics/RD_Pbp/lambda_multReconstructed.csv", externalPath + "/pp_lhcb_lambda_multReconstructed.csv"}, {"pPb", "Pbp", "pp"}, {false, false, false}),
        // pp : LHCb vs ATLAS
        PlotConfig("radius_mult_rapidity", ";N_ch;R [fm]", "R : rapidity", std::make_pair(0., 250.0), std::make_pair(0.0, 3.5), "radius", {externalPath + "/pp_atlas_radius_multUnfolded_MB.csv", externalPath + "/pp_atlas_radius_multUnfolded_HM.csv", externalPath + "/pp_lhcb_radius_multUnfolded.csv"}, {"pp ATLAS (MB)", "pp ATLAS (HM)", "pp LHCb"}, {false, false, false}, 0),
    };

    for (const auto &plot : plots)
    {
        discussionGeneric(plot);
    }
}
