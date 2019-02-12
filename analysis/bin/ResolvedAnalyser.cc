#include "delphys/analysis/interface/ResolvedAnalyser.h"

#include "TString.h"
#include "TSystem.h"
#include "TInterpreter.h"

#include <algorithm>
#include <numeric>
#include <iterator>
#include <memory>

using namespace delphys;

ResolvedAnalyser::ResolvedAnalyser(const TString & in_path,
                                   const TString & out_path,
                                   const TString & out_tree_name)
    : BaseAnalyser(in_path, out_path, out_tree_name) {

  SetBranchAddress({"Vertex"}, /*drop=*/true);
  MakeBranch();

  TString base_name(gSystem->BaseName(in_path));
  if (base_name.Contains("TT")) {
    label_ = 1;
  } else if (base_name.Contains("QCD")) {
    label_ = 0;
  } else {
    std::cout << base_name << std::endl;
    std::abort();
  }

  std::cout << "ctor" << std::endl;
}


ResolvedAnalyser::~ResolvedAnalyser() {
  out_tree_->Print();
  out_file_->Write();
  out_file_->Close();
  std::cout << "dtor" << std::endl;
}

void ResolvedAnalyser::MakeBranch() {
  Reset();
  gInterpreter->GenerateDictionary("vector<vector<Int_t> >", "vector"); 
  gInterpreter->GenerateDictionary("vector<vector<TLorentzVector> >", "vector"); 

  // per event
  out_tree_->Branch("label", &label_, "label/I");

  // per jet : vector of 
  out_tree_->Branch("num_jet", &num_jet_, "num_jet/I");
  out_tree_->Branch("jet_p4", &jet_p4_);

  out_tree_->Branch("jet_num_chad", &jet_num_chad_);
  out_tree_->Branch("jet_num_nhad", &jet_num_nhad_);
  out_tree_->Branch("jet_num_electron", &jet_num_electron_);
  out_tree_->Branch("jet_num_muon", &jet_num_muon_);
  out_tree_->Branch("jet_num_photon", &jet_num_photon_);

  out_tree_->Branch("jet_major_axis", &jet_major_axis_);
  out_tree_->Branch("jet_minor_axis", &jet_minor_axis_);
  out_tree_->Branch("jet_eccentricity", &jet_eccentricity_);
  out_tree_->Branch("jet_ptd", &jet_ptd_);

  out_tree_->Branch("jet_b_tag", &jet_b_tag_);
  out_tree_->Branch("jet_b_dr_matching", &jet_b_dr_matching_);
  out_tree_->Branch("jet_b_tracking", &jet_b_tracking_);

  // per constituents : vector of vector of
  out_tree_->Branch("num_constituent", &num_constituent_);
  out_tree_->Branch("constituent_p4", &constituent_p4_);
  out_tree_->Branch("constituent_pid", &constituent_pid_);
  out_tree_->Branch("constituent_type", &constituent_type_);

  return ;
}


void ResolvedAnalyser::Reset() {
  label_ = -1;

  num_jet_ = -1;
  jet_p4_.clear();

  jet_num_chad_.clear();
  jet_num_nhad_.clear();
  jet_num_electron_.clear();
  jet_num_muon_.clear();
  jet_num_photon_.clear();

  jet_major_axis_.clear();
  jet_minor_axis_.clear();
  jet_eccentricity_.clear();
  jet_ptd_.clear();

  jet_b_tag_.clear();
  jet_b_dr_matching_.clear();
  jet_b_tracking_.clear();

  num_constituent_.clear();
  constituent_p4_.clear();
  constituent_pid_.clear();
  constituent_type_.clear();
}


std::vector<const Jet*> ResolvedAnalyser::SelectJet() {
  std::vector<const Jet*> selected_jets;

  for (Int_t i = 0; i < jets_->GetEntries(); i++) {
    auto jet = dynamic_cast<const Jet*>(jets_->At(i));

    if(jet->PT < 45.0f) continue;
    if(std::fabs(jet->Eta) > 2.4f) continue;

    selected_jets.push_back(jet);
  }
  return selected_jets;
}


Bool_t ResolvedAnalyser::SelectEvent() {
  if (jets_->GetEntries() < 6) {
    return kFALSE;
  }

  // NOTE Select objects
  selected_jets_ = SelectJet();
  if(selected_jets_.size() < 6) {
    return kFALSE;
  }

  return kTRUE;
}


Bool_t ResolvedAnalyser::TrackBottomQuark(const GenParticle* p) {
  Bool_t found_b = false;
  while (p->M1 != -1) {
    p = dynamic_cast<GenParticle*>(particles_->At(p->M1));
    if ((std::abs(p->PID) == 5) and (p->Status == 23)) {
      found_b = true;
      break;
    }
  }
  return found_b;
}

Float_t ResolvedAnalyser::GetBDaughterRatio(const Jet* jet) {
  Int_t num_particles = jet->Particles.GetEntries();

  Int_t num_from_b = 0;
  for (Int_t idx = 0; idx < num_particles; idx++) {
    auto p = dynamic_cast<const GenParticle*>(jet->Particles[idx]);
    if (TrackBottomQuark(p)) {
      num_from_b++;
    }
  }

  Float_t b_ratio = static_cast<Float_t>(num_from_b) / num_particles;
  return b_ratio;
}


void ResolvedAnalyser::AnalyseEvent() {

  std::vector<GenParticle*> bottom_quarks;
  for (Int_t p_idx = 0; p_idx < particles_->GetEntries(); p_idx++) {
    auto p = dynamic_cast<GenParticle*>(particles_->At(p_idx));
    if ((std::abs(p->PID) == 5) and (p->Status == 23)) {
      bottom_quarks.push_back(p);
    }
  } 

  for (const auto & jet : selected_jets_) {
    TLorentzVector j_p4 = jet->P4();
    Float_t j_eta = jet->Eta;
    Float_t j_phi = jet->Phi;

    constituent_p4_.push_back(std::vector<TLorentzVector>());
    constituent_pid_.push_back(std::vector<Int_t>());
    constituent_type_.push_back(std::vector<Int_t>());

    Int_t num_chad = 0, num_nhad = 0, num_electron = 0, num_muon = 0, num_photon = 0;
    std::vector<Double_t> cons_deta, cons_dphi, cons_pt;

    for (Int_t con_idx = 0; con_idx < jet->Constituents.GetEntries(); con_idx++) {
      auto constituent = jet->Constituents.At(con_idx);

      TLorentzVector c_p4;
      Int_t c_type = -1;
      Int_t pid = kWrongPID_;

      if (auto track = dynamic_cast<Track*>(constituent)) {
        c_p4 = track->P4();
        pid = track->PID;

        if (std::abs(pid) == kElectronPID_) { 
          c_type = kElectronType_;
          num_electron++;
        } else if (std::abs(pid) == kMuonPID_) {
          c_type = kMuonType_;
          num_muon++;
        } else { 
          c_type = kChargedHadronType_;
          num_chad++;
        }

      } else if (auto tower = dynamic_cast<Tower*>(constituent)) {
        c_p4 = tower->P4();

        if (tower->Eem == 0.0) {
          pid = 0;

          c_type = kNeutralHadronType_;
          num_nhad++;
        } else if (tower->Ehad == 0.0) {
          pid = 22;
          c_type = kPhotonType_;
          num_photon++;
        } else
          std::cout << "[[ERROR]]: Tower with Had " << tower->Ehad << " and EM " << tower->Eem << " energy" << std::endl;
      } else {
        std::cout << "[[ERROR]]: BAD DAUGHTER! " << constituent << std::endl;
      }

      constituent_p4_.back().push_back(c_p4);
      constituent_pid_.back().push_back(pid);
      constituent_type_.back().push_back(c_type);

      cons_deta.push_back(c_p4.Eta() - j_eta);
      cons_dphi.push_back(TVector2::Phi_mpi_pi(c_p4.Phi() - j_phi));
      cons_pt.push_back(c_p4.Pt());

    } // end loop over constituents

    jet_p4_.push_back(j_p4);
    jet_num_chad_.push_back(num_chad);
    jet_num_nhad_.push_back(num_nhad);
    jet_num_electron_.push_back(num_electron);
    jet_num_muon_.push_back(num_muon);
    jet_num_photon_.push_back(num_photon);

    Float_t major_axis, minor_axis, eccentricity;
    std::tie(major_axis, minor_axis, eccentricity) = ComputeAxes<Double_t>(
        cons_deta, cons_dphi, cons_pt);

    jet_major_axis_.push_back(major_axis);
    jet_minor_axis_.push_back(minor_axis);
    jet_eccentricity_.push_back(eccentricity);

    // jet energy sharing variable
    Float_t pt_sum = std::accumulate(cons_pt.begin(), cons_pt.end(), 0.0f);
    Float_t pt_square_sum = std::inner_product(cons_pt.begin(), cons_pt.end(),
                                               cons_pt.begin(), 0.0f); 
    Float_t ptd = std::sqrt(pt_square_sum) / pt_sum;
    jet_ptd_.push_back(ptd);


    // NOTE b-jet properties
    jet_b_tag_.push_back(jet->BTag);

    // b-matching
    Bool_t found_matched_b = false;
    for (const auto & b : bottom_quarks) {
      Double_t delta_r = b->P4().DeltaR(jet->P4());
      if (delta_r < kBMatchingDeltaR_) {
        found_matched_b = true;
        break;
      }
    }
    jet_b_dr_matching_.push_back(found_matched_b);

    Float_t b_dau_ratio = GetBDaughterRatio(jet);
    jet_b_tracking_.push_back(b_dau_ratio);

  } // end loop over jets


  out_tree_->Fill();
}

void ResolvedAnalyser::Loop() {
  Int_t kNumEntries = in_tree_->GetEntries();
  Int_t kPrintFreq = kNumEntries / 100;
  TString kMessageTemplate = "[%d/%d (%.2f %)] # of passed events: %d (%.2f %)\n";

  Int_t num_passed = 0;

  for (Long64_t entry=0; entry < in_tree_->GetEntries(); entry++) {
    in_tree_->GetEntry(entry);
    Reset();

    if (entry % kPrintFreq == 0) {
      TString msg = TString::Format(
          kMessageTemplate,
          entry + 1,
          kNumEntries,
          100 * static_cast<Float_t>(entry + 1) / kNumEntries,
          num_passed,
          100 * static_cast<Float_t>(num_passed) / (entry + 1));

      std::cout << msg;
    }

    // Bool_t is_selected = SelectEvent()
    if (SelectEvent()) {
      num_passed++;
      AnalyseEvent();
    }
  }
}

int main(int argc, char* argv[]) {
  TString in_path(argv[1]);
  TString out_path(argv[2]);

  ResolvedAnalyser analyser(in_path, out_path, "test");
  analyser.Loop();

  return 0;
}
