## Contents of this directory

The scripts in this directory are used to create the PID distribution 
estimation from the PIDCalib control channels. 

* MakeTuples.py - The script to extract ntuples from the PIDCalib datasets.
                  Ntuples are stored to EOS

* PIDPdf.py - The main script which uses Meerkat to create nonparametric 
              estimation of the PID PDF in four dimensions.

* PIDPdf.sh - Auxiliary shell script used to run PIDPdf.py on LSF

* CreatePIDPdf.py - Script to launch PID PDF estimation on the lxplus LSF

* PlotPIDComparison.py - Script to check the quality of the PID PDF estimation 
                         in bins of Pt, Eta, Ntracks

* ComparePDFs.py - Script which runs PlotPIDComparison.py for all year/polarity 
                   combinations

## Preparing new PID PDF

### Adding new config

Start with adding a new configuration in Config.py.  There are two dictionaries that need to be made: 

"samples" is a description of calibration sample (e.g. pion, kaon, muon etc. tracks). 
Example of its configuration: 

```python
  "pi" : { # Name of the dataset, to be later used in "configs" dictionary
    'reco' : 'Reco14_DATA', # Reco version for PIDCalib file path
    'datasets' : {          # Dictionary of datasets
    # Format is: 
    # "Polarity_Year" : ("filename template", number of files in the dataset)
      'MagUp_2012'   : ("DSt_Pi_MagUp_Strip20_%d.root", 72+1),    
      'MagDown_2012' : ("DSt_Pi_MagDown_Strip20_%d.root", 71+1),
      'MagUp_2011'   : ("DSt_Pi_MagUp_Strip20r1_%d.root", 24+1),
      'MagDown_2011' : ("DSt_Pi_MagDown_Strip20r1_%d.root", 35+1),
    },
    'tree'    : "RSDStCalib",
    'pt'      : "Pi_PT",   # Track PT variable
    'eta'     : "Pi_Eta",  # Track ETA variable
    'ntracks' : "nTracks", # Multiplicity variable
  },
```

"configs" describes the actual Particle-variable combination: 

```python
  "pi_V3ProbNNpi" : {        # Name of the PID config
    "sample" : "pi",         # dataset to use
    "var" : "Pi_V3ProbNNpi", # PID variable
    "bins" : 100,            # Number of bins for PID variable
    "transform_forward"  : "1.-(1.-x)**0.3",        # Forward PID variable transform
    "transform_backward" : "(1.-(1.-x)**(1./0.3))", # Bacward PID variable tranform
    'scale_default' : 0.6,   # Default kernel width scale factor
    'scale_syst' : 0.9,      # Kernel width scale factor for "syst" variation
    'scale_pid' : 1.0,       # PID variable kernel width scale factor
    'toystat' : 10000000,    # Number of toys for PDF normalisation
    'controlstat' : 5000000, # Number of events in the control Ntuple
    'nbootstrap' : 10        # Number of bootstrapped samples
  }, 
```

### Preparing calibration Ntuple

This is done by calling 

```bash
  > python MakeTuples.py [PID config]
```
For example, 
```bash
  > python MakeTuples.py pi_MC15TuneV1_ProbNNpi_Brunel
```
  
This will create EOS directory for the new PID config, and produce Ntuples for running kernel estimation 
for all available datasets. 

When creating the new entry in "configs" one should pick up the reasonable gamma correction factor which is 
applied by MakeTuples.py, in such a way that the PID distribution is more or less flat (normally it's peaked at 
either 0, for misID, or at 1, for correct ID). The typical values for "gamma" are -0.2...-0.5 for correct ID hypotheses
and 0.2...0.5 for misID. To check that the chosen valus of gamma is reasonable, run MakeTuples.py in "test" mode: 

```bash
  > python MakeTuples.py [PID config] test MagDown_2016
```

This will run the MakeTuples.pt script on a few input ROOT files rather than on the whole datased. Then check the 
produced ntuple file in ROOT (the address of the output ntuple will be reported by the MakeTuples.py script
at the end of its execution). 

```bash
  > root [ntuple file]
  root [1] pid->Draw("PID","w")
```
  
Check that the transformed PID distribution does not have sharp peaks at 0 or 1 which will be difficult to deal 
for the kernel estimation procedure. 

### Runnung kernel density estimation

```bash
  > python CreatePIDPdf.py [PID config]
```
  
This will submit jobs to LSF queues, Make sure this runs on a machine that can submit to lxplus LSF. The output 
will be placed to EOS. 

### Creating control plots 

```bash
  > python ComparePDFs.py [PID config]
```  
