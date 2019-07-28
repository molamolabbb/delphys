#include "delphys/analysis/interface/TopPolarizationAnalyser.h"

#include "TSystem.h"

int main(int argc, char* argv[]) {
  // TODO argument parser
  if (argc != 5) {
    std::cerr << "Usage: analyseTopPolarization inputFile outputFile helicity mass_point" << std::endl;
    return 1;
  }

  TString in_path(argv[1]);
  TString out_path(argv[2]);
  TString helicity(argv[3]);
  Float_t mass_point = std::atoi(argv[4]);

  TopPolarizationAnalyser analyser(in_path, out_path, helicity, mass_point);
  analyser.loop();

  return 0;
}
