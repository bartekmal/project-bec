#ifndef SELECTIONCLASS_H
#define SELECTIONCLASS_H

#include <cstdio>

//local
#include "Units.hpp"
#include "TreeData.hpp"
#include "HBTParticleClasses.hpp"

class SelectionClass{
    
    private:
        //PV selection
        HBT::Units::FloatType m_zPvMin;
        HBT::Units::FloatType m_zPvMax;
   
        //track selection
        HBT::Units::FloatType m_trackChi2Cut;
        HBT::Units::FloatType m_ipCut;
        HBT::Units::FloatType m_pCut;
        HBT::Units::FloatType m_ptCut;
        HBT::Units::FloatType m_sharedVeloHitsCut;
        HBT::Units::FloatType m_ghostNNCut;

        //PID
        HBT::Units::FloatType m_piNNCut;
        HBT::Units::FloatType m_kNNCut;
        HBT::Units::FloatType m_pNNCut;
        
        //pair selection
        HBT::Units::FloatType m_qCut;
        HBT::Units::FloatType m_openingAngleCut;

        //analysis binning
        std::vector< HBT::Units::FloatType > m_binsOfMultiplicity;
        std::vector< HBT::Units::FloatType > m_binsOfMultiplicityForKt;
        std::vector< HBT::Units::FloatType > m_binsOfKt;
    
    public:
        //methods
        void initializeSelectionCuts();
        void printSelectionCuts();

        bool passEventSelection();
        bool passTrackSelection( const int& part_i  );
        bool passPIDSelection( const int& part_i, const bool& MC );
        bool passPairSelection( const HBT::ParticlePair &pair );
        
        bool isLikePair( const HBT::ParticlePair &pair );
        bool isUnlikePair( const HBT::ParticlePair &pair );
        bool isInResonanceRange( const HBT::ParticlePair &pair );

        //getters
        const std::vector< HBT::Units::FloatType > getBinsOfMultiplicity() { return this->m_binsOfMultiplicity; }
        const std::vector< HBT::Units::FloatType > getBinsOfMultiplicityForKt() { return this->m_binsOfMultiplicityForKt; }
        const std::vector< HBT::Units::FloatType > getBinsOfKt() { return this->m_binsOfKt; }

    SelectionClass(){
        initializeSelectionCuts();
    }
        
};

inline void SelectionClass::initializeSelectionCuts(){

    this->m_zPvMin = -500.;
    this->m_zPvMax = 500.;
   
    this->m_trackChi2Cut = 2.0f;
    this->m_ipCut = 0.4f;
    this->m_pCut = 2.0f;
    this->m_ptCut = 0.1f;
    this->m_sharedVeloHitsCut = 0.999999f;
    this->m_ghostNNCut = 0.25f;

    this->m_piNNCut = 0.65f;
    this->m_kNNCut = 0.5f;
    this->m_pNNCut = 0.5f;

    this->m_qCut = 0.05f;
    this->m_openingAngleCut = 0.0003f; //rad

    this->m_binsOfMultiplicity = { 0, 34, 45, 54, 64, 78, 141 };
    this->m_binsOfMultiplicityForKt = { 0, 45, 64, 141 };
    this->m_binsOfKt = { 0., 0.3, 0.6, 1.2 };
    //this->m_binsOfKt = { 0., 0.25, 0.4, 0.6, 1.2 }; //4 bins version

} 

inline void SelectionClass::printSelectionCuts(){
       
    printf( "\n--> The following selection cuts are used:\n" );

    printf( "\t zPV [mm]\t : \t [ %3.2f : %3.2f ] \n", m_zPvMin, m_zPvMax );

    printf( "\t TrackChi2\t : \t < %3.2f \n", m_trackChi2Cut );
    printf( "\t IP [mm]\t : \t < %3.2f \n", m_ipCut );
    printf( "\t p [GeV]\t : \t > %3.2f \n", m_pCut );
    printf( "\t pT [GeV]\t : \t > %3.2f \n", m_ptCut );
    printf( "\t ShVeloHits [%%]\t : \t < %3.2f \n", m_sharedVeloHitsCut * 100 );
    printf( "\t ghostNN\t : \t < %3.2f \n", m_ghostNNCut );
    printf( "\t piNN\t\t : \t > %3.2f \n", m_piNNCut );
    printf( "\t kNN\t\t : \t < %3.2f \n", m_kNNCut );
    printf( "\t pNN\t\t : \t < %3.2f \n", m_pNNCut );

    printf( "\t Q [GeV]\t : \t > %3.2f \n", m_qCut );
    printf( "\t op. angle [mrad]\t : \t > %3.2f \n", m_openingAngleCut * 1000 );

    printf( "<-- End of selection\n\n" );

} 

inline bool SelectionClass::passEventSelection(){
    auto passStatus = true;

    if ( nrPVs != 1 ) passStatus = false;
    else if ( N == 0 ) passStatus = false;
    else if ( zBestPV[0] < m_zPvMin || zBestPV[0] > m_zPvMax ) return false;

    return passStatus;
}

inline bool SelectionClass::passTrackSelection( const int& part_i ){
    auto passStatus = true;

    if ( charge[part_i] == 0 ) passStatus = false;
    else if ( isMuon[part_i] != 0 ) passStatus = false;
    else if ( eta[part_i] < 2.0 || eta[part_i] > 5.0 ) passStatus = false;

    else if ( trkChi2[part_i] > m_trackChi2Cut ) passStatus = false;
    else if ( ip[part_i] > m_ipCut ) passStatus = false;
    else if ( p[part_i] < m_pCut ) passStatus = false;
    else if ( pt[part_i] < m_ptCut ) passStatus = false;
    else if ( ghostNN[part_i] > m_ghostNNCut ) passStatus = false;
    else if ( shareVHits[part_i]  < -m_sharedVeloHitsCut ) passStatus = false;

    return passStatus;
}

inline bool SelectionClass::passPIDSelection( const int& part_i, const bool& MC ){
    auto passStatus = true;

    if ( !MC ) { 
        if ( pNN[part_i] > m_pNNCut ) passStatus = false;
        else if ( kNN[part_i] > m_kNNCut ) passStatus = false;
        else if ( piNN[part_i] < m_piNNCut ) passStatus = false;
    } else {
        if ( fabs( pNNcor[part_i] ) > m_pNNCut ) passStatus = false;
        else if ( fabs( kNNcor[part_i] ) > m_kNNCut ) passStatus = false;
        else if ( fabs( piNNcor[part_i] ) < m_piNNCut ) passStatus = false;
    }
    return passStatus;
}

inline bool SelectionClass::passPairSelection( const HBT::ParticlePair &pair ){

    auto passStatus = true;

    if ( pair.m_Q_LAB < m_qCut ) passStatus = false;

    if ( fabs( pair.m_chargeParticle1 ) != fabs( pair.m_chargeParticle2 ) ) {
        printf( "\nERROR: Different charges of particles in a pair (should be checked).\n" );
        passStatus = false;
    }

    //opening angle
    if( ( pair.m_slopeDiffX < m_openingAngleCut ) && ( pair.m_slopeDiffY < m_openingAngleCut ) ) passStatus = false;

    return passStatus;

}

inline bool SelectionClass::isLikePair( const HBT::ParticlePair &pair ){

    auto passStatus = true;

    if ( !( ( pair.m_chargeParticle1 == pair.m_chargeParticle2 ) && ( pair.m_pvIndexParticle1 == pair.m_pvIndexParticle2 ) ) ) passStatus = false;

    return passStatus;

}

inline bool SelectionClass::isUnlikePair( const HBT::ParticlePair &pair ){

    auto passStatus = true;

    if ( !( ( pair.m_chargeParticle1 == -1.0 * pair.m_chargeParticle2 ) && ( pair.m_pvIndexParticle1 == pair.m_pvIndexParticle2 ) ) ) passStatus = false;

    return passStatus;

}

inline bool SelectionClass::isInResonanceRange( const HBT::ParticlePair &pair ){

    auto passStatus = false;

    if ( ( fabs( pair.m_invariantMass - HBT::Units::MassRho ) < HBT::Units::WidthRho ) || ( fabs( pair.m_invariantMass - HBT::Units::MassKs ) < HBT::Units::WidthKs ) ) passStatus = true;
    return passStatus;

}





#endif /* !SELECTIONCLASS_H */