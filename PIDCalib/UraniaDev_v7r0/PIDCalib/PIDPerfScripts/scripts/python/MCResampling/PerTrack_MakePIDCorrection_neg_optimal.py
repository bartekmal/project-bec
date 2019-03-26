#################################################################################
###
###
###
#################################################################################

from PIDPerfScripts.StartScreen import *

from ROOT import *
from Utils import *

import array
import argparse
import sys
import os.path

hist_buffer = {}
hist_false_flag = {}

def getRandomFromTNSparseBuffered(sparse,tmp,nDim,dim, particle, bin1, bin2, bin3):
    """
    Generates the radom values for all dimensions simultaniously and saves them into tmp
    Returns 1 for success and 0 if the bins were empty
    """

    mykey = particle + "-" + str(bin1) + "-" + str(bin2) + "-" + str(bin3)
    if mykey in hist_false_flag.keys():
#        print mykey, " FAILED ####################################"
        return 0
    if mykey in hist_buffer.keys():
#        print mykey, "from >>>>>  BUFFER  <<<<<<"
        temp_hists = hist_buffer[mykey]
        for i in xrange(len(dim)):
            tmp[i] = temp_hists[i].GetRandom()
#            print "mean from buffer ",temp_hists[i].GetMean()
        return 1

#    print mykey, "from SPARSE"
    tmp_hist = []
    for i in xrange(len(dim)):
        tmp_hist.append(sparse.Projection(dim[i]))
        nBins = tmp_hist[i].GetNbinsX()
        inte = tmp_hist[i].ComputeIntegral()
        if(nBins!=0 and inte!=0):
            tmp_val = tmp_hist[i].GetRandom()
            tmp[i] = tmp_val
#            print "mean sparse ",tmp_hist[i].GetMean()
        else:
            tmp_hist[i].IsA().Destructor(tmp_hist[i])
            hist_false_flag[mykey]=1
#            print mykey,i,dim[i], " FAILED IN HIST ####################################"
            return 0
#        tmp_hist[i].IsA().Destructor(tmp_hist[i])

    hist_buffer[mykey]=tmp_hist
    return 1

def getRandomFromTNSparse(sparse,tmp,nDim,dim):
    """
    Generates the radom values for all dimensions simultaniously and saves them into tmp
    Returns 1 for success and 0 if the bins were empty
    """

    for i in xrange(len(dim)):
        tmp_hist = sparse.Projection(dim[i])
        nBins = tmp_hist.GetNbinsX()
        inte = tmp_hist.ComputeIntegral()
        if(nBins!=0 and inte!=0):
            tmp_val = tmp_hist.GetRandom()
            tmp[i] = tmp_val
        else:
            tmp_hist.IsA().Destructor(tmp_hist)
            return 0
        tmp_hist.IsA().Destructor(tmp_hist)
    return 1

class ShowArgumentsParserMCreweight(argparse.ArgumentParser):
    def error(self, message):
        sys.stderr.write('error: %s\n\n' %message)
        parser.print_usage(sys.stderr)
        sys.stderr.write('\n'+self.description)
        sys.exit(2)


if '__main__' == __name__:
    start()
    print ""
    parser = ShowArgumentsParserMCreweight(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        prog=os.path.basename(sys.argv[0]),
        description=("""Make PID correction for a given:
        a) MC file <mcFile> ("Complete path to the file")
        b) Path to the TTree inside the MC file <mcFilePathToTree>
        c) particle type <partName> (\"K\", \"P\", \"Pi\",\"Mu\", "Muplus" and so on)
        d) PID variable to correct for each particle, <pidCut>

For a full list of arguments, do: 'python {0} -h'
""").format(os.path.basename(sys.argv[0]))
        )

    ## add the positional arguments
    parser.add_argument('mcFile', metavar='<mcFile>',
                        help="Sets the MC file to correct the PID variables")

    parser.add_argument('mcFilePathToTree',metavar='<mcFilePathToTree>',
                        help="Sets the internal path to the TTree of the MC file")

    parser.add_argument('partName', metavar='<partName>',
                        help="Sets the particle type")

    parser.add_argument('pidVars', metavar='<pidVars>',
                        help="Sets the PID variables to store")
    #------------------------------------------------------------------------------
    parser.add_argument('pidLibrary', metavar='<pidLibrary>',
                        help="Sets the library file with the PID distributions")

    opts = parser.parse_args()

    pid_dict = {'PIDK':'PIDK','PIDp':'PIDp','PIDmu':'PIDmu','PIDpi':'PIDpi','PIDpK':'PIDpK','PIDe':'PIDe','ProbNNK':'ProbNNk','ProbNNp':'ProbNNp','ProbNNmu':'ProbNNmu','ProbNNpi':'ProbNNpi','ProbNNe':'ProbNNe','V4ProbNNK':'ProbNNk','V4ProbNNp':'ProbNNp','V4ProbNNmu':'ProbNNmu','V4ProbNNpi':'ProbNNpi','V4ProbNNe':'ProbNNe','V3ProbNNK':'ProbNNk','V3ProbNNp':'ProbNNp','V3ProbNNmu':'ProbNNmu','V3ProbNNpi':'ProbNNpi','V3ProbNNe':'ProbNNe','V2ProbNNK':'ProbNNk','V2ProbNNp':'ProbNNp','V2ProbNNmu':'ProbNNmu','V2ProbNNpi':'ProbNNpi','V2ProbNNe':'ProbNNe', 'MC12TuneV2_ProbNNK':'ProbNNk','MC12TuneV2_ProbNNp':'ProbNNp','MC12TuneV2_ProbNNmu':'ProbNNmu','MC12TuneV2_ProbNNpi':'ProbNNpi','MC12TuneV2_ProbNNe':'ProbNNe'}


    # load the the file to correct
    fileToCorrect = TFile(opts.mcFile)
    treeToCorrect = fileToCorrect.Get(opts.mcFilePathToTree)


    # load the pid library file
    fileLibrary = TFile(opts.pidLibrary)

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
    ct=0

    pidVars_dict={}
    for v in pidVars:
        pidVars_dict[ct]=v
        ct=ct+1


    #Start of PID corection
    #----------------------
    #load the libraries for each particle
    part_dict = {}
    for i in YourPart_dict:
        tmp = []
        tmp.append(fileLibrary.Get(YourPart_dict[i]).Get("histo_PID"))
        tmp.append(fileLibrary.Get(YourPart_dict[i]).Get("H1D_P"))
        tmp.append(fileLibrary.Get(YourPart_dict[i]).Get("H1D_ETA"))
        tmp.append(fileLibrary.Get(YourPart_dict[i]).Get("H1D_nTracks"))
        part_dict[i] = tmp

    #get dictionary for the particle to axis
    listOfAxes = tmp[0].GetListOfAxes()
    axis_dict = {}
    for j in xrange(3,listOfAxes.GetEntries()):
        axis_dict[j] = pid_dict[listOfAxes[j].GetName()]


    ### start to loop over entries in the TTree
    nEntries = treeToCorrect.GetEntries()
    nDim = len(axis_dict)

    dim = array.array("i",std.vector(int)(nDim))
    for j in xrange(nDim):
        dim[j] = j+3

#    mom_dict = {}
#    eta_dict = {}
#    for p in YourPart_dict.keys():
#        mom = (array.array('d',[0]))
#        eta = (array.array('d',[0]))
#        treeToCorrect.SetBranchAddress( p+"_"+"P", mom )
#        treeToCorrect.SetBranchAddress( p +"_"+"ETA", eta )
#        mom_dict[p] = mom
#        eta_dict[p] = eta
#        del mom
#        del eta

    tmpResults_dict = {}

    treeN = TTree('DecayTree', 'DecayTree')
    res = {}

#PERT prepare MCID dictionary
    d_mcid = {"211":"pi","321":"K","2212":"p","13":"mu","11":"e"}

#PERT prepare arrays for new branches, create file and cloned tree 
    piNN_cor = array.array('f', std.vector(float)(2000))
    kNN_cor  = array.array('f', std.vector(float)(2000))
    pNN_cor  = array.array('f', std.vector(float)(2000))

    newfile = TFile("newfilename.root","RECREATE")  
    newtree = treeToCorrect.CloneTree(0)
    newtree.Branch("piNNcor",piNN_cor,"piNNcor[N]/F");
    newtree.Branch("kNNcor",kNN_cor,"kNNcor[N]/F");
    newtree.Branch("pNNcor",pNN_cor,"pNNcor[N]/F");

#PERT prepare average distribution over whole phase space
    for aparticle in part_dict:
        atmp_hist = part_dict[aparticle] 
        nDim = len(dim)
        atmp = array.array('d', std.vector(float)(nDim))
        test = getRandomFromTNSparseBuffered(atmp_hist[0],atmp,nDim,dim, aparticle, 0, 0, 0 )

#counters
    licz_part_all={}
    licz_part_fail={}
    licz_part_fail1={}
    licz_part_fail2={}
    for aparticle in part_dict:
        licz_part_all[aparticle]=0
        licz_part_fail[aparticle]=0
        licz_part_fail1[aparticle]=0
        licz_part_fail2[aparticle]=0

# counter for failures. If failed average over all phase space is assigned
    ifail = 0
    ifail1 = 0
    ifail2 = 0
    ifail3 = 0
    iall =0 
# counter for mcid without entry in PIDHisto calib file
    inotknown = 0
# counter for mcid = 0
    izeroid = 0

    for iE in xrange(nEntries):
#    for iE in xrange(10000):
        treeToCorrect.GetEntry(iE)
        if(iE%1000)==0:
            print "entries left ",nEntries-iE

####################
#        if  iE>50 :
#            break

        for track in range(treeToCorrect.N):

            iall = iall + 1
            tmpParticleResults_dict = {}
            mcid = abs(treeToCorrect.MCID[track])
            if mcid == 0:  # if unnknow MC particle assign original MC Probs
                izeroid = izeroid + 1
                piNN_cor[track] = treeToCorrect.piNN[track]
                kNN_cor[track]  = treeToCorrect.kNN[track]
                pNN_cor[track]  = treeToCorrect.pNN[track]
            else:
                str_mcid = str(mcid)
                if str_mcid in d_mcid.keys():            
                    part_name = d_mcid[str_mcid]
                else:
                    part_name = "pi" 
                    inotknown = inotknown + 1
                    print "============================== unknown MCID: ", mcid
                    
                tmp_histos = part_dict[part_name] 

                bin_P       = tmp_histos[1].FindBin(1000.*treeToCorrect.p[track])  # ntuple in GeV, Calin in MeV
                bin_ETA     = tmp_histos[2].FindBin(treeToCorrect.eta[track])
# factor 1.3 to correct for DATA-MC discrepancy in track multiplicity
                bin_nTrack  = tmp_histos[3].FindBin(1.3*treeToCorrect.multNoPreSel)
#                print "BBBBBAAAAA",bin_P,bin_ETA,bin_nTrack,treeToCorrect.p[track], treeToCorrect.eta[track],treeToCorrect.multNoPreSel 
                if ( bin_P==0):
                    bin_P=1
#                if ( bin_P == 0 or bin_ETA == 0 or bin_nTrack == 0):
#                    print "BBBBBAAAAA",bin_P,bin_ETA,bin_nTrack,treeToCorrect.p[track], treeToCorrect.eta[track],treeToCorrect.multNoPreSel 

            #set projection range
                tmp_histos[0].GetAxis(0).SetRange(bin_P,bin_P)
                tmp_histos[0].GetAxis(1).SetRange(bin_ETA,bin_ETA)
                tmp_histos[0].GetAxis(2).SetRange(bin_nTrack,bin_nTrack)
                nDim = len(dim)
                tmp = array.array('d', std.vector(float)(nDim))
            #            test = getRandomFromTNSparse(tmp_histos[0],tmp,nDim,dim)
                failflag = 1;
                licz_part_all[part_name]=licz_part_all[part_name]+1
                test = getRandomFromTNSparseBuffered(tmp_histos[0],tmp,nDim,dim, part_name, bin_P, bin_ETA, bin_nTrack )
                if(test==0):
#                    print "BBBBB",treeToCorrect.p[track], treeToCorrect.eta[track],treeToCorrect.multNoPreSel 
                    ifail = ifail + 1
                    failflag = -1
                    licz_part_fail[part_name]=licz_part_fail[part_name]+1
                    n_bins_track = tmp_histos[3].GetNbinsX()                
                    tmp_histos[0].GetAxis(2).SetRange(1,n_bins_track)
                    test = getRandomFromTNSparseBuffered(tmp_histos[0],tmp,nDim,dim, part_name, bin_P, bin_ETA, 0 )
                    if(test==0):
                        ifail1 = ifail1 + 1
                        failflag = -1
                        licz_part_fail1[part_name]=licz_part_fail1[part_name]+1
                        n_bins_eta = tmp_histos[2].GetNbinsX()
                        tmp_histos[0].GetAxis(1).SetRange(1,n_bins_eta)
                        n_bins_track = tmp_histos[3].GetNbinsX()                
                        tmp_histos[0].GetAxis(2).SetRange(1,n_bins_track)
                        test = getRandomFromTNSparseBuffered(tmp_histos[0],tmp,nDim,dim, part_name, bin_P, 0, 0 )
                        if (test==0):
# take average over everything
                            ifail2 = ifail2 + 1
                            test = getRandomFromTNSparseBuffered(tmp_histos[0],tmp,nDim,dim, part_name, 0, 0, 0 )
                            licz_part_fail2[part_name]=licz_part_fail2[part_name]+1
                            if (test==0):
                                ifail3 = ifail3 + 1
                                print "Error in generation of average values. Should never happen."
                                continue 

                for i in xrange(nDim):
                    tmpParticleResults_dict[axis_dict[i+3]] = tmp[i]

#                if failflag < 0 :
#                    print "FAIL ",part_name,bin_P,bin_ETA,bin_nTrack,treeToCorrect.p[track], treeToCorrect.eta[track],treeToCorrect.multNoPreSel 
#                    print part_name, failflag, tmpParticleResults_dict

                piNN_cor[track] = failflag * tmpParticleResults_dict['ProbNNpi']
                kNN_cor[track]  = failflag * tmpParticleResults_dict['ProbNNk']
                pNN_cor[track]  = failflag * tmpParticleResults_dict['ProbNNp']

        newtree.Fill()

    newtree.Write()
    newfile.Save()
    newfile.Close()

    print "Statistics :"
    print "------------"
    print "MCID = 0 ", 1.0*izeroid/iall, " (", izeroid, "/", iall,")"
    print "unknown MCID ", 1.0*inotknown/iall, " (", inotknown, "/", iall,")"
    print "fail rate ", 1.0*ifail/iall, " (", ifail, "/", iall,")"
    print "fail1 rate ", 1.0*ifail1/iall, " (", ifail1, "/", iall,")"
    print "fail2 rate ", 1.0*ifail2/iall, " (", ifail2, "/", iall,")"
    print "fail3 rate ", 1.0*ifail3/iall, " (", ifail3, "/", iall,")"
    print "   "
    print "Failures for individual particles   "
    for i in licz_part_fail:
        print i,1.0*licz_part_fail[i]/licz_part_all[i], " (", licz_part_fail[i] , "/", licz_part_all[i],")"

    print "Failures 1 for individual particles (p,eta) - integrated over nTracks  "
    for i in licz_part_fail1:
        print i,1.0*licz_part_fail1[i]/licz_part_all[i], " (", licz_part_fail1[i] , "/", licz_part_all[i],")"

    print "Failures 2 for individual particles (p) - integrated over ntracks and eta  "
    for i in licz_part_fail2:
        print i,1.0*licz_part_fail2[i]/licz_part_all[i], " (", licz_part_fail2[i] , "/", licz_part_all[i],")"

    print "Done, have a nice day!"
