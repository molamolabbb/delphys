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
                                     Int_t label)
    : BaseAnalyser(in_path, out_path, "delphys"),
      label_(label) {
  std::cout << "ctor begin" << std::endl;

  std::cout << "SetBranchAddress begin" << std::endl;
  setBranchAddress({"Vertex"}, /*drop=*/true);
  std::cout << "SetBranchAddress end" << std::endl;

  makeBranch();

  std::cout << "ctor end" << std::endl;
}


TTAllJetsAnalyser::~TTAllJetsAnalyser() {
  std::cout << "dtor begin" << std::endl;
  out_tree_->Print();
  out_file_->Write();
  out_file_->Close();
  std::cout << "dtor end" << std::endl;
}

void TTAllJetsAnalyser::makeBranch() {
  std::cout << "MakeBranch begin" << std::endl;

  resetBranch();
  gInterpreter->GenerateDictionary("vector<vector<Int_t> >", "vector"); 
  gInterpreter->GenerateDictionary("vector<vector<Float_t> >", "vector"); 

  #define BRANCH_(name, suffix) out_tree_->Branch(#name, & name##_ , #name "/" #suffix);
  #define BRANCH_I(name) BRANCH_(name, I);
  #define BRANCH_F(name) BRANCH_(name, F);
  #define BRANCH_O(name) BRANCH_(name, O);

  #define BRANCH_A_(name, size, suffix) out_tree_->Branch(#name, & name##_ , #name"["#size"]/"#suffix);
  #define BRANCH_AF(name, size)  BRANCH_A_(name, size, F);

  #define BRANCH_V_(name, type) out_tree_->Branch(#name, "vector<"#type">", & name##_ );
  #define BRANCH_VF(name) BRANCH_V_(name, Float_t);
  #define BRANCH_VI(name) BRANCH_V_(name, Int_t);
  #define BRANCH_VO(name) BRANCH_V_(name, Bool_t);

  #define BRANCH_ANY(name) out_tree_->Branch(#name, & name##_ );

  BRANCH_I(label);

  // jet unordered set
  BRANCH_I(num_eflow);

  BRANCH_VF(eflow_pt);
  BRANCH_VF(eflow_eta);
  BRANCH_VF(eflow_phi);
  BRANCH_VI(eflow_charge);
  BRANCH_VI(eflow_pid);
  BRANCH_VI(eflow_type)

  BRANCH_F(met);
  BRANCH_F(met_eta);
  BRANCH_F(met_phi);

  // jet variables
  BRANCH_VI(jet_num_chad);
  BRANCH_VI(jet_num_nhad);
  BRANCH_VI(jet_num_electron);
  BRANCH_VI(jet_num_muon);
  BRANCH_VI(jet_num_photon);

  BRANCH_VF(jet_major_axis);
  BRANCH_VF(jet_minor_axis);
  BRANCH_VF(jet_ptd);

  BRANCH_VO(jet_b_tag);
  BRANCH_VO(jet_b_dr_matching);
  BRANCH_VO(jet_b_tracking);

  // BRANCH_ANY(num_con);
  BRANCH_ANY(con_pt);
  BRANCH_ANY(con_deta);
  BRANCH_ANY(con_dphi);
  BRANCH_ANY(con_charge);
  BRANCH_ANY(con_pid);
  BRANCH_ANY(con_type);

  std::cout << "MakeBranch end" << std::endl;
  return ;
}


void TTAllJetsAnalyser::resetBranch() {
  // CAUTION do not reset label

  num_eflow_ = -1;
  eflow_pt_.clear();
  eflow_eta_.clear();
  eflow_phi_.clear();
  eflow_charge_.clear();
  eflow_pid_.clear();
  eflow_type_.clear();

  met_ = -1.0f;
  met_eta_ = 100.0f;
  met_phi_ = 100.0f;

  jet_pt_.clear();
  jet_eta_.clear();
  jet_phi_.clear();
  jet_pid_.clear();

  jet_num_chad_.clear();
  jet_num_nhad_.clear();
  jet_num_electron_.clear();
  jet_num_muon_.clear();
  jet_num_photon_.clear();

  jet_major_axis_.clear();
  jet_minor_axis_.clear();
  jet_ptd_.clear();

  jet_b_tag_.clear();
  jet_b_dr_matching_.clear();
  jet_b_tracking_.clear();

  con_pt_.clear();
  con_deta_.clear();
  con_dphi_.clear();
  con_charge_.clear();
  con_pid_.clear();
  con_type_.clear();


}


Bool_t TTAllJetsAnalyser::selectEvent() {
  if (jets_->GetEntries() < kMinJetMultiplicity_) return false;

  // FIXME move clear to reset method. rename resetBranch to reset..?
  selected_jets_.clear();
  for (Int_t i = 0; i < jets_->GetEntries(); i++) {
    auto jet = dynamic_cast<const Jet*>(jets_->At(i));

    if(jet->PT < kMinJetPT_) continue;
    if(std::fabs(jet->Eta) > kMaxJetEta_) continue;

    selected_jets_.push_back(jet);
  }

  if (selected_jets_.size() < kMinJetMultiplicity_) return false;

  return true;
}

Int_t TTAllJetsAnalyser::getPFIndex(Int_t pid, Int_t charge) {
  if      (pid == pdgid::kElectron)     return pfindex::kElectron;
  else if (pid == pdgid::kAntiElectron) return pfindex::kAntiElectron;
  else if (pid == pdgid::kMuon)         return pfindex::kMuon;
  else if (pid == pdgid::kAntiMuon)     return pfindex::kAntiMuon;
  else if (charge > 0)                  return pfindex::kPositivelyChargedHadron;
  else                                  return pfindex::kNegativelyChargedHadron;
}

void TTAllJetsAnalyser::fillEFlow() {
  Int_t num_tracks = eflow_tracks_->GetEntries();
  Int_t num_nhads = eflow_neutral_hadrons_->GetEntries();
  Int_t num_photons = eflow_photons_->GetEntries();

  num_eflow_ = num_tracks + num_nhads + num_photons;

  // for (Int_t idx = 0; idx < eflow_tracks_->GetEntries(); idx++) {
  for (Int_t idx = 0; idx < num_tracks; idx++) {
    auto track = dynamic_cast<Track*>(eflow_tracks_->At(idx));
    Int_t pid = track->PID;
    Int_t charge = track->Charge;
    Int_t pf_index = getPFIndex(pid, charge);

    eflow_pt_.push_back(track->PT);
    eflow_eta_.push_back(track->Eta);
    eflow_phi_.push_back(track->Phi);
    eflow_charge_.push_back(track->Charge);
    eflow_pid_.push_back(pid);
    eflow_type_.push_back(pf_index);
  }

  // for (Int_t idx = 0; idx < eflow_neutral_hadrons_->GetEntries(); idx++) {
  for (Int_t idx = 0; idx < num_nhads; idx++) {
    auto nhad = dynamic_cast<Tower*>(eflow_neutral_hadrons_->At(idx));
    eflow_pt_.push_back(nhad->ET);
    eflow_eta_.push_back(nhad->Eta);
    eflow_phi_.push_back(nhad->Phi);
    eflow_charge_.push_back(0);
    eflow_pid_.push_back(pdgid::kNeutralHadron);
    eflow_type_.push_back(pfindex::kNeutralHadron);
  }

  // for (Int_t idx = 0; idx < eflow_photons_->GetEntries(); idx++) {
  for (Int_t idx = 0; idx < num_photons; idx++) {
    auto photon = dynamic_cast<Tower*>(eflow_photons_->At(idx));
    eflow_pt_.push_back(photon->ET);
    eflow_eta_.push_back(photon->Eta);
    eflow_phi_.push_back(photon->Phi);
    eflow_charge_.push_back(0);
    eflow_pid_.push_back(pdgid::kPhoton);
    eflow_type_.push_back(pfindex::kPhoton);
  }

}



Bool_t TTAllJetsAnalyser::trackBottomQuark(const GenParticle* p) {
  Bool_t found_b = false;
  // FIXME consider M2
  while (p->M1 != -1) {
    p = dynamic_cast<GenParticle*>(particles_->At(p->M1));
    if ((std::abs(p->PID) == pdgid::kBottom) and (p->Status == p8status::kBottom)) {
      found_b = true;
      break;
    }
  }
  return found_b;
}


Float_t TTAllJetsAnalyser::getBDaughterRatio(const Jet* jet) {
  Int_t num_particles = jet->Particles.GetEntries();

  Int_t num_from_b = 0;
  for (Int_t idx = 0; idx < num_particles; idx++) {
    auto p = dynamic_cast<const GenParticle*>(jet->Particles[idx]);
    if (trackBottomQuark(p)) {
      num_from_b++;
    }
  }

  Float_t b_ratio = static_cast<Float_t>(num_from_b) / num_particles;
  return b_ratio;
}



void TTAllJetsAnalyser::fillJetVariables() {
  std::vector<GenParticle*> bottom_quarks;
  for (Int_t p_idx = 0; p_idx < particles_->GetEntries(); p_idx++) {
    auto p = dynamic_cast<GenParticle*>(particles_->At(p_idx));
    if ((std::abs(p->PID) == pdgid::kBottom) and (p->Status == p8status::kBottom)) {
      bottom_quarks.push_back(p);
    }
  }

  // FIXME make fillJetVaariable to return boolean
  // if (bottom_quarks.size() != 2) return false

  for (const auto & jet : selected_jets_) {
    TLorentzVector jet_p4 = jet->P4();
    Float_t j_pt = jet->PT;
    Float_t j_eta = jet->Eta;
    Float_t j_phi = jet->Phi;

    jet_pt_.push_back(j_pt);
    jet_eta_.push_back(j_eta);
    jet_phi_.push_back(j_phi);

    // Loop over cons
    con_pt_.push_back(std::vector<Float_t>());
    con_deta_.push_back(std::vector<Float_t>());
    con_dphi_.push_back(std::vector<Float_t>());
    con_charge_.push_back(std::vector<Int_t>());
    con_pid_.push_back(std::vector<Int_t>());
    con_type_.push_back(std::vector<Int_t>());
    
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

      con_pt_.back().push_back(c_pt);
      con_deta_.back().push_back(c_deta);
      con_dphi_.back().push_back(c_dphi);
      con_charge_.back().push_back(c_charge);
      con_pid_.back().push_back(c_pid);
      con_type_.back().push_back(c_type);

    } // end loop over cons

    ////////////////////////////////////////////////////////////////////////////
    // NOTE CMS Variables
    ////////////////////////////////////////////////////////////////////////////
    jet_num_chad_.push_back(num_chad);
    jet_num_nhad_.push_back(num_nhad);
    jet_num_electron_.push_back(num_electron);
    jet_num_muon_.push_back(num_muon);
    jet_num_photon_.push_back(num_photon);

    // Move CMSSW to higher version and use structured bindings
    Float_t major_axis, minor_axis, eccentricity;
    std::tie(major_axis, minor_axis, eccentricity) = ComputeAxes<Float_t>(
        con_deta_.back(), con_dphi_.back(), con_pt_.back());

    jet_major_axis_.push_back(major_axis);
    jet_minor_axis_.push_back(minor_axis);
    // jet_eccentricity_.push_back(eccentricity);

    // jet energy sharing variable
    Float_t pt_l1_norm = std::accumulate(
        con_pt_.back().begin(), con_pt_.back().end(), 0.0f);
    Float_t pt_l2_norm_squared = std::inner_product(
        con_pt_.back().begin(), con_pt_.back().end(),
        con_pt_.back().begin(), 0.0f); 
    Float_t ptd = std::sqrt(pt_l2_norm_squared) / pt_l1_norm;
    jet_ptd_.push_back(ptd);

    ////////////////////////////////////////////////////////////////////////////
    // NOTE b-jet properties
    ////////////////////////////////////////////////////////////////////////////
    jet_b_tag_.push_back(jet->BTag);


    Bool_t found_matched_b = false;
    for (const auto & b : bottom_quarks) {
      Double_t delta_r = b->P4().DeltaR(jet->P4());
      if (delta_r < kBMatchingDeltaR_) {
        found_matched_b = true;
        break;
      }
    }
    jet_b_dr_matching_.push_back(found_matched_b);

    Float_t b_dau_ratio = getBDaughterRatio(jet);
    jet_b_tracking_.push_back(b_dau_ratio);

  } // end loop over jets
}



void TTAllJetsAnalyser::analyse() {

  fillEFlow();
  fillJetVariables();

  auto missing_et = dynamic_cast<MissingET*>(mets_->At(0));
  met_ = missing_et->MET;
  met_eta_ = missing_et->Eta;
  met_phi_ = missing_et->Phi;

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
    if (selectEvent()) {
      analyse();
    }
  }
}
