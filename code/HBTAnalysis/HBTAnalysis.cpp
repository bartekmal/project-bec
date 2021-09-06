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

int main(int argc, char **argv)
{

  time_t timeAtStart;
  time(&timeAtStart);

  //read input args
  //input file path, data type (RD/MC) and output file name needs to be provided
  //optionally: #events to process (otherwise process all of them)
  if (argc < 4)
  {
    cout << "Input file, data type (RD/MC) and output file name needs to be provided" << endl
         << "optionally: #events to process (otherwise process all of them)" << endl;
    return 1;
  }

  const auto inputFilePath = TString(argv[1]);
  const auto MC = TString(argv[2]).Contains("MC") ? true : false;
  const auto outputFileName = argc > 3 ? TString(argv[3]) : TString("hbtAnalysis.root");
  const unsigned int nrEventsToProcess = argc > 4 ? TString(argv[4]).Atoi() : 0;

  //prepare output file
  auto *outputFile = new TFile(outputFileName, "RECREATE");

  //load in HBT utils
  auto hbtSelection = SelectionClass();
  hbtSelection.printSelectionCuts();
  auto hbtEventMixing = HBT::EventMixing();

  //define histograms
  auto histogramSet100 = HBT::Histograms::CreateHistogramSeries100("10");
  auto histogramSet200 = HBT::Histograms::CreateHistogramSeries200("20");
  auto histogramSet1000 = HBT::Histograms::CreateHistogramSeries1000("10");
  auto histogram2DSet1000 = HBT::Histograms::Create2DHistogramSeries1000("10");
  //auto histogramSet1100 = HBT::Histograms::CreateHistogramSeries1000( "11" );
  auto histogramSet1200NoBins = HBT::Histograms::CreateHistogramSeries1000("12");
  auto histogramSet1200MultiplicityBins = HBT::Histograms::CreateHistogramSeries1000InBins("12", hbtSelection.getBinsOfMultiplicity());

  auto histogramSet2000 = HBT::Histograms::CreateHistogramSeries2000("20", "LIKE pairs");
  auto histogramSet2100 = HBT::Histograms::CreateHistogramSeries2000("21", "UNLIKE pairs");
  auto histogramSet2500 = HBT::Histograms::CreateHistogramSeries2000("25", "EVMIX LIKE pairs");
  auto histogramSet2600 = HBT::Histograms::CreateHistogramSeries2000("26", "EVMIX UNLIKE pairs");

  //no bins
  auto histogramSet3000NoBins = HBT::Histograms::CreateHistogramSeries3000InBins("30", "LIKE pairs");
  auto histogramSet3100NoBins = HBT::Histograms::CreateHistogramSeries3000InBins("31", "UNLIKE pairs");
  auto histogramSet3500NoBins = HBT::Histograms::CreateHistogramSeries3000InBins("35", "EVMIX LIKE pairs");
  auto histogramSet3600NoBins = HBT::Histograms::CreateHistogramSeries3000InBins("36", "EVMIX UNLIKE pairs");

  //multiplicity bins only
  auto histogramSet3000InMultiplicityBins = HBT::Histograms::CreateHistogramSeries3000InBins("30", "LIKE pairs", hbtSelection.getBinsOfMultiplicity());
  auto histogramSet3100InMultiplicityBins = HBT::Histograms::CreateHistogramSeries3000InBins("31", "UNLIKE pairs", hbtSelection.getBinsOfMultiplicity());
  auto histogramSet3500InMultiplicityBins = HBT::Histograms::CreateHistogramSeries3000InBins("35", "EVMIX LIKE pairs", hbtSelection.getBinsOfMultiplicity());
  auto histogramSet3600InMultiplicityBins = HBT::Histograms::CreateHistogramSeries3000InBins("36", "EVMIX UNLIKE pairs", hbtSelection.getBinsOfMultiplicity());

  //multiplicity and kT bins
  // auto histogramSet3000InMultiplicityAndKtBins = HBT::Histograms::CreateHistogramSeries3000InBins("30", "LIKE pairs", hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt());
  // auto histogramSet3100InMultiplicityAndKtBins = HBT::Histograms::CreateHistogramSeries3000InBins("31", "UNLIKE pairs", hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt());
  // auto histogramSet3500InMultiplicityAndKtBins = HBT::Histograms::CreateHistogramSeries3000InBins("35", "EVMIX LIKE pairs", hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt());
  // auto histogramSet3600InMultiplicityAndKtBins = HBT::Histograms::CreateHistogramSeries3000InBins("36", "EVMIX UNLIKE pairs", hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt());

  //setup reading from the ntuple
  auto *inputFile = new TFile(inputFilePath);
  if (!inputFile)
  {
    cout << "Input file not found: " << inputFilePath << endl;
    return 1;
  }
  auto *hbtTuple = (TTree *)gDirectory->Get("HBT");
  if (!hbtTuple)
  {
    cout << "HBT tree not found in: " << inputFilePath << endl;
    return 1;
  }
  setBranches(hbtTuple);
  const unsigned int nrEntries = hbtTuple->GetEntries();
  const unsigned int maxNrEvents = (nrEventsToProcess == 0) ? nrEntries : std::min(nrEntries, nrEventsToProcess);
  cout << "Reading # entries:\t" << maxNrEvents << endl
       << "from:\t" << inputFilePath << endl;

  //loop over events in the input file
  for (unsigned int iEvent = 0; iEvent < maxNrEvents; ++iEvent)
  {
    hbtTuple->GetEntry(iEvent);

    if (!hbtSelection.passEventSelection())
      continue;

    //fill histograms
    // HBT::Histograms::FillHistogramSetWithEventLevelInfo(histogramSet100);
    // HBT::Histograms::FillHistogramSeries200(histogramSet200);

    //prepare particle / pair containers for the event
    std::vector<HBT::Particle> selectedPions;
    selectedPions.reserve(hbtEventMixing.m_maxExpectedNumberOfSelectedPionsPerEvent);

    //loop over particles in an event
    for (int i = 0; i < N; i++)
    {

      //fill histograms
      HBT::Histograms::FillHistogramSetWithParticleLevelInfo(histogramSet1000, i);
      // HBT::Histograms::Fill2DHistogramSeries1000(histogram2DSet1000, i);

      // if (!hbtSelection.passTrackSelection(i))
      //   continue;

      // //HBT::Histograms::FillHistogramSetWithParticleLevelInfo( histogramSet1100, i );

      // if (!hbtSelection.passPIDSelection(i, MC))
      //   continue;

      // HBT::Histograms::FillHistogramSetWithParticleLevelInfo(histogramSet1200NoBins, i);
      // HBT::Histograms::FillHistogramSeries1000InBins(histogramSet1200MultiplicityBins, i, hbtSelection.getBinsOfMultiplicity());

      // selectedPions.emplace_back(i, MC, HBT::Units::PIDPion, HBT::Units::MassPion);
    }

    // //safety check for 'empty' events
    // if (selectedPions.empty())
    //   continue;

    // //check if event-mixing is ready for this event class
    // const auto isEventMixingReady = hbtEventMixing.isListOfCandidatesForMixingReady(selectedPions);

    // //create particle pairs
    // for (auto firstParticleIterator = selectedPions.begin(); firstParticleIterator != selectedPions.end() - 1; firstParticleIterator++)
    // {
    //   for (auto secondParticleIterator = firstParticleIterator + 1; secondParticleIterator != selectedPions.end(); secondParticleIterator++)
    //   {

    //     auto currentPair = HBT::ParticlePair(*firstParticleIterator, *secondParticleIterator);

    //     if (!hbtSelection.passPairSelection(currentPair))
    //       continue;

    //     if (hbtSelection.isLikePair(currentPair))
    //     {
    //       HBT::Histograms::FillHistogramSeries2000(histogramSet2000, currentPair);
    //       HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3000NoBins, currentPair, MC);
    //       HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3000InMultiplicityBins, currentPair, MC, hbtSelection.getBinsOfMultiplicity());
    //       // HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3000InMultiplicityAndKtBins, currentPair, MC, hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt());

    //       // try to get a reference pair (or pairs) for each LIKE one
    //       if (isEventMixingReady)
    //       {
    //         const auto pairsFromEventMixingLike = hbtEventMixing.getValidPairsFromEventMixing(*firstParticleIterator, 1, true, hbtSelection);

    //         for (const auto &pairFromEventMixing : pairsFromEventMixingLike)
    //         {
    //           HBT::Histograms::FillHistogramSeries2000(histogramSet2500, pairFromEventMixing);
    //           HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3500NoBins, pairFromEventMixing, MC);
    //           HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3500InMultiplicityBins, pairFromEventMixing, MC, hbtSelection.getBinsOfMultiplicity());
    //           // HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3500InMultiplicityAndKtBins, pairFromEventMixing, MC, hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt());
    //         }
    //       }
    //     }
    //     else if (hbtSelection.isUnlikePair(currentPair))
    //     {
    //       HBT::Histograms::FillHistogramSeries2000(histogramSet2100, currentPair);
    //       HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3100NoBins, currentPair, MC);
    //       HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3100InMultiplicityBins, currentPair, MC, hbtSelection.getBinsOfMultiplicity());
    //       // HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3100InMultiplicityAndKtBins, currentPair, MC, hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt());

    //       // try to get a reference pair (or pairs) for each UNLIKE one
    //       if (isEventMixingReady)
    //       {
    //         const auto pairsFromEventMixingUnlike = hbtEventMixing.getValidPairsFromEventMixing(*firstParticleIterator, 1, false, hbtSelection);

    //         for (const auto &pairFromEventMixing : pairsFromEventMixingUnlike)
    //         {
    //           HBT::Histograms::FillHistogramSeries2000(histogramSet2600, pairFromEventMixing);
    //           HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3600NoBins, pairFromEventMixing, MC);
    //           HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3600InMultiplicityBins, pairFromEventMixing, MC, hbtSelection.getBinsOfMultiplicity());
    //           // HBT::Histograms::FillHistogramSeries3000InBins(histogramSet3600InMultiplicityAndKtBins, pairFromEventMixing, MC, hbtSelection.getBinsOfMultiplicityForKt(), hbtSelection.getBinsOfKt());
    //         }
    //       }
    //     }
    //   }
    // }

    // hbtEventMixing.updateListOfCandidatesForMixing(selectedPions);
  }

  //summary
  time_t timeAtEnd;
  time(&timeAtEnd);
  auto executionTimeInMinutes = difftime(timeAtEnd, timeAtStart) / 60.;

  hbtEventMixing.printEventMixingConfiguration();
  hbtEventMixing.printEventMixingSummary();

  // summary on #pairs
  {
    const auto idOfHistogramToUseForSummary = 4;

    HBT::Units::PairsCounterType nrOfPairsLikeTotal = histogramSet3000NoBins[0][0][idOfHistogramToUseForSummary].GetEntries();
    HBT::Units::PairsCounterType nrOfPairsUnlikeTotal = histogramSet3100NoBins[0][0][idOfHistogramToUseForSummary].GetEntries();
    HBT::Units::PairsCounterType nrOfPairsEvmixLikeTotal = histogramSet3500NoBins[0][0][idOfHistogramToUseForSummary].GetEntries();
    HBT::Units::PairsCounterType nrOfPairsEvmixUnlikeTotal = histogramSet3600NoBins[0][0][idOfHistogramToUseForSummary].GetEntries();

    // for each mult bin
    for (unsigned int i = 0; i < histogramSet3000InMultiplicityBins.size(); ++i)
    {
      HBT::Units::PairsCounterType nrOfPairsLikeInBin = histogramSet3000InMultiplicityBins[i][0][idOfHistogramToUseForSummary].GetEntries();
      HBT::Units::PairsCounterType nrOfPairsUnlikeInBin = histogramSet3100InMultiplicityBins[i][0][idOfHistogramToUseForSummary].GetEntries();
      HBT::Units::PairsCounterType nrOfPairsEvmixLikeInBin = histogramSet3500InMultiplicityBins[i][0][idOfHistogramToUseForSummary].GetEntries();
      HBT::Units::PairsCounterType nrOfPairsEvmixUnlikeInBin = histogramSet3600InMultiplicityBins[i][0][idOfHistogramToUseForSummary].GetEntries();

      cout << "Total number and fraction of pairs of the given type in mult bin nr: " << i + 1 << endl;
      printPairsSummaryEntryType1("LIKE", nrOfPairsLikeInBin, nrOfPairsLikeTotal);
      printPairsSummaryEntryType1("UNLIKE", nrOfPairsUnlikeInBin, nrOfPairsUnlikeTotal);
      printPairsSummaryEntryType1("EVMIX LIKE", nrOfPairsEvmixLikeInBin, nrOfPairsEvmixLikeTotal);
      printPairsSummaryEntryType1("EVMIX UNLIKE", nrOfPairsEvmixUnlikeInBin, nrOfPairsEvmixUnlikeTotal);

      printPairsSummaryEntryType2("LIKE / UNLIKE", nrOfPairsLikeInBin, nrOfPairsUnlikeInBin);
      printPairsSummaryEntryType2("LIKE / UNLIKE (EVMIX)", nrOfPairsEvmixLikeInBin, nrOfPairsEvmixUnlikeInBin);
      printPairsSummaryEntryType2("LIKE / EVMIX LIKE", nrOfPairsLikeInBin, nrOfPairsEvmixLikeInBin);
      printPairsSummaryEntryType2("UNLIKE / EVMIX UNLIKE", nrOfPairsUnlikeInBin, nrOfPairsEvmixUnlikeInBin);
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
