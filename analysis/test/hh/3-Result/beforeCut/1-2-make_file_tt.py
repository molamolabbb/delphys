import ROOT, math
hlist = { "bfc": {"tt_all":dict(),"tatabb":dict(),"ttv":dict(),"tth":dict(),"twj":dict(),"llbj":dict(), "hh":dict()}}
sample_list = ["tt_all","tatabb","ttv","tth","twj","llbj","hh"]

limit_list = { 'll_pt'               : { "bfc":[60,0,700],    "afc":[60,0,700] },
               'bb_pt'               : { "bfc":[60,0,900],    "afc":[60,0,900] }, 
               'l1_pt'               : { "bfc":[60,0,500],    "afc":[60,0,500] },
               'l2_pt'               : { "bfc":[60,0,500],    "afc":[60,0,500] },
               'missing_et'          : { "bfc":[60,0,700],    "afc":[60,0,700] }, 
               'missing_et_phi'      : { "bfc":[60,-4,4],     "afc":[60,-4,4] }, 
               'bbll_deltaPhi'       : { "bfc":[60,-4,4],     "afc":[60,-4,4] },
               'bbll_mass'           : { "bfc":[100,0,700],   "afc":[100,0,700] }, 
               "bb_mass"             : { "bfc":[100,0,2000],  "afc":[100,0,2000] }, 
               "ll_mass"             : { "bfc":[100,0,1000],  "afc":[100,0,1000] },
               "basic_MT2_332_bbll"  : { "bfc":[80,0,500],    "afc":[80,0,400] },
               "basic_MT2_332_blbl"  : { "bfc":[80,0,500],    "afc":[80,0,400] },
               "basic_MT2_332_b"     : { "bfc":[80,0,700],    "afc":[80,0,700] },
               "basic_MT2_332_l"     : { "bfc":[80,0,500],    "afc":[80,0,200] },
               "ch_bisect_MT2_332"   : { "bfc":[80,0,500],    "afc":[80,0,400] },
               "ch_bisect_MT2_332_b" : { "bfc":[80,0,700],    "afc":[80,0,700] },
               "ch_bisect_MT2_332_l" : { "bfc":[80,0,300],    "afc":[80,0,300] },
               "bb_deltaR"           : { "bfc":[50,0,6],      "afc":[50,0,6] },
               "ll_deltaR"           : { "bfc":[50,0,6],      "afc":[50,0,6] },
               "bbll_deltaR"         : { "bfc":[50,0,5],      "afc":[50,0,5] },
               "bl_deltaR"           : { "bfc":[50,0,5],      "afc":[50,0,5] },
               "bl_min_deltaR"       : { "bfc":[50,0,5],      "afc":[50,0,5] },
               "mT"                  : { "bfc":[75,0,300],    "afc":[75,0,300] }, 
               "higgsness"           : { "bfc":[75,-15,15],   "afc":[75,-15,15] },
               "topness"             : { "bfc":[75,-15,15],   "afc":[75,-15,15] },
               "hig_top_2d"          : { "bfc":[40,-10,10], "afc":[40,-10,10]}
              }
def getHistogram(title,binning,lower_limit,upper_limit):
    return ROOT.TH1D(title,title,binning,lower_limit,upper_limit)

for key in ["bfc"]:
    for key2 in sample_list:
        for key3 in limit_list.keys():
            if key3 is "hig_top_2d":
                hlist[key][key2][key3] = ROOT.TH2F(key2+" "+key+" "+key3.replace("_"," "),key2+" "+key+" "+key3.replace("_"," "),limit_list[key3][key][0],limit_list[key3][key][1],limit_list[key3][key][2],limit_list[key3][key][0],limit_list[key3][key][1],limit_list[key3][key][2])
            else:
                hlist[key][key2][key3] = getHistogram(key2+" "+key+" "+key3.replace("_"," "),limit_list[key3][key][0],limit_list[key3][key][1],limit_list[key3][key][2])


data_path = "/cms/ldap_home/molamolabb/delPhys/src/delphys/analysis/test/hh/1-Anlayser/result"
for sample in sample_list:
    globals()[sample+'{}'.format("_c")] = ROOT.TChain("events")
    globals()[sample+'{}'.format("_c")].Add(data_path+"/"+"pp_"+sample+"/"+"pp_"+sample+".root")


#sample_lists = {"tt_ditau":tt_ditau_c,"hh":hh_c,"tatabb":tatabb_c}
sample_lists = {"tt_all":tt_all_c,"tatabb":tatabb_c,"ttv":ttv_c,"tth":tth_c,"twj":twj_c,"llbj":llbj_c,"hh":hh_c}
f = ROOT.TFile("distribution_tt.root","RECREATE")

#for sample in ["tt", "hh", "hh_B6", "hh_B11"]:
for sample in sample_list:
    c = sample_lists[sample]
    for ie, e in enumerate(c):
        cut = ""
        if e.step == 4 and e.tmva_bdtg_output > 0:
            cut = "bfc"
            hlist[cut][sample]['ll_pt'].Fill(e.ll.Pt())
            hlist[cut][sample]['bb_pt'].Fill(e.bb.Pt())
            hlist[cut][sample]['l1_pt'].Fill(e.lep1.Pt())
            hlist[cut][sample]['l2_pt'].Fill(e.lep2.Pt())
            hlist[cut][sample]["missing_et_phi"].Fill(e.MET.Phi())
            hlist[cut][sample]["missing_et"].Fill(e.MET.E())
            hlist[cut][sample]['bbll_deltaPhi'].Fill(e.bbll_deltaPhi)
            hlist[cut][sample]["bbll_mass"].Fill(e.bbll.M())
            hlist[cut][sample]["bb_mass"].Fill(e.bb.M())
            hlist[cut][sample]["ll_mass"].Fill(e.ll.M())
            hlist[cut][sample]["basic_MT2_332_bbll"].Fill(e.basic_MT2_332_bbll)
            hlist[cut][sample]["basic_MT2_332_blbl"].Fill(e.basic_MT2_332_blbl)
            hlist[cut][sample]["basic_MT2_332_b"].Fill(e.basic_MT2_332_b)
            hlist[cut][sample]["basic_MT2_332_l"].Fill(e.basic_MT2_332_l)
            hlist[cut][sample]["ch_bisect_MT2_332"].Fill(e.ch_bisect_MT2_332)
            hlist[cut][sample]["ch_bisect_MT2_332_b"].Fill(e.ch_bisect_MT2_332_b)
            hlist[cut][sample]["ch_bisect_MT2_332_l"].Fill(e.ch_bisect_MT2_332_l)
            hlist[cut][sample]["bb_deltaR"].Fill(e.bb_deltaR)
            hlist[cut][sample]["ll_deltaR"].Fill(e.ll_deltaR)
            hlist[cut][sample]["bbll_deltaR"].Fill(e.bbll_deltaR)
            hlist[cut][sample]["bl_min_deltaR"].Fill(e.bl_min_deltaR)
            hlist[cut][sample]["mT"].Fill(e.mT)
            hlist[cut][sample]['hig_top_2d'].Fill(e.higgsness, e.topness)
            hlist[cut][sample]['higgsness'].Fill(e.higgsness)
            hlist[cut][sample]['topness'].Fill(e.topness)
            for bld in e.bl_deltaR:
                hlist[cut][sample]["bl_deltaR"].Fill(bld)
num_bg = 0
for sample in sample_list:
    num_bg += hlist["bfc"][sample]["mT"].GetEntries()
num_bg = num_bg * 3869480.648 / 1559547
num_sg = hlist["bfc"]["hh"]["mT"].GetEntries()
num_sg = num_sg * 104.06 / 899997
sensitivity = num_sg/math.sqrt(num_bg+num_sg)
print "number of background : " + str(num_bg) + "\n"
print "number of signal : " + str(num_sg) + "\n"
print "sensitivity : " + str(sensitivity) + "\n"


for key in ["bfc"]:
    for key2 in sample_list:
        for key3 in hlist[key][key2].keys():
            #integ = hlist[key][key2][key3].Integral()
            #if integ == 0:
            #    print "%s %s %s has 0 integral" % (key, key2, key3)
            #else:
            #    hlist[key][key2][key3].Scale(1/integ)
            hlist[key][key2][key3].Write()

f.Close()
