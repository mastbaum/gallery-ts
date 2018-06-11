#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <dlfcn.h>
#include <json/json.h>
#include <ProcessorBase.hh>
#include <ProcessorBlock.hh>

namespace core {

Json::Value* LoadConfig(char* configfile) {
  Json::Value* config = NULL;

  if (configfile) {
    std::cout << "Loading configuration: " << configfile << "... ";
    std::ifstream configstream(configfile, std::ifstream::binary);
    config = new Json::Value;
    Json::Reader reader;
    bool r = reader.parse(configstream, *(config));
    if (!r) {
      std::cerr << "Error parsing configuration file "
                << configfile << std::endl;
      exit(2);
    }
    std::cout << "OK" << std::endl;
  }

  return config;
}

}  // namespace core

