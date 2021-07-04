#ifndef HBTANALYSIS_H
#define HBTANALYSIS_H

#include <TFile.h>
#include <TNtuple.h>
#include <TRandom3.h>

#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <time.h>

//local
#include "Units.hpp"
#include "TreeData.hpp"
#include "SelectionClass.hpp"
#include "Histograms.hpp"
#include "HBTParticleClasses.hpp"
#include "EventMixing.hpp"

//using namespace std;
using std::cout;
using std::endl;

inline void printPairsSummaryEntryType1(const std::string &text, const HBT::Units::PairsCounterType &nrOfPairsInBin, const HBT::Units::PairsCounterType &nrOfPairsTotal)
{
    printf("\t %-30s \t %12lu \t (%5.2f %%) \n", text.c_str(), nrOfPairsInBin, (double)nrOfPairsInBin / nrOfPairsTotal * 100.);
}

inline void printPairsSummaryEntryType2(const std::string &text, const HBT::Units::PairsCounterType &nrOfPairsMain, const HBT::Units::PairsCounterType &nrOfPairsRef)
{
    printf("\t %-30s \t %12.2f \n", text.c_str(), (double)nrOfPairsMain / nrOfPairsRef);
}

#endif /* !HBTANALYSIS_H */
