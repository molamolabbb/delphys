#ifndef DELPHYS_ANALYSIS_RESOLVEDALYSER_H_
#define DELPHYS_ANALYSIS_RESOLVEDALYSER_H_

#include "delphys/analysis/interface/BaseAnalyser.h"

#include <numeric>


class ResolvedAnalyser : private BaseAnalyser {
 public:
  ResolvedAnalyser(const TString & in_path,
                   const TString & out_path,
                   const TString & out_tree_name);
  ~ResolvedAnalyser();
  void Loop();

 private:
  // inherited
  void MakeBranch();
  void Reset();
  Bool_t SelectEvent();
  void AnalyseEvent();

  std::vector<const Jet*> SelectJet();
  std::vector<const Jet*> selected_jets_;

  Bool_t TrackBottomQuark(const GenParticle* p);
  Float_t GetBDaughterRatio(const Jet* jet);

  // per event
  Int_t label_;

  // per jet
  Int_t num_jet_;
  std::vector<TLorentzVector> jet_p4_;
  std::vector<Int_t> jet_num_chad_;
  std::vector<Int_t> jet_num_nhad_;
  std::vector<Int_t> jet_num_electron_;
  std::vector<Int_t> jet_num_muon_;
  std::vector<Int_t> jet_num_photon_;

  std::vector<Float_t> jet_major_axis_;
  std::vector<Float_t> jet_minor_axis_;
  std::vector<Float_t> jet_eccentricity_;
  std::vector<Float_t> jet_ptd_; // jet energy sharing variable = L2 norm / L1 norm

  // 0: false / 1: true
  std::vector<Int_t> jet_b_tag_;
  std::vector<Int_t> jet_b_dr_matching_;
  std::vector<Float_t> jet_b_tracking_;

  // per constituents
  std::vector<Int_t> num_constituent_;
  std::vector<std::vector<TLorentzVector> > constituent_p4_;
  std::vector<std::vector<Int_t> > constituent_pid_;
  std::vector<std::vector<Int_t> > constituent_type_;

  // constants
  const Double_t kBMatchingDeltaR_ = 0.3;

  const Int_t kChargedHadronType_ = 1;
  const Int_t kNeutralHadronType_ = 2;
  const Int_t kElectronType_ = 3;
  const Int_t kMuonType_ = 4;
  const Int_t kPhotonType_ = 5;

  const Int_t kElectronPID_ = 11;
  const Int_t kMuonPID_ = 13;
  const Int_t kWrongPID_ = std::numeric_limits<Int_t>::max();

};



#endif //  DELPHYS_ANALYSIS_RESOLVEDALYSER_H_
