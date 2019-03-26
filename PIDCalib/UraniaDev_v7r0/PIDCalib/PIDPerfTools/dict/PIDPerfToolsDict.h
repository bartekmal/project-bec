#ifndef DICT_PIDPERFTOOLSDICT_H
#define DICT_PIDPERFTOOLSDICT_H 1
// Include files

/** @file PIDPerfToolsDict.h dict/PIDPerfToolsDict.h
 *
 *
 *  @author Andrew POWELL
 *  @date   2010-10-08
 */

#if defined(__clang__) || defined(__CLING__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#elif defined(__GNUC__) && __GNUC__ >= 5
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif

#include "PIDPerfTools/TrackDataSet.h"
//#include "PIDPerfTools/EvtTrackDataSet.h"
//#include "PIDPerfTools/RICHTrackDataSet.h"
//#include "PIDPerfTools/MUONTrackDataSet.h"
//#include "PIDPerfTools/PIDTrackDataSet.h"
//#include "PIDPerfTools/LHCbPIDTrackDataSet.h"
#include "PIDPerfTools/GenericDataSet.h"
#include "PIDPerfTools/DataBinCuts.h"
#include "PIDPerfTools/PerfCalculator.h"
#include "PIDPerfTools/PIDResult.h"
#include "PIDPerfTools/PerfCalculator.h"
#include "PIDPerfTools/MultiPerfCalculator.h"
#include "PIDPerfTools/PIDTable.h"
#include "PIDPerfTools/PIDCrossTable.h"
#include "PIDPerfTools/TrkPIDParams.h"
#include "PIDPerfTools/MultiTrackCalibTool.h"
#include "PIDPerfTools/WeightDataSetTool.h"

namespace
{
  struct _Instantiations
  {
    PerfCalculator<TrackDataSet>                    a;
    //    PerfCalculator<EvtTrackDataSet>                 b;
    //PerfCalculator<RICHTrackDataSet>                c;
    //PerfCalculator<MUONTrackDataSet>                d;
    //PerfCalculator<PIDTrackDataSet>                 e;
    //PerfCalculator<LHCbPIDTrackDataSet>             ee;
    PerfCalculator<GenericDataSet>                  eee;
    std::pair<std::string, RooBinning*>             f;
    std::list<std::pair<std::string, RooBinning*> > g;
    std::vector<RooBinning*>                        h;
    PIDResult::Container                            i;
    std::vector<std::string>                        j;
    WeightDataSetTool<TrackDataSet>                 k;
    //    WeightDataSetTool<EvtTrackDataSet>              l;
    //WeightDataSetTool<RICHTrackDataSet>             m;
    //WeightDataSetTool<MUONTrackDataSet>             n;
    //WeightDataSetTool<PIDTrackDataSet>              o;
    std::pair<std::string, std::string>             r;
    std::vector<std::pair<std::string, std::string> > s;
  };
}

#if defined(__clang__) || defined(__CLING__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) && __GNUC__ >= 5
#pragma GCC diagnostic pop
#endif

#endif // DICT_PIDPERFTOOLSDICT_H
