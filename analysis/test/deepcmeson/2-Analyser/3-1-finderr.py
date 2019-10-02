import os,sys
import ROOT

txt = './txtFiles/bfrm.txt'
with open(txt,'r') as txt_file:
  input_path_list = [each.rstrip("\n") for each in txt_file.readlines()]
zombie = 0

for input_path in input_path_list:
  root_file = ROOT.TFile(input_path)
  if root_file.IsZombie():
    zombie = zombie+1
    root_file.Close()
    del root_file

    print(input_path)
    command = "rm {}".format(input_path)
    os.system(command)
    
print("# of zombie : {}".format(zombie))
print("# of data : {}".format(len(input_path_list)))
