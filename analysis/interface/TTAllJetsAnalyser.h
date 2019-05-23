#ifndef DELPHYS_ANALYSIS_TTALLJETSANALYSER_H_
#define DELPHYS_ANALYSIS_TTALLJETSANALYSER_H_

#include "delphys/analysis/interface/BaseAnalyser.h"

#include <numeric>

namespace pdgid {
  static const Int_t kBottom = 5;
  static const Int_t kTop = 6;
  static const Int_t kElectron = 11;
  static const Int_t kMuon = 13;
  static const Int_t kPhoton = 22;
  static const Int_t kWBoson = 24;

  static const Int_t kAntiElectron = -11;
  static const Int_t kAntiMuon = -13;

  // NOTE
  static const Int_t kWrong = std::numeric_limits<Int_t>::max();
  // In TDatabasePDG, 0 means Rootino, which indicates unidentified particle.
  static const Int_t kNeutralHadron = 0;

} // pdgid

// indices for ParticleFlow objects
// consider Make json file and then read from that file
// In deep learning framework like Keras, embedding layaer takes these indices
// as input.
namespace pfindex {
  static const Int_t kElectron = 1;
  static const Int_t kAntiElectron = 2;
  static const Int_t kMuon = 3;
  static const Int_t kAntiMuon = 4;
  static const Int_t kPositivelyChargedHadron = 5;
  static const Int_t kNegativelyChargedHadron = 6;
  static const Int_t kNeutralHadron = 7; 
  static const Int_t kPhoton = 8;
} //pfindex

// FIXME write the comment
namespace p8status {
  static const Int_t kBottom = 23;
  static const Int_t kTop = 62;
} // p8status



class TTAllJetsAnalyser : private BaseAnalyser {
 public:
  TTAllJetsAnalyser(const TString & in_path,
                    const TString & out_path,
                    Int_t label,
                    bool do_offline_selection);

  ~TTAllJetsAnalyser();
  void loop();

 private:
  // inherited
  void makeBranch();
  void resetBranch();
  void resetMemberData();


  Bool_t selectEvent();
  void analyse();

  void analyseEFlow();
  void analyseJet();

  std::vector<const GenParticle*> getDaughters(const GenParticle* mother); 
  std::vector<const GenParticle*> getPartons(const GenParticle* top); 
  Bool_t matchPartonsWithJets();

  Int_t getPFIndex(Int_t pid, Int_t charge);

  //////////////////////////////////////////////////////////////////////////////
  // NOTE member data
  //////////////////////////////////////////////////////////////////////////////

  std::vector<const Jet*> selected_jets_;

  const GenParticle* top_quark_;
  const GenParticle* anti_top_quark_;

  // partons from top / anti-top quark
  std::vector<const GenParticle*> partons_t_;
  std::vector<const GenParticle*> partons_tbar_;

  std::map<const GenParticle*, const Jet*> parton2jet_;
  std::map<const Jet*, const GenParticle*> jet2parton_;
  std::map<const Jet*, Int_t> jet_ttbar_assignment_;

  // NOTE stats
  UInt_t num_ambiguous_;

  //////////////////////////////////////////////////////////////////////////////
  // NOTE Branches
  //////////////////////////////////////////////////////////////////////////////
  // per event
  Int_t b_label_;
  Int_t b_num_eflow_;
  Int_t b_num_track_;
  Int_t b_num_tower_;

  // per EnergyFlow
  std::vector<Float_t> b_eflow_pt_;
  std::vector<Float_t> b_eflow_eta_;
  std::vector<Float_t> b_eflow_phi_;
  std::vector<Int_t>   b_eflow_charge_;
  std::vector<Int_t>   b_eflow_pid_; // PDG id
  std::vector<Int_t>   b_eflow_type_; // index for Embedding

  std::vector<Float_t> b_track_pt_;
  std::vector<Float_t> b_track_eta_;
  std::vector<Float_t> b_track_phi_;
  std::vector<Int_t>   b_track_charge_;
  std::vector<Int_t>   b_track_pid_;
  std::vector<Int_t>   b_track_type_;
  std::vector<Float_t> b_track_dz_;
  std::vector<Float_t> b_track_d0_;

  std::vector<Float_t> b_tower_pt_; // actually ET
  std::vector<Float_t> b_tower_eta_;
  std::vector<Float_t> b_tower_phi_;
  std::vector<Int_t> b_tower_charge_;
  std::vector<Int_t> b_tower_pid_;
  std::vector<Int_t> b_tower_type_;

  Float_t b_met_;
  Float_t b_met_eta_;
  Float_t b_met_phi_;

  // NOTE jet
  Int_t b_num_b_jets_; // b-jet multiplicity
  Int_t b_num_b_jets_6_; // b-jet multiplicity in the highest six jets

  std::vector<Float_t> b_jet_pt_;
  std::vector<Float_t> b_jet_eta_;
  std::vector<Float_t> b_jet_phi_;
  std::vector<Float_t> b_jet_mass_;

  std::vector<Int_t> b_jet_num_chad_;
  std::vector<Int_t> b_jet_num_nhad_;
  std::vector<Int_t> b_jet_num_electron_;
  std::vector<Int_t> b_jet_num_muon_;
  std::vector<Int_t> b_jet_num_photon_;

  // refer CMS-PAS-JME-13-002
  std::vector<Float_t> b_jet_major_axis_;
  std::vector<Float_t> b_jet_minor_axis_;
  std::vector<Float_t> b_jet_ptd_;

  // refer https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagMCTools#Legacy_parton_based_jet_flavour
  std::vector<Bool_t> b_jet_b_tag_;

  std::vector<Int_t> b_jet_ttbar_assignment_;
  std::vector<Int_t> b_jet_b_assignment_;

  // NOTE constituents of jet
  std::vector<std::vector<Float_t> > b_con_pt_;
  std::vector<std::vector<Float_t> > b_con_deta_;
  std::vector<std::vector<Float_t> > b_con_dphi_;
  std::vector<std::vector<Int_t> >   b_con_charge_;
  std::vector<std::vector<Int_t> >   b_con_pid_;
  std::vector<std::vector<Int_t> >   b_con_type_;

  //////////////////////////////////////////////////////////////////////////////
  // NOTE selection cut
  // Measurement of the top quark mass in the all-jets final state at
  // \sqrt{s} = 13 TeV and combination with the lepton+jets channel
  // https://link.springer.com/article/10.1140/epjc/s10052-019-6788-2
  //////////////////////////////////////////////////////////////////////////////
  // NOTE Object Selection
  const Float_t kMinJetPT_  = 30.0f;
  const Float_t kMaxJetEta_ = 2.4f;

  // NOTE HLT
  const UInt_t  kHLTMinNumJets_  = 6;
  const Float_t kHLTMinSixthJetPT_ = 40.0f; // GeV
  const Float_t kHLTMinHT_       = 450.0f;
  const UInt_t  kHLTMinNumBJets_ = 1; // b-tagged jets

  // NOTE Offline Selection
  const UInt_t  kOffMinNumJet_   = 6;
  const Float_t kOffMinSixthJetPT_ = 40.0f; // GeV
  const Float_t kOffMinHT_       = 450.0f; // GeV
  const UInt_t  kOffMinNumBJets_ = 2;
  const Float_t kOffMinDeltaRbb_ = 2.0f; // between two b jets

  const Bool_t kDoOfflineSelection_;

  // my cuts
  const Float_t kJetPartonMatchingDeltaR_ = 0.3;
};



#endif //  DELPHYS_ANALYSIS_TTALLJETSANALYSER_H_
