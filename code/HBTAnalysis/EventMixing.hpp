#ifndef EVENTMIXING_H
#define EVENTMIXING_H

#include <deque>
#include <vector>
#include <memory>

#include <TRandom3.h>

#include "Units.hpp"
#include "HBTParticleClasses.hpp"

namespace HBT {

  //class providing method needed for event-mixing 
  //it is assummed that there is only 1PV in an event (e.g. all particles come from a PV with the same multiplicity)
  class EventMixing {

    //helper functions
    inline int getNrOfBins( const HBT::Units::FloatType xMax,  const HBT::Units::FloatType xMin,  const HBT::Units::FloatType binWidth ){
      return static_cast<int>( floor( ( xMax - xMin ) / binWidth ) ) + 1;
    }

    inline bool getBinIndex( int &binIndex, const HBT::Units::FloatType x, const HBT::Units::FloatType xMin, const HBT::Units::FloatType binWidth, const int nrOfBins ){
      
      binIndex = static_cast<int>( floor( ( x - xMin ) / binWidth ) );

      if ( ( binIndex < 0 ) || ( binIndex >= nrOfBins ) ) return false;
      else return true;

    }
    inline bool getBinIndexMult( int &binIndex, const HBT::Units::FloatType x ){
      return getBinIndex( binIndex, x, m_multiplicityClassMin, m_multiplicityClassWidth, m_multiplicityClassNrOfBins );
    }
    inline bool getBinIndexZPv( int &binIndex, const HBT::Units::FloatType x ){
      return getBinIndex( binIndex, x, m_zPvClassMin, m_zPvClassWidth, m_zPvClassNrOfBins );
    }
    inline int getCurrentMult( const HBT::Particle &particle ){
      return particle.m_chargedParticleMultiplicity;
    }
    inline HBT::Units::FloatType getCurrentZPv( const HBT::Particle &particle ){
      return particle.m_zBestPV;
    }

    //define a container for particles to be mixed (events are divided into classes of multiplicity/zBestPV; positively/negatively charged particles are stored separately )
    typedef std::vector<HBT::Particle> ParticlesVector;
    typedef std::deque<std::array<std::unique_ptr<ParticlesVector>, 2>> EventsForMixingSingleClass;
    typedef std::vector<std::unique_ptr<EventsForMixingSingleClass>> EventsForMixingInZPv;
    typedef std::vector< std::unique_ptr< EventsForMixingInZPv > > EventsForMixingInZPvAndMult;

    private:
      int m_numberOfEventsForMixing;

      //parameters for event classes
      int m_multiplicityClassMin;
      int m_multiplicityClassMax;
      int m_multiplicityClassWidth;
      int m_multiplicityClassNrOfBins;

      HBT::Units::FloatType m_zPvClassMin;
      HBT::Units::FloatType m_zPvClassMax;
      HBT::Units::FloatType m_zPvClassWidth;
      int m_zPvClassNrOfBins;
      
      EventsForMixingInZPvAndMult m_eventsForMixing;

      TRandom3 m_randomNumberGeneratorForEvent;
      TRandom3 m_randomNumberGeneratorForParticle;

      //counters
      unsigned long m_counterErrorType1{0};
      unsigned long m_counterErrorType2{0};
      unsigned long m_counterErrorType3{0};

    public:

      int m_maxExpectedNumberOfSelectedPionsPerEvent;
      int m_maxExpectedNumberOfEventMixedPairsPerParticle;

      //methods
      void configureEventMixing();
      void printEventMixingConfiguration();
      void printEventMixingSummary();

      void updateListOfCandidatesForMixing( const std::vector<HBT::Particle> &particleVector );
      // provide the required type of pairs from event mixing (valid: after selection and not within the resonance range)
      std::vector<HBT::ParticlePair> getValidPairsFromEventMixing(const HBT::Particle &currentParticle, const bool &sameSign, SelectionClass &hbtSelectionObj);
      bool isListOfCandidatesForMixingReady(const std::vector<HBT::Particle> &particleVector);

      EventMixing(){
         configureEventMixing(); 
      }

  };

}

inline void HBT::EventMixing::configureEventMixing(){

    m_numberOfEventsForMixing = 5;

    //parameters for event classes
    m_multiplicityClassMin = 1;
    m_multiplicityClassMax = 200;
    m_multiplicityClassWidth = 3;
    m_multiplicityClassNrOfBins = getNrOfBins( m_multiplicityClassMax, m_multiplicityClassMin, m_multiplicityClassWidth );

    m_zPvClassMin = -200.0;
    m_zPvClassMax = 200.0;
    m_zPvClassWidth = 10.0;
    m_zPvClassNrOfBins = getNrOfBins( m_zPvClassMax, m_zPvClassMin, m_zPvClassWidth );

    m_maxExpectedNumberOfSelectedPionsPerEvent = 150;
    m_maxExpectedNumberOfEventMixedPairsPerParticle = m_numberOfEventsForMixing;
    
    m_randomNumberGeneratorForEvent = TRandom3();
    m_randomNumberGeneratorForParticle = TRandom3();

    //initialize container for event classes
    m_eventsForMixing = EventsForMixingInZPvAndMult( m_multiplicityClassNrOfBins );
    
    for( auto &eventInMultiplicity : m_eventsForMixing ){

      eventInMultiplicity = std::unique_ptr< EventsForMixingInZPv >( new EventsForMixingInZPv( m_zPvClassNrOfBins ) );

      for( auto &eventInZPv : *eventInMultiplicity ){

        eventInZPv = std::unique_ptr< EventsForMixingSingleClass >( new EventsForMixingSingleClass( m_numberOfEventsForMixing ) );

        for( auto &singleEvent : *eventInZPv ){
          // initialise separate containers for particles with positive/negative charge
          singleEvent[0] = std::unique_ptr<ParticlesVector>(new ParticlesVector());
          singleEvent[0]->reserve(m_maxExpectedNumberOfSelectedPionsPerEvent);
          singleEvent[1] = std::unique_ptr<ParticlesVector>(new ParticlesVector());
          singleEvent[1]->reserve(m_maxExpectedNumberOfSelectedPionsPerEvent);
        } 

      }

    } 

}

inline void HBT::EventMixing::printEventMixingConfiguration(){

    printf( "\n--> Event mixing configuration:\n" );

    printf( "\t - multiplcity range         : \t [ %d : %d ] \n", m_multiplicityClassMin, m_multiplicityClassMax );
    printf( "\t - multiplcity bins          : \t [ width : %d -> # bins : %d ] \n", m_multiplicityClassWidth, m_multiplicityClassNrOfBins );
    printf( "\t - zPv range                 : \t [ %3.2f : %3.2f ] \n", m_zPvClassMin, m_zPvClassMax );
    printf( "\t - zPv bins                  : \t [ width : %3.2f -> # bins : %d ] \n", m_zPvClassWidth, m_zPvClassNrOfBins );
    printf( "\t - #events in given class    : \t %d \n", m_numberOfEventsForMixing );

    printf( "<-- End of event mixing configuration\n\n" );  

}

inline void HBT::EventMixing::printEventMixingSummary(){

    printf( "\n--> Event mixing summary:\n" );

    printf( "\t - #errors [type 1] ( region not covered by event classes )         : \t %lu \n", m_counterErrorType1 );
    printf( "\t - #errors [type 2] ( required #events for mixing not available )   : \t %lu \n", m_counterErrorType2 );
    printf( "\t - #errors [type 3] ( no more particles for mixing in the event )   : \t %lu \n", m_counterErrorType3 );
    printf( "\n");
    printf( "\t - #errors [total]                                                  : \t %lu \n", m_counterErrorType1 + m_counterErrorType2 + m_counterErrorType3 );


    printf( "<-- End of event mixing summary\n\n" );  

}

inline void HBT::EventMixing::updateListOfCandidatesForMixing( const std::vector<HBT::Particle> &particleVector ){

  //remove the 'oldest' event from mixing and add a new non-empty event
  if ( !particleVector.empty() ){

    int multIndex = 0;
    int zPvIndex = 0;
    if( !( getBinIndexMult( multIndex,  getCurrentMult( particleVector[0] ) ) && getBinIndexZPv( zPvIndex, getCurrentZPv( particleVector[0] ) ) ) ){
      return;
    } 

    m_eventsForMixing[ multIndex ]->at( zPvIndex )->pop_back();

    // store the positively/negatively charged particles separately
    std::array<std::unique_ptr<ParticlesVector>, 2> arrayForChargedParticlesVectors = {std::unique_ptr<ParticlesVector>(new ParticlesVector()), std::unique_ptr<ParticlesVector>(new ParticlesVector())};
    arrayForChargedParticlesVectors[0]->reserve(m_maxExpectedNumberOfSelectedPionsPerEvent);
    arrayForChargedParticlesVectors[1]->reserve(m_maxExpectedNumberOfSelectedPionsPerEvent);

    for (auto const &particle : particleVector)
    {
      if (particle.m_charge > 0)
        arrayForChargedParticlesVectors[0]->push_back(particle);
      else
        arrayForChargedParticlesVectors[1]->push_back(particle);
    }

    m_eventsForMixing[multIndex]->at(zPvIndex)->push_front(std::move(arrayForChargedParticlesVectors));
  }
}

inline bool HBT::EventMixing::isListOfCandidatesForMixingReady( const std::vector<HBT::Particle> &particleVector ){

  const auto particleFromCurrentEventClass = particleVector[0];

  int multIndex = 0;
  int zPvIndex = 0;
  if( !( getBinIndexMult( multIndex, getCurrentMult( particleFromCurrentEventClass ) ) && getBinIndexZPv( zPvIndex, getCurrentZPv( particleFromCurrentEventClass ) ) ) ){
    m_counterErrorType1 += particleVector.size();
    return false;
  }

  //check if required #events for mixing is already filled
  if (m_eventsForMixing[multIndex]->at(zPvIndex)->back().at(0)->empty() && m_eventsForMixing[multIndex]->at(zPvIndex)->back().at(1)->empty())
  {
    m_counterErrorType2 += particleVector.size();
    return false;
  }
  else
  {
    return true;
  }
}

inline std::vector< HBT::ParticlePair > HBT::EventMixing::getValidPairsFromEventMixing( const HBT::Particle &currentParticle, const bool &sameSign, SelectionClass &hbtSelectionObj ){

    std::vector< HBT::ParticlePair > pairsForEventMixing;
    pairsForEventMixing.reserve( m_maxExpectedNumberOfEventMixedPairsPerParticle );

    int multIndex = 0;
    int zPvIndex = 0;
    if( !( getBinIndexMult( multIndex, getCurrentMult( currentParticle ) ) && getBinIndexZPv( zPvIndex, getCurrentZPv( currentParticle ) ) ) ){
      return pairsForEventMixing;
    } 

    const int signDueToCharge = sameSign ? 1. : -1. ;
    const int requiredChargeIndex = currentParticle.m_charge * signDueToCharge > 0 ? 0 : 1;

    // mix particles with each of the last m_numberOfEventsForMixing events
    for (auto const &currentEvent : *m_eventsForMixing[multIndex]->at(zPvIndex))
    {
      const auto eventSize = currentEvent.at(requiredChargeIndex)->size();
      //check if event is empty to avoid out of range calls
      if (eventSize != 0)
      {
        // try to create a pair that will pass the selection N times
        const unsigned int maxNrOfTries = 2 * eventSize;
        for (unsigned int i = 0; i < maxNrOfTries; ++i)
        {
          const auto particleIndex = m_randomNumberGeneratorForParticle.Integer(eventSize);
          const auto tmpPair = HBT::ParticlePair(currentParticle, currentEvent.at(requiredChargeIndex)->at(particleIndex));

          const auto passesSelection = sameSign ? hbtSelectionObj.passPairSelection(tmpPair) : hbtSelectionObj.passPairSelection(tmpPair) && !hbtSelectionObj.isInResonanceRange(tmpPair);

          if (passesSelection)
          {
            pairsForEventMixing.push_back(std::move(tmpPair));
            break;
          }
          else
          {
            // if that was the last try
            if (i == maxNrOfTries - 1)
              ++m_counterErrorType3;
          }
        }
      }
    }

    return pairsForEventMixing;

}


#endif /* !EVENTMIXING_H */