# PIDGen package

## Introduction

PIDGen is the package to correct the PID response for MC signal samples 
based on PID calibration data (the same data as used by PIDCalib package).
It implements two alternative approaches:

  * PID resampling, where the PID response is completely replaced 
    by the one randomly generated from calibration PDFs. 

  * Transformation of PID variables, where the PID variables from the 
    simulation are transformed in such a way that they are distributed 
    as in data. (This is only available for Run1 for the moment). 

Like in PIDCalib, the PID correction is a function of track kinematics 
(momentum and pseudorapidity) and event multiplicity (number of tracks).
Unlike PIDCalib, the correction is done with an unbinned approach, where 
the calibration PDFs in four dimensions (PID, Pt, eta, Ntracsk) are described 
by a kernel density estimation procedure using the Meerkat library. 

The corrected PID response from both the PID resampling and PID variable 
transformation can be used as an input to a multivariate classifier.

The PID resampling approach has an important limitation that the PID 
variables for the same track are generated independently, and thus no 
correlations between them are reproduced. Therefore, only one PID variable 
per track can be used in the selection (correlations between variables for 
different tracks are, naturally, preserved via correlations with kinematics
of tracks). 

The approach with PID variable transformation aims to remove this limitation. 
In that case, the corrected PID variables preserve correlations with 
the output of simulation, and through the correlations in simulation the 
correlations between PID variables for the same track are reproduced 
"in the first order". This is not perfect by probably acceptable in most 
cases. The drawback of this approach is that it also relies on the 
parametrisation of PID PDFs in simulation (which are extracted from samples
that are typically much smaller than calibration data). 

More details about the approach can be found here: 
  https://indico.cern.ch/event/572934/contributions/2318786/attachments/1357558/2052812/pidcorr.pdf

## Contents of the package

  * PIDGen.py - Python script for PID resampling.
  * PIDCorr.py - Python script for PID variable transformation.
  * Examples/Lb2Lcpi/Lb2Lcpi_pidgen.py - Example PID resampling script for Lb->Lcpi MC sample
  * Examples/Lb2Lcpi/Lb2Lcpi_pidcorr.py - Example script for PID variable transformation for Lb->Lcpi MC sample

Note that the scripts use data stored in CERN EOS, and assume that it's 
accessible at root://eoslhcb.cern.ch/ server. The location can be modified
in expert/runN/Config.py and expert/runN/ConfigMC.py files to allow local 
installation ouside CERN. 

## Creating PID PDF from calibration samples

See separate README files in the PIDGenExpert directory

## Using PIDGen with the MC samples

Generation of PID response is performed by one of the two scripts, PIDGen.py or PIDCorr.py
The scripts require many parameters (MC data location, variable names, location of the calibrated PID response etc.). 
The sample python scripts are provided, 
Examples/Lb2Lcpi/Lb2Lcpi_{pidgen, pidcorr}, which the user can modify to 
actually run the PID generation for their own MC sample.

### Running PID resampling (PIDGen)

To run PIDGen resampling, you need to setup Urania nightly, copy the example 
script to a directory with write access (the output ntuple will be created there) 
and run it:
```bash
$ lb-run --nightly-cvmfs --nightly lhcb-prerelease Urania/master bash
$ cp $PIDPERFSCRIPTSROOT/scripts/python/PIDGenUser/Examples/Lb2Lcpi/Lb2Lcpi_pidgen.py {your_place}
$ cd {your_place}
$ python Lb2Lcpi_pidgen.py
```

The complete list of PID configurations available in Run 1 and Run 2 can be obtained by running PIDGen.py without arguments:
```bash
$ python $PIDPERFSCRIPTSROOT/scripts/python/PIDGenUser/PIDGen.py
```

Note that Run 1 and Run 2 PID configurations have different names!

### Running PID variable transformation (PIDCorr)

To run PIDCorr variable transformation, you need to setup Urania nightly, 
copy the example script to a directory with write access 
(the output ntuple will be created there) and run it:
```bash
$ lb-run --nightly-cvmfs --nightly lhcb-prerelease Urania/master bash
$ cp $PIDPERFSCRIPTSROOT/scripts/python/PIDGenUser/Examples/Lb2Lcpi/Lb2Lcpi_pidcorr.py {your_place}
$ cd {your_place}
$ python Lb2Lcpi_pidcorr.py
```

The result of PIDCorr is dependent on the version of MC used to generate your signal! 
Only **Sim08 Run 1** PID configurations are available for the moment. 
The complete list of PID configurations available can be obtained 
by running PIDCorr.py without arguments:

```bash
$ python $PIDPERFSCRIPTSROOT/scripts/python/PIDGenUser/PIDGen.py
```
