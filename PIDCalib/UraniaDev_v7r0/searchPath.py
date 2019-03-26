from LbConfiguration.SP2.options import (SearchPath, SearchPathEntry,
                                         EnvSearchPathEntry, NightlyPathEntry,
                                         LHCbDevPathEntry)

path = SearchPath([EnvSearchPathEntry('User_release_area', '/people/plgbmalecki/cmtuser'), EnvSearchPathEntry('CMAKE_PREFIX_PATH', '/cvmfs/lhcb.cern.ch/lib/lhcb/LBSCRIPTS/LBSCRIPTS_v9r2p4/LbRelease/data/DataPkgEnvs:/cvmfs/lhcb.cern.ch/lib/lhcb/LBSCRIPTS/LBSCRIPTS_v9r2p4/LbUtils/cmake'), EnvSearchPathEntry('CMTPROJECTPATH', '/people/plgbmalecki/cmtuser:/cvmfs/lhcb.cern.ch/lib/lhcb:/cvmfs/lhcb.cern.ch/lib/lcg/releases:/cvmfs/lhcb.cern.ch/lib/lcg/app/releases:/cvmfs/lhcb.cern.ch/lib/lcg/external'), EnvSearchPathEntry('LHCBPROJECTPATH', '/cvmfs/lhcb.cern.ch/lib/lhcb:/cvmfs/lhcb.cern.ch/lib/lcg/releases:/cvmfs/lhcb.cern.ch/lib/lcg/app/releases:/cvmfs/lhcb.cern.ch/lib/lcg/external')])
