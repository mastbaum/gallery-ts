#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>
#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TPaveText.h"
#include "TTree.h"
#include "TStyle.h"
#include "Covariance.hh"
#include "Event.hh"

namespace util {

std::vector<std::vector<TGraph*> > BinCorrelations(
    TH1D* enu, std::vector<TH1D*> enu_syst) {
  size_t nbins = enu->GetNbinsX();
  size_t nuni = enu_syst.size();

  std::vector<std::vector<TGraph*> > v(nbins);
  for (size_t i=0; i<nbins; i++) {
    v[i] = std::vector<TGraph*>(nbins);
    for (size_t j=0; j<nbins; j++) {
      v[i][j] = new TGraph(nuni);
    }
  }

  for (size_t i=0; i<nbins; i++) {
    for (size_t j=0; j<nbins; j++) {
      for (size_t k=0; k<nuni; k++) {
        v[i][j]->SetPoint(k, enu_syst[k]->GetBinContent(i),
                             enu_syst[k]->GetBinContent(j));
      }
    }
  }

  return v;
}


/******************************************************************************
 ** Covariance::EventSample implementation                                 **
 *****************************************************************************/

Covariance::EventSample::EventSample(std::string _name,
                                       size_t nbins,
                                       double elo, double ehi,
                                       size_t nweights)
    : name(_name), enu(nullptr), cov(nullptr) {
  enu = new TH1D(("enu_" + name).c_str(),
                 ";#nu Energy [MeV];Entries per bin",
                 nbins, elo, ehi);
  enu->Sumw2();

  Resize(nweights);
}


Covariance::EventSample::~EventSample() {
  delete cov;
  delete enu;
}


TGraphErrors* Covariance::EventSample::EnuCollapsed() {
  // Compute the covariance matrix first, if we haven't already
  if (!cov) {
    CovarianceMatrix();
  }

  // x/y values and (symmetric) errors
  const size_t nbins = enu->GetNbinsX();
  double xv[nbins];
  double xe[nbins];
  double yv[nbins];
  double ye[nbins];

  for (size_t i=0; i<nbins; i++) {
    xv[i] = enu->GetBinCenter(i);
    xe[i] = enu->GetBinWidth(i) / 2;
    yv[i] = enu->GetBinContent(i);
    ye[i] = sqrt(cov->GetBinContent(i, i));
  }

  return new TGraphErrors(nbins, xv, yv, xe, ye);
}


void Covariance::EventSample::Resize(size_t nweights) {
  enu_syst.clear();
  for (size_t i=0; i<nweights; i++) {
    std::string hname = Form("enu_%s_%zu", name.c_str(), i);
    TH1D* h = (TH1D*) enu->Clone(hname.c_str());
    h->SetDirectory(NULL);
    enu_syst.push_back(h);
  }
}


TH2D* Covariance::EventSample::CovarianceMatrix(
    TH1D* nom, std::vector<TH1D*> syst) {
  int nbins = nom->GetNbinsX();

  TH2D* _cov = new TH2D("cov", "", nbins, 0, nbins, nbins, 0, nbins);

  for (int i=1; i<nbins+1; i++) {
    for (int j=1; j<nbins+1; j++) {
      double vij = 0;
      for (size_t k=0; k<syst.size(); k++) {
        double vi = nom->GetBinContent(i) - syst[k]->GetBinContent(i);
        double vj = nom->GetBinContent(j) - syst[k]->GetBinContent(j);
        vij += vi * vj / syst.size();
      }
      _cov->SetBinContent(i, j, vij);
    }
  }

  return _cov;
}


TH2D* Covariance::EventSample::CovarianceMatrix() {
  delete cov;
  cov = CovarianceMatrix(enu, enu_syst);
  cov->SetName(("cov_" + name).c_str());
  return cov;
}



TH2D* Covariance::EventSample::CorrelationMatrix(TH2D* _cov) {
  TH2D* cor = (TH2D*) _cov->Clone("cor");

  for (int i=1; i<_cov->GetNbinsX()+1; i++) {
    for (int j=1; j<_cov->GetNbinsY()+1; j++) {
      double vij = _cov->GetBinContent(i, j);
      double si = sqrt(_cov->GetBinContent(i, i));
      double sj = sqrt(_cov->GetBinContent(j, j));
      cor->SetBinContent(i, j, vij / (si * sj));
    }
  }

  return cor;
}


TH2D* Covariance::EventSample::CorrelationMatrix() {
  // Compute the covariance matrix first, if we haven't already
  if (!cov) {
    CovarianceMatrix();
  }

  TH2D* cor = CorrelationMatrix(cov);
  cor->SetName(("cor_" + name).c_str());
  return cor;
}


/******************************************************************************
 ** Covariance implementation                                              **
 *****************************************************************************/

Covariance::Covariance() {}


void Covariance::AddWeight(std::string w) {
  use_weights.insert(w);
}


void Covariance::init() {
  assert(!fInputFiles.empty());

  fFile = TFile::Open(fOutputFile.c_str(), "recreate");
  assert(fFile);

  /////////////////////////////////////////////////////////////////////////////
  // FIXME: Sample definitions for matrix blocks
  samples.push_back(new EventSample("nue"));
  samples.push_back(new EventSample("numu"));
  /////////////////////////////////////////////////////////////////////////////

  std::cout << "Covariance: Initialized. Weights: ";
  for (auto it : use_weights) {
    std::cout << it << " ";
  }
  std::cout << std::endl;

  std::cout << "Covariance: Writing output to " << fOutputFile << std::endl;
}


void Covariance::analyze() {

  for (size_t ii=0; ii<fInputFiles.size(); ii++) {
    TFile f(fInputFiles[ii].c_str());
    TTree* _tree = (TTree*) f.Get("tsana");
    assert(_tree && _tree->GetEntries() > 0);

    Event* event = new Event;
    double reco_e;
    _tree->SetBranchAddress("events", &event);
    _tree->SetBranchAddress("reco_e", &reco_e);

    // Event loop
    for (long k=0; k<_tree->GetEntries(); k++) {
      _tree->GetEntry(k);

      if (event->ninteractions == 0) {
        continue;
      }

      Event::Interaction& interaction = event->interactions[0];

      // Iterate through all the weighting functions to compute a set of
      // total weights for this event. mcWeight is a mapping from reweighting
      // function name to a vector of weights for each "universe."
      std::vector<double> weights;
      size_t wmin = 100000;
      for (auto const& it : interaction.weights) {
        if (use_weights.find(it.first) == use_weights.end()) { continue; }
        if (it.second.size() < wmin) {
          wmin = it.second.size();
        }

        assert(wmin < 1000000);
        weights.resize(wmin, 1.0);

        // Compute universe-wise product of all requsted weights
        if (use_weights.find("*") != use_weights.end() ||
            use_weights.find(it.first) != use_weights.end()) {
          std::cout << it.first << " " << it.second[0] << std::endl;
          for (size_t i=0; i<weights.size(); i++) {
            weights[i] *= it.second[i];
          }
        }
      }

      // The observable
      double nuEnergy = reco_e; // interaction.neutrino.energy * 1000;

      /////////////////////////////////////////////////////////////////////////
      // FIXME: Determine which sample this event belongs to
      EventSample* sample = nullptr;
      double fs = 1.0;  // Scale factor

      // Scale for CCnue or inclusive sample
      // 1e1p in nue sample
      if (ii == 0) {
        fs = 6.6e20 / 5.0315296e+22;
        sample = samples[0];
      }
      // 1m1p in bnb sample
      else if (ii == 1) {
        if (abs(interaction.neutrino.pdg) == 12 && interaction.neutrino.ccnc == 0) {
          // Just throw out tiny CCnue(bar) from the inclusive sample
          continue;
        }
        fs = 6.6e20 / 1.72072967e+21;
        sample = samples[1];
      }
      // 1e1p in bnb sample
      else if (ii == 2) {
        if (abs(interaction.neutrino.pdg) == 12 && interaction.neutrino.ccnc == 0) {
          // Just throw out tiny CCnue(bar) from the inclusive sample
          continue;
        }
        fs = 6.6e20 / 1.72072967e+21;
        sample = samples[0];
      }
      /////////////////////////////////////////////////////////////////////////

      // Fill histograms for this event sample
      if (sample->enu_syst.empty()) {
        sample->Resize(weights.size());
      }
      else {
        assert(sample->enu_syst.size() == weights.size());
      }

      // CV histogram
      sample->enu->Fill(nuEnergy, fs);

      // Systematics histograms with weights
      for (size_t i=0; i<weights.size(); i++) {
        sample->enu_syst[i]->Fill(nuEnergy, weights[i] * fs);
      }
    }
  }

  /////////////////////////////////////////////////////////
  // Output
  size_t total_bins = 0;

  fFile->cd();

  // Write out sample=-wise distributions
  for (size_t i=0; i<samples.size(); i++) {
    samples[i]->enu->Write();
    total_bins += samples[i]->enu->GetNbinsX();

    TH2D* cov = samples[i]->CovarianceMatrix();
    cov->Write();

    TH2D* cor = samples[i]->CorrelationMatrix();
    cor->Write();

    TGraphErrors* g = samples[i]->EnuCollapsed();
    g->SetName(("err_" + samples[i]->name).c_str());
    g->Write();
  }

  // Global (sample-to-sample) distributions
  // Build glued-together energy spectra for the nominal and each systematics
  // universe, and feed into the correlation matrix calculator.
  total_bins -= samples.size();
  TH1D hg("hg", ";E_{#nu};Entries per bin", total_bins, 0, total_bins);
  hg.Sumw2();
  std::vector<TH1D*> hgsys;
  for (size_t i=0; i<samples[0]->enu_syst.size(); i++) {
    hgsys.push_back(new TH1D(Form("hg%zu", i), "", total_bins, 0, total_bins));
    hgsys[i]->Sumw2();
  }
  size_t ibin = 0;
  for (size_t i=0; i<samples.size(); i++) {
    for(int j=1; j<samples[i]->enu->GetNbinsX()+1; j++) {
      hg.SetBinContent(ibin, samples[i]->enu->GetBinContent(j));
      hg.SetBinError(ibin, samples[i]->enu->GetBinError(j));
      if (hgsys.size() != samples[i]->enu_syst.size()) {
        continue;
      }
      for (size_t k=0; k<hgsys.size(); k++) {
        hgsys[k]->SetBinContent(ibin, samples[i]->enu_syst[k]->GetBinContent(j));
      }
      ibin++;
    }
  }

  hg.Write();

  TH2D* gcov = EventSample::CovarianceMatrix(&hg, hgsys);
  gcov->Write();

  TH2D* gcor = EventSample::CorrelationMatrix(gcov);
  gcor->Write();
}

}  // namespace util

