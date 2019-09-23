#include "delphys/analysis/interface/TTAllJetsSelector.h"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Usage: selectTTAllJets <path/to/input.root> <path/to/output.root>"
              << std::endl;
  }

  TString in_path(argv[1]);
  TString out_path(argv[2]);

  TTAllJetsSelector analyser(in_path, out_path);
  analyser.loop();

  return 0;
}
