import ROOT, math
import datetime
from ROOT import *
import os
hlist = {"bfc":{"tt_all":dict(),"tatabb":dict(), "ttv":dict(),"tth":dict(),"twj":dict(), "llbj":dict(), "hh":dict()},
          "afc":{"tt_all":dict(),"tatabb":dict(), "ttv":dict(),"tth":dict(),"twj":dict(), "llbj":dict(), "hh":dict()}}
sample_list = ["tatabb","tt_all","twj","tth","ttv","llbj","hh"]
var_list = [ 'll_pt',
               'bb_pt',
               'l1_pt',
               'l2_pt',
               'missing_et',
               'missing_et_phi',
               'bbll_deltaPhi',
               'bbll_mass',
               'bb_mass',
               'll_mass',
               'basic_MT2_332_bbll',
               'basic_MT2_332_b',
               'basic_MT2_332_l',
               'basic_MT2_332_blbl',
               'bb_deltaR',
               'll_deltaR',
               'bbll_deltaR',
               'bl_deltaR',
               'bl_min_deltaR',
               'mT',
               'hig_top_2d',
               'higgsness',
               'topness']

color_list = {"tt_all":kRed-4,
              "tatabb":kViolet-6,
              "ttv"   :kSpring+10,
              "tth"   :kYellow,
              "twj"   :kTeal+2,
              "llbj"  :kAzure+7,
              "hh"    :kBlack}

f_after  = ROOT.TFile("/cms/ldap_home/molamolabb/delPhys/src/delphys/analysis/test/hh/3-Result/afterCut/distribution_tt.root")
f_before = ROOT.TFile("/cms/ldap_home/molamolabb/delPhys/src/delphys/analysis/test/hh/3-Result/beforeCut/distribution_tt.root")
cvs = ROOT.TCanvas()
cvs.SetGrid()
cvs.SetLeftMargin(0.15)
cvs.SetBottomMargin(0.12)

for cut in ["bfc","afc"]:
    for sample in sample_list:
        for var in var_list:
            if cut == "afc" : hlist[cut][sample][var] = f_after.Get(sample+" "+cut+" "+var.replace("_"," "))
            if cut == "bfc" : hlist[cut][sample][var] = f_before.Get(sample+" "+cut+" "+var.replace("_"," "))

scale = {"tt_all"   :30603/hlist["afc"]["tt_all"]["mT"].GetEntries(),
         "tatabb"   :7320.0/hlist["afc"]["tatabb"]["mT"].GetEntries(),
         "ttv"      :2459054.565/hlist["afc"]["ttv"]["mT"].GetEntries(),
         "tth"      :1049562.954/hlist["afc"]["tth"]["mT"].GetEntries(),
         "twj"      :372465.0508/hlist["afc"]["twj"]["mT"].GetEntries(),
         "llbj"     :3450000/hlist["afc"]["llbj"]["mT"].GetEntries(),
         "hh"       :104.06/hlist["afc"]["hh"]["mT"].GetEntries()}

for sample in sample_list:
    for var in var_list:
        legend = ROOT.TLegend(0.7,0.55,0.9,0.9)
        for cut in ["bfc","afc"]:
            h = hlist[cut][sample][var]
            h.SetLineColor(color_list[sample])
            legend.SetTextSize(0.06)
            legend.AddEntry(h, cut)
            h.SetTitle(sample.replace("_"," ")+" "+var.replace("_"," ")+" Comparison before and after BDTG")
            h.SetStats(False)
            h.SetLabelSize(0.05,"X")
            h.SetLabelSize(0.05,"Y")
            h.SetLineWidth(3)
            h.SetTickLength(0.03)
            if ('deltaR' or 'higgsness' or 'topness' or 'deltaPhi') not in var:
                h.GetXaxis().SetTitle("[GeV]")
            if "mass" in var:
                h.GetXaxis().SetRangeUser(0,300)
            h.GetXaxis().SetTickSize(0.03)
            h.GetXaxis().SetTitleSize(0.06)
            h.GetXaxis().SetAxisColor(ROOT.kBlack)
            cvs.RedrawAxis()
            h.GetYaxis().SetAxisColor(ROOT.kBlack)
            #h.GetYaxis().SetTitle("Normalized to unity")
            h.GetYaxis().SetTitleSize(0.05)
            if cut is "bfc": 
                h.SetLineColor(kGreen)
                h.Draw("hist")
            else : 
                h.SetLineColor(kRed)
                h.Draw("hist same")
                legend.Draw("same")
        cvs.SetTitle(var.replace("_"," ")+" Comparison before and after BDTG")
        now = datetime.datetime.now()
        nowDate = now.strftime('%Y%m%d')
        if not os.path.isdir("plots/"+nowDate):
          os.mkdir("plots/"+nowDate)
        cvs.SaveAs("plots/"+nowDate+"/Comparing_"+sample+"_"+var+".png")



f_after.Close()
f_before.Close()
