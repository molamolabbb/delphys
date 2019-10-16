import ROOT, math
from ROOT import *

hlist = {"bfc":{"tt":dict(), "hh":dict()}, "afc":{"tt":dict(), "hh":dict()}}
sample_list = ["tt","hh"]
order_list = ['ll_pt','bb_pt','missing_et','missing_et_phi','bbll_mass','bb_mass','ll_mass','basic_MT2_332_bbll','basic_MT2_332_b','basic_MT2_332_l','basic_MT2_332_blbl','ch_bisect_MT2_332','ch_bisect_MT2_332_b','ch_bisect_MT2_332_l','bb_deltaR','ll_deltaR','bbll_deltaR','bl_deltaR','bl_min_deltaR','mT','hig_top_2d']

color_list = {"tt":kGreen+2, "hh":kRed}

f = TFile("plots.root")
cvs = TCanvas()
cvs.SetGrid()
cvs.SetLeftMargin(0.15)
cvs.SetBottomMargin(0.12)

for cut in ["bfc","afc"]:
    for sig_bg in ["hh","tt"]:
        for order in order_list:
            hlist[cut][sig_bg][order] = f.Get(sig_bg+" "+cut+" "+order.replace("_"," "))

#Before cut need to be stacking
stack = THStack("stack","")
hlist["bfc"]['hh']['mT'].SetFillColor(kGreen)
hlist["bfc"]['hh']['mT'].SetLineColor(kGreen)
hlist["afc"]['hh']['mT'].SetLineColor(kGreen)
hlist["afc"]["hh"]["mT"].SetFillColor(kGreen)
stack.Add(hlist["bfc"]["hh"]["mT"])
stack.Add(hlist["afc"]["hh"]["mT"])
stack.Draw()
            
            
            
           
            
scale = {"hh":338.0*7.46193820646/hlist["bfc"]["hh"]["mT"].Integral(), "tt":26960510.0*4529.2983283/hlist["bfc"]["tt"]["mT"].Integral()}

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
    '''
    if (order == "ll_pt") or (order == "bb_pt") :
        t.Draw("hist")
        h.Draw("hist same")
    else :    
        h.Draw("hist")
        t.Draw("hist same")
    if order != "hig_top_2d":
        legend.Draw("same")
    '''
    cvs.SetTitle("Before_cut "+order)
    cvs.SaveAs("bf_af_plot/Before_cut"+"_"+order+".png")

# Plotting After Cut
for order in order_list:
    legend = TLegend(0.70,0.55,0.90,0.90)
    #ttbar background
    scale_factor = scale["tt"]
    t = hlist["afc"]["tt"][order]
    integ = t.Integral()
    t.SetLineWidth(3)
    t.SetLineColor(color_list["tt"])
    legend.SetTextSize(0.06)
    legend.AddEntry(t,"tt")
    t.SetTitle("After cut "+order.replace("_"," "))
    t.SetStats(False)
    t.SetLabelSize(0.05,"X")
    t.SetLabelSize(0.05,"Y")
    t.SetLineWidth(3)
    t.SetTickLength(0.03)
    if integ !=0 :
        t.Scale(1/integ)

    #hh signal    
    scale_factor = scale["hh"]
    h = hlist["afc"]["hh"][order]
    integ = h.Integral()
    h.SetLineWidth(3)
    h.SetLineColor(color_list["hh"])
    legend.SetTextSize(0.06)
    legend.AddEntry(h,"hh")
    h.SetLineWidth(3)
    h.SetTickLength(0.03)
    if integ !=0 :
        h.Scale(1/integ)
    
    if (order == "hig_top_2d") :
        t.Draw("colz")
        h.Draw("colz same")
    '''
    else :    
        t.Draw("hist")
        h.Draw("hist same")
    
    t.Draw("hist")
    h.Draw("hist same")
    if order != 'hig_top_2d':
        legend.Draw("same")
    '''
    cvs.SetTitle("after_cut_"+order)
    cvs.SaveAs("bf_af_plot/After_cut"+"_"+order+".png")

f.Close()
