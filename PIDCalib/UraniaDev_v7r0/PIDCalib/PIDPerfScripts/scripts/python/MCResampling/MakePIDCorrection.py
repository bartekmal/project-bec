#################################################################################
### MakePIDCorrection
### Author: Marco Tresch
### Email:  marco.tresch@cern.ch
### Date:   January 23, 2015
#################################################################################

from PIDPerfScripts.StartScreen import *

import ROOT
from ROOT import *

import array
import argparse
import sys
import os.path
import numpy as np

class ShowArgumentsParserMCreweight(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n\n' %message)
        parser.print_usage(sys.stderr)
        sys.stderr.write('\n'+self.description)
        sys.exit(2)


import time

if '__main__' == __name__:
    start()
    ROOT.gROOT.SetBatch(True)
    print ""
    parser = ShowArgumentsParserMCreweight(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        prog=os.path.basename(sys.argv[0]),
        description=("""Make PID correction for a given:
        a) MC file <mcFile> ("Complete path to the file")
        b) Path to the TTree inside the MC file <mcFilePathToTree>
        c) Connects the particle type to the corresponding folder
        d) Sets the particle to correct for
        e) PID variable to correct for each particle
        f) Sets the path to the library file from MakePIDdistributionRunRange (\"K:lab1/K\",\"Pi:lab2/Pi\")

        For a full list of arguments, do: 'python {0} -h' and
        for more detailed information look at the documentation file
        """).format(os.path.basename(sys.argv[0]))
        )
    #---------------------------------------------------------------------------
    # add the positional arguments
    #---------------------------------------------------------------------------
    parser.add_argument('mcFile', metavar='<mcFile>',
                        help="Sets the MC file to correct the PID variables")

    parser.add_argument('mcFilePathToTree',metavar='<mcFilePathToTree>',
                        help="Sets the internal path to the TTree of the MC file")

    parser.add_argument('partName',metavar='<partName>',
                        help="Sets the particle type to load")

    parser.add_argument('particles', metavar='<particles>',
                        help="Sets the particle type to correct")

    parser.add_argument('pidVars', metavar='<pidVars>',
                        help="Sets the PID variables to store")
    parser.add_argument('pidLibrary', metavar='<pidLibrary>',
                        help="Sets the library file with the PID distributions")

    binGroup = parser.add_argument_group("binning options")
    binGroup.add_argument("-X", "--xVarName", dest="xVarName", metavar="NAME", default="P", help="Sets the NAME of the 1st (x) bin variable "
                          "(default: %(default)s)")
    binGroup.add_argument("-Y", "--yVarName", dest="yVarName", metavar="NAME", default="ETA", help="Sets the NAME of the 2nd (y) bin variable "
                          "(default: %(default)s). "
                          "If 1D binning is required, then this option should "
                          "be set to an empty string")
    binGroup.add_argument("-Z", "--zVarName", dest="zVarName", metavar="NAME", default="nTracks", help="Sets the NAME of the 3rd (z) bin variable "
                          "(default: %(default)s). "
                          "If 2D binning is required, then this option should "
                          "be set to an empty string")

    #---------------------------------------------------------------------------
    addGroup = parser.add_argument_group("further options")
    addGroup.add_argument("-id", "--trueID", dest="usetrueID", action="store_true",
                          default=False,
                          help="Use the TRUE_ID information of the corrected particles")
                          
    addGroup.add_argument("-corr", "--withCorrelation", dest="correlation", action="store_true", default=False, help="takes into account the correlation between pidvariables! It is slower.")

    addGroup.add_argument("-s", "--selection", dest="selection",default="",
                          help="selection to apply on input tree before resampling (use input tree leaf names)")

    addGroup.add_argument("-n", "--nickname", dest="nickname",default="",
                          help="nickname to add to output ROOT file")

    addGroup.add_argument("-nE", "--runEvents", dest="runEvents",default=None,
			  help="number of Events to run over")

    addGroup.add_argument("-d", "--debug", dest="debug",action="store_true",default=False,
			  help="Print out debug information")

    addGroup.add_argument('-fD','--dataFile', dest='dataFile',default=None,
                        help="Sets the data file to get reasonable multiplicity distribution (ntracks)")

    addGroup.add_argument('-fDP','--dataFilePathToTree',dest='dataFilePathToTree',default=None,
                        help="Sets the internal path to the TTree of the data file")

    addGroup.add_argument('-dWeight','--dataWithWeight',dest='withWeight',action="store_true",default=False,help="uses 'weight' as weighting factor for data")

    opts = parser.parse_args()


    from Utils import TTree


    #---------------------------------------------------------------------------
    # Dictionaries for resampling
    #---------------------------------------------------------------------------
    #PID variale names matching for data file
    pid_dict_data = {'PIDK':'PIDK','PIDp':'PIDp','PIDmu':'PIDmu','PIDpi':'PIDpi','PIDpK':'PIDpK','PIDe':'PIDe','ProbNNK':'ProbNNk','ProbNNp':'ProbNNp','ProbNNmu':'ProbNNmu','ProbNNpi':'ProbNNpi','ProbNNe':'ProbNNe','V4ProbNNK':'ProbNNk','V4ProbNNp':'ProbNNp','V4ProbNNmu':'ProbNNmu','V4ProbNNpi':'ProbNNpi','V4ProbNNe':'ProbNNe','V3ProbNNK':'ProbNNk','V3ProbNNp':'ProbNNp','V3ProbNNmu':'ProbNNmu','V3ProbNNpi':'ProbNNpi','V3ProbNNe':'ProbNNe','V2ProbNNK':'ProbNNk','V2ProbNNp':'ProbNNp','V2ProbNNmu':'ProbNNmu','V2ProbNNpi':'ProbNNpi','V2ProbNNe':'ProbNNe'}

    #TrueID dictionary
    ID_dict = {11:'e',-11:'e',321:'K',-321:'K',211:'Pi',-211:'Pi',13:'Muplus',-13:'Muminus',2212:'P',-2212:'P'}
    
    #---------------------------------------------------------------------------
    # start time measurement
    #---------------------------------------------------------------------------
    start = time.time()
    print time.asctime(time.localtime())

    #---------------------------------------------------------------------------
    # load files for correcting
    #---------------------------------------------------------------------------
    print "Loading files"
    # load the MC file and corresponding TTree to correct
    fileToCorrect = ROOT.TFile.Open(opts.mcFile)
    treeToCorrect_temp = fileToCorrect.Get(opts.mcFilePathToTree)

    # create new file
    outname = opts.mcFile.replace(".root","_PID-corrected_new.root")
    if opts.nickname != "":
        outname = outname.replace("_PID-corrected_new.root","_PID-corrected_new_"+opts.nickname+".root")
    fileN = ROOT.TFile.Open(outname,"RECREATE")
    fileN.cd()

    if opts.selection != "":
        print "Apply following selection on input tree:"
        print opts.selection
        treeToCorrect = treeToCorrect_temp.CopyTree(opts.selection)
    else:
        print "No selection is applied"
        treeToCorrect = treeToCorrect_temp

    # load the data file and corresponding TTree to correct
    if(opts.dataFile!=None and opts.dataFilePathToTree!=None):
        fileData = ROOT.TFile.Open(opts.dataFile)
        tree_data = fileData.Get(opts.dataFilePathToTree)

    # load the pid library file
    fileLibrary = ROOT.TFile.Open(opts.pidLibrary)

    #---------------------------------------------------------------------------
    # setup all the input arguments
    #---------------------------------------------------------------------------
    print "Setup input arguments"
    # set the particles to correct
    YourPart = opts.partName

    if YourPart.startswith("["):
        if not YourPart.endswith("]"):
            parser.error("Invalid Particles string %s" %YourPart)
        YourPart = YourPart[1:-1].split(',')
    elif YourPart.startswith("("):
        if not YourPart.endswith(")"):
            parser.error("Invalid Particles string %s" %YourPart)
        YourPart = YourPart[1:-1].split(',')
    else:
        YourPart = (YourPart,)
    if len(YourPart)>0:
        YourPart_dict={}

    for ypt in YourPart:
        tmps=ypt.split(':')
        YourPart_dict[tmps[0]] = tmps[1]

    # set pid variables to store
    pidVars = opts.pidVars

    if pidVars.startswith("["):
        if not pidVars.endswith("]"):
            parser.error("Invalid pid variables string %s" %pidVars)
        pidVars = pidVars[1:-1].split(',')
    elif pidVars.startswith("("):
        if not pidVars.endswith(")"):
            parser.error("Invalid pid variables string %s" %pidVars)
        pidVars = pidVars[1:-1].split(',')
    else:
        pidVars = (pidVars,)

    # set particles to correct
    particles = opts.particles

    if particles.startswith("["):
        if not particles.endswith("]"):
            parser.error("Invalid pid variables string %s" %pidVars)
        particles = particles[1:-1].split(',')
    elif particles.startswith("("):
        if not particles.endswith(")"):
            parser.error("Invalid pid variables string %s" %pidVars)
        particles = particles[1:-1].split(',')
    else:
        particles = (particles,)

    XVarName = opts.xVarName
    if XVarName=='':
        parser.error("Argument to --xBinVarName is an empty string.")

    YVarName = opts.yVarName
    ZVarName = opts.zVarName

    if ZVarName!='' and YVarName=='':
        parser.error("Argument to --yVarName is an empty string,"
                     "but argument to --zVarName. If you planned to"
                     "include only two binning variables. Did you"
                     "mean to do --yVarName='{0}' --zVarName='{1}'?".format(
            ZVarName, YVarName))

    debug = opts.debug
    usetrueID = opts.usetrueID
    runEvents = opts.runEvents
    dataFile = opts.dataFile
    dataFilePathToTree = opts.dataFilePathToTree
    withWeight = opts.withWeight
    correlation = opts.correlation

    #---------------------------------------------------------------------------
    # Print out conditions
    #---------------------------------------------------------------------------

    print "------------------------------------------------------------------"
    print "MC TTree " + opts.mcFilePathToTree + " in file " + opts.mcFile
    if(opts.dataFile!=None and opts.dataFilePathToTree!=None):
        print "Data TTree " + opts.dataFilePathToTree + " in file " + opts.dataFile
    else:
        print "using ntracks from MC file"
    print "using trueID", usetrueID
    print "using debug mode", debug
    print "correcting Particles", particles
    print "PID variables to correct", pidVars
    print "maping particles to folders", YourPart_dict
    print "------------------------------------------------------------------"


    #---------------------------------------------------------------------------
    # load the resampling libraries
    #---------------------------------------------------------------------------
    print "Load resampling libraries"
    part_hists_dict = {}  #contains the 1D histogramms for (P,ETA,nTracks) per p
    particleprojdict = {} #contains all PID projections per (p,PID)
    for particle,folder in YourPart_dict.iteritems():
        hists = []
        #this is the datafile
        hists.append(fileLibrary.Get(folder).Get("histo_PID"))
        #these are the binning scheme
        if(XVarName!=''):
            hists.append(fileLibrary.Get(folder).Get("H1D_"+XVarName))
        if(YVarName!=''):
            hists.append(fileLibrary.Get(folder).Get("H1D_"+YVarName))
        if(ZVarName!=''):
            hists.append(fileLibrary.Get(folder).Get("H1D_"+ZVarName))
        part_hists_dict[particle] = hists

        listOfAxes = hists[0].GetListOfAxes()

        #match vairables to correct to axis indices
        pidvars_to_axes = {}
        j = 0
        for axis in listOfAxes :
            for pidvar in pidVars :
                if pidvar == axis.GetName().replace("DLL","PID") :
                    pidvars_to_axes[pidvar] = j
            j+=1

        if(debug):
            print "list of histograms per particle", particle, hists
            print "--------------------------------"
            print "map of pidvars to axes indices", pidvars_to_axes
            print "********************************"
        #-----------------------------------------------------------------------
        #load the projections
        #-----------------------------------------------------------------------
        pidprojdict = {}
        totalhist = part_hists_dict[particle][0]
        if(XVarName!='' and YVarName!='' and ZVarName!=''):
            nummombins = totalhist.GetAxis(0).GetNbins()
            numetabins = totalhist.GetAxis(1).GetNbins()
            numntrbins = totalhist.GetAxis(2).GetNbins()
            if(correlation):
                for i in range(0,nummombins+2) :
                    for j in range(0,numetabins+2) :
                        for k in range(0,numntrbins+2) :
                            hist_projPIDsName = "histo_PIDhisto_PID_proj_Pbin%d_ETAbin%d_nTracksbin%d" % (i,j,k)
                            hist_projPIDs = fileLibrary.Get(folder).Get(hist_projPIDsName)
                            index = "%d%d%d" % (i,j,k)
                            for pidvar in pidVars:
                                upValue = hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-3).GetXmax()
                                # if(upValue==1.0):
                                #     # hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-3).SetRangeUser(0.0,upValue)
                                # else:
                                if(upValue!=1.0):
                                    hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-3).SetRangeUser(-upValue,upValue)
                            pidprojdict[index] = hist_projPIDs
            else:
                for pidvar in pidVars :
                    projdict = {}
                    for i in range(0,nummombins+2) :
                        for j in range(0,numetabins+2) :
                            for k in range(0,numntrbins+2) :
                                hist_projPIDsName = "histo_PIDhisto_PID_proj_Pbin%d_ETAbin%d_nTracksbin%d" % (i,j,k)
                                hist_projPIDs = fileLibrary.Get(folder).Get(hist_projPIDsName)
                                index = "%d%d%d" % (i,j,k)
                                upValue = hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-3).GetXmax()
                                # if(upValue==1.0):
                                #     # hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-3).SetRangeUser(0.0,upValue)
                                # else:
                                if(upValue!=1.0):
                                    hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-3).SetRangeUser(-upValue,upValue)
                                projhist = hist_projPIDs.Projection( pidvars_to_axes[pidvar]-3 )
                                projhist.SetName( particle+"_"+pidvar+"_"+str(index) )
                                if(debug):
                                    print hist_projPIDsName
                                    print projhist
                                projdict[index] = projhist
                    pidprojdict[pidvar] = projdict

        if(XVarName!='' and YVarName!='' and ZVarName==''):
            nummombins = totalhist.GetAxis(0).GetNbins()
            numetabins = totalhist.GetAxis(1).GetNbins()
            if(correlation):
                for i in range(0,nummombins+2) :
                    for j in range(0,numetabins+2) :
                        hist_projPIDsName = "histo_PIDhisto_PID_proj_Pbin%d_ETAbin%d" % (i,j)
                        hist_projPIDs = fileLibrary.Get(folder).Get(hist_projPIDsName)
                        index = "%d%d" % (i,j)
                        for pidvar in pidVars:
                            upValue = hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-2).GetXmax()
                            if(upValue!=1.0):
                            #     hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-2).SetRangeUser(0.0,upValue)
                            # else:
                                hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-2).SetRangeUser(-upValue,upValue)
                        pidprojdict[index] = hist_projPIDs
            else:
                for pidvar in pidVars :
                    projdict = {}
                    for i in range(0,nummombins+2) :
                        for j in range(0,numetabins+2) :
                            hist_projPIDsName = "histo_PIDhisto_PID_proj_Pbin%d_ETAbin%d" % (i,j)
                            hist_projPIDs = fileLibrary.Get(folder).Get(hist_projPIDsName)
                            index = "%d%d" % (i,j)
                            upValue = hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-2).GetXmax()
                            if(upValue!=1.0):
                            #     hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-2).SetRangeUser(0.0,upValue)
                            # else:
                                hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-2).SetRangeUser(-upValue,upValue)
                            projhist = hist_projPIDs.Projection( pidvars_to_axes[pidvar]-2 )
                            projhist.SetName( particle+"_"+pidvar+"_"+str(index) )
                            if(debug):
                                print hist_projPIDsName
                                print projhist
                            projdict[index] = projhist
                    pidprojdict[pidvar] = projdict

        if(XVarName!='' and YVarName=='' and ZVarName==''):
            nummombins = totalhist.GetAxis(0).GetNbins()
            if(correlation):
                for i in range(0,nummombins+2) :
                        hist_projPIDsName = "histo_PIDhisto_PID_proj_Pbin%d" % (i)
                        hist_projPIDs = fileLibrary.Get(folder).Get(hist_projPIDsName)
                        index = "%d" % (i)
                        for pidvar in pidVars:
                            upValue = hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-1).GetXmax()
                            if(upValue!=1.0):
                            #     hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-1).SetRangeUser(0.0,upValue)
                            # else:
                                hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-1).SetRangeUser(-upValue,upValue)
                        pidprojdict[index] = hist_projPIDs
                        
            else:
                for pidvar in pidVars :
                    projdict = {}
                    for i in range(0,nummombins+2) :
                            hist_projPIDsName = "histo_PIDhisto_PID_proj_Pbin%d" % (i)
                            hist_projPIDs = fileLibrary.Get(folder).Get(hist_projPIDsName)
                            index = "%d" % (i)
                            upValue = hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-1).GetXmax()
                            if(upValue!=1.0):
                            #     hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-1).SetRangeUser(0.0,upValue)
                            # else:
                                hist_projPIDs.GetAxis(pidvars_to_axes[pidvar]-1).SetRangeUser(-upValue,upValue)
                            projhist = hist_projPIDs.Projection( pidvars_to_axes[pidvar]-1 )
                            projhist.SetName( particle+"_"+pidvar+"_"+str(index) )
                            if(debug):
                                print hist_projPIDsName
                                print projhist
                            projdict[index] = projhist
                    pidprojdict[pidvar] = projdict
        particleprojdict[particle] = pidprojdict
        
    #---------------------------------------------------------------------------
    # Load necessary branches and disable the others
    #---------------------------------------------------------------------------
    print "Load necessary branches and disable the others"
    #speed for iterating
    treeToCorrect.SetBranchStatus("*",0)

    mom_dict = {}
    momz_dict={}
    eta_dict = {}
    trueID_dict = {}
    old_pid = {}
    #Eta calcualtion if necessary
    calculateEta = False
    if(XVarName=='P' and YVarName=='ETA' and ZVarName=='nTracks'):
        #sets the branch addresses for each particle
        for p in particles:
            mom_branchName = p+"_P"
            mom_type = treeToCorrect.GetBranch(mom_branchName)
            mom_array_type = mom_type.GetTitle().replace(mom_branchName+"/","").lower()
            mom =  array.array(mom_array_type,[0])
            treeToCorrect.SetBranchStatus(mom_branchName, 1 )
            treeToCorrect.SetBranchAddress(mom_branchName, mom )
            mom_dict[p] = mom
            #del mom
            #eta calculation not standard
            b = treeToCorrect.GetBranch( p +"_"+"ETA")
            if b :
                eta_branchName = p+"_ETA"
                eta_type = treeToCorrect.GetBranch(eta_branchName)
                eta_array_type = eta_type.GetTitle().replace(eta_branchName+"/","").lower()
                eta =  array.array(eta_array_type,[0])
                treeToCorrect.SetBranchStatus(eta_branchName, 1 )
                treeToCorrect.SetBranchAddress(eta_branchName, eta )
                eta_dict[p] = eta
                #del eta
            else:
                b = treeToCorrect.GetBranch( p +"_"+"LOKI_ETA")
                if b :
                    eta_branchName = p+"_LOKI_ETA"
                    eta_type = treeToCorrect.GetBranch(eta_branchName)
                    eta_array_type = eta_type.GetTitle().replace(eta_branchName+"/","").lower()
                    eta =  array.array(eta_array_type,[0])
                    treeToCorrect.SetBranchStatus(eta_branchName, 1 )
                    treeToCorrect.SetBranchAddress(eta_branchName, eta )
                    eta_dict[p] = eta
                    #del eta
                else :
                    calculateEta = True
                    momz_branchName = p+"_PZ"
                    momz_type = treeToCorrect.GetBranch(momz_branchName)
                    momz_array_type = momz_type.GetTitle().replace(momz_branchName+"/","").lower()
                    momz =  array.array(momz_array_type,[0])
                    treeToCorrect.SetBranchStatus(momz_branchName, 1 )
                    treeToCorrect.SetBranchAddress(momz_branchName, momz )
                    momz_dict[p] = momz
                #del momz
            if usetrueID:
                trueID_branchName = p+"_TRUEID"
                trueID_type = treeToCorrect.GetBranch(trueID_branchName)
                trueID_array_type = trueID_type.GetTitle().replace(trueID_branchName+"/","").lower()
                trueID =  array.array(trueID_array_type,[0])
                treeToCorrect.SetBranchStatus(trueID_branchName, 1)
                treeToCorrect.SetBranchAddress(trueID_branchName, trueID)
                trueID_dict[p] = trueID
                #del trueID
            old_pid_var = {}
            for t_pid in pidVars:
                tmp_branchName = p+"_"+pid_dict_data[t_pid]
                tmp_type = treeToCorrect.GetBranch(tmp_branchName)
                tmp_array_type = tmp_type.GetTitle().replace(tmp_branchName+"/","").lower()
                tmp =  array.array(tmp_array_type,[0])
                treeToCorrect.SetBranchStatus(tmp_branchName,1)
                treeToCorrect.SetBranchAddress(tmp_branchName,tmp)
                old_pid_var[t_pid] = tmp
                #del tmp
            old_pid[p]=old_pid_var

        #if no dataFile attached, use the ntracks from MC
        if(opts.dataFile==None and opts.dataFilePathToTree==None):
            ntracks_branchName ="nTracks"
            ntracks_type = treeToCorrect.GetBranch(ntracks_branchName)
            ntracks_array_type = ntracks_type.GetTitle().replace(ntracks_branchName+"/","").lower()
            ntracks_tree =  array.array(ntracks_array_type,[0])
            treeToCorrect.SetBranchStatus(ntracks_branchName, 1 )
            treeToCorrect.SetBranchAddress(ntracks_branchName, ntracks_tree )

    if(XVarName=='P' and YVarName=='ETA' and ZVarName==''):
        #sets the branch addresses for each particle
        for p in particles:
            mom_branchName = p+"_P"
            mom_type = treeToCorrect.GetBranch(mom_branchName)
            mom_array_type = mom_type.GetTitle().replace(mom_branchName+"/","").lower()
            mom =  array.array(mom_array_type,[0])
            treeToCorrect.SetBranchStatus(mom_branchName, 1 )
            treeToCorrect.SetBranchAddress(mom_branchName, mom )
            mom_dict[p] = mom
            #del mom
            #eta calculation not standard
            b = treeToCorrect.GetBranch( p +"_"+"ETA")
            if b :
                eta_branchName = p+"_ETA"
                eta_type = treeToCorrect.GetBranch(eta_branchName)
                eta_array_type = eta_type.GetTitle().replace(eta_branchName+"/","").lower()
                eta =  array.array(eta_array_type,[0])
                treeToCorrect.SetBranchStatus(eta_branchName, 1 )
                treeToCorrect.SetBranchAddress(eta_branchName, eta )
                eta_dict[p] = eta
                #del eta
            else:
                b = treeToCorrect.GetBranch( p +"_"+"LOKI_ETA")
                if b :
                    eta_branchName = p+"_LOKI_ETA"
                    eta_type = treeToCorrect.GetBranch(eta_branchName)
                    eta_array_type = eta_type.GetTitle().replace(eta_branchName+"/","").lower()
                    eta =  array.array(eta_array_type,[0])
                    treeToCorrect.SetBranchStatus(eta_branchName, 1 )
                    treeToCorrect.SetBranchAddress(eta_branchName, eta )
                    eta_dict[p] = eta
                    #del eta
                else :
                    calculateEta = True
                    momz_branchName = p+"_PZ"
                    momz_type = treeToCorrect.GetBranch(momz_branchName)
                    momz_array_type = momz_type.GetTitle().replace(momz_branchName+"/","").lower()
                    momz =  array.array(momz_array_type,[0])
                    treeToCorrect.SetBranchStatus(momz_branchName, 1 )
                    treeToCorrect.SetBranchAddress(momz_branchName, momz )
                    momz_dict[p] = momz
                #del momz
            if usetrueID:
                trueID_branchName = p+"_TRUEID"
                trueID_type = treeToCorrect.GetBranch(trueID_branchName)
                trueID_array_type = trueID_type.GetTitle().replace(trueID_branchName+"/","").lower()
                trueID =  array.array(trueID_array_type,[0])
                treeToCorrect.SetBranchStatus(trueID_branchName, 1)
                treeToCorrect.SetBranchAddress(trueID_branchName, trueID)
                trueID_dict[p] = trueID
                #del trueID
            old_pid_var = {}
            for t_pid in pidVars:
                tmp_branchName = p+"_"+pid_dict_data[t_pid]
                tmp_type = treeToCorrect.GetBranch(tmp_branchName)
                tmp_array_type = tmp_type.GetTitle().replace(tmp_branchName+"/","").lower()
                tmp =  array.array(tmp_array_type,[0])
                treeToCorrect.SetBranchStatus(tmp_branchName,1)
                treeToCorrect.SetBranchAddress(tmp_branchName,tmp)
                old_pid_var[t_pid] = tmp
                #del tmp
            old_pid[p]=old_pid_var
    if(XVarName=='P' and YVarName=='' and ZVarName==''):
        #sets the branch addresses for each particle
        for p in particles:
            mom_branchName = p+"_P"
            mom_type = treeToCorrect.GetBranch(mom_branchName)
            mom_array_type = mom_type.GetTitle().replace(mom_branchName+"/","").lower()
            mom =  array.array(mom_array_type,[0])
            treeToCorrect.SetBranchStatus(mom_branchName, 1 )
            treeToCorrect.SetBranchAddress(mom_branchName, mom )
            mom_dict[p] = mom
            #del mom
            #eta calculation not standard
            if usetrueID:
                trueID_branchName = p+"_TRUEID"
                trueID_type = treeToCorrect.GetBranch(trueID_branchName)
                trueID_array_type = trueID_type.GetTitle().replace(trueID_branchName+"/","").lower()
                trueID =  array.array(trueID_array_type,[0])
                treeToCorrect.SetBranchStatus(trueID_branchName, 1)
                treeToCorrect.SetBranchAddress(trueID_branchName, trueID)
                trueID_dict[p] = trueID
                #del trueID
            old_pid_var = {}
            for t_pid in pidVars:
                tmp_branchName = p+"_"+pid_dict_data[t_pid]
                tmp_type = treeToCorrect.GetBranch(tmp_branchName)
                tmp_array_type = tmp_type.GetTitle().replace(tmp_branchName+"/","").lower()
                tmp =  array.array(tmp_array_type,[0])
                treeToCorrect.SetBranchStatus(tmp_branchName,1)
                treeToCorrect.SetBranchAddress(tmp_branchName,tmp)
                old_pid_var[t_pid] = tmp
                #del tmp
            old_pid[p]=old_pid_var


    #---------------------------------------------------------------------------
    # Start to run over events of MC file
    #---------------------------------------------------------------------------
    print "Start loop over MC events"
    res = {}
    #Set maximum of events to run over
    if(runEvents==None):
      nEntries = treeToCorrect.GetEntries()
      saveEntries = -1
    else:
      nEntries = int(runEvents)
      saveEntries = nEntries

    #if dataFile attached, load ntracks and ask for weight.
    if(XVarName=='P' and YVarName=='ETA' and ZVarName=='nTracks'):
        if(dataFile!=None and dataFilePathToTree!=None):
            hist_ntracks = ROOT.TH1D("hist_ntracks","",10000,0,2000)
            string_weight = ""
            if(withWeight):
                string_weight = "weight"
            tree_data.Draw("nTracks >> hist_ntracks",string_weight)

    random = ROOT.TRandom3()
    
    #will be set according to binning variable
    nBinningVariable = 3
    #start loop
    for iE in xrange(nEntries):
        tmpResults_dict = {}
        treeToCorrect.GetEntry(iE)
        if(iE%10000)==0:
            print iE
        diff = time.time() - start
        if(debug):
            print diff
        if(XVarName=='P' and YVarName=='ETA' and ZVarName=='nTracks'):
            #if dataFile attached get nTracks from loaded histogram, otherwise use nTracks from MC
            if(dataFile!=None and dataFilePathToTree!=None):
                nTracks = hist_ntracks.GetRandom();
            else:
                nTracks = ntracks_tree[0]

        #iterate over particles
        for particle in particles:

            tmpParticleResults_dict = {} #dictionary of values for each PID branch
            if(XVarName=='P' and YVarName=='ETA' and ZVarName=='nTracks'):
                # load PID projections for given particle
                pid_available = True
                if(usetrueID):
                    if(trueID_dict[particle][0] in ID_dict.keys()):
                        true_particle = ID_dict[trueID_dict[particle][0]]
                        if ((true_particle in particleprojdict.keys()) and (true_particle in part_hists_dict.keys())):
                            pidprojection_particle = particleprojdict[true_particle]
                            pid_histos = part_hists_dict[true_particle]
                            #find correct bins
                            if(calculateEta):
                                eta = 0.5*ROOT.TMath.Log((1+momz_dict[particle][0]/mom_dict[particle][0])/(1-momz_dict[particle][0]/mom_dict[particle][0]));
                                bin_ETA = pid_histos[2].FindFixBin(eta)
                            else:
                                bin_ETA = pid_histos[2].FindFixBin(eta_dict[particle][0])
                
                            bin_P = pid_histos[1].FindFixBin(mom_dict[particle][0])
                            bin_nTrack = pid_histos[3].FindFixBin(nTracks)
                            pid_histos_index = "%d%d%d" % (bin_P,bin_ETA,bin_nTrack)
                        else:
                            pid_available = False
                            if(debug):
                                print str(true_particle) + " no histograms available for this particle!"
                    else:
                        pid_available = False
                        if(debug):
                            print str(trueID_dict[particle][0]) + " not in the ID dictionary!"
                else:
                    pidprojection_particle = particleprojdict[particle]
                    pid_histos = part_hists_dict[particle]
                    #find correct bins
                    if(calculateEta):
                        eta = 0.5*ROOT.TMath.Log((1+momz_dict[particle][0]/mom_dict[particle][0])/(1-momz_dict[particle][0]/mom_dict[particle][0]));
                        bin_ETA = pid_histos[2].FindFixBin(eta)
                    else:
                        bin_ETA = pid_histos[2].FindFixBin(eta_dict[particle][0])
                            
                    bin_P = pid_histos[1].FindFixBin(mom_dict[particle][0])
                    bin_nTrack = pid_histos[3].FindFixBin(nTracks)
                    pid_histos_index = "%d%d%d" % (bin_P,bin_ETA,bin_nTrack)

            if(XVarName=='P' and YVarName=='ETA' and ZVarName==''):
                nBinningVariable = 2
                # load PID projections for given particle
                pid_available = True
                if(usetrueID):
                    if(trueID_dict[particle][0] in ID_dict.keys()):
                        true_particle = ID_dict[trueID_dict[particle][0]]
                        if ((true_particle in particleprojdict.keys()) and (true_particle in part_hists_dict.keys())):
                            pidprojection_particle = particleprojdict[true_particle]
                            pid_histos = part_hists_dict[true_particle]
                            #find correct bins
                            if(calculateEta):
                                eta = 0.5*ROOT.TMath.Log((1+momz_dict[particle][0]/mom_dict[particle][0])/(1-momz_dict[particle][0]/mom_dict[particle][0]));
                                bin_ETA = pid_histos[2].FindFixBin(eta)
                            else:
                                bin_ETA = pid_histos[2].FindFixBin(eta_dict[particle][0])
                            
                            bin_P = pid_histos[1].FindFixBin(mom_dict[particle][0])
                            pid_histos_index = "%d%d" % (bin_P,bin_ETA)
                        else:
                            pid_available = False
                            if(debug):
                                print str(true_particle) + " no histograms available for this particle!"
                    else:
                        pid_available = False
                        if(debug):
                            print str(trueID_dict[particle][0]) + " not in the ID dictionary!"
                else:
                    pidprojection_particle = particleprojdict[particle]
                    pid_histos = part_hists_dict[particle]
                    #find correct bins
                    if(calculateEta):
                        eta = 0.5*ROOT.TMath.Log((1+momz_dict[particle][0]/mom_dict[particle][0])/(1-momz_dict[particle][0]/mom_dict[particle][0]));
                        bin_ETA = pid_histos[2].FindFixBin(eta)
                    else:
                        bin_ETA = pid_histos[2].FindFixBin(eta_dict[particle][0])
                    
                    bin_P = pid_histos[1].FindFixBin(mom_dict[particle][0])
                    pid_histos_index = "%d%d" % (bin_P,bin_ETA)

            if(XVarName=='P' and YVarName=='' and ZVarName==''):
                nBinningVariable = 1
                # load PID projections for given particle
                pid_available = True
                if(usetrueID):
                    if(trueID_dict[particle][0] in ID_dict.keys()):
                        true_particle = ID_dict[trueID_dict[particle][0]]
                        if ((true_particle in particleprojdict.keys()) and (true_particle in part_hists_dict.keys())):
                            pidprojection_particle = particleprojdict[true_particle]
                            pid_histos = part_hists_dict[true_particle]
                            bin_P = pid_histos[1].FindFixBin(mom_dict[particle][0])
                            pid_histos_index = "%d" % (bin_P)
                        else:
                            pid_available = False
                            if(debug):
                                print str(true_particle) + " no histograms available for this particle!"
                    else:
                        pid_available = False
                        if(debug):
                            print str(trueID_dict[particle][0]) + " not in the ID dictionary!"
                else:
                    pidprojection_particle = particleprojdict[particle]
                    pid_histos = part_hists_dict[particle]
                    #find correct bins
                    bin_P = pid_histos[1].FindFixBin(mom_dict[particle][0])
                    pid_histos_index = "%d" % (bin_P)


            old_pidvars = old_pid[particle]

            #iterate over PID variables requested
            if not correlation:
                for pidvar in pidVars :
                    pidval = old_pidvars[pidvar][0]
                    if(pid_available):
                        pid_projection = pidprojection_particle[pidvar]
                        projhisto = pid_projection[pid_histos_index]
                        nBins = projhisto.GetNbinsX()
                        projhisto.StatOverflows(ROOT.kFALSE)
                        entries = projhisto.GetEffectiveEntries()
                        if(entries==0):
                            inte = 0
                        else:
                            inte = projhisto.ComputeIntegral()
                        if(debug):
                            print "number of bins in histogram", nBins, "integral", inte, "with", entries, "entries"
                        if(inte>1E-5):
                            pidval = projhisto.GetRandom()
                            if(pidval<0 and ("ProbNN" in pidvar)):
                                pidval = -1000
                    
                    tmpParticleResults_dict[pidvar] = pidval
                tmpResults_dict[particle] = tmpParticleResults_dict
            else:
                inte = 0
                pidval = np.array([-1000 for i in xrange(len(pidVars))], dtype=np.double)
                if(pid_available):
                    projhisto = pidprojection_particle[pid_histos_index]
                    for pidvar in pidVars:
                        upValue = projhisto.GetAxis(pidvars_to_axes[pidvar]-3).GetXmax()
                        if(upValue!=1.0):
                            projhisto.GetAxis(pidvars_to_axes[pidvar]-3).SetRangeUser(-upValue,upValue)
                    nBins = projhisto.GetNbins()
                    entries = projhisto.GetEntries()
                    if(entries!=0 and nBins!=0):
                        inte = projhisto.ComputeIntegral()
                    if(debug):
                        print "number of bins in histogram", nBins, "integral", inte, "with", entries, "entries"
                    if(inte>1E-5):
                        projhisto.GetRandom(pidval)
                for pidvar in pidVars:
                    pidval0 = old_pidvars[pidvar][0]
                    if(pid_available and inte>1E-5):
                        axes_number = pidvars_to_axes[pidvar]-nBinningVariable
                        pidval0 = pidval[axes_number]
                        if(pidval0<0  and ("ProbNN" in pidvar)):
                            pidval0=-1000
                    tmpParticleResults_dict[pidvar] = pidval0
                tmpResults_dict[particle] = tmpParticleResults_dict
                 
                    

        #start filling TTree with corrected PIDs
        for particle in particles:
            for pidvar in pidVars:
                treeToCorrect.AddVar(tmpResults_dict[particle][pidvar],particle+"_"+pid_dict_data[pidvar]+"corr",res) 
        treeToCorrect.FillVars(res)
    #---------------------------------------------------------------------------
    # Write TTree
    #---------------------------------------------------------------------------
    treeToCorrect.SetBranchStatus("*",1)
    fileN.cd()
    #treeN = treeToCorrect.CloneTree(saveEntries,'fast')
    #treeN.AutoSave()
    #del fileN
    treeToCorrect.Write("",TObject.kWriteDelete)
    print "Done, have a nice day!"
    fileN.Flush()
    fileN.Close()

    end = time.time()
    print time.asctime(time.localtime())
    print "time taken", end-start
