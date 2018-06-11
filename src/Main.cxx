#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <vector>
#include <json/json.h>
#include <ProcessorBase.hh>
#include <ProcessorBlock.hh>
#include <TruthSelection.hh>
#include <Config.hh>

int main(int argc, char* argv[]) {
  // Parse command line arguments
  std::vector<char*> config_names;

  int c;
  while ((c=getopt(argc, argv, "c:")) != -1) {
    switch (c) {
      case 'c':
        config_names.push_back(optarg);
        break;
      case '?':
        if (optopt == 'c')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        abort();
    }
  }

  if (argc - optind < 1) {
    std::cout << "Usage: " << argv[0] << " [-c [Config]] "
              << "INPUTDEF [...]" << std::endl;
    return 0;
  }

  // Process input file definition
  std::string filedef = argv[optind];
  std::string list_suffix = ".list";
  std::vector<std::string> filenames;

  if (std::equal(list_suffix.rbegin(), list_suffix.rend(), filedef.rbegin())) {
    // File list
    std::ifstream infile(filedef);
    std::string filename;
    while (infile >> filename) {
      filenames.push_back(filename);
    }
  }
  else {
    // Files listed on command line
    for (int i=optind; i<argc; i++) {
      filenames.push_back(argv[i]);
    }
  }

  assert(!filenames.empty());

  // Setup
  int n_processors = config_names.empty() ? 1 : config_names.size();
  std::vector<Json::Value*> configs(n_processors);
  std::vector<ana::TruthSelection::TruthSelection*> procs(n_processors);

  std::cout << "Configuring... " << std::endl;
  for (size_t i=0; i<n_processors; i++) {
    procs[i] = new ana::TruthSelection::TruthSelection;
    configs[i] = config_names.empty() ? NULL : core::LoadConfig(config_names[i]);
  }

  core::ProcessorBlock block;
  for (int i=0; i<procs.size(); i++) {
    block.AddProcessor(procs[i], configs[i]);
  }

  std::cout << "Running... " << std::endl;
  block.ProcessFiles(filenames);

  block.DeleteProcessors();
  std::cout << "Done!" << std::endl;

  return 0;
}

