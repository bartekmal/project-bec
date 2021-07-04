#ifndef COULOMBCORRECTION_H
#define COULOMBCORRECTION_H

#include <TMath.h>

#include "Units.hpp"

namespace HBT
{

  namespace Coulomb
  {

    // assumed source radius used for Coulomb correction
    const HBT::Units::FloatType REffForPions = 0.5;

    inline HBT::Units::FloatType Gamov(const HBT::Units::FloatType &Q, const bool &sameSign, const HBT::Units::FloatType &particleMass)
    {

      const int sign = sameSign ? 1 : -1;

      const HBT::Units::FloatType tmpEtaTwoPi = HBT::Units::TwoPi * particleMass * HBT::Units::FineStructureConstant / Q;

      return sign * tmpEtaTwoPi / (exp(sign * tmpEtaTwoPi) - 1);
    }

    inline HBT::Units::FloatType BS(const HBT::Units::FloatType &Q, const bool &sameSign, const HBT::Units::FloatType &assumedR, const HBT::Units::FloatType &particleMass)
    {

      const HBT::Units::FloatType tmpDzeta = particleMass * HBT::Units::FineStructureConstant / Q;

      // following the parametrisation used by CMS
      const HBT::Units::FloatType weight = (1 + HBT::Units::Pi * tmpDzeta * Q * assumedR / (1.26 + Q * assumedR));

      return HBT::Coulomb::Gamov(Q, sameSign, particleMass) * weight;
    }
  } // namespace Coulomb
} // namespace HBT

#endif /* !COULOMBCORRECTION_H */