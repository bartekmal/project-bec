from ROOT import TTree
from array import array

 
def __AddVar__(self, var, name, res):
    try:
        res[name][0] = float(var)
    except KeyError:
        comp = array('f', [0])
        self.Branch(name, comp, name+'/F')
        comp[0]= float(var)
        aux = {name: comp}
        res.update(aux)
    return res
    
 
TTree.AddVar = __AddVar__
 
 
def __AddVarInt__(self, var, name, res):
    try:
        res[name][0] = int(var)
    except KeyError:
        comp = array('i', [0])
        self.Branch(name, comp, name+'/I')
        comp[0]= int(var)
        aux = {name: comp}
        res.update(aux)
    return res
        
TTree.AddVarInt = __AddVarInt__
 
def __AddVarLong__(self, var, name, res):
    try:
        res[name][0] = int(var)
    except KeyError:
        comp = array('l', [0])
        self.Branch(name, comp, name+'/l')
        comp[0]= int(var)
        aux = {name: comp}
        res.update(aux)
    return res
        
TTree.AddVarLong = __AddVarLong__


def __FillVars__(self, res_tuple):
    """fill the branches in the dictionary"""
    for name, val in res_tuple.iteritems() :
        b = self.GetBranch(name)
        b.Fill()

TTree.FillVars = __FillVars__


def CopyTree(tree, newtree, res_tuple):

    branches = tree.GetListOfBranches()
    var_list = []
    for b in branches:
        var_list.append(b.GetName())

    for n in var_list:
        if type(tree.__getattr__(n))==type(float()):
            res_tuple = newtree.AddVar(tree.__getattr__(n), n, res_tuple)
        elif  type(tree.__getattr__(n))==type(int()):
            res_tuple = newtree.AddVarInt(tree.__getattr__(n), n, res_tuple)


def CopyBranches(tree, newtree, res_tuple, list_branches):

    for n in list_branches:
        if type(tree.__getattr__(n))==type(float()):
            res_tuple = newtree.AddVar(tree.__getattr__(n), n, res_tuple)
        elif  type(tree.__getattr__(n))==type(int()):
            res_tuple = newtree.AddVarInt(tree.__getattr__(n), n, res_tuple)
        elif type(tree.__getattr__(n))==type(long()):
            res_tuple = newtree.AddVarLong(tree.__getattr__(n), n, res_tuple)
