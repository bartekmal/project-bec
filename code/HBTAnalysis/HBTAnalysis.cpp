//-------------------------------------------------------
// Analysis of the HBT correlations
// Author: Marcin Kucharczyk
// Date: 18-04-2013
//-------------------------------------------------------
// Code adapted for pPb collisions
// Author: Bartosz Malecki
// Date: 04-10-2019
// Mail: bartosz.piotr.malecki@cern.ch
//-------------------------------------------------------

#include <time.h>
#include "HBTAnalysis.hpp"

int main( int argc, char** argv ) {

  time_t timeAtStart;
  time( &timeAtStart );

  //read input args
  //input file path, data type (RD/MC) and output file name needs to be provided
  //optionally: #events to process (otherwise process all of them)
  if ( argc<4 ){
    cout << "Input file, data type (RD/MC) and output file name needs to be provided" << endl
	 << "optionally: #events to process (otherwise process all of them)" << endl;
    return 1;
  } 
  
  const auto inputFilePath =  TString( argv[1] );
  const auto MC =  TString( argv[2] ).Contains("MC") ? true : false;
  const auto outputFileName =  argc>3 ? TString( argv[3] ) : TString( "hbtAnalysis.root" );
  const unsigned int nrEventsToProcess = argc>4 ? TString( argv[4] ).Atoi() : 0;
 
  //prepare output file
  auto* outputFile = new TFile( outputFileName,"RECREATE" );

  //load in HBT utils
  auto hbtSelection = SelectionClass();
  hbtSelection.printSelectionCuts();
  auto hbtEventMixing = HBT::EventMixing();

  //define histograms
  auto histogramSet100 = HBT::Histograms::CreateHistogramSeries100( "10" );
  auto histogramSet1000 = HBT::Histograms::CreateHistogramSeries1000( "10" );
  //auto histogramSet1100 = HBT::Histograms::CreateHistogramSeries1000( "11" );
  auto histogramSet1200 = HBT::Histograms::CreateHistogramSeries1000( "12" );

  auto histogramSet2000 = HBT::Histograms::CreateHistogramSeries2000( "20", "LIKE pairs" );
  //auto histogramSet2100 = HBT::Histograms::CreateHistogramSeries2000( "21", "UNLIKE pairs" );
  auto histogramSet2200 = HBT::Histograms::CreateHistogramSeries2000( "22", "UNLIKE pairs - no resonances" );
  auto histogramSet2500 = HBT::Histograms::CreateHistogramSeries2000( "25", "EVMIX LIKE pairs" );
  auto histogramSet2600 = HBT::Histograms::CreateHistogramSeries2000( "26", "EVMIX UNLIKE pairs - no resonances" );

  //no bins
  auto histogramSet3000NoBins = HBT::Histograms::CreateHistogramSeries3000InBins( "30", "LIKE pairs" );
  //auto histogramSet3100NoBins = HBT::Histograms::CreateHistogramSeries3000InBins( "31", "UNLIKE pairs" );
  auto histogramSet3200NoBins = HBT::Histograms::CreateHistogramSeries3000InBins( "32", "UNLIKE pairs - no resonances" );
  auto histogramSet3500NoBins = HBT::Histograms::CreateHistogramSeries3000InBins( "35", "EVMIX LIKE pairs" );
  auto histogramSet3600NoBins = HBT::Histograms::CreateHistogramSeries3000InBins( "36", "EVMIX UNLIKE pairs - no resonances" );

  //multiplicity bins only
  auto histogramSet3000InMultiplicityBins = HBT::Histograms::CreateHistogramSeries3000InBins( "30", "LIKE pairs", hbtSelection.getBinsOfMultiplicity() );
  //auto histogramSet3100InMultiplicityBins = HBT::Histograms::CreateHistogramSeries3000InBins( "31", "UNLIKE pairs", hbtSelection.getBinsOfMultiplicity() );
  auto histogramSet3200InMultiplicityBins = HBT::Histograms::CreateHistogramSeries3000InBins( "32", "UNLIKE pairs - no resonances", hbtSelection.getBinsOfMultiplicity() );
  auto histogramSet3500InMultiplicityBins = HBT::Histograms::CreateHistogramSeries3000InBins( "35", "EVMIX LIKE pairs", hbtSelection.getBinsOfMultiplicity() );
  auto histogramSet3600InMultiplicityBins = HBT::Histograms::CreateHistogramSeries3000InBins( "36", "EVMIX UNLIKE pairs - no resonances", hbtSelection.getBinsOfMultiplicity() );

  //multiplicity and kT bins
  auto histogramSet3000InMultiplicityAndKtBins = HBT::Histograms::CreateHistogramSeries3000InBins( "30", "LIKE pairs", hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );
  //auto histogramSet3100InMultiplicityAndKtBins = HBT::Histograms::CreateHistogramSeries3000InBins( "31", "UNLIKE pairs", hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );
  auto histogramSet3200InMultiplicityAndKtBins = HBT::Histograms::CreateHistogramSeries3000InBins( "32", "UNLIKE pairs - no resonances", hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );
  auto histogramSet3500InMultiplicityAndKtBins = HBT::Histograms::CreateHistogramSeries3000InBins( "35", "EVMIX LIKE pairs", hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );
  auto histogramSet3600InMultiplicityAndKtBins = HBT::Histograms::CreateHistogramSeries3000InBins( "36", "EVMIX UNLIKE pairs - no resonances", hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );

  //setup reading from the ntuple
  auto* inputFile = new TFile( inputFilePath );
  if ( !inputFile ){
    cout << "Input file not found: " << inputFilePath << endl;
    return 1;
  }
  auto* hbtTuple = (TTree*) gDirectory->Get( "HBT" );
  if ( !hbtTuple ){
    cout << "HBT tree not found in: " << inputFilePath << endl;
    return 1;
  }
  setBranches( hbtTuple );
  const unsigned int nrEntries = hbtTuple->GetEntries();
  const unsigned int maxNrEvents = ( nrEventsToProcess == 0 ) ? nrEntries : std::min( nrEntries, nrEventsToProcess );
  cout << "Reading # entries:\t" << maxNrEvents << endl
       << "from:\t" << inputFilePath << endl;
  
  //loop over events in the input file
  for ( unsigned int iEvent=0; iEvent<maxNrEvents; ++iEvent ){
    hbtTuple->GetEntry( iEvent );
    
    if ( !hbtSelection.passEventSelection() ) continue;

    //fill histograms
    HBT::Histograms::FillHistogramSetWithEventLevelInfo( histogramSet100 );

    //prepare particle / pair containers for the event
    std::vector<HBT::Particle> selectedPions;
    selectedPions.reserve( hbtEventMixing.m_maxExpectedNumberOfSelectedPionsPerEvent );

    //loop over particles in an event
    for(int i = 0; i < N; i++) {

      //fill histograms
      HBT::Histograms::FillHistogramSetWithParticleLevelInfo( histogramSet1000, i );

      if ( !hbtSelection.passTrackSelection( i ) ) continue;
      
      //HBT::Histograms::FillHistogramSetWithParticleLevelInfo( histogramSet1100, i );

      if ( !hbtSelection.passPIDSelection( i, MC ) ) continue;      

      HBT::Histograms::FillHistogramSetWithParticleLevelInfo( histogramSet1200, i );

      selectedPions.emplace_back( i, MC, HBT::Units::PIDPion, HBT::Units::MassPion );

    }
    
    //safety check for 'empty' events
    if ( selectedPions.empty() ) continue;

    //check if event-mixing is ready for this event class
    const auto isEventMixingReady = hbtEventMixing.isListOfCandidatesForMixingReady( selectedPions );

    //create particle pairs
    for ( auto firstParticleIterator = selectedPions.begin(); firstParticleIterator != selectedPions.end() - 1; firstParticleIterator++ ){
      for ( auto secondParticleIterator = firstParticleIterator + 1; secondParticleIterator != selectedPions.end(); secondParticleIterator++ ){
        
        auto currentPair = HBT::ParticlePair( *firstParticleIterator, *secondParticleIterator );
        
        if ( !hbtSelection.passPairSelection( currentPair ) ) continue;

        if ( hbtSelection.isLikePair( currentPair ) ) {
          HBT::Histograms::FillHistogramSeries2000( histogramSet2000, currentPair );
          HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3000NoBins, currentPair, true, MC );
          HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3000InMultiplicityBins, currentPair, true, MC, hbtSelection.getBinsOfMultiplicity() );
          HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3000InMultiplicityAndKtBins, currentPair, true, MC, hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );
        }
        else if ( hbtSelection.isUnlikePair( currentPair ) ) {

          // HBT::Histograms::FillHistogramSeries2000( histogramSet2100, currentPair );
          // HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3100NoBins, currentPair, false, MC );
          // HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3100InMultiplicityBins, currentPair, false, MC, hbtSelection.getBinsOfMultiplicity() );
          // HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3100InMultiplicityAndKtBins, currentPair, false, MC, hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );
         
          // if ( !hbtSelection.isInResonanceRangeInQ( currentPair ) ) {
            
            HBT::Histograms::FillHistogramSeries2000( histogramSet2200, currentPair );
            HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3200NoBins, currentPair, false, MC );
            HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3200InMultiplicityBins, currentPair, false, MC, hbtSelection.getBinsOfMultiplicity() );
            HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3200InMultiplicityAndKtBins, currentPair, false, MC, hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );
          // }

        }
        
      }
      
      //event-mixing
      if ( isEventMixingReady ){

        // EVMIX LIKE pairs
        const auto pairsFromEventMixingLike = hbtEventMixing.getValidPairsFromEventMixing(*firstParticleIterator, true, hbtSelection);

        for ( const auto &pairFromEventMixing : pairsFromEventMixingLike ){

          HBT::Histograms::FillHistogramSeries2000( histogramSet2500, pairFromEventMixing );
          HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3500NoBins, pairFromEventMixing, true, MC );        
          HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3500InMultiplicityBins, pairFromEventMixing, true, MC, hbtSelection.getBinsOfMultiplicity() );
          HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3500InMultiplicityAndKtBins, pairFromEventMixing, true, MC, hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );

        }

        // EVMIX UNLIKE pairs
        const auto pairsFromEventMixingUnlike = hbtEventMixing.getValidPairsFromEventMixing(*firstParticleIterator, false, hbtSelection);

        for ( const auto &pairFromEventMixing : pairsFromEventMixingUnlike ){

            HBT::Histograms::FillHistogramSeries2000( histogramSet2600, pairFromEventMixing );
            HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3600NoBins, pairFromEventMixing, false, MC );
            HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3600InMultiplicityBins, pairFromEventMixing, false, MC, hbtSelection.getBinsOfMultiplicity() );
            HBT::Histograms::FillHistogramSeries3000InBins( histogramSet3600InMultiplicityAndKtBins, pairFromEventMixing, false, MC, hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt() );
        }
      }
    }

    hbtEventMixing.updateListOfCandidatesForMixing( selectedPions );

  }

  //summary
  time_t timeAtEnd;
  time( &timeAtEnd );
  auto executionTimeInMinutes = difftime(timeAtEnd, timeAtStart) / 60.;

  hbtEventMixing.printEventMixingConfiguration();
  hbtEventMixing.printEventMixingSummary();

  // summary on #pairs
  {
    const unsigned int nrOfMultBins = histogramSet3000InMultiplicityBins.size();

    for (unsigned int i = 0; i < nrOfMultBins; ++i)
    {
      const unsigned long totalNumberOfPairsLike = histogramSet3000InMultiplicityBins[i][0][4].GetEntries();
      const unsigned long totalNumberOfPairsUnlike = histogramSet3200InMultiplicityBins[i][0][4].GetEntries();
      const unsigned long totalNumberOfPairsEvmixLike = histogramSet3500InMultiplicityBins[i][0][4].GetEntries();
      const unsigned long totalNumberOfPairsEvmixUnlike = histogramSet3600InMultiplicityBins[i][0][4].GetEntries();

      cout << "Total number of pairs of the given type in mult bin nr: " << i + 1 << endl;
      cout << "\t LIKE \t\t\t" << totalNumberOfPairsLike << endl;
      cout << "\t UNLIKE \t\t" << totalNumberOfPairsUnlike << endl;
      cout << "\t LIKE / UNLIKE \t\t" << (double)totalNumberOfPairsLike / totalNumberOfPairsUnlike << endl;
      cout << "\t EVMIX LIKE \t\t" << totalNumberOfPairsEvmixLike << endl;
      cout << "\t EVMIX UNLIKE \t\t" << totalNumberOfPairsEvmixUnlike << endl;
      cout << "\t LIKE / UNLIKE (EVMIX)\t" << (double)totalNumberOfPairsEvmixLike / totalNumberOfPairsEvmixUnlike << endl;
      cout << "\t LIKE / EVMIX LIKE\t" << (double)totalNumberOfPairsLike / totalNumberOfPairsEvmixLike << endl;
      cout << "\t UNLIKE / EVMIX UNLIKE\t" << (double)totalNumberOfPairsUnlike / totalNumberOfPairsEvmixUnlike << endl;

      cout << endl;
    }
  }

  cout << "Histograms saved to: " << outputFile->GetName() << endl; 
  cout << "Execution time [min]: " << executionTimeInMinutes << endl;

  //save & cleanup
  outputFile->Write();
  inputFile->Close();
  outputFile->Close();
  delete inputFile;
  delete outputFile;

  return 0;
}
