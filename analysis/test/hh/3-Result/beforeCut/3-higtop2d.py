import ROOT, math
from ROOT import *
import datetime
import os


f_bg = TFile("background_higtop_2d.root")
f_sig = TFile("distribution.root")

cvs = TCanvas()
cvs.SetGrid()
cvs.SetLogy()
cvs.SetLeftMargin(0.15)
cvs.SetBottomMargin(0.12)

hist_bg = f_bg.Get("backgrounds hig top 2d")
hist_sig = f_sig.Get("hh bfc hig top 2d")

# Plotting Before Cut
t = hist_bg
integ = t.Integral()
t.SetLineWidth(3)
t.SetTitle("higgsness topness 2D background")
t.SetStats(False)
t.SetLineWidth(3)
t.SetTickLength(0.03)
if integ !=0 :
   t.Scale(1/integ)
t.Draw("colz")
t.GetXaxis().SetTitle("log(T) Normalized");
t.GetYaxis().SetTitle("log(H) Normalized");

cvs.SetTitle("higgsness topness background")
now = datetime.datetime.now()
nowDate = now.strftime('%Y%m%d')
if not os.path.isdir("plots/"+nowDate):
  os.mkdir("plots/"+nowDate)
cvs.SaveAs("plots/"+nowDate+"/higtop2d_background.png")
  
#hh signal event
h = hist_sig
h.SetLineWidth(3)
h.SetLineWidth(3)
h.SetStats(False)
h.SetTickLength(0.03)
h.SetTitle("higgsness topness 2D signal")
integ = h.Integral()
if integ !=0 :
    h.Scale(1/integ)
h.GetXaxis().SetTitle("log(T) normalized")
h.GetYaxis().SetTitle("log(H) normalized")
h.Draw("colz")
cvs.SetTitle("higgsness topness signal")
now = datetime.datetime.now()
nowDate = now.strftime('%Y%m%d')
if not os.path.isdir("plots/"+nowDate):
  os.mkdir("plots/"+nowDate)
cvs.SaveAs("plots/"+nowDate+"/higtop2d_signal.png")


f_sig.Close()
f_bg.Close()
