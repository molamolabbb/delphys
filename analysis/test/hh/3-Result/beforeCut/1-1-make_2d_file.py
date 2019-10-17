from ROOT import *
import math

bg = TChain("events")
data_path = "/cms/ldap_home/molamolabb/delPhys/src/delphys/analysis/test/hh/1-Analyser/result"
sample_list = ["tt","tt_ditau","tt_leptau","tatabb","ttv","tth","twj","llbj"]

for sample in sample_list:
  bg.Add(data_path+"/"+"pp_"+sample+"/"+"pp_"+sample+".root")

hist = TH2F("backgrounds hig top 2d","backgrounds hig top 2d",100,-10,10,100,-10,10)
f = TFile("background_higtop_2d.root","RECREATE")

for ie,e in enumerate(bg):
  if e.step >= 0:
    hist.Fill(e.higgsness, e.topness)

hist.Write()
f.Close()
