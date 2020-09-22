#ifndef UNITS_H
#define UNITS_H

#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>

namespace HBT {

  namespace Units{

    //typedefs
    typedef double FloatType;
    typedef TH1D TH1FloatType;
    typedef TH2D TH2FloatType;
    typedef float FloatTypeForNNCorrectedValues;
    typedef unsigned long PairsCounterType;

    //define consts (maths)
    const FloatType Pi = TMath::Pi();
    const FloatType TwoPi = 2*TMath::Pi();
    const FloatType RadToDeg = 180.0 / TMath::Pi();

    //define consts (physics)
    const FloatType MassPion = 0.140;
    const FloatType MassKaon = 0.494;
    const FloatType MassProton = 0.938;
    const FloatType MassRho = 0.770;
    const FloatType MassKs = 0.497;

    const FloatType WidthRho = 0.150;
    const FloatType WidthKs = 0.020;

    const int PIDPion = 211;
    const int PIDKaon = 321;
    const int PIDProton = 2212;

    const FloatType FineStructureConstant = 1.0 / 137.0;

  }

}

#endif /* !UNITS_H */
