#include "../HBTAnalysis/Units.hpp"
#include "../HBTAnalysis/Utils.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"
#include "../HBTAnalysis/CoulombCorrection.hpp"

#include <map>
#include <utility>
#include <iostream>

#include "TF1.h"
#include "Fit/Fitter.h"

// use consts from Units
using namespace HBT::Units;
using Range = std::pair<FloatType, FloatType>;

/*-------------- helpers -------------------*/

class FitParam
{
private:
    const unsigned int m_id{};
    const std::string m_name{};
    const FloatType m_initValue{};
    const bool m_isFixed{};
    const Range m_limits{};
    const Range m_rangeTrends{};
    const Range m_rangeDiffs{};

public:
    FitParam() = default;
    FitParam(const unsigned int &iId, const std::string &iName, const FloatType &iInitValue, const bool &iIsFixed = false, const Range &iLimits = {}, const Range &iRangeTrends = {}, const Range &iRangeDiffs = {}) : m_id(iId), m_name(iName), m_initValue(iInitValue), m_isFixed(iIsFixed), m_limits(iLimits), m_rangeTrends(iRangeTrends), m_rangeDiffs(iRangeDiffs){};

    friend std::ostream &operator<<(std::ostream &os, const FitParam &param)
    {
        os << param.id() << "\t"
           << param.name() << "\t"
           << param.initValue() << "\t"
           << param.isFixed() << "\t"
           << "(" << param.limitMin() << "," << param.limitMax() << ")"
           << "\t";

        return os;
    }

    // getters
    inline unsigned int id() const { return m_id; }
    inline std::string name() const { return m_name; }
    inline FloatType initValue() const { return m_initValue; }
    inline bool isFixed() const { return m_isFixed; }
    inline FloatType limitMin() const { return m_limits.first; }
    inline FloatType limitMax() const { return m_limits.second; }
    inline FloatType rangeTrendsMin() const { return m_rangeTrends.first; }
    inline FloatType rangeTrendsMax() const { return m_rangeTrends.second; }
    inline FloatType rangeDiffsMin() const { return m_rangeDiffs.first; }
    inline FloatType rangeDiffsMax() const { return m_rangeDiffs.second; }
};

/*-------------- end of helpers -------------------*/

/*-------------- configuration -------------------*/

const Double_t R_eff = HBT::Coulomb::REffForPions;

std::map<std::string, FitParam> prepareFitParams()
{
    // keep an eye on the param IDs (should be unique, also used in the fit model below)
    return {
        {"norm", FitParam(0, "N", 0.85, false, {0., 1.}, {0.5, 1.5}, {-60., 60.})},
        {"delta", FitParam(1, "#delta", 0.15, false, {0., 1.}, {0., 1.}, {-100., 100.})},

        {"sigmaBkgMult", FitParam(2, "mult(#sigma_{bkg})", 0, true, {0, 200}, {0, 200}, {-40., 40.})},
        {"sigmaBkgKt", FitParam(3, "k_{T}(#sigma_{bkg})", 0., true, {0., 2.0}, {0.0, 2.0}, {-40., 40.})},
        {"sigmaBkgSig0", FitParam(4, "#sigma_{0}", 0.5, false, {0.0, 2.0}, {0.0, 1.0}, {-40., 40.})},
        {"sigmaBkgSig1", FitParam(5, "#sigma_{1}", 0.5, false, {0.0, 2.0}, {0.0, 2.0}, {-40., 40.})},
        {"sigmaBkgMult0", FitParam(6, "mult_{0}", 40, false, {0, 200}, {0.0, 200.0}, {-40., 40.})},
        {"sigmaBkgExpKt", FitParam(7, "n_{k_{T}}", 0.3, false, {0., 2.}, {0.0, 2.0}, {-40., 40.})},

        {"amplBkgMult", FitParam(8, "mult(A_{bkg})", 0, true, {0, 200}, {0, 200}, {-40., 40.})},
        {"amplBkgKt", FitParam(9, "k_{T}(A_{bkg})", 0., true, {0.0, 2.0}, {0.0, 2.0}, {-40., 40.})},
        {"amplBkgAmpl0", FitParam(10, "A_{0}", 2.0, false, {0.0, 10.0}, {0.0, 10.0}, {-40., 40.})},
        {"amplBkgKt0", FitParam(11, "k_{0}", 2.5, false, {0.0, 5.0}, {0.0, 5.0}, {-40., 40.})},
        {"amplBkgExpMult", FitParam(12, "n_{mult}", 0.7, false, {0.0, 2.0}, {0.4, 1.0}, {-40., 40.})},

        {"scaleZ", FitParam(13, "z", 1.0, false, {0., 2.}, {0., 2.}, {-40., 40.})},

        {"radiusEff", FitParam(14, "R_{eff} [fm]", R_eff, true, {0., 5.}, {0., 5.}, {-100., 100.})},

        {"radius", FitParam(15, "R [GeV^{-1}]", 15.0, false, {0., 50.}, {0., 30.}, {-40., 40.})},
        {"alphaLevy", FitParam(16, "#alpha_{L}", 1.0, true, {0., 2.}, {0., 2.}, {-60., 60.})},
        {"lambda", FitParam(17, "#lambda", 0.7, false, {0., 1.5}, {0., 1.5}, {-40., 40.})},
    };
}

std::map<std::string, FitParam> prepareFitParamsForTrends()
{
    auto params = prepareFitParams();
    // special cases (in particular, param ID not important)
    // params.emplace("amplBkgScaled", FitParam(99, "z*A_{bkg}", 1.0, true, {}, {0., 5.}, {-40., 40.}));
    params.emplace("MinFcn", FitParam(99, "MinFcn / ndf", 1.0, false, {}, {0., 7.}, {-60., 60.}));

    return params;
}

std::vector<std::string> prepareKeyListForTrends()
{
    return {
        "MinFcn",
        "norm",
        "delta",
        "sigmaBkgSig0",
        "sigmaBkgSig1",
        "sigmaBkgMult0",
        "sigmaBkgExpKt",
        "amplBkgAmpl0",
        "amplBkgKt0",
        "amplBkgExpMult",
        "radiusEff",
        "scaleZ",
        "alphaLevy",
        "radius",
        "lambda",
    };
}

/*-------------- enf of configuration -------------*/

/*-------------- helpers -------------------*/

void setParNames(TF1 *f, const std::map<std::string, FitParam> &params)
{
    for (const auto &el : params)
    {
        const auto &param = el.second;
        f->SetParName(param.id(), param.name().c_str());
    }
}

void setInitValues(ROOT::Fit::Fitter &fitter, const std::map<std::string, FitParam> &params)
{
    const auto nrOfParams = params.size();

    double initValues[nrOfParams]; // keep an eye on initialisation
    for (const auto &el : params)
    {
        const auto &param = el.second;
        initValues[param.id()] = param.initValue();
    }

    fitter.Config().SetParamsSettings(nrOfParams, initValues);
}

void setLimits(ROOT::Fit::Fitter &fitter, const std::map<std::string, FitParam> &params)
{
    for (const auto &el : params)
    {
        const auto &param = el.second;
        fitter.Config().ParSettings(param.id()).SetLimits(param.limitMin(), param.limitMax());
    }
}

void setFixed(ROOT::Fit::Fitter &fitter, const std::map<std::string, FitParam> &params, TF1 *funcFit)
{
    for (const auto &el : params)
    {
        const auto &param = el.second;
        if (param.isFixed())
        {
            fitter.Config().ParSettings(param.id()).Fix();
            // set also in elementary TF1 (could be improved not to do it twice; just for plotting the results)
            funcFit->FixParameter(param.id(), param.initValue());
        }
    }
}

void setBinCentres(ROOT::Fit::Fitter &fitter, const std::map<std::string, FitParam> &params, const FloatType &mult, const FloatType &kT, const bool &isMultBinsOnly)
{
    fitter.Config().ParSettings(params.at("sigmaBkgMult").id()).SetValue(mult);
    fitter.Config().ParSettings(params.at("amplBkgMult").id()).SetValue(mult);

    // set neutral kT-part if only in mult bins
    if (isMultBinsOnly)
    {
        fitter.Config().ParSettings(params.at("sigmaBkgKt").id()).SetValue(0.4846); // use some const for kT (average value in the sample : 0.4846 GeV)
        // fitter.Config().ParSettings(params.at("sigmaBkgExpKt").id()).SetValue(0);
        // fitter.Config().ParSettings(params.at("sigmaBkgExpKt").id()).Fix();

        // fitter.Config().ParSettings(params.at("amplBkgKt").id()).SetValue(0.); // use some const for kT (average value in the sample : 0.4846 GeV)
        // fitter.Config().ParSettings(params.at("amplBkgKt0").id()).SetValue(1.);
        // fitter.Config().ParSettings(params.at("amplBkgKt0").id()).Fix();
        fitter.Config().ParSettings(params.at("amplBkgKt").id()).SetValue(0.4846);
    }
    else
    {
        fitter.Config().ParSettings(params.at("sigmaBkgKt").id()).SetValue(kT);
        fitter.Config().ParSettings(params.at("amplBkgKt").id()).SetValue(kT);
    }
}

// /*-------------- end of helpers -------------------*/

/*-------------- define partial fit functions -------------------*/

Double_t funcBkgSigma(Double_t *x, Double_t *par)
{
    return (par[2] + par[3] * exp(-par[0] / par[4])) * pow(par[1], par[5]);
}

Double_t funcBkgAmpl(Double_t *x, Double_t *par)
{
    return par[2] / (pow(par[0], par[4]) * exp(par[1] / par[3]));
}

Double_t funcBkg(Double_t *x, Double_t *par)
{
    return par[0] * (1 + par[1] * x[0]) * (1 + par[13] * funcBkgAmpl(x, &par[8]) / (sqrt(HBT::Units::TwoPi) * funcBkgSigma(x, &par[2])) * exp(-0.5 * pow(x[0] / funcBkgSigma(x, &par[2]), 2)));
}

Double_t funcCoulombLike(Double_t *x, Double_t *par)
{
    return HBT::Coulomb::BS(x[0], true, par[0], HBT::Units::MassPion);
}

Double_t funcCoulombUnlike(Double_t *x, Double_t *par)
{
    return HBT::Coulomb::BS(x[0], false, par[0], HBT::Units::MassPion);
}

Double_t funcBec(Double_t *x, Double_t *par)
{
    return 1 + exp(-pow(fabs(par[0] * x[0]), par[1]));
}

// /*-------------- define final fit functions -------------------*/

Double_t funcFullUnlike(Double_t *x, Double_t *par)
{
    return funcBkg(x, &par[0]) * funcCoulombUnlike(x, &par[14]);
}

Double_t funcFullLike(Double_t *x, Double_t *par)
{
    return funcBkg(x, &par[0]) * (1 - par[17] + par[17] * funcCoulombLike(x, &par[14]) * funcBec(x, &par[15]));
}

/*----------------------------------------------------------------*/

// void fitModel()
// {
//     const auto fitParams = prepareFitParams();

//     for (const auto &el : fitParams)
//     {
//         const auto &param = el.second;
//         std::cout << param << std::endl;
//     }
// }