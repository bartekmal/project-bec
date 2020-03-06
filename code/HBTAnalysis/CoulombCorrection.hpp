#ifndef COULOMBCORRECTION_H
#define COULOMBCORRECTION_H

#include <TMath.h>

#include "Units.hpp"

namespace HBT {

  namespace Coulomb {

    inline HBT::Units::FloatType gamovFactorForPionPair( const HBT::Units::FloatType &Q, const bool &sameSign ){
        
        const int sign = sameSign ? 1 : -1;

        const HBT::Units::FloatType tmpEtaTwoPi = HBT::Units::TwoPi * HBT::Units::MassPion * HBT::Units::FineStructureConstant / Q;

        return sign * tmpEtaTwoPi / ( exp( sign * tmpEtaTwoPi ) - 1 );
    }
    
  }

}

#endif /* !COULOMBCORRECTION_H */