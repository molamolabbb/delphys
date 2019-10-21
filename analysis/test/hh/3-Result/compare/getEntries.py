from ROOT import *
import math

gen_path = "/xrootd_user/molamolabb/xrootd/doubleHiggs/sample/"
cut_path = "/cms/ldap_home/molamolabb/delPhys/src/delphys/analysis/test/hh/1-Analyser/result/"
bdt_path = "/cms/ldap_home/molamolabb/delPhys/src/delphys/analysis/test/hh/2-TMVA/result/"

gen_samples = ["pp_hh","pp_tt","pp_tt_ditau","pp_tt_leptau","pp_llbj","pp_tatabb","pp_tth","pp_ttv","pp_twj"]
cut_samples = ["pp_hh","pp_tt_all","pp_llbj","pp_tatabb","pp_tth","pp_ttv","pp_twj"]

chain = {"gen":"Delphes","cut":"events","bdt":"events"}
cross_sig = 104.06
cross_bg = 3901820.746

def getEntries(samples,path,cut):
  sig_entries = 0
  bg_entries = 0
  for sample in samples:
    tree = TChain(chain[cut])
    tree.Add(path+str(sample)+"/*.root")
    if cut == "bdt":
      entries = 0 
      for e in tree:
          if e.step==4 and e.tmva_bdtg_output>0.5:
            entries=entries+1
    else : entries = tree.GetEntries()
    print("Number of {} : {}".format(sample, entries))
    if str(sample) == "pp_hh":
      sig_entries = entries
    else :
      bg_entries = bg_entries + entries  
  return sig_entries, bg_entries
 

gen_sig, gen_bg = getEntries(gen_samples,gen_path,"gen")
cut_sig, cut_bg = getEntries(cut_samples,cut_path,"cut")
bdt_sig, bdt_bg = getEntries(cut_samples,bdt_path,"bdt")

print("Number of Gen backgrounds entries : {}".format(gen_bg))
print("Number of Gen signal entries : {}".format(gen_sig))
print("Number of cut backgrounds entries : {}".format(cut_bg))
print("Number of cut signal entries : {}".format(cut_sig))
print("Number of bdt backgrounds entries : {}".format(bdt_bg))
print("Number of bdt signal entries : {}".format(bdt_sig))

num_sig = cut_sig*cross_sig/gen_sig 
num_bg  = cut_bg*cross_bg/gen_bg
genSensitivity = num_sig/math.sqrt(num_bg+num_sig) 
print("Cut based Sensitivity : {}".format(genSensitivity))

num_sig = bdt_sig*cross_sig/gen_sig 
num_bg  = bdt_bg*cross_bg/gen_bg
bdtSensitivity = num_sig/math.sqrt(num_bg+num_sig) 
print("BDT Sensitivity : {}".format(bdtSensitivity))
