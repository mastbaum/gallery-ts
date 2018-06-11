#ifndef __ts_core_Loader__
#define __ts_core_Loader__

/**
 * \file Config.hh
 *
 * Loading Processors from shared libraries.
 *
 * Author: A. Mastbaum <mastbaum@uchicago.edu>, 2018/02/05
 */

#include <string>
#include <vector>

namespace Json {
  class Value;
}

namespace core {

class ProcessorBase;

/**
 * Load configuration from JSON file to object.
 *
 * \param configfile Path to the JSON file
 * \returns Configuration as a JSON object
 */
Json::Value* LoadConfig(char* configfile);

}  // namespace core

#endif  // __ts_core_Loader__

