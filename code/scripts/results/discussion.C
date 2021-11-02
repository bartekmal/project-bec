#include "../../HBTAnalysis/Units.hpp"
#include "../../HBTAnalysis/Utils.hpp"
#include "../../utils/Styles.hpp"
#include "../../HBTAnalysis/SelectionClass.hpp"

#include "../fitModel.C"

#include "TF1.h"
#include "TStyle.h"
#include "TGraphErrors.h"

#include <memory>
#include <string>

/*-------------- global config -------------------*/
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

// provide custom style (based on the general ones)
void setStyleLocal(const unsigned int &flagStyle)
{
    HBT::Styles::setStyle(flagStyle);

    // local style settings
    if (HBT::Styles::flagShowStats(flagStyle))
    {
        gStyle->SetStatW(0.13);
    }
    gROOT->ForceStyle();
}

void discussionGeneric(const PlotConfig &plot, const unsigned int &flagStyle)
{
    // global config
    // const std::string overlappingGraphLabel{"Data sample"};

    // set ROOT style
    setStyleLocal(flagStyle);

    // prepare I/O files
    auto fOut = std::make_unique<TFile>("discussion.root", "UPDATE");

    // prepare canvas for the current parameter
    auto tc = std::make_unique<TCanvas>(plot.plotName.c_str(), plot.plotTitle.c_str(), HBT::Styles::defaultCanvasSizeX, HBT::Styles::defaultCanvasSizeY);
    tc->SetGrid();
    tc->DrawFrame(plot.axisRangeX.first, plot.axisRangeY.first, plot.axisRangeX.second, plot.axisRangeY.second, tc->GetTitle());

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
    TLegend *tl = new TLegend(0.45, 0.20, 0.80, 0.375);
    for (auto i = 0; i < tGraphs.size(); ++i)
    {
        auto &graphs = tGraphs[i];
        const auto color = HBT::Styles::getColor(i);

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
            fTrend->SetParNames("b", "a");
            fTrend->SetParameters(0.0, 1.0);
            fTrend->SetParLimits(0, -10.0, 10.0);
            fTrend->SetParLimits(1, 0, 10);
            fTrend->SetLineColor(color);

            // do fit and save if valid
            const TFitResultPtr fitResult = graph.Fit(fTrend.get(), "SREX0"); // ! do not use errors in X for fits
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

    // add LHCb label
    auto lhcbLabel = HBT::Styles::makeLhcbLabel();
    HBT::Utils::addMultilineText("LHCb Unofficial", lhcbLabel);
    lhcbLabel->Draw();
    tc->Update();

    // add legend
    tl->Draw("SAME");

    // add description (optional)
    if (HBT::Styles::flagShowDescription(flagStyle))
    {
        auto plotDescription = HBT::Styles::makePlotDescription();
        HBT::Utils::addMultilineText(plot.plotDescription, plotDescription);

        plotDescription->Draw();
        tc->Update();
    }

    // save the current plot
    tc->SaveAs(".pdf");

    fOut->Close();
}

void discussion(const std::string basePath, const unsigned int &flagStyle = HBT::Styles::defaultStyleFlag)
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

    // ! useful string below
    // #font[12]{#sqrt{s_{#font[122]{NN}}}} = 5.02 TeV
    // #font[12]{#sqrt{s}} = 7 TeV
    // #font[12]{pp}

    const std::vector<PlotConfig> plots{
        // basic mult dependence
        PlotConfig("lambda_mult_pPb", ";#font[12]{N}_{VELO};#lambda", "#lambda : mult (pPb)", std::make_pair(0., multMax), std::make_pair(0., lambdaMax), "lambda", {basePath + "/main/output/systematics/RD_pPb/lambda_multReconstructed.csv"}, {"#font[12]{p}Pb 5.02 TeV"}, {false}),
        PlotConfig("lambda_mult_Pbp", ";#font[12]{N}_{VELO};#lambda", "#lambda : mult (Pbp)", std::make_pair(0., multMax), std::make_pair(0., lambdaMax), "lambda", {basePath + "/main/output/systematics/RD_Pbp/lambda_multReconstructed.csv"}, {"Pb#font[12]{p} 5.02 TeV"}, {false}),
        PlotConfig("radius_mult_pPb", ";#font[12]{N}_{VELO};#font[12]{R} [fm]", "R : mult (pPb)", std::make_pair(0., multMax), std::make_pair(0.0, radiusMax), "radius", {basePath + "/main/output/systematics/RD_pPb/radius_multReconstructed.csv"}, {"#font[12]{p}Pb 5.02 TeV"}, {true}),
        PlotConfig("radius_mult_Pbp", ";#font[12]{N}_{VELO};#font[12]{R} [fm]", "R : mult (Pbp)", std::make_pair(0., multMax), std::make_pair(0.0, radiusMax), "radius", {basePath + "/main/output/systematics/RD_Pbp/radius_multReconstructed.csv"}, {"Pb#font[12]{p} 5.02 TeV"}, {true}),
        // compare data samples
        PlotConfig("radius_mult_compareSystems", ";#font[12]{N_{rec}};#font[12]{R} [fm]", "R : data samples", std::make_pair(0., multMax), std::make_pair(0.0, radiusMax), "radius", {basePath + "/main/output/systematics/RD_pPb/radius_multReconstructed.csv", basePath + "/main/output/systematics/RD_Pbp/radius_multReconstructed.csv", externalPath + "/pp_lhcb_radius_multReconstructed.csv"}, {"#font[12]{p}Pb 5.02 TeV", "Pb#font[12]{p} 5.02 TeV", "#font[12]{pp} 7 TeV"}, {true, true, false}, 1),
        PlotConfig("lambda_mult_compareSystems", ";#font[12]{N_{rec}};#lambda", "#lambda : data samples", std::make_pair(0., multMax), std::make_pair(0., lambdaMax), "lambda", {basePath + "/main/output/systematics/RD_pPb/lambda_multReconstructed.csv", basePath + "/main/output/systematics/RD_Pbp/lambda_multReconstructed.csv", externalPath + "/pp_lhcb_lambda_multReconstructed.csv"}, {"#font[12]{p}Pb 5.02 TeV", "Pb#font[12]{p} 5.02 TeV", "#font[12]{pp} 7 TeV"}, {false, false, false}),
        // pp : LHCb vs ATLAS (manual: ? wider syst errors)
        PlotConfig("radius_mult_rapidity", ";#font[12]{N_{ch}};#font[12]{R} [fm]", "R : rapidity", std::make_pair(0., 250.0), std::make_pair(0.0, 3.5), "radius", {externalPath + "/pp_atlas_radius_multUnfolded_MB.csv", externalPath + "/pp_atlas_radius_multUnfolded_HM.csv", externalPath + "/pp_lhcb_radius_multUnfoldedAtlas.csv"}, {"ATLAS #font[12]{pp} 7 TeV (MB)", "ATLAS #font[12]{pp} 7 TeV (HM)", "LHCb #font[12]{pp} 7 TeV"}, {false, false, false}, 0),
        // pp : basic mult dependence (manual: mv legend to LHCb label)
        PlotConfig("radius_mult_pp", ";#font[12]{N_{ch}};#font[12]{R} [fm]", "R : mult (pp)", std::make_pair(0., 100.0), std::make_pair(0.0, 2.5), "radius", {externalPath + "/pp_lhcb_radius_multUnfolded.csv"}, {"#font[12]{pp} 7 TeV"}, {false}, 0),
        PlotConfig("lambda_mult_pp", ";#font[12]{N_{ch}};#lambda", "#lambda : mult (pp)", std::make_pair(0., 100.0), std::make_pair(0.3, lambdaMax), "lambda", {externalPath + "/pp_lhcb_lambda_multUnfolded.csv"}, {"#font[12]{pp} 7 TeV"}, {false}, 0),
    };

    for (const auto &plot : plots)
    {
        discussionGeneric(plot, flagStyle);
    }
}
