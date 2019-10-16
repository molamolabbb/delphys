import ROOT, math
from ROOT import *
import datetime
import os

hlist = {"bfc":{"tt":dict(),"tt_ditau":dict(),"tt_leptau":dict(),"tatabb":dict(), "ttv":dict(),"tth":dict(),"twj":dict(), "llbj":dict(), "hh":dict()}}
#sample_list = ["tt","tt_ditau","tt_leptau","tatabb","ttv","tth","twj","llbj","hh"]
sample_list = ["tatabb","tt_ditau","tt","tt_leptau","twj","tth","ttv","llbj","hh"]
order_list = ['ll_pt','bb_pt','missing_et','missing_et_phi','bbll_mass','bb_mass','ll_mass','basic_MT2_332_bbll','basic_MT2_332_b','basic_MT2_332_l','basic_MT2_332_blbl','ch_bisect_MT2_332','ch_bisect_MT2_332_b','ch_bisect_MT2_332_l','bb_deltaR','ll_deltaR','bbll_deltaR','bl_deltaR','bl_min_deltaR','mT','hig_top_2d','higgsness','topness']
color_list = {"tt"    :kRed-4,"tt_ditau":kRed-4,"tt_leptau":kRed-4,
              "tatabb":kViolet-6,
              "ttv"   :kSpring+10,
              "tth"   :kYellow,
              "twj"   :kTeal+2,
              "llbj"  :kAzure+7,
              "hh"    :kBlack}

f = TFile("before_plots.root")
cvs = TCanvas()
cvs.SetGrid()
cvs.SetLogy()
cvs.SetLeftMargin(0.15)
cvs.SetBottomMargin(0.12)

for cut in ["bfc"]:
    for sig_bg in sample_list:
        for order in order_list:
            hlist[cut][sig_bg][order] = f.Get(sig_bg+" "+cut+" "+order.replace("_"," "))

#Before cut need to be stacking
'''
stack = THStack("stack","")
hlist["bfc"]['hh']['mT'].SetFillColor(kGreen)
hlist["bfc"]['hh']['mT'].SetLineColor(kGreen)
stack.Add(hlist["bfc"]["hh"]["mT"])
stack.Draw()
'''         
            
            
scale = {"tt"       :13026.59269/hlist["bfc"]["tt"]["mT"].GetEntries(),
         "tt_ditau" :3536.845092/hlist["bfc"]["tt_ditau"]["mT"].GetEntries(),
         "tt_leptau":14091.76248/hlist["bfc"]["tt_leptau"]["mT"].GetEntries(),
         "tatabb"   :7320.0/hlist["bfc"]["tatabb"]["mT"].GetEntries(),
         "ttv"      :2459054.565/hlist["bfc"]["ttv"]["mT"].GetEntries(),
         "tth"      :1049562.954/hlist["bfc"]["tth"]["mT"].GetEntries(),
         "twj"      :372465.0508/hlist["bfc"]["twj"]["mT"].GetEntries(),
         "llbj"     :3450000/hlist["bfc"]["llbj"]["mT"].GetEntries(),
         "hh"       :104.06/hlist["bfc"]["hh"]["mT"].GetEntries()}

for order in order_list:
    for sample in sample_list:
        legend = TLegend(0.70,0.55,0.90,0.90)
        #ttbar background event
        t = hlist["bfc"][sample][order]
        integ = t.Integral()
        t.SetLineWidth(3)
        t.SetLineColor(color_list[sample])
        legend.SetTextSize(0.06)
        t.SetTitle("Before cut "+order.replace("_"," "))
        t.SetStats(False)
        t.SetLabelSize(0.05,"X")
        t.SetLabelSize(0.05,"Y")
        t.SetLineWidth(3)
        t.SetTickLength(0.03)
        #t.Scale(1/integ) 
        if (order == "hig_top_2d") :
            t.GetXaxis().SetTitle("log(T)")
            t.GetYaxis().SetTitle("log(H)")
            t.Draw("colz")
            #t.PaintTitle("Normalized")
        
        if sample=="tatabb":
            stack = THStack("stack","Before cut "+order.replace("_", " "))
            t.SetFillColor(color_list[sample])
            t.Scale(scale[sample])
            stack.Add(t)
            legend.AddEntry(t,sample)
        elif sample == "hh":
            t.Scale(scale[sample]*10000)
            t.Draw("hist same")
            legend.AddEntry(t,sample)
        elif sample=="llbj":
            t.Scale(scale[sample])
            stack.Add(t)
            t.SetFillColor(color_list[sample])
            stack.Draw("hist")
        else :
            t.Scale(scale[sample])
            stack.Add(t)
            t.SetFillColor(color_list[sample])
            #stack.Draw("hist")
            legend.AddEntry(t,sample)
    #legend.AddEntry(t,sample)
    legend.Draw()
    cvs.SetTitle("Before BDT cut "+order)
    now = datetime.datetime.now()
    nowDate = now.strftime('%Y%m%d')
    if not os.path.isdir("bf_plots/"+nowDate):
      os.mkdir("bf_plots/"+nowDate)
    cvs.SaveAs("bf_plots/"+nowDate+"/Before_cut"+"_"+order+".png")
'''
# Plotting Before Cut
for order in order_list:
    legend = TLegend(0.70,0.55,0.90,0.90)
    #ttbar background event
    t = hlist["bfc"]["tt"][order]
    integ = t.Integral()
    t.SetLineWidth(3)
    t.SetLineColor(color_list["tt"])
    legend.SetTextSize(0.06)
    legend.AddEntry(t,"tt")
    t.SetTitle("Before cut "+order.replace("_"," "))
    t.SetStats(False)
    t.SetLabelSize(0.05,"X")
    t.SetLabelSize(0.05,"Y")
    t.SetLineWidth(3)
    t.SetTickLength(0.03)
    if integ !=0 :
       t.Scale(1/integ)

    #hh signal event
    h = hlist["bfc"]["hh"][order]
    h.SetLineWidth(3)
    h.SetLineColor(color_list["hh"])
    legend.SetTextSize(0.06)
    legend.AddEntry(h,"hh")
    h.SetLineWidth(3)
    h.SetTickLength(0.03)
    h.SetTitle("Before cut "+order.replace("_"," "))
    integ = h.Integral()
    if integ !=0 :
        h.Scale(1/integ)
    if (order == "hig_top_2d") :
        h.GetXaxis().SetTitle("log(T)")
        h.GetYaxis().SetTitle("log(H)")
        
        #t.Draw("colz")
        h.Draw("colz")
        #h.PaintTitle("Normalized")
    
    if (order == "ll_pt") or (order == "bb_pt") :
        t.Draw("hist")
        h.Draw("hist same")
    else :    
        h.Draw("hist")
        t.Draw("hist same")
    if order != "hig_top_2d":
        legend.Draw("same")
    
    cvs.SetTitle("Before_cut "+order)
    cvs.SaveAs("bf_af_plot/Before_cut"+"_"+order+".png")
'''

f.Close()
