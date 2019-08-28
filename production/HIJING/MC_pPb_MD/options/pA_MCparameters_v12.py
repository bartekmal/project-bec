

Def={"sufix":"", "version":"v", "snEvts":"5", "Config_pA":"pA", "Config_M":"MD", "sRunNumber":"1000", "sRunNumbers":[], "sRunNumbersBoole":[], "sFill":"-1", "year":"2013", "Simulation":"True", "DataType":"MC","Energy":"5TeV",
     "hipr1_2":"0.65", "hipr1_4":"0.9", "hipr1_6":"0.12", "hipr1_8":"2.0"}

def div10(x):
    return x/10

def params(DEBUG=1, parameters=[]):
    print "****************************************************************************"
    print "parameters",parameters
    if DEBUG:
        deb=Def.copy()
        deb["sufix"]="_test"
        deb["version"]="vtest"
        return deb, Def
    pars=Def.copy()
    pars["Simulation"]=parameters[1]
    pars["version"]=parameters[2]
    pars["Config_pA"]=parameters[3]
    pars["Config_M"]=parameters[4]
    pars["Energy"]=parameters[5]
    if pars["Simulation"]=="True":
        pars["DataType"]="MC"
        if pars["Config_pA"]!="pp":
            pars["snEvts"]=parameters[6]
            pars["sRunNumber"]=parameters[7]
            if len(parameters)==8:
                pars["hipr1_2"]="0.65"
                pars["hipr1_4"]="2.0"
                pars["hipr1_6"]="0.12"
                pars["hipr1_8"]="2.0"
            else:
                pars["hipr1_2"]=parameters[8]
                pars["hipr1_4"]=parameters[9]
                pars["hipr1_6"]=parameters[10]    
                pars["hipr1_8"]=parameters[11]    
        else:
            pars["year"]=parameters[6]
    else:
        pars["DataType"]="RealData"
        if pars["Config_pA"]=="pp": pars["year"]="2015"
        else: pars["year"]="2013"
        
    if "-" in pars["sRunNumber"]:
        tmp=map(int,pars["sRunNumber"].split("-"))
        pars["sRunNumbersBoole"]=map(str,range(tmp[0],tmp[1]+1))
        #tmp1=map(div10,tmp)
        pars["sRunNumbers"]=range(tmp[0],tmp[1]+1)
        pars["sFill"]=str(pars["sRunNumbers"][0])
            
    return pars, Def
