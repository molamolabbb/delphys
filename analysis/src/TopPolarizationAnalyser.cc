#include "delphys/analysis/interface/TopPolarizationAnalyser.h"

#include "TString.h"
#include "TSystem.h"
#include "TInterpreter.h"

#include <algorithm>
#include <numeric>
#include <iterator>
#include <memory>

using namespace delphys;

TopPolarizationAnalyser::TopPolarizationAnalyser(const TString & in_path,
                                                 const TString & out_path,
                                                 const TString & helicity,
                                                 Float_t mass_point)
    : b_m_(mass_point) {
  std::cout << "ctor begin" << std::endl;

  if (helicity.EqualTo("left")) {
    b_helicity_ = 0;
  } else if (helicity.EqualTo("right")) {
    b_helicity_ = 1;
  } else {
    std::cout << helicity << " is wrong. it has to be left or right" << std::endl;
    std::exit(1);
  }

  std::cout << helicity << std::endl;

  in_file_ = TFile::Open(in_path, "READ");
  in_tree_ = dynamic_cast<TTree*>(in_file_->Get("Delphes"));

  out_file_ = TFile::Open(out_path, "RECREATE");
  out_tree_ = new TTree(helicity, helicity);
  out_tree_->SetDirectory(out_file_);

  std::cout << "setBranchAddress begin" << std::endl;
  setBranchAddress({"Vertex"}, /*drop=*/true);
  std::cout << "setBranchAddress end" << std::endl;

  std::cout << "makeBranch begin" << std::endl;
  makeBranch();
  std::cout << "makeBranch end" << std::endl;

  TString cutflow_name = "cutflow_" + helicity;
  if (b_helicity_ == 0) {
    cutflow_name += "left";
  } else if (b_helicity_ == 1) {
    cutflow_name += "right";
  }

  // cutflow_ = std::make_shared<TH1F>(cutflow_name, cutflow_title, 6, -0.5, 5.5);
  cutflow_ = new TH1F(cutflow_name, cutflow_name, 6, -0.5, 5.5);
  cutflow_->SetDirectory(out_file_);
  auto x_axis = cutflow_->GetXaxis();
  x_axis->SetBinLabel(1, "initial");
  x_axis->SetBinLabel(2, "Is dileptonic");
  x_axis->SetBinLabel(3, "N_{leptons} #ge 2");
  x_axis->SetBinLabel(4, "leptons OS");
  x_axis->SetBinLabel(5, "N_{jets} #ge 2");
  x_axis->SetBinLabel(6, "N_{b-jets} #ge 2");

  std::cout << "ctor end" << std::endl;
}


TopPolarizationAnalyser::~TopPolarizationAnalyser() {
  std::cout << "dtor begin" << std::endl;
  out_tree_->Print();

  out_file_->Write();
  out_file_->Close();

  std::cout << "dtor end" << std::endl;
}

void TopPolarizationAnalyser::makeBranch() {

  resetBranch();
  gInterpreter->GenerateDictionary("vector<vector<Int_t> >", "vector"); 
  gInterpreter->GenerateDictionary("vector<vector<Float_t> >", "vector"); 

  #define BRANCH_(name, suffix) out_tree_->Branch(#name, & b_##name##_ , #name "/" #suffix);
  #define BRANCH_I(name) BRANCH_(name, I);
  #define BRANCH_F(name) BRANCH_(name, F);
  #define BRANCH_O(name) BRANCH_(name, O);

  #define BRANCH_A_(name, size, suffix) out_tree_->Branch(#name, & b_##name##_ , #name"["#size"]/"#suffix);
  #define BRANCH_AF(name, size)  BRANCH_A_(name, size, F);

  #define BRANCH_V_(name, type) out_tree_->Branch(#name, "vector<"#type">", & b_##name##_ );
  #define BRANCH_VF(name) BRANCH_V_(name, Float_t);
  #define BRANCH_VI(name) BRANCH_V_(name, Int_t);
  #define BRANCH_VO(name) BRANCH_V_(name, Bool_t);

  #define BRANCH_ANY(name) out_tree_->Branch(#name, & b_##name##_ );

  BRANCH_F(m);
  BRANCH_F(helicity);

  BRANCH_VF(lep_pt);
  BRANCH_VF(lep_eta);
  BRANCH_VF(lep_phi);
  BRANCH_VI(lep_charge);
  BRANCH_VI(lep_pid);
  BRANCH_VI(lep_type);

  BRANCH_VF(jet_pt);
  BRANCH_VF(jet_eta);
  BRANCH_VF(jet_phi);
  BRANCH_VF(jet_m);
  BRANCH_VI(jet_flavour);
  BRANCH_VI(jet_btag);

  BRANCH_ANY(dau_jet_pt);
  BRANCH_ANY(dau_jet_eta);
  BRANCH_ANY(dau_jet_phi);
  BRANCH_ANY(dau_jet_pid);
  BRANCH_ANY(dau_jet_charge);
  BRANCH_ANY(dau_jet_type);

  BRANCH_F(met);
  BRANCH_F(met_eta);
  BRANCH_F(met_phi);

  BRANCH_F(ht);
  BRANCH_F(ht_event);

  BRANCH_VF(lep_ratio);
  BRANCH_VF(jet_ratio);

  BRANCH_VI(njetcons);

  return ;
}


void TopPolarizationAnalyser::resetBranch() {
  // CAUTION do not reset m, helicity
  
  b_lep_pt_.clear();
  b_lep_eta_.clear();
  b_lep_phi_.clear();
  b_lep_pid_.clear();
  b_lep_charge_.clear();
  b_lep_type_.clear();

  b_jet_pt_.clear();
  b_jet_eta_.clear();
  b_jet_phi_.clear();
  b_jet_m_.clear();
  b_jet_flavour_.clear();
  b_jet_btag_.clear();

  b_dau_jet_pt_.clear();
  b_dau_jet_eta_.clear();
  b_dau_jet_phi_.clear();
  b_dau_jet_pid_.clear();
  b_dau_jet_charge_.clear();
  b_dau_jet_type_.clear();

  b_met_ = -100.0f;
  b_met_eta_ = -100.0f;
  b_met_phi_ = -100.0f;

  b_ht_ = -100.0f;
  b_ht_event_ = -100.0f;

  b_lep_ratio_.clear();
  b_jet_ratio_.clear();

  b_njetcons_.clear();

  return ;
}


void TopPolarizationAnalyser::resetMemberData() {
  selected_leptons_.clear();
  selected_jets_.clear();

  return ;
}


bool TopPolarizationAnalyser::isDileptonic() {
  UInt_t num_leptons = 0;

  for (Int_t idx = 0; idx < particles_->GetEntries(); idx++) {

    auto gen = dynamic_cast<const GenParticle*>(particles_->At(idx));

    Int_t abs_pid = std::abs(gen->PID);
    if ((abs_pid != 11) and (abs_pid != 13)) continue;
    if (gen->M1 == -1) continue;

    auto mother = dynamic_cast<const GenParticle*>(particles_->At(gen->M1));
    if (std::abs(mother->PID) != 24) continue;

    num_leptons++;
  }

  // FIXME
  bool is_dileptonic = num_leptons == 2;
  return is_dileptonic;
}


bool TopPolarizationAnalyser::selectElectron(const Electron* electron) {
  if (electron->PT < kElectronMinPT_)             return false;
  if (std::abs(electron->Eta) > kElectronMaxEta_) return false;

  return true;
}


bool TopPolarizationAnalyser::selectMuon(const Muon* muon) {
  if (muon->PT < kMuonMinPT_)             return false;
  if (std::abs(muon->Eta) > kMuonMaxEta_) return false;

  return true;
}


bool TopPolarizationAnalyser::selectJet(const Jet* jet) {
  if (jet->PT < kJetMinPT_)             return false;
  if (std::abs(jet->Eta) > kJetMaxEta_) return false;

  return true;
}



Bool_t TopPolarizationAnalyser::selectEvent() {
  cutflow_->Fill(0);

  // NOTE 
  if (not isDileptonic()) return false;
  cutflow_->Fill(1);

  // NOTE 
  for (Int_t idx = 0; idx < muons_->GetEntries(); idx++) { 
    auto muon = dynamic_cast<const Muon*>(muons_->At(idx));
    if (selectMuon(muon)) {
      const MyLepton* lepton = new MyLepton(muon);
      selected_leptons_.push_back(lepton);
    }
  }


  for (Int_t idx = 0; idx < electrons_->GetEntries(); idx++) {
    auto electron = dynamic_cast<const Electron*>(electrons_->At(idx));
    if (selectElectron(electron)) {
      const MyLepton* lepton = new MyLepton(electron);
      selected_leptons_.push_back(lepton);
    }
  }

  if (selected_leptons_.size() < 2) return false;
  cutflow_->Fill(2);


  // FIXME seungjin: I think we need to find best two lepton pair. not two first leptons
  if (selected_leptons_[0]->Charge * selected_leptons_[1]->Charge > 0) return false;
  cutflow_->Fill(3);

  UInt_t num_jets = 0;
  UInt_t num_bjets = 0;

  // NOTE object - selection - jet
  for (Int_t idx = 0; idx < jets_->GetEntries(); idx++) { 
    auto jet = dynamic_cast<const Jet*>(jets_->At(idx));
    if (not selectJet(jet)) continue;

    selected_jets_.push_back(jet);
    num_jets++;

    if (jet->BTag) num_bjets++;
  }


  if (num_jets < 2) return false;
  cutflow_->Fill(4);


  if (num_bjets < 2) return false;
  cutflow_->Fill(5);


  return true;
}


void TopPolarizationAnalyser::analyse() {
  // NOTE leptons
  for (const auto & each : selected_leptons_) {
    b_lep_pt_.push_back(each->PT);
    b_lep_eta_.push_back(each->Eta);
    b_lep_phi_.push_back(each->Phi);
    b_lep_pid_.push_back(each->PID);
    b_lep_charge_.push_back(each->Charge);
    b_lep_type_.push_back(each->Type);
  }

  // NOTE jets
  for (const auto & jet : selected_jets_) {
    b_jet_pt_.push_back(jet->PT);
    b_jet_eta_.push_back(jet->Eta);
    b_jet_phi_.push_back(jet->Phi);
    b_jet_m_.push_back(jet->Mass);
    b_jet_flavour_.push_back(jet->Flavor);
    b_jet_btag_.push_back(jet->BTag);

    // NOTE constituents
    b_dau_jet_pt_.push_back(std::vector<Float_t>());
    b_dau_jet_eta_.push_back(std::vector<Float_t>());
    b_dau_jet_phi_.push_back(std::vector<Float_t>());
    b_dau_jet_charge_.push_back(std::vector<Int_t>());
    b_dau_jet_pid_.push_back(std::vector<Int_t>());
    b_dau_jet_type_.push_back(std::vector<Int_t>());
    
    for (Int_t con_idx = 0; con_idx < jet->Constituents.GetEntries(); con_idx++) {
      auto constituent = jet->Constituents.At(con_idx);

      Float_t pt  = -1.0f;
      Float_t eta = -77.0f;
      Float_t phi = -77.0f;
      Int_t charge = 77;
      Int_t pid = 0;
      Int_t particle_type = 0;

      if (auto track = dynamic_cast<const Track*>(constituent)) {
        pt  = track->PT;
        eta = track->Eta;
        phi = track->Phi;
        pid = track->PID;
        charge = track->Charge;

        Int_t abs_pid = std::abs(pid);
        if      (abs_pid == 11) particle_type = 1;
        else if (abs_pid == 13) particle_type = 2;
        else                    particle_type = 3;

      } else if (auto tower = dynamic_cast<const Tower*>(constituent)) {
        pt  = tower->ET;
        eta = tower->Eta;
        phi = tower->Phi;
        charge = 0;

        if (tower->Eem == 0.0) {
          // neutral hadron
          pid = 0;
          particle_type = 4;
        } else if (tower->Ehad == 0.0) {
          // photon
          pid = 22;
          particle_type = 5;
        } else {
          std::cerr << "[[ERROR]]: Tower with Had " << tower->Ehad << " and EM "
                    << tower->Eem << " energy" << std::endl;
        }
      } else {
        std::cerr << "[[ERROR]]: BAD DAUGHTER! " << constituent << std::endl;
      }

      b_dau_jet_pt_.back().push_back(pt);
      b_dau_jet_eta_.back().push_back(eta);
      b_dau_jet_phi_.back().push_back(phi);
      b_dau_jet_charge_.back().push_back(charge);
      b_dau_jet_pid_.back().push_back(pid);
      b_dau_jet_type_.back().push_back(particle_type);

      b_njetcons_.push_back(b_dau_jet_pt_.back().size());

    } // constituent loop
  } // jet loop

  // NOTE MET
  auto missing_et = dynamic_cast<const MissingET*>(mets_->At(0));
  b_met_     = missing_et->MET;
  // b_met_eta_ = missing_et->Eta;
  b_met_eta_ = 0.0f;
  b_met_phi_ = missing_et->Phi;

  // ScalarHT
  b_ht_ = b_lep_pt_[0] + b_lep_pt_[1] + b_jet_pt_[0] + b_jet_pt_[1];

  auto scalar_ht = dynamic_cast<const ScalarHT*>(scalar_hts_->At(0));
  b_ht_event_ = scalar_ht->HT;

  b_lep_ratio_.push_back(b_lep_pt_[0] / b_ht_);
  b_lep_ratio_.push_back(b_lep_pt_[1] / b_ht_);

  for (const auto & each : b_jet_pt_) {
    b_jet_ratio_.push_back(each / b_ht_);
  }

  out_tree_->Fill();
}



void TopPolarizationAnalyser::loop() {
  const Int_t kNumTotal = in_tree_->GetEntries();
  const Int_t kPrintFreq = kNumTotal / 20;
  TString msg_fmt = TString::Format("[%s/%d (%s %%)]", "%d", kNumTotal, "%.2f");

  for (Long64_t entry=0; entry < in_tree_->GetEntries(); entry++) {
    in_tree_->GetEntry(entry);

    if ((entry == 0) or (entry % kPrintFreq == 0)) {
      Float_t progress = 100 * Float_t(entry + 1) / kNumTotal;
      std::cout << TString::Format(msg_fmt, entry + 1, progress) << std::endl;
    }

    resetBranch();
    resetMemberData();

    if (selectEvent()) {
      analyse();
    }


  }
}
