#ifndef DELPHYS_ANALYSIS_TTALLJETSSELECTOR_H_
#define DELPHYS_ANALYSIS_TTALLJETSSELECTOR_H_

#include "delphys/analysis/interface/BaseAnalyser.h"

#include "TH1F.h"

namespace pdgid {
  static const Int_t kBottom = 5;
  static const Int_t kTop = 6;
  static const Int_t kElectron = 11;
  static const Int_t kMuon = 13;
  static const Int_t kTau = 15;
  static const Int_t kPhoton = 22;
  static const Int_t kWBoson = 24;
} // pdgid

namespace p8status {
  static const Int_t kTop = 62;
} // p8status


class TTAllJetsSelector : private BaseAnalyser {
 public:
  TTAllJetsSelector(const TString & in_path,
                    const TString & out_path);
  ~TTAllJetsSelector();
  void loop();

 private:
  Int_t checkDecayChannel(const GenParticle* top);

  Bool_t selectEvent();
  void analyse();

  void bookHistograms();

  TH1F* h_decay_channel_;
  TH1F* h_decay_channel_detail_;

};

#endif //  DELPHYS_ANALYSIS_TTALLJETSSELECTOR_H_
