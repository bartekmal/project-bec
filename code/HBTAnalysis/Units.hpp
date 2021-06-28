#ifndef UNITS_H
#define UNITS_H

#include <unordered_map>
#include <limits>

#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>

namespace HBT
{

  namespace Units
  {

    //typedefs
    typedef double FloatType;
    typedef TH1D TH1FloatType;
    typedef TH2D TH2FloatType;
    typedef float FloatTypeForNNCorrectedValues;
    typedef unsigned long PairsCounterType;

    //define consts (maths)
    const FloatType Pi = TMath::Pi();
    const FloatType TwoPi = 2 * TMath::Pi();
    const FloatType RadToDeg = 180.0 / TMath::Pi();
    const float Infinity = std::numeric_limits<float>::max();

    //define consts (physics)
    const FloatType MassPion = 0.140;
    const FloatType MassKaon = 0.494;
    const FloatType MassProton = 0.938;

    const int PIDPion = 211;
    const int PIDKaon = 321;
    const int PIDProton = 2212;

    const FloatType FineStructureConstant = 1.0 / 137.0;

    // resonances info
    struct Resonance
    {

    private:
      const FloatType mass = 0.;
      const FloatType width = 0.;
      const FloatType qRangeMin = 0.;
      const FloatType qRangeMax = 0.;

    public:
      Resonance(const FloatType &iMass, const FloatType &iWidth, const FloatType &qMin, const FloatType &qMax) : mass(iMass), width(iWidth), qRangeMin(qMin), qRangeMax(qMax){};

      FloatType getQRangeMin() const { return this->qRangeMin; }
      FloatType getQRangeMax() const { return this->qRangeMax; }
    };

    // widths are related to the resonance width or the detector resolution; ranges in Q are set to match the edges of the Q bins (avoid 'half-empty' bins)
    std::unordered_map<std::string, Resonance> Resonances{
        {"rho0(770)", Resonance(0.770, 0.150, 0.55, 0.88)},
        {"K0s", Resonance(0.497, 0.020, 0.38, 0.44)},
        {"f0(980)", Resonance(0.980, 0.0, 0.91, 0.97)},
        {"f2(1270)", Resonance(1.270, 0.0, 1.21, 1.27)}};

  } // namespace Units

} // namespace HBT

#endif /* !UNITS_H */
