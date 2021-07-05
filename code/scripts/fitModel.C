#include "../HBTAnalysis/Units.hpp"
#include "../HBTAnalysis/Utils.hpp"
#include "../HBTAnalysis/SelectionClass.hpp"
#include "../HBTAnalysis/CoulombCorrection.hpp"

#include <map>
#include <utility>
#include <iostream>

#include "TF3.h"
#include "Fit/Fitter.h"

// use consts from Units
using namespace HBT::Units;
using Range = std::pair<FloatType, FloatType>;

/*-------------- helpers -------------------*/

class FitParam
{
private:
    unsigned int m_id{};
    std::string m_name{};
    FloatType m_initValue{};
    bool m_isFixed{};
    Range m_limits{};
    Range m_rangeTrends{};
    Range m_rangeDiffs{};

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

    // setters
    inline void setInitValue(const FloatType &val) { m_initValue = val; }
    inline void setIsFixed(const bool &val) { m_isFixed = val; }
    inline void fixToValue(const FloatType &val)
    {
        m_initValue = val;
        m_isFixed = true;
        // modify limits to contain the fixed value
        m_limits.first = val;
        m_limits.second = val;
    }
};

/*-------------- end of helpers -------------------*/

/*-------------- configuration -------------------*/

const Double_t R_eff = HBT::Coulomb::REffForPions;

std::vector<std::string> listOfParamsCommon()
{
    return {
        "sigmaBkgSig0",
        "sigmaBkgSig1",
        "sigmaBkgMult0",
        "sigmaBkgExpKt",
        "amplBkgAmpl0",
        "amplBkgKt0",
        "amplBkgExpMult",

    };
}

std::vector<std::string> listOfParamsIndividual()
{
    return {
        "norm",
        "delta",
        "radiusEff",
        "scaleZ",
        "alphaLevy",
        "radius",
        "lambda",
    };
}

std::map<std::string, FitParam> prepareParamsBkgSigma()
{
    return {
        {"sigmaBkgSig0", FitParam(0, "#font[12]{#sigma_{0}}", 0.5, false, {0.0, 2.0}, {0.0, 2.0}, {-40., 40.})},
        {"sigmaBkgSig1", FitParam(1, "#font[12]{#sigma_{1}}", 0.5, false, {0.0, 2.0}, {0.0, 2.0}, {-40., 40.})},
        {"sigmaBkgMult0", FitParam(2, "#font[12]{N_{0}}", 10, true, {0, 100}, {0.0, 100.0}, {-40., 40.})},
        {"sigmaBkgExpKt", FitParam(3, "#font[12]{n_{k_{#font[122]{T}}}}", 0.1, false, {0., 1.}, {0.0, 1.0}, {-40., 40.})},
    };
}

std::map<std::string, FitParam> prepareParamsBkgAmpl()
{
    return {
        {"amplBkgAmpl0", FitParam(4, "#font[12]{A_{0}}", 0.4, false, {0.0, 5.0}, {0.0, 5.0}, {-40., 40.})},
        {"amplBkgExpMult", FitParam(5, "#font[12]{n_{N_{A}}}", 1.0, false, {0.0, 2.0}, {0.3, 1.3}, {-40., 40.})},
        {"amplBkgKt0", FitParam(6, "#font[12]{k_{0}} [GeV]", 0.2, false, {0.0, 3.0}, {0.0, 3.0}, {-40., 40.})},
    };
}

std::map<std::string, FitParam> prepareParamsGeneral()
{
    // keep an eye on the param IDs (should be unique, also used in the fit model below)
    return {
        {"norm", FitParam(7, "#font[12]{N}", 0.85, false, {0., 1.}, {0.5, 1.2}, {-60., 60.})},
        {"delta", FitParam(8, "#font[12]{#delta} [GeV^{-1}]", 0.15, false, {0., 1.}, {0., 1.}, {-100., 100.})},

        {"scaleZ", FitParam(9, "#font[12]{z}", 1.0, false, {0., 2.}, {0.4, 1.4}, {-40., 40.})},

        {"radiusEff", FitParam(10, "#font[12]{R_{#font[122]{eff}}} [fm]", R_eff, true, {0., 5.}, {0., 5.}, {-100., 100.})},

        {"radius", FitParam(11, "#font[12]{R} [GeV^{-1}]", 15.0, false, {0., 50.}, {0., 30.}, {-40., 40.})},
        {"alphaLevy", FitParam(12, "#font[12]{#alpha_{#font[122]{L}}}", 1.0, true, {0., 2.}, {0., 2.}, {-60., 60.})},
        {"lambda", FitParam(13, "#font[12]{#lambda}", 0.7, false, {0., 1.5}, {0., 1.4}, {-40., 40.})},
    };
}

std::map<std::string, FitParam> prepareParamsFull(const bool &fixNeutralKtForMultBins = false)
{
    // keep an eye on the param IDs (should be unique, also used in the fit model below)
    auto params = prepareParamsBkgSigma();
    const auto params1 = prepareParamsBkgAmpl();
    const auto params2 = prepareParamsGeneral();
    params.insert(params1.begin(), params1.end());
    params.insert(params2.begin(), params2.end());

    if (fixNeutralKtForMultBins)
    {
        params["sigmaBkgExpKt"].fixToValue(0);
        params["amplBkgKt0"].fixToValue(HBT::Units::Infinity); // could be improved by finding another way to make exp(kT/k0)->1
    }

    return params;
}

std::map<std::string, FitParam> prepareParamsUnlike(const bool &fixNeutralKtForMultBins = false)
{
    // get the general config
    auto params = prepareParamsFull(fixNeutralKtForMultBins);

    // make specific modifications
    params["scaleZ"].fixToValue(1.);
    params["radius"].fixToValue(0.);
    params["alphaLevy"].fixToValue(1.);
    params["lambda"].fixToValue(0.);

    return params;
}

std::map<std::string, FitParam> prepareFitParamsForTrends()
{
    auto params = prepareParamsFull();
    // special cases (in particular, param ID not important)
    // params.emplace("amplBkgScaled", FitParam(99, "#font[12]{z*A_{#font[122]{bkg}}}", 1.0, true, {}, {0., 5.}, {-40., 40.}));
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

void configFromParamsList(const std::map<std::string, FitParam> &params, ROOT::Fit::Fitter &fitter, TF3 *func = nullptr)
{
    const auto nrOfParams = params.size();
    double initValues[nrOfParams]; // keep an eye on initialisation

    for (const auto &el : params)
    {
        const auto &param = el.second;

        if (func)
        {
            func->SetParName(param.id(), param.name().c_str());
        }

        initValues[param.id()] = param.initValue();

        fitter.Config().ParSettings(param.id()).SetLimits(param.limitMin(), param.limitMax());

        if (param.isFixed())
        {
            fitter.Config().ParSettings(param.id()).Fix();
            // set also in elementary TF3 (could be improved not to do it twice; just for plotting the results)
            if (func)
            {
                func->FixParameter(param.id(), param.initValue());
            }
        }
    }

    fitter.Config().SetParamsSettings(nrOfParams, initValues);
}

void configFromParamsListGlobal(const std::map<std::string, FitParam> &params, ROOT::Fit::Fitter &fitter, const unsigned int &iParGlobalBegin = 0, const unsigned int &idOffset = 0)
{
    for (const auto &el : params)
    {
        const auto &param = el.second;

        // offset param ID if needed (e.g. for individual parameters, which come after the common ones)
        const auto idParam = param.id() - idOffset;

        fitter.Config().ParSettings(iParGlobalBegin + idParam).SetValue(param.initValue());
        fitter.Config().ParSettings(iParGlobalBegin + idParam).SetLimits(param.limitMin(), param.limitMax());

        if (param.isFixed())
        {
            fitter.Config().ParSettings(iParGlobalBegin + idParam).Fix();
        }
    }
}

std::map<std::string, FitParam> getListOfParams(const std::map<std::string, FitParam> &params, const std::vector<std::string> &keyList)
{
    std::map<std::string, FitParam> newMap{};

    for (const auto &el : keyList)
    {
        newMap[el.c_str()] = params.at(el.c_str());
    }

    return newMap;
}

// /*-------------- end of helpers -------------------*/

/*-------------- define partial fit functions -------------------*/

Double_t funcBkgSigma(Double_t *x, Double_t *par)
{
    return (par[0] + par[1] * exp(-x[1] / par[2])) * pow(x[2], par[3]);
}

Double_t funcBkgAmpl(Double_t *x, Double_t *par)
{
    return par[0] / pow(x[1], par[1]) * exp(x[2] / par[2]);
}

Double_t funcBkg(Double_t *x, Double_t *par)
{
    return par[7] * (1 + par[8] * x[0]) * (1 + par[9] * funcBkgAmpl(x, &par[4]) / (sqrt(HBT::Units::TwoPi) * funcBkgSigma(x, &par[0])) * exp(-0.5 * pow(x[0] / funcBkgSigma(x, &par[0]), 2)));
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
    return funcBkg(x, &par[0]) * funcCoulombUnlike(x, &par[10]);
}

Double_t funcFullLike(Double_t *x, Double_t *par)
{
    return funcBkg(x, &par[0]) * (1 - par[13] + par[13] * funcCoulombLike(x, &par[10]) * funcBec(x, &par[11]));
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