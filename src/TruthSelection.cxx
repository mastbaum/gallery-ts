#include <iostream>
#include <vector>
#include <TH2D.h>
#include <TTree.h>
#include <json/json.h>
#include "gallery/ValidHandle.h"
#include "canvas/Utilities/InputTag.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCFlux.h"
#include "lardataobj/MCBase/MCTrack.h"
#include "lardataobj/MCBase/MCShower.h"
#include "uboone/EventWeight/MCEventWeight.h"
#include "ProcessorBase.hh"
#include "TruthSelection.hh"
#include "Selections.hh"

namespace ana {
  namespace TruthSelection {

TruthSelection::TruthSelection()
    : ProcessorBase(), fEventCounter(0), fSelectedCounter(0) {}


void TruthSelection::Initialize(Json::Value* config) {
  // Load configuration parameters
  fSelectionType = "ccnue_true";
  fTruthTag = { "generator" };
  fFluxTag = { "generator" };
  fTrackTag = { "mcreco" };
  fShowerTag = { "mcreco" };
  fWeightTag = { "eventweight" };

  if (config) {
    fSelectionType = (*config)["TruthSelection"].get("SelectionType", fSelectionType).asString();
    fTruthTag = { (*config)["TruthSelection"].get("MCTruthTag", "generator").asString() };
    fFluxTag = { (*config)["TruthSelection"].get("MCFluxTag", "generator").asString() };
    fTrackTag = { (*config)["TruthSelection"].get("MCTrackTag", "mcreco").asString() };
    fShowerTag = { (*config)["TruthSelection"].get("MCShowerTag", "mcreco").asString() };
    fWeightTag = { (*config)["TruthSelection"].get("MCWeightTag", "eventweight").asString() };
  }

  // Add custom branches
  AddBranch("reco_wgh", &fWeight);
  AddBranch("reco_pdg", &fRecoPDG);

  AddBranch("run", &fRun);
  AddBranch("subrun", &fSubrun);
  AddBranch("event", &fEventID);
  AddBranch("reco_energy", &fRecoEnergy);
  AddBranch("true_nu_energy", &fNuEnergy);
  AddBranch("true_nu_L", &fNuBaseline);
  AddBranch("weights", &fWeights);
}


void TruthSelection::Finalize() {}


bool TruthSelection::ProcessEvent(gallery::Event& ev) {
  if (fEventCounter % 10 == 0) {
    std::cout << "TruthSelection: Processing event " << fEventCounter << " "
              << "(" << fSelectedCounter << " neutrinos selected)"
              << std::endl;
  }
  fEventCounter++;

  // Grab a data product from the event
  auto const& mcfluxes = *ev.getValidHandle<std::vector<simb::MCFlux> >(fFluxTag);
  auto const& mctruths = *ev.getValidHandle<std::vector<simb::MCTruth> >(fTruthTag);
  auto const& mctracks = *ev.getValidHandle<std::vector<sim::MCTrack> >(fTrackTag);
  auto const& mcshowers = *ev.getValidHandle<std::vector<sim::MCShower> >(fShowerTag);

  // Event auxiliary information
  auto const& evaux = ev.eventAuxiliary();
  fRun = evaux.run();
  fSubrun = evaux.subRun();
  fEventID = evaux.event();

  // Other truth-level information
  gallery::Handle<std::vector<::evwgh::MCEventWeight> > wgh;
  bool hasWeights = ev.getByLabel(fWeightTag, wgh);

  if (hasWeights) {
    if (wgh->size() != mctruths.size()) {
      std::cerr << wgh->size() << " " << mctruths.size() << std::endl;
    }
    assert(wgh->size() == mctruths.size());
    fWeights = wgh->at(0).fWeight;
  }

  if (mctruths.size() > 1) {
    std::cerr << "Multiple neutrinos, using true energy for the first one." << std::endl;
  }

  if (!mctruths.empty()) {
    const simb::MCNeutrino& nu = mctruths.at(0).GetNeutrino();
    fNuEnergy = nu.Nu().EndMomentum().Energy() * 1000;  // GeV to MeV

    // Distance from parent decay to interaction point (FIXME)
    const simb::MCFlux& flux = mcfluxes.at(0);
    TVector3 vi(flux.fvx, flux.fvy, flux.fvz);
    TVector3 vo = nu.Nu().EndPosition().Vect();
    //fNuBaseline = (vo - vi).Mag();
    fNuBaseline = -999;
  }

  // Apply selection using tracks and showers
  bool pass = false;
  double fWeight = 1.0;

  if (fSelectionType == "ccnue_true") {
    pass = selections::CCNueTrue(mctruths, mctracks, mcshowers, fRecoEnergy, fWeight);
    fRecoPDG = 11;
  }
  else if (fSelectionType == "ccnum_true") {
    pass = selections::CCNumuTrue(mctruths, mctracks, mcshowers, fRecoEnergy, fWeight);
    fRecoPDG = 13;
  }
  else if (fSelectionType == "ccnue") {
    pass = selections::CCNue(mctruths, mctracks, mcshowers, fRecoEnergy, fWeight);
    fRecoPDG = 11;
  }
  else if (fSelectionType == "ccnumu") {
    pass = selections::CCNumu(mctruths, mctracks, mcshowers, fRecoEnergy, fWeight);
    fRecoPDG = 13;
  }
  else if (fSelectionType == "1e1p") {
    pass = selections::True1l1p0pi0(mctruths, mctracks, mcshowers, 11, fRecoEnergy, fWeight);
    fRecoPDG = 11;
  }
  else if (fSelectionType == "1m1p") {
    pass = selections::True1l1p0pi0(mctruths, mctracks, mcshowers, 13, fRecoEnergy, fWeight);
    fRecoPDG = 13;
  }
  else if (fSelectionType == "ccpi0") {
    pass = selections::CCPi0(mctruths, mctracks, mcshowers, fRecoEnergy, fWeight);
    fRecoPDG = 111;
  }
  else {
    std::cerr << "TruthSelection: Unknown selection type \""
              << fSelectionType << "\"" << std::endl;
    assert(false);
  }

  if (pass) {
    fSelectedCounter++;
    return true;
  }

  return false; 
}

  }  // namespace TruthSelection
}  // namespace ana

