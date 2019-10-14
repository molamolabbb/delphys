#!/usr/bin/env python
import os
import pprint

pp = pprint.pprint
filename = './txtFiles/doubleHiggs.txt'
f = open(filename , "w")
a = []
pathlist=  ["/xrootd/store/user/jua/doubleHiggs/sample/pp_hh/",
              "/xrootd/store/user/jua/doubleHiggs/sample/pp_llbj/",
              "/xrootd/store/user/jua/doubleHiggs/sample/pp_tatabb/",
              "/xrootd/store/user/jua/doubleHiggs/sample/pp_tt/",
              "/xrootd/store/user/jua/doubleHiggs/sample/pp_tt_ditau/",
              "/xrootd/store/user/jua/doubleHiggs/sample/pp_tt_leptau/",
              "/xrootd/store/user/jua/doubleHiggs/sample/pp_tth/",
              "/xrootd/store/user/jua/doubleHiggs/sample/pp_ttv/",
              "/xrootd/store/user/jua/doubleHiggs/sample/pp_twj/"
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
