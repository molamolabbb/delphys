#ifndef DELPHYS_ANALYSIS_TTALLJETSANALYSER_H_
#define DELPHYS_ANALYSIS_TTALLJETSANALYSER_H_

#include "delphys/analysis/interface/BaseAnalyser.h"

#include "TH1F.h"

#include <numeric>


class MyLepton {
 public:
  MyLepton(const Electron* electron) {
    PT = electron->PT;
    Eta = electron->Eta;
    Phi = electron->Phi;
    Charge = electron->Charge;
    PID = electron->Charge * -11;
    Type = 1;
  }

  MyLepton(const Muon* muon) {
    PT = muon->PT;
    Eta = muon->Eta;
    Phi = muon->Phi;
    Charge = muon->Charge;
    PID = muon->Charge * -13;
    Type = 2;
  }

  Float_t PT;
  Float_t Eta;
  Float_t Phi;
  Int_t Charge;
  Int_t PID;
  Int_t Type;
};



class TopPolarizationAnalyser : private BaseAnalyser {
 public:
  TopPolarizationAnalyser(const TString & in_path,
                          const TString & out_path,
                          const TString & helicity,
                          Float_t mass_point);

  ~TopPolarizationAnalyser();
  void loop();

 private:
  // inherited
  void makeBranch();
  void resetBranch();
  void resetMemberData();

  Bool_t selectEvent();
  void analyse();

  Bool_t isDileptonic();
  Bool_t selectElectron(const Electron *);
  Bool_t selectMuon(const Muon *);
  Bool_t selectJet(const Jet *);

  //////////////////////////////////////////////////////////////////////////////
  // NOTE branches
  //////////////////////////////////////////////////////////////////////////////
  // do not reset
  Float_t b_m_; // mass point
  Float_t b_helicity_;

  std::vector<Float_t> b_lep_pt_;
  std::vector<Float_t> b_lep_eta_;
  std::vector<Float_t> b_lep_phi_;
  std::vector<Int_t>   b_lep_pid_;
  std::vector<Int_t>   b_lep_charge_;
  std::vector<Int_t>   b_lep_type_;

  std::vector<Float_t> b_jet_pt_;
  std::vector<Float_t> b_jet_eta_;
  std::vector<Float_t> b_jet_phi_;
  std::vector<Float_t> b_jet_m_;
  std::vector<Int_t>   b_jet_flavour_;
  std::vector<Int_t>   b_jet_btag_;

  std::vector<std::vector<Float_t> > b_dau_jet_pt_;
  std::vector<std::vector<Float_t> > b_dau_jet_eta_;
  std::vector<std::vector<Float_t> > b_dau_jet_phi_;
  std::vector<std::vector<Int_t> >   b_dau_jet_pid_;
  std::vector<std::vector<Int_t> >   b_dau_jet_charge_;
  std::vector<std::vector<Int_t> >   b_dau_jet_type_;

  Float_t b_met_;
  Float_t b_met_eta_;
  Float_t b_met_phi_;

  Float_t b_ht_;
  Float_t b_ht_event_;

  std::vector<Float_t> b_lep_ratio_;
  std::vector<Float_t> b_jet_ratio_;

  std::vector<Int_t> b_njetcons_;

  //////////////////////////////////////////////////////////////////////////////
  // NOTE member data
  //////////////////////////////////////////////////////////////////////////////
  std::vector<const MyLepton*> selected_leptons_;
  std::vector<const Jet*>      selected_jets_;

  TH1F* cutflow_;


  //////////////////////////////////////////////////////////////////////////////
  // NOTE selection cut
  //////////////////////////////////////////////////////////////////////////////

  Float_t kElectronMinPT_ = 20.0f;
  Float_t kElectronMaxEta_ = 2.4f;
  Float_t kMuonMinPT_ = 20.0f;
  Float_t kMuonMaxEta_ = 2.4f;
  Float_t kJetMinPT_ = 30.0f;
  Float_t kJetMaxEta_ = 2.4f;

  // NOTE Object Selection

  // NOTE Offline Selection

};



#endif //  DELPHYS_ANALYSIS_TTALLJETSANALYSER_H_
