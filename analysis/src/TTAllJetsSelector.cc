#include "delphys/analysis/interface/TTAllJetsSelector.h"

#include "TString.h"
#include "TSystem.h"
#include "TLorentzVector.h"

#include <algorithm>
#include <iterator>
#include <queue>


TTAllJetsSelector::TTAllJetsSelector(const TString & in_path,
                                     const TString & out_path) :
    BaseAnalyser(in_path, out_path) {
  std::cout << "begin ctor" << std::endl;

  setBranchAddress();

  bookHistograms();


  std::cout << "end ctor" << std::endl;
}


TTAllJetsSelector::~TTAllJetsSelector() {
  std::cout << "dtor" << std::endl;
  out_file_->Write();
  out_file_->Close();
  std::cout << "dtor end" << std::endl;
}

void TTAllJetsSelector::bookHistograms() {
  h_decay_channel_ = new TH1F("decay_channel", "Decay Channel", 3, -0.5, 2.5);
  TAxis* x_axis = h_decay_channel_->GetXaxis();
  x_axis->SetBinLabel(1, "all-jets");
  x_axis->SetBinLabel(2, "lepton+jets");
  x_axis->SetBinLabel(3, "dilepton");

  h_decay_channel_detail_ = new TH1F("decay_channel_detail", "Decay Channel",
                                     10, -0.5, 9.5);
  x_axis = h_decay_channel_detail_->GetXaxis();
  // all-jets
  x_axis->SetBinLabel(1, "all-jets");

  // lepton+jets
  x_axis->SetBinLabel(2, "#e+jets");
  x_axis->SetBinLabel(3, "#mu+jets");
  x_axis->SetBinLabel(4, "#tau+jets");

  // dilepton
  x_axis->SetBinLabel(5, "e+e");
  x_axis->SetBinLabel(6, "e+#mu");
  x_axis->SetBinLabel(7, "e+#tau");

  x_axis->SetBinLabel(8, "#mu+#mu");
  x_axis->SetBinLabel(9, "#mu+#tau");

  x_axis->SetBinLabel(10, "#tau+#tau");

}


Int_t TTAllJetsSelector::checkDecayChannel(const GenParticle* top) {
  // NOTE if ((std::abs(top->PID) == 6) or (top->Status == 62))


  // NOTE D1 - quark / D2 is W boson
  // auto quark = dynamic_cast<const GenParticle*>(particles_->At(top->D1));

  auto w_boson = dynamic_cast<const GenParticle*>(particles_->At(top->D2));

  while (w_boson->D1 == w_boson->D2) {
    w_boson = dynamic_cast<const GenParticle*>(particles_->At(w_boson->D1));
  }

  if (std::abs(w_boson->PID) != 24) {
    std::cerr << "WRONG PID: " << w_boson->PID << std::endl;
  }

  // NOTE if W boson decays leptonically, D1 is the neutrino and D2 is the lepton.

  auto dau = dynamic_cast<const GenParticle*>(particles_->At(w_boson->D1));
  Int_t abs_pid = std::abs(dau->PID);

  Int_t code = -1;

  if      ((abs_pid >= 1)  and (abs_pid <= 5))  code = 0;
  else if ((abs_pid == 11) or  (abs_pid == 12)) code = 1;
  else if ((abs_pid == 13) or  (abs_pid == 14)) code = 10;
  else if ((abs_pid == 15) or  (abs_pid == 16)) code = 100;

  if (code == -1) {
    std::cerr << "Wrong PID of daughter of W boson: " << dau->PID << std::endl;
    std::exit(1);
  }

  return code;
}


Bool_t TTAllJetsSelector::selectEvent() {
  const GenParticle* top = nullptr;
  const GenParticle* anti_top = nullptr;

  for (Int_t idx = 0; idx < particles_->GetEntries(); idx++) {
    auto gen = dynamic_cast<const GenParticle*>(particles_->At(idx));

    if ((gen->PID == 6) and (gen->Status == p8status::kTop)) {
      top = gen;
      continue;
    } else if ((gen->PID == -6) and (gen->Status == p8status::kTop)) {
      anti_top = gen;
      continue;
    }

    if ((top != nullptr) and (anti_top != nullptr)) break;

  }

  if ((top == nullptr) or (anti_top == nullptr)) {
    std::cout << "SOMETHING WRONG" << std::endl;
    std::exit(1);
  }

  Int_t top_code = checkDecayChannel(top);
  Int_t anti_top_code = checkDecayChannel(anti_top);
  Int_t event_code = top_code + anti_top_code;

  switch (event_code) {
    case 0:
      // all-jets
      h_decay_channel_->Fill(0);
      h_decay_channel_detail_->Fill(0);
      break;
    case 1:
      // e + jets;
      h_decay_channel_->Fill(1);
      h_decay_channel_detail_->Fill(1);
      break;
    case 10:
      // mu + jets;
      h_decay_channel_->Fill(1);
      h_decay_channel_detail_->Fill(2);
      break;
    case 100:
      // mu + jets;
      h_decay_channel_->Fill(1);
      h_decay_channel_detail_->Fill(3);
      break;
    case 2:
      // e + e
      h_decay_channel_->Fill(2);
      h_decay_channel_detail_->Fill(4);
      break;
    case 11:
      // e + mu
      h_decay_channel_->Fill(2);
      h_decay_channel_detail_->Fill(5);
      break;
    case 101:
      // e + tau
      h_decay_channel_->Fill(2);
      h_decay_channel_detail_->Fill(6);
      break;
    case 20:
      // mu + mu
      h_decay_channel_->Fill(2);
      h_decay_channel_detail_->Fill(7);
      break;
    case 110:
      // mu + tau
      h_decay_channel_->Fill(2);
      h_decay_channel_detail_->Fill(8);
      break;
    case 200:
      // tau + tau
      h_decay_channel_->Fill(2);
      h_decay_channel_detail_->Fill(9);
      break;
    default:
      std::cerr << "WRONG EVENT CODE: " << event_code << std::endl;
      std::exit(1);
  }

  bool is_alljets = event_code == 0;
  return is_alljets;
}



void TTAllJetsSelector::analyse() {
  out_tree_->Fill();
}

void TTAllJetsSelector::loop() {
  const Int_t kNumEntries = in_tree_->GetEntries();
  const Int_t kPrintFreq = kNumEntries / 100;
  const TString kMsgFmt = "[%d/%d (%.2f %)] # of passed events: %d (%.2f %)";

  Int_t num_passed = 0;

  for (Long64_t entry=0; entry < in_tree_->GetEntries(); entry++) {
    in_tree_->GetEntry(entry);

    if (entry % kPrintFreq == 0) {
      Float_t progress = 100 * static_cast<Float_t>(entry + 1) / kNumEntries;
      Float_t eff = 100 * static_cast<Float_t>(num_passed) / (entry + 1);

      auto msg = TString::Format(kMsgFmt, entry + 1, kNumEntries, progress,
                                 num_passed, eff);
      std::cout << msg << std::endl;
    } 

    if (selectEvent()) {
      analyse();
      num_passed++;
    }
  }
}
