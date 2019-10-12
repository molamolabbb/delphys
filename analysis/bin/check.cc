#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TMath.h>
#include <TString.h>
#include <TSystem.h>
#include <TRefArray.h>
#include <TMatrixDfwd.h>
#include <TVectorD.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include "TMinuit.h"
#include "TError.h"
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <sys/stat.h>
#include "delphys/analysis/interface/doubleHiggsAnalyser.h"
#include "delphys/external/interface/HiggsnessTopness.h"

#include "classes/DelphesClasses.h"
#include "delphys/external/interface/lester_mt2_bisect.h"
#include "delphys/external/interface/Basic_Mt2_332_Calculator.h"
#include "delphys/external/interface/ChengHanBisect_Mt2_332_Calculator.h"

using namespace std;

// return mother particle(GenParticle) of particle 'p' among 'particles'.
GenParticle* getMother(TClonesArray* particles, const GenParticle* p){
  if (p->M1==-1) return nullptr;
  auto mom = static_cast<GenParticle *>(particles->At(p->M1));
  while (mom->PID == p->PID){
    mom = static_cast<GenParticle *>(particles->At(mom->M1));
    if (mom->M1==-1) return nullptr;
  }
  return mom;
}

// return the PID of the mother particle of the particle at 'ip' among 'particles'
std::pair<int,int> isFrom(TClonesArray* particles, int ip){
   auto p = static_cast<GenParticle *>(particles->At(ip));
   // check if it's from Higgs
   auto mom = getMother(particles, p); 
   if (mom==nullptr) return std::make_pair(0,0);
   auto grmom = getMother(particles, mom);
   if (grmom==nullptr) return std::make_pair(0,0);
   auto pedigree = std::make_pair(mom->PID, grmom->PID);
   return pedigree;
}

void doubleHiggsAnalyser::MakeOutputBranch(TTree *tree) {
  n_events_tree->Branch("event_size",&event_size,"event_size/I");
  // lepton kinematic variables
  tree->Branch("lep1","TLorentzVector", &lepton1);
  tree->Branch("lep2","TLorentzVector", &lepton2);
  tree->Branch("ll","TLorentzVector", &leptonlepton);
  tree->Branch("ll_deltaR",&ll_deltaR,"ll_deltaR/F");
  tree->Branch("ll_deltaPhi",&ll_deltaPhi,"ll_deltaPhi/F");

  // truth matching variables
  tree->Branch("lepton1MotherPID",&lep1_mother,"lep1_mother/I");
  tree->Branch("lepton2MotherPID",&lep2_mother,"lep2_mother/I");
  tree->Branch("lepton1GrMotherPID",&lep1_grmother,"lep1_grmother/I");
  tree->Branch("lepton2GrMotherPID",&lep2_grmother,"lep2_grmother/I");

  // cut flow
  tree->Branch("step",&step,"step/I");

}

void doubleHiggsAnalyser::SetOutput(TString output_file_name) {
  out_file = TFile::Open(output_file_name,"RECREATE");
  out_tree = new TTree("events","events"); 
  n_events_tree = new TTree("nevents","nevents");
}

/*
void doubleHiggsAnalyser::SetTMVA(TString weight_file_path) {
  bdtg_reader = new TMVA::Reader();
  bdtg_reader->AddVariable("ll_deltaR", &ll_deltaR);
  bdtg_reader->AddVariable("ll.Pt()", &ll_Pt);
  bdtg_reader->AddVariable("ll.M()", &ll_M);
  bdtg_reader->AddVariable("bb_deltaR", &bb_deltaR);
  bdtg_reader->AddVariable("bb.Pt()", &bb_Pt);
  bdtg_reader->AddVariable("bb.M()", &bb_M);
  bdtg_reader->AddVariable("bl_min_deltaR", &bl_min_deltaR);
  bdtg_reader->AddVariable("bbll_deltaR", &bbll_deltaR);
  bdtg_reader->AddVariable("bbll_deltaPhi", &bbll_deltaPhi);
  bdtg_reader->AddVariable("mT", &mT);
  bdtg_reader->AddVariable("basic_MT2_332_bbll", &basic_MT2_332_bbll);
  bdtg_reader->AddVariable("topness",&topness);
  bdtg_reader->AddVariable("higgsness",&higgsness);
  
  bdtg_reader->BookMVA("BDTG", weight_file_path);
}
*/
void doubleHiggsAnalyser::SetBranchAddress() {
  del_tree->SetBranchAddress("Particle",&particles);
  del_tree->SetBranchAddress("Muon",&muons);
  del_tree->SetBranchAddress("Electron",&electrons);
  del_tree->SetBranchAddress("MissingET",&missings);
  del_tree->SetBranchAddress("Jet",&jets);
}

void doubleHiggsAnalyser::Initiate(TString output_file_name) {
  // set output file
  doubleHiggsAnalyser::SetOutput(output_file_name);
  // make output branch
  doubleHiggsAnalyser::MakeOutputBranch(out_tree);
  // set branch address
  doubleHiggsAnalyser::SetBranchAddress();
}

void doubleHiggsAnalyser::ResetVariables() {

  // truth matching variables 
  lep1_mother = 0;
  lep2_mother = 0;
  lep1_grmother = 0;
  lep2_grmother = 0;

  // cut flow
  step = 0;

  
  // lepton and bottom maps
  leptons.clear();
  bottoms.clear();
}

bool doubleHiggsAnalyser::Analysis(){
    doubleHiggsAnalyser::ResetVariables();
  //map<Float_t, int, greater<Float_t>> leptons : map of <pt,index>:<K,V> of muon sorted by pt.
  //base selections : MissingET > 20, pT(lepton) > 20, deltaR(ll) < 1.0, m(ll) < 65, deltaR(bb) < 1.3, 95 < m(bb) < 140
    
    // Missing ET
    auto m = static_cast<const MissingET *>(missings->At(0)); // There is always one MET object.
    missing.SetPtEtaPhiM(m->MET,0,m->Phi,0);
    // missing_et baseline selection
    if (missing.E()<20) {return false;}

    // GenParticle     
    // collet leptons
    // Electrons
    for (int ip = 0; ip < particles->GetEntries(); ip++){
      auto p = static_cast<const GenParticle *>(particles->At(ip));
      if (abs(p->PID)!=doubleHiggsAnalyser::Electron_PID) continue;
      if (fabs(p->Eta) > 2.4 || fabs(p->PT) < 20) continue;
      leptons.insert(make_pair(p->PT,make_pair(doubleHiggsAnalyser::Electron_PID*p->Charge,ip)));
      //if (p->Status < 20) return false;
    }
    // muons
    for (int ip = 0; ip < particles->GetEntries(); ip++){
      auto p = static_cast<const GenParticle *>(particles->At(ip));
      if (abs(p->PID)!=doubleHiggsAnalyser::Muon_PID) continue;
      if (fabs(p->Eta) > 2.4 || fabs(p->PT) < 20) continue;
      leptons.insert(make_pair(p->PT,make_pair(doubleHiggsAnalyser::Muon_PID*p->Charge,ip)));
      //if (p->Status < 20) return false;
    }
    /*
    // tau
    for (int ip = 0; ip < particles->GetEntries(); ip++){
      auto p = static_cast<const GenParticle *>(particles->At(ip));
      if (abs(p->PID)!=doubleHiggsAnalyser::Tau_PID) continue;
      //if (abs(p->PID)!=doubleHiggsAnalyser::Electron_PID && abs(p->PID)!=doubleHiggsAnalyser::Muon_PID) continue;
      if (fabs(p->Eta) > 2.4 || fabs(p->PT) < 20) continue;
      leptons.insert(make_pair(p->PT,make_pair(doubleHiggsAnalyser::Tau_PID*p->Charge,ip)));
      //if (p->Status < 20) return false;
    }
    */
    if (leptons.size()!=2) {
      return false;
    }
    
    lepton_iter = leptons.begin();
    auto l1_info = lepton_iter->second;
    int pid1 = l1_info.first;
    int index_l1 = l1_info.second;
    lepton_iter++;
    auto l2_info = lepton_iter->second;
    int pid2 = l2_info.first;
    int index_l2 = l2_info.second;
    lepton_iter++;
    
    while (pid1*pid2 > 0 && (fabs(pid1)!=fabs(pid2)) && (lepton_iter!=leptons.end())){
      l2_info = lepton_iter->second;
      pid2 = l2_info.first;
      index_l2 = l2_info.second;
      lepton_iter++;
    }
    
    if (pid1*pid2 > 0) return false;
    if(abs(pid1)>0){
      auto lep1 = static_cast<const GenParticle *>(particles->At(index_l1));
      cout<<lep1->Mass<<endl;
      lepton1.SetPtEtaPhiM(lep1->PT,lep1->Eta,lep1->Phi,lep1->Mass);
    } 
    if(abs(pid2)>0){
      auto lep2 = static_cast<const GenParticle *>(particles->At(index_l2));
      cout<<lep2->Mass<<endl;
      lepton2.SetPtEtaPhiM(lep2->PT,lep2->Eta,lep2->Phi,lep2->Mass);
    } 
    //auto lep1 = static_cast<const GenParticle *>(particles->At(lepton_iter->second));
    //lepton1.SetPtEtaPhiM(lep1->PT,lep1->Eta,lep1->Phi,lep1->Mass);
    //auto lepton1_pdg = isFrom(particles, lepton_iter->second); // lepton truth matching
    //lep1_mother = abs(lepton1_pdg.first);
    //lep1_grmother = abs(lepton1_pdg.second);
    //++lepton_iter;
    //lepton2.SetPtEtaPhiM(lep2->PT,lep2->Eta,lep2->Phi,lep2->Mass);
    //auto lepton2_pdg = isFrom(particles, lepton_iter->second); // lepton truth matching
    //lep2_mother = abs(lepton2_pdg.first);
    //lep2_grmother = abs(lepton2_pdg.second);
    leptonlepton = lepton1+lepton2;
    ll_M = leptonlepton.M();
    ll_Pt = leptonlepton.Pt();
    ll_deltaR = fabs(lepton1.DeltaR(lepton2));
    ll_deltaPhi = fabs(lepton1.DeltaPhi(lepton2));
     
    // mT
    mT = sqrt(2*leptonlepton.Pt()*missing.E()*(1-cos(leptonlepton.Phi()-missing.Phi())));

    // lepton baseline selections
    if (ll_deltaR < 0.07 || ll_deltaR > 3.3) {return true;} step++; // pre-MVA selection 1
    if (leptonlepton.M() < 5 || leptonlepton.M() > 100) {return true;} step++; // pre-MVA selection 2
    // bottom baseline selections
    if (bb_deltaR > 5) {return true;} step++; // pre-MVA selection 3
    if (bottombottom.M() < 22) {return true;} step++; // pre-MVA selection 4

    
    // get TLorentzVector to the global variable for Minuit
    g_missing = missing;
    g_lepton1 = lepton1; g_lepton2 = lepton2;
    return true;
}

void doubleHiggsAnalyser::Loop() {
  TTree *t = 0;
  bool keep = false;
  t = del_tree;
  int nevents = t->GetEntries();
  event_size = 1;
  int proc = 0; int temp = 0;
  int nevent = 0;
  for (int iev = 0; iev < nevents; iev++) {
    t->GetEntry(iev);
    keep = doubleHiggsAnalyser::Analysis();
    if (keep) out_tree->Fill();
    nevent ++; temp = nevent*100/nevents;
    if ( temp != proc ){
        proc ++;
        cout << "###############################" << endl;
        cout << " proceeding : " << proc << " %" << endl;
        cout << "###############################" << endl;
    }
    n_events_tree->Fill();
  }
  cout << "n_events are " << nevents << endl;
}

void doubleHiggsAnalyser::Finalize() {
  out_tree->Write();
  out_file->Close();
}


int main(Int_t argc,Char_t** argv)
{
  struct tm* datetime;
  time_t t;
  t = time(NULL);
  datetime = localtime(&t);
  TString output_path = "output_analyser/"+std::to_string(datetime->tm_year+1900)+std::to_string(datetime->tm_mon+1)+std::to_string(datetime->tm_mday);
  mkdir(output_path,0777);

  TChain *tree = new TChain("Delphes");
  TString output_name;
 
  std::string filename = "/xrootd_user/molamolabb/xrootd/doubleHiggs/sample/pp_hh/1.root";
  tree->Add(filename.c_str());
        
  output_name = "output_analyser/"+std::to_string(datetime->tm_year+1900)+std::to_string(datetime->tm_mon+1)+std::to_string(datetime->tm_mday)+"/"+"pp_hh.root";
  tree->SetBranchStatus("*",true);
    
  doubleHiggsAnalyser ana(tree, true);
  ana.Initiate(output_name); // usage : Initiate("outputfilename.root")
  ana.Loop(); // Loop through the events and do doubleHIggsAnalyser::Analysis() per event.
  ana.Finalize(); // Write the tree and Close the file.
  return 0;
}