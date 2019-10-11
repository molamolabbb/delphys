#!/usr/bin/env python
import os
import pprint

pp = pprint.pprint
filename = './txtFiles/delphes.txt'
f = open(filename , "w")
a = []
pathlist=  [
            #"/xrootd/store/user/seyang/DeepCMeson/1-Delphes/"            
            "/xrootd/store/user/seyang/TTvsQCD/ip_btag/1-delphes/TTJets_aMC"     
]   
for path in pathlist:
    temp = os.listdir(path)
    temp = map(lambda p: os.path.join(path, p), temp) 
    a +=  temp
for i in a:
    newstr = str.replace(i, '/xrootd','root://cms-xrdr.private.lo:2094///xrd'
          # ' root://cms-xrdr.sdfarm.kr:1094///xrd'
            )
    f.write(newstr + "\n")

pp(newstr)
f.close()    
