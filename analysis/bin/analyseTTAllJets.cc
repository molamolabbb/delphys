#include "delphys/analysis/interface/TTAllJetsAnalyser.h"

#include "TSystem.h"

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << "Usage: analyseTTAllJets inputFile outputFile label" << std::endl;
    std::cerr << "Usage: analyseTTAllJets inputFile outputFile label" << std::endl;
    return 1;
  }

  TString in_path(argv[1]);
  TString out_path(argv[2]);
  Int_t label = std::atoi(argv[3]);

  TString in_name = gSystem->BaseName(in_path);

  TTAllJetsAnalyser analyser(in_path, out_path, label);
  analyser.loop();

  return 0;
}
