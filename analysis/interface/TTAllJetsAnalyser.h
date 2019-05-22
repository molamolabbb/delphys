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
                    Int_t label);

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

  // Bool_t trackBottomQuark(const GenParticle* p);
  // Float_t getBDaughterRatio(const Jet* jet);
  Int_t getPFIndex(Int_t pid, Int_t charge);


  std::vector<const Jet*> selected_jets_;

  const GenParticle* top_quark_;
  const GenParticle* anti_top_quark_;

  // partons from top / anti-top quark
  std::vector<const GenParticle*> partons_t_;
  std::vector<const GenParticle*> partons_tbar_;

  std::map<const GenParticle*, const Jet*> parton2jet_;
  std::map<const Jet*, const GenParticle*> jet2parton_;
  std::map<const Jet*, Int_t> jet_ttbar_assignment_;


  //////////////////////////////////////////////////////////////////////////////
  // NOTE Branches
  //////////////////////////////////////////////////////////////////////////////
  // per event
  // NOTE it doesn't follow the CMSSW naming rule to be consistent with other 
  Int_t b_label_;

  // unordered set
  Int_t b_num_eflow_;
  Int_t b_num_track_;
  Int_t b_num_tower_;

  std::vector<Float_t> b_eflow_pt_;
  std::vector<Float_t> b_eflow_eta_;
  std::vector<Float_t> b_eflow_phi_;
  std::vector<Int_t>   b_eflow_charge_;
  std::vector<Int_t>   b_eflow_pid_; // PDG id
  std::vector<Int_t>   b_eflow_type_; // index for Embedding

  std::vector<Float_t> b_track_pt_;
  std::vector<Float_t> b_track_eta_;
  std::vector<Float_t> b_track_phi_;
  std::vector<Float_t> b_track_ctg_theta_;
  std::vector<Int_t>   b_track_charge_;
  std::vector<Int_t>   b_track_pid_;
  std::vector<Int_t>   b_track_type_;
  // NOTE
  // https://cp3.irmp.ucl.ac.be/projects/delphes/wiki/WorkBook/RootTreeDescription
  // https://github.com/delphes/delphes/blob/190cfa0452684435bbe0070f6460421bd036a836/classes/DelphesClasses.h#L441-L469
  /*
  std::vector<Float_t> b_track_eta_outer_;
  std::vector<Float_t> b_track_phi_outer_;
  std::vector<Float_t> b_track_t_; 
  std::vector<Float_t> b_track_x_;
  std::vector<Float_t> b_track_y_;
  std::vector<Float_t> b_track_z_;
  std::vector<Float_t> b_track_t_outer_;
  std::vector<Float_t> b_track_x_outer_;
  std::vector<Float_t> b_track_y_outer_;
  std::vector<Float_t> b_track_z_outer_;
  std::vector<Float_t> b_track_xd_;
  std::vector<Float_t> b_track_yd_;
  std::vector<Float_t> b_track_zd_;
  std::vector<Float_t> b_track_l_;
  */
  std::vector<Float_t> b_track_dz_;
  std::vector<Float_t> b_track_d0_;
  /*
  std::vector<Float_t> b_track_error_p_;
  std::vector<Float_t> b_track_error_pt_;
  std::vector<Float_t> b_track_error_phi_;
  std::vector<Float_t> b_track_error_ctg_theta_;
  std::vector<Float_t> b_track_error_t_;
  std::vector<Float_t> b_track_error_d0_;
  std::vector<Float_t> b_track_error_dz_;
  */
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

  // std::vector<Bool_t> b_jet_b_dr_matching_;
  // std::vector<Bool_t> b_jet_b_tracking_;

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
  // https://arxiv.org/abs/1812.10534
  // Measurement of the top quark mass in the all-jets final state at
  // \sqrt(s) = 13 TeV and combination with the lepton+jets channel
  //////////////////////////////////////////////////////////////////////////////
  const Float_t kMinJetPT_ = 30.0f;
  const Float_t kMaxJetEta_ = 2.4f;

  // NOTE HLT
  const Float_t kHLTMinNumJets_ = 6;
  const Float_t kHLTMinHT_ = 450.0f;
  const Float_t kHLTMinNumBJets_= 1;

  // NOTE Offline Selection
  // vertex z 24cm, xy 2cm
  const UInt_t kMinNumJet_ = 6; // PF jets
  const Int_t kMinNumBJet_ = 2;
  const Float_t kMinSixthJetPT_ = 40.0f; // GeV
  // kMinHT_: hadronic activity
  // It is the scalar p_T sum of all jets in the event
  const Float_t kMinHT_ = 450.0f; // GeV
  const Float_t kMaxDeltaRTwoBJets = 2.0;

  // my cuts
  const Float_t kBMatchingDeltaR_ = 0.3;
};



#endif //  DELPHYS_ANALYSIS_TTALLJETSANALYSER_H_
