#include "delphys/analysis/interface/TTAllJetsAnalyser.h"

#include "TString.h"
#include "TSystem.h"
#include "TInterpreter.h"

#include <algorithm>
#include <numeric>
#include <iterator>
#include <memory>

using namespace delphys;

TTAllJetsAnalyser::TTAllJetsAnalyser(const TString & in_path,
                                     const TString & out_path,
                                     Int_t label,
                                     bool do_offline_selection)
    : BaseAnalyser(in_path, out_path, "delphys"),
      b_label_(label),
      kDoOfflineSelection_(do_offline_selection) {

  std::cout << "ctor begin" << std::endl;

  std::cout << "setBranchAddress begin" << std::endl;
  setBranchAddress({"Vertex"}, /*drop=*/true);
  std::cout << "setBranchAddress end" << std::endl;

  std::cout << "makeBranch begin" << std::endl;
  makeBranch();
  std::cout << "makeBranch end" << std::endl;

  // NOTE Initialise member data
  num_ambiguous_ = 0;

  std::cout << "ctor end" << std::endl;
}


TTAllJetsAnalyser::~TTAllJetsAnalyser() {
  std::cout << "dtor begin" << std::endl;
  out_tree_->Print();
  out_file_->Write();
  out_file_->Close();

  std::cout << "# of events having ambiguous jet-parton assignment: "
            << num_ambiguous_
            << std::endl;

  std::cout << "dtor end" << std::endl;
}

void TTAllJetsAnalyser::makeBranch() {

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

  BRANCH_I(label);

  // jet unordered set
  BRANCH_I(num_eflow);
  BRANCH_I(num_track);
  BRANCH_I(num_tower);

  // NOTE track
  BRANCH_VF(track_pt);
  BRANCH_VF(track_eta);
  BRANCH_VF(track_phi);

  BRANCH_VI(track_charge);
  BRANCH_VI(track_pid);
  BRANCH_VI(track_type);

  BRANCH_VF(track_dz);
  BRANCH_VF(track_d0);

  // NOTE tower
  BRANCH_VF(tower_pt);
  BRANCH_VF(tower_eta);
  BRANCH_VF(tower_phi);
  BRANCH_VI(tower_charge);
  BRANCH_VI(tower_pid);
  BRANCH_VI(tower_type);

  // NOTE MEET
  BRANCH_F(met);
  BRANCH_F(met_eta);
  BRANCH_F(met_phi);

  // NOTE Jet
  // jet variables
  BRANCH_I(num_b_jets);
  BRANCH_I(num_b_jets_6);
  BRANCH_VF(jet_pt);
  BRANCH_VF(jet_eta);
  BRANCH_VF(jet_phi);
  BRANCH_VF(jet_mass);

  BRANCH_VI(jet_num_chad);
  BRANCH_VI(jet_num_nhad);
  BRANCH_VI(jet_num_electron);
  BRANCH_VI(jet_num_muon);
  BRANCH_VI(jet_num_photon);

  BRANCH_VF(jet_major_axis);
  BRANCH_VF(jet_minor_axis);
  BRANCH_VF(jet_ptd);

  BRANCH_VO(jet_b_tag);

  BRANCH_VI(jet_ttbar_assignment);
  BRANCH_VI(jet_b_assignment);

  BRANCH_ANY(con_pt);
  BRANCH_ANY(con_deta);
  BRANCH_ANY(con_dphi);
  BRANCH_ANY(con_charge);
  BRANCH_ANY(con_pid);
  BRANCH_ANY(con_type);

  return ;
}


void TTAllJetsAnalyser::resetBranch() {
  // CAUTION do not reset label

  b_num_eflow_ = -1;
  b_num_track_ = -1;
  b_num_tower_ = -1;

  b_track_pt_.clear();
  b_track_eta_.clear();
  b_track_phi_.clear();
  b_track_charge_.clear();
  b_track_pid_.clear();
  b_track_type_.clear();
  b_track_dz_.clear();
  b_track_d0_.clear();

  b_tower_pt_.clear();
  b_tower_eta_.clear();
  b_tower_phi_.clear();
  b_tower_charge_.clear();
  b_tower_pid_.clear();
  b_tower_type_.clear();

  b_met_     = -1.0f;
  b_met_eta_ = 100.0f;
  b_met_phi_ = 100.0f;

  b_num_b_jets_   = 0;
  b_num_b_jets_6_ = 0;

  b_jet_pt_.clear();
  b_jet_eta_.clear();
  b_jet_phi_.clear();
  b_jet_mass_.clear();

  b_jet_num_chad_.clear();
  b_jet_num_nhad_.clear();
  b_jet_num_electron_.clear();
  b_jet_num_muon_.clear();
  b_jet_num_photon_.clear();

  b_jet_major_axis_.clear();
  b_jet_minor_axis_.clear();
  b_jet_ptd_.clear();

  b_jet_b_tag_.clear();

  b_jet_ttbar_assignment_.clear();
  b_jet_b_assignment_.clear();

  b_con_pt_.clear();
  b_con_deta_.clear();
  b_con_dphi_.clear();
  b_con_charge_.clear();
  b_con_pid_.clear();
  b_con_type_.clear();
}


std::vector<const GenParticle*> TTAllJetsAnalyser::getDaughters(
    const GenParticle* mother) {

  std::vector<const GenParticle*> daughters;
  if (mother->D1 == -1) {
    return daughters;
  }

  for (Int_t dau_idx = mother->D1; dau_idx <= mother->D2; dau_idx++) {
    auto dau = dynamic_cast<const GenParticle*>(particles_->At(dau_idx));
    daughters.push_back(dau);
  }

  return daughters;
}


std::vector<const GenParticle*> TTAllJetsAnalyser::getPartons(
    const GenParticle* top) {

  auto candidates = getDaughters(top);

  std::vector<const GenParticle*> partons;

  while (candidates.size() > 0) {
    auto cand = candidates.back();
    candidates.pop_back();

    if ((std::abs(cand->PID) <= 5) and (cand->Status == 23)) {
      partons.push_back(cand);
    } else if (cand->D1 != -1) {
      auto new_cands = getDaughters(cand);
      candidates.insert(candidates.end(), new_cands.begin(), new_cands.end());
    }
  }

  return partons;
}


void TTAllJetsAnalyser::resetMemberData() {
  selected_jets_.clear();
  top_quark_ = nullptr;
  anti_top_quark_ = nullptr;
  partons_t_.clear();
  partons_tbar_.clear();
  parton2jet_.clear();
  jet2parton_.clear();
  jet_ttbar_assignment_.clear();
}


Bool_t TTAllJetsAnalyser::matchPartonsWithJets() {
  // NOTE find top quark and anti top quark
  for (Int_t gen_p_idx = 0; gen_p_idx < particles_->GetEntries(); gen_p_idx++) {
    auto gen_p = dynamic_cast<const GenParticle*>(particles_->At(gen_p_idx));

    if ((gen_p->PID == 6) and (gen_p->Status == p8status::kTop)) {
      top_quark_ = gen_p;
    } else if ((gen_p->PID == -6) and (gen_p->Status == p8status::kTop)) {
      anti_top_quark_ = gen_p;
    }

    if ((top_quark_ != nullptr) and (anti_top_quark_ != nullptr)) {
      break;
    }
  }

  if ((top_quark_ == nullptr) or (anti_top_quark_ == nullptr)) {
    std::cerr << "[NOT FOUND]" << std::endl;
    return false;
  }

  // NOTE
  partons_t_ = getPartons(top_quark_);
  partons_tbar_ = getPartons(anti_top_quark_);

  std::vector<const GenParticle*> partons;
  partons.insert(partons.end(), partons_t_.begin(), partons_t_.end());
  partons.insert(partons.end(), partons_tbar_.begin(), partons_tbar_.end());

  // NOTE
  for (const auto & parton : partons) {
    parton2jet_[parton] = nullptr;
  }

  for (const auto & jet : selected_jets_) {
    jet2parton_[jet] = nullptr;
  }

  bool has_ambiguity = false;
  for (const auto & parton : partons) {
    for (const auto & jet : selected_jets_) {
      Float_t dr = parton->P4().DeltaR(jet->P4());

      if (dr <= kJetPartonMatchingDeltaR_) {
        if ((parton2jet_[parton] == nullptr) and (jet2parton_[jet] == nullptr)) {
          parton2jet_[parton] = jet;
          jet2parton_[jet] = parton;
        } else {
          has_ambiguity = true;
          // break;
        }
      } // dr
    } // jet
    // if (has_ambiguity) break;
  } // parton


  // NOTE unmatched
  for (const auto & kv : parton2jet_) {
    if (kv.second == nullptr) {
      return false;
    }
  }

  if (has_ambiguity) {
    num_ambiguous_++;
    return false;
  }

  for (const auto& jet : selected_jets_) {
    if (jet2parton_[jet] == nullptr) {
      jet_ttbar_assignment_[jet] = 0;
    } else {
      auto parton = jet2parton_[jet];
      if (std::find(partons_t_.begin(), partons_t_.end(), parton) != partons_t_.end()) {
        jet_ttbar_assignment_[jet] = 1;
      } else {
        jet_ttbar_assignment_[jet] = -1;
      }      
    }
  }

  return true;
}


Bool_t TTAllJetsAnalyser::selectEvent() {
  // NOTE to speed up selection
  UInt_t num_jets = jets_->GetEntries();
  if (num_jets < kHLTMinNumJets_) return false;

  // the scalar p_T sum of all jets in the event
  Float_t hadronic_activity = 0.0f;

  for (UInt_t i = 0; i < num_jets; i++) {
    auto jet = dynamic_cast<const Jet*>(jets_->At(i));
    if (jet->PT < kMinJetPT_) continue;
    if (std::fabs(jet->Eta) > kMaxJetEta_) continue;

    selected_jets_.push_back(jet);
    hadronic_activity += jet->PT;
  }

  // NOTE HLT
  if (hadronic_activity < kHLTMinHT_) return false;

  UInt_t  num_selected_jets  = selected_jets_.size();
  if (num_selected_jets < kHLTMinNumJets_) return false;

  Float_t sixth_jet_pt       = selected_jets_.at(5)->PT;
  if (sixth_jet_pt < kHLTMinSixthJetPT_) return false;

  std::vector<const Jet*> selected_b_jets;
  std::copy_if(selected_jets_.begin(),
               selected_jets_.begin() + 6,
               std::back_inserter(selected_b_jets),
               [](const Jet* each) {return each->BTag == 1;});

  UInt_t  num_b_jets         = selected_b_jets.size();
  if (num_b_jets < kHLTMinNumBJets_) return false;

  // NOTE Offline
  if (kDoOfflineSelection_) {
    if (num_b_jets < kOffMinNumBJets_) return false;

    auto bjet0 = selected_b_jets.at(0);
    auto bjet1 = selected_b_jets.at(1);
    Float_t dr_bb = bjet0->P4().DeltaR(bjet1->P4());

    // NOTE cut value is the same as that of HLT in the reference paper. 
    // if (num_selected_jets < kOffMinNumJets_)    return false;
    // if (sixth_jet_pt      < kOffMinSixthJetPT_) return false;
    // if (hadronic_activity < kOffMinHT_)         return false;

    if (dr_bb < kOffMinDeltaRbb_) return false; 
  }

  if (b_label_ == 1) {
    // NOTE Jet-Parton Assignment
    bool has_correct = matchPartonsWithJets();
    return has_correct;
  } else {
    return true;
  }
}

Int_t TTAllJetsAnalyser::getPFIndex(Int_t pid, Int_t charge) {
  if      (pid == pdgid::kElectron)     return pfindex::kElectron;
  else if (pid == pdgid::kAntiElectron) return pfindex::kAntiElectron;
  else if (pid == pdgid::kMuon)         return pfindex::kMuon;
  else if (pid == pdgid::kAntiMuon)     return pfindex::kAntiMuon;
  else if (charge > 0)                  return pfindex::kPositivelyChargedHadron;
  else                                  return pfindex::kNegativelyChargedHadron;
}

void TTAllJetsAnalyser::analyseEFlow() {
  b_num_track_ = eflow_tracks_->GetEntries();

  Int_t num_nhad = eflow_neutral_hadrons_->GetEntries();
  Int_t num_photon = eflow_photons_->GetEntries();
  b_num_tower_ = num_nhad + num_photon;

  b_num_eflow_ = b_num_track_ + b_num_tower_;

  // for (Int_t idx = 0; idx < eflow_tracks_->GetEntries(); idx++) {
  for (Int_t idx = 0; idx < b_num_track_; idx++) {
    auto track = dynamic_cast<Track*>(eflow_tracks_->At(idx));
    Int_t pid = track->PID;
    Int_t charge = track->Charge;
    Int_t pf_index = getPFIndex(pid, charge);

    b_track_pt_.push_back(track->PT);
    b_track_eta_.push_back(track->Eta);
    b_track_phi_.push_back(track->Phi);
    b_track_charge_.push_back(track->Charge);
    b_track_pid_.push_back(pid);
    b_track_type_.push_back(pf_index);
    b_track_dz_.push_back(track->DZ);
  }

  // for (Int_t idx = 0; idx < eflow_neutral_hadrons_->GetEntries(); idx++) {
  for (Int_t idx = 0; idx < num_nhad; idx++) {
    auto nhad = dynamic_cast<Tower*>(eflow_neutral_hadrons_->At(idx));
    b_tower_pt_.push_back(nhad->ET);
    b_tower_eta_.push_back(nhad->Eta);
    b_tower_phi_.push_back(nhad->Phi);
    b_tower_charge_.push_back(0);
    b_tower_pid_.push_back(pdgid::kNeutralHadron);
    b_tower_type_.push_back(pfindex::kNeutralHadron);
  }

  // for (Int_t idx = 0; idx < eflow_photons_->GetEntries(); idx++) {
  for (Int_t idx = 0; idx < num_photon; idx++) {
    auto photon = dynamic_cast<Tower*>(eflow_photons_->At(idx));
    b_tower_pt_.push_back(photon->ET);
    b_tower_eta_.push_back(photon->Eta);
    b_tower_phi_.push_back(photon->Phi);
    b_tower_charge_.push_back(0);
    b_tower_pid_.push_back(pdgid::kPhoton);
    b_tower_type_.push_back(pfindex::kPhoton);
  }

}


void TTAllJetsAnalyser::analyseJet() {

  Int_t jet_count = 0;
  for (const auto & jet : selected_jets_) {
    jet_count++;

    TLorentzVector jet_p4 = jet->P4();
    Float_t j_pt = jet->PT;
    Float_t j_eta = jet->Eta;
    Float_t j_phi = jet->Phi;

    if (jet->BTag == 1) {
      b_num_b_jets_++;
    
      if(jet_count <= 6) b_num_b_jets_6_++;
    }

    b_jet_pt_.push_back(j_pt);
    b_jet_eta_.push_back(j_eta);
    b_jet_phi_.push_back(j_phi);
    b_jet_mass_.push_back(jet->Mass);

    // Loop over cons
    b_con_pt_.push_back(std::vector<Float_t>());
    b_con_deta_.push_back(std::vector<Float_t>());
    b_con_dphi_.push_back(std::vector<Float_t>());
    b_con_charge_.push_back(std::vector<Int_t>());
    b_con_pid_.push_back(std::vector<Int_t>());
    b_con_type_.push_back(std::vector<Int_t>());
    
    Int_t num_chad = 0, num_nhad = 0;
    Int_t num_electron = 0, num_muon = 0, num_photon = 0;

    for (Int_t con_idx = 0; con_idx < jet->Constituents.GetEntries(); con_idx++) {
      auto con = jet->Constituents.At(con_idx);

      Float_t c_pt = -1;
      Float_t c_eta = 100;
      Float_t c_phi = 100;
      Int_t   c_charge = 100;
      Int_t   c_pid = pdgid::kWrong;
      Int_t   c_type = -1;

      if (auto track = dynamic_cast<Track*>(con)) {
        c_pt = track->PT;
        c_eta = track->Eta;
        c_phi = track->Phi;
        c_charge = track->Charge;
        c_pid = track->PID;

        Int_t abs_con_pid = std::abs(c_pid);
        if (abs_con_pid == pdgid::kElectron)  num_electron++;
        else if (abs_con_pid == pdgid::kMuon) num_muon++;
        else                                  num_chad++;

      } else if (auto tower = dynamic_cast<Tower*>(con)) {
        c_pt = tower->ET;
        c_eta = tower->Eta;
        c_phi = tower->Phi;
        c_charge = 0;

        if (tower->Eem == 0.0) {
          c_pid = pdgid::kNeutralHadron;
          c_type = pfindex::kNeutralHadron; // FIXME
          num_nhad++;
        } else if (tower->Ehad == 0.0) {
          c_pid = pdgid::kPhoton;
          c_type = pfindex::kPhoton; // FIXME
          num_photon++;
        } else {
          std::cout << "[[ERROR]]: Tower with Had " << tower->Ehad
                    << " and EM " << tower->Eem << " energy" << std::endl;
          }
      } else {
        std::cout << "[[ERROR]]: BAD DAUGHTER! " << con << std::endl;
      }

      Float_t c_deta = c_eta - j_eta;
      Float_t c_dphi = TVector2::Phi_mpi_pi(c_phi - j_phi);

      b_con_pt_.back().push_back(c_pt);
      b_con_deta_.back().push_back(c_deta);
      b_con_dphi_.back().push_back(c_dphi);
      b_con_charge_.back().push_back(c_charge);
      b_con_pid_.back().push_back(c_pid);
      b_con_type_.back().push_back(c_type);

    } // end loop over cons

    ////////////////////////////////////////////////////////////////////////////
    // NOTE CMS Variables
    ////////////////////////////////////////////////////////////////////////////
    b_jet_num_chad_.push_back(num_chad);
    b_jet_num_nhad_.push_back(num_nhad);
    b_jet_num_electron_.push_back(num_electron);
    b_jet_num_muon_.push_back(num_muon);
    b_jet_num_photon_.push_back(num_photon);

    // Move CMSSW to higher version and use structured bindings
    Float_t major_axis, minor_axis, eccentricity;
    std::tie(major_axis, minor_axis, eccentricity) = ComputeAxes<Float_t>(
        b_con_deta_.back(), b_con_dphi_.back(), b_con_pt_.back());

    b_jet_major_axis_.push_back(major_axis);
    b_jet_minor_axis_.push_back(minor_axis);
    // jet_eccentricity_.push_back(eccentricity);

    // jet energy sharing variable
    Float_t pt_l1_norm = std::accumulate(
        b_con_pt_.back().begin(), b_con_pt_.back().end(), 0.0f);
    Float_t pt_l2_norm_squared = std::inner_product(
        b_con_pt_.back().begin(), b_con_pt_.back().end(),
        b_con_pt_.back().begin(), 0.0f); 
    Float_t ptd = std::sqrt(pt_l2_norm_squared) / pt_l1_norm;
    b_jet_ptd_.push_back(ptd);

    ////////////////////////////////////////////////////////////////////////////
    // NOTE b-jet properties
    ////////////////////////////////////////////////////////////////////////////
    b_jet_b_tag_.push_back(jet->BTag);

    b_jet_ttbar_assignment_.push_back(jet_ttbar_assignment_[jet]);


    if (jet2parton_[jet] == nullptr) {
      b_jet_b_assignment_.push_back(0);
    }  else {

      if (std::abs(jet2parton_[jet]->PID) == 5) {
        b_jet_b_assignment_.push_back(1);
      } else {
        b_jet_b_assignment_.push_back(0);
      }

    }

  } // end loop over jets
}



void TTAllJetsAnalyser::analyse() {
  analyseEFlow();
  analyseJet();

  auto missing_et = dynamic_cast<MissingET*>(mets_->At(0));
  b_met_     = missing_et->MET;
  b_met_eta_ = missing_et->Eta;
  b_met_phi_ = missing_et->Phi;

  out_tree_->Fill();
}



void TTAllJetsAnalyser::loop() {
  const Int_t kNumTotal = in_tree_->GetEntries();
  const Int_t kPrintFreq = kNumTotal / 20;
  TString msg_fmt = TString::Format("[%s/%d (%s %%)]", "%d", kNumTotal, "%.2f");

  for (Long64_t entry=0; entry < in_tree_->GetEntries(); entry++) {
    in_tree_->GetEntry(entry);

    if (entry % kPrintFreq == 0) {
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
