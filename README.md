# General info
This is a project for analyzing data from the LHCb experiment at the Large Hadron Collider. It focuses on studying so-called Bose-Einstein correlations in proton-lead collisions. The data is stored in a dedicated working group area on the CERN EOS file system.

Results of this analysis are [published](https://doi.org/10.1007/JHEP09(2023)172). Technical documentation is available in this [PhD thesis](https://cds.cern.ch/record/2804165).

## Submodules
* DaVinci for ntuple production (https://gitlab.cern.ch/lhcb/DaVinci/-/tree/phys-hbtCorrelations)

* Alexandria package for LHCb styles management (used in `code/utils/Styles.C`)

## Directories

* `software`: software 'external' to the analysis code (e.g. LHCb packages)
    * `software/DaVinci`: DaVinci used for ntuple production from DST files
* `code`: analysis-specific code
    * `code/HBTAnalysis`: code for producing ROOT histogram files from ntuples
    * `code/scripts`: scripts for the 'final' analysis on the ROOT histogram files (constructing correlation functions, fits, final trends, etc.)
* `analysis`: directory for running the analysis 'studies'
* `external`: input from other analyses (needed for the results discussion)

**'Study' refers to a single analysis 'run' with consistent code (software version, selection, etc.). It consists of many 'jobs', which are run separately for different input files.**

# Prerequisites

* LHCb environment (lb-* scripts / Python3 / ROOT / CVMFS)
* CERN EOS access

**The setup described in [quick start](#quick-start) should work 'out of the box' (currently) only on lxplus. The repository must be somewhere in the user `/afs/cern.ch/work` area. This can be improved/adapted in the setup script `setupBEC.sh`.**

# Quick start

To initialize the repository and set up the environment:
```
git clone --recurse-submodules https://github.com/bartekmal/project-bec.git
cd BEC_pPb
source setupBEC.sh
```
**The `setupBEC.sh` script should be sourced each time you start working with this repository in a new terminal etc.**

To run your first study, first produce the relevant ROOT histogram files from the ntuple files (it can take up to ~24h to complete all jobs in the CERN batch system - you can check the jobs status with `condor_q`):
```
bec-make-study main
bec-make
runHBTJobs()
```

Then, continuing in the same area (and again 'within' `bec-make`):
```
runMergeDownstream()
```
will produce a set of results in the `output` area.

**Your first study should be named as `main` and - in principle - correspond to the central result. Any other created study will by default look for this directory as a reference. In the standard pipeline, current results are compared to this reference to enable quick comparisons (see `output` and the `runDiffs*()` commands).**

# Important 'references'

## `setupBEC.sh`
* defines a set of useful aliases/functions within `bec-*` 'namespace'
* defines the current workspace (see `$BEC_BASE*` variables)

## `code/runHBT.py` (~analysis pipeline)

**This script serves as a `Make-like` file. It is copied to any new study folder while creating a study by `bec-make-study`, which enables local configuration.**

* defines steps in the analysis pipeline for an individual study (see `run*()`)
* tries to emulate `Make-like` behavior by running the full pipeline starting from a given step (see `run*Downstream()`)
* also defines steps that require input from many individual studies (systematics / final 'discussion' / xchecks / selection optimization scans)

**It is rather a working version, but has a lot of configuration information related to this analysis. Please get in touch if you have troubles using it - many of the relevenat studies can be run by this at least semi-automatically.**

## `code/HBTAnalysis/HBTAnalysis.cpp`
* the 'main' file for producing ROOT histogram files from ntuples

