from ROOT import *
import math

gen_path = "/xrootd_user/molamolabb/xrootd/doubleHiggs/sample/"
cut_path = "/cms/ldap_home/molamolabb/delPhys/src/delphys/analysis/test/hh/1-Analyser/result/"
bdt_path = "/cms/ldap_home/molamolabb/delPhys/src/delphys/analysis/test/hh/2-TMVA/result/"

gen_samples = ["pp_hh","pp_tt","pp_tt_ditau","pp_tt_leptau","pp_llbj","pp_tatabb","pp_tth","pp_ttv","pp_twj"]
cut_samples = ["pp_hh","pp_tt_all","pp_llbj","pp_tatabb","pp_tth","pp_ttv","pp_twj"]

chain = {"gen":"Delphes","cut":"events","bdt":"events"}
cross_sig = 104.06
cross_bg = {"pp_hh":104,"pp_tt_all":30655.20025,"pp_llbj":3450000,"pp_tatabb":7320,"pp_ttv":2459054.565,"pp_tth":1049562.954,"pp_twj":372465.0508}

def getGenEntries(samples,path,cut):
  sig_entries = 0
  pp_tt_entries = 0
  bg_entries = {}
  for sample in samples:
    tree = TChain(chain[cut])
    tree.Add(path+str(sample)+"/*.root")
    entries = tree.GetEntries()
    bg_entries[sample] = entries
    if (sample is "pp_tt") or (sample is "pp_tt_ditau") or(sample is "pp_tt_leptau"):
      pp_tt_entries = pp_tt_entries+entries
    print("Number of {} : {}".format(sample, entries))
    if str(sample) == "pp_hh":
      sig_entries = entries
    bg_entries["pp_tt_all"] = pp_tt_entries
  print(sig_entries,bg_entries)
  print("=====================================")
  return sig_entries, bg_entries

def getEntries(samples,path,cut):
  sig_entries = 0
  pp_tt_entries = 0
  bg_entries = {}
  for sample in samples:
    tree = TChain(chain[cut])
    if cut == "bdt":
      tree.Add(path+str(sample)+"2/*.root")
      entries = 0 
      for e in tree:
        if e.step==4 and e.tmva_bdtg_output>0:
          entries=entries+1
    elif cut == "cut":
      tree.Add(path+str(sample)+"2/*.root")
      entries = 0 
      for e in tree:
          if e.step==4 :
            entries=entries+1
    if str(sample) == "pp_hh": 
      sig_entries = entries
      print("Number of {} : {}".format(sample, entries))
    else : 
      bg_entri = float(entries)*float(cross_bg[sample]/gen_bg[sample])
      bg_entries[sample] = bg_entri
      print("Number of {} : {}".format(sample, bg_entri))
  print(sig_entries,bg_entries)
  print("=====================================")
  return sig_entries, bg_entries
 

gen_sig, gen_bg = getGenEntries(gen_samples,gen_path,"gen")
cut_sig, cut_bg = getEntries(cut_samples,cut_path,"cut")
bdt_sig, bdt_bg = getEntries(cut_samples,bdt_path,"bdt")

num_bg = 0
for sample in samples:
  num_bg = num_bg+bdt_bg[sample]
num_sig = bdt_sig*cross_sig/gen_sig 
print(num_sig)
print(num_bg)
bdtSensitivity = num_sig/math.sqrt(num_bg+num_sig) 
print("BDT Sensitivity : {}".format(bdtSensitivity))

num_bg = 0
for sample in samples:
  num_bg = num_bg+cut_bg[sample]
num_sig = cut_sig*cross_sig/gen_sig 
print(num_sig)
print(num_bg)
cutSensitivity = num_sig/math.sqrt(num_bg+num_sig) 
print("Cut Based Sensitivity : {}".format(cutSensitivity))

'''
print("Number of Gen backgrounds entries : {}".format(gen_bg))
print("Number of Gen signal entries : {}".format(gen_sig))
print("Number of cut backgrounds entries : {}".format(cut_bg))
print("Number of cut signal entries : {}".format(cut_sig))
print("Number of bdt backgrounds entries : {}".format(bdt_bg))
print("Number of bdt signal entries : {}".format(bdt_sig))
print("===================================================")
num_sig = cut_sig*cross_sig/gen_sig 
num_bg  = cut_bg*cross_bg/gen_bg
print(num_sig)
print(num_bg)
genSensitivity = num_sig/math.sqrt(num_bg+num_sig) 
print("Cut based Sensitivity : {}".format(genSensitivity))
print("===================================================")

num_sig = bdt_sig*cross_sig/gen_sig 
num_bg  = bdt_bg*cross_bg/gen_bg

print(num_sig)
print(num_bg)
bdtSensitivity = num_sig/math.sqrt(num_bg+num_sig) 
print("BDT Sensitivity : {}".format(bdtSensitivity))
'''
