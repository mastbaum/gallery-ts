#include <iostream>
#include <string>
#include <vector>
#include <Covariance.hh>

int main(int argc, char* argv[]) {
  util::Covariance cov;

  if (argc != 4) {
    std::cout << "Usage: " << argv[0]
              << " output.root nue.root numu.root"
              << std::endl;
    return 1;
  }

  // Input/output files
  cov.SetOutputFile(argv[1]);
  cov.AddInputFile(argv[2]);
  cov.AddInputFile(argv[3]);

  // Weights
  cov.AddWeight("kminus_PrimaryHadronNormalization");
  cov.AddWeight("kplus_PrimaryHadronFeynmanScaling");
  cov.AddWeight("kzero_PrimaryHadronSanfordWang");
  cov.AddWeight("nucleoninexsec_FluxUnisim");
  cov.AddWeight("nucleonqexsec_FluxUnisim");
  cov.AddWeight("nucleontotxsec_FluxUnisim");
  cov.AddWeight("piminus_PrimaryHadronSWCentralSplineVariation");
  cov.AddWeight("pioninexsec_FluxUnisim");
  cov.AddWeight("pionqexsec_FluxUnisim");
  cov.AddWeight("piontotxsec_FluxUnisim");
  cov.AddWeight("piplus_PrimaryHadronSWCentralSplineVariation");
  cov.AddWeight("expskin_FluxUnisim");
  cov.AddWeight("horncurrent_FluxUnisim");

  cov.init();
  cov.analyze();

  return 0;
}

