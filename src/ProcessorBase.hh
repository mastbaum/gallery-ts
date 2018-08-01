#ifndef __ts_core_ProcessorBase__
#define __ts_core_ProcessorBase__

/**
 * \file ProcessorBase.hh
 *
 * A generic processor that writes a ts tree.
 *
 * Author: A. Mastbaum <mastbaum@uchicago.edu>, 2018/01/25
 */

#include <string>
#include <vector>
#include <TTree.h>
#include "gallery/Event.h"

class TBranch;
class TFile;
class Event;

namespace Json {
  class Value;
}

/** Core framework functionality. */
namespace core {

/**
 * \class core::ProcessorBase
 * \brief A generic tree-writing event-by-event processor.
 */
class ProcessorBase {
friend class ProcessorBlock;
public:
  /** Constructor */
  ProcessorBase();

  /** Destructor */
  virtual ~ProcessorBase();

  /**
   * Fill the tree and increment the event index.
   */
  virtual void FillTree();

  /**
   * Add a branch to the output tree.
   *
   * Called in user subclasses to augment the default event tree.
   * This mirrors the TTree::Branch API.
   *
   * \param name The branch name
   * \param obj A pointer to the object
   * \returns A pointer to the created TBranch (we retain ownership)
   */
  template<class T>
  TBranch* AddBranch(std::string name, T* obj) {
    return fTree->Branch(name.c_str(), obj);
  }

  /**
   * Process one event.
   *
   * This also serves as a filter: if the function results false, it acts as a
   * filter and the event is not written out.
   *
   * \param ev The event, as a gallery::Event
   * \returns True if event passes filter
   */
  virtual bool ProcessEvent(gallery::Event& ev) = 0;

protected:
  /**
   * Perform user-level initialization.
   *
   * \param config A configuration, as a JSON filename.
   */
  virtual void Initialize(char* config=NULL);

  /**
   * Perform user-level initialization.
   *
   * \param config A configuration, as a JSON object.
   */
  virtual void Initialize(Json::Value* config=NULL) = 0;

  /** Perform user-level finalization. */
  virtual void Finalize() = 0;

  /**
   * Perform framework-level initialization.
   *
   * \param config A configuration as a JSON filename.
   */
  virtual void Setup(char* config=NULL);

  /**
   * Perform framework-level initialization.
   *
   * \param config A configuration as a JSON object
   */
  virtual void Setup(Json::Value* config=NULL);

  /** Perform framework-level finalization. */
  virtual void Teardown();

  /**
   * Populate the default event tree variables.
   *
   * \param ev The current gallery event
  */
  void BuildEventTree(gallery::Event& ev);

  unsigned long fEventIndex;  //!< An incrementing index
  std::string fOutputFilename;  //!< The output filename
  TFile* fOutputFile;  //!< The output ROOT file
  TTree* fTree;  //!< The output ROOT tree
  Event* fEvent;  //!< The standard output event data structure
  art::InputTag fTruthTag;  //!< art tag for MCTruth information
  art::InputTag fWeightTag;  //!< art tag for MCEventWeight information
};

}  // namespace core

#endif  // __ts_core_ProcessorBase__

