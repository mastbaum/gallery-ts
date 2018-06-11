#ifndef __ts_core_Event__
#define __ts_core_Event__

/**
 * \file Event.hh
 *
 * The standard minimum output tree.
 *
 * This event structure is written out by every Processor subclass.
 *
 * Author: A. Mastbaum <mastbaum@uchicago.edu>, 2018/01/25
 */

#include <map>
#include <string>
#include <vector>
#include <TTree.h>
#include <TVector3.h>

#include "canvas/Persistency/Common/Wrapper.h"

/** art Wrapper instance to help the build. */
art::Wrapper<std::vector<std::map<std::string, std::vector<double> > > > a;

/**
 * \class Event
 * \brief The standard event data definition.
 */
class Event {
public:
  /** Maximum number of interactions. */
  static const size_t kMaxInteractions = 3;

  /** Maximum number of final state particles. */
  static const size_t kMaxFinalState = 50;

  /**
   * \class Event::Metadata
   * \brief Event-level information
   */
  class Metadata {
  public:
    Metadata() : run(-999), subrun(-999), eventID(-999) {}
    int run;  //!< Run ID
    int subrun;  //!< Subrun ID
    int eventID;  //!< Event ID
  };

  /**
   * \class Event::Neutrino
   * \brief Neutrino interaction information
   */
  class Neutrino {
  public:
    Neutrino()
      : ccnc(0), pdg(0), targetPDG(0), intcode(0), bjorkenX(-9999),
        inelasticityY(-9999), q2(-9999), w(-9999), t(-9999), energy(-9999),
        momentum(TVector3(0,0,0)) {}
               
    bool ccnc;  //!< CC (true) or NC (false)
    int pdg;  //!< PDG code of neutrino
    int targetPDG;  //!< PDG code of struck target
    int intcode;  //!< Interaction code (as for LArSoft MCNeutrino)
    double bjorkenX;  //!< Bjorken x
    double inelasticityY;  //!< Inelasticity y
    double q2;  //!< Q squared
    double w;  //!< Hadronic invariant mass W
    double t;  //!< Kinematic t
    double energy;  //!< Neutrino energy
    TVector3 momentum;  //!< Neutrino three-momentum
  };

  /**
   * \class Event::FinalStateParticle
   * \brief Final state particle information
   */
  class FinalStateParticle {
  public:
    FinalStateParticle() : pdg(0), energy(-9999), momentum(TVector3(0,0,0)) {}
    int pdg;  //!< PDG Code
    double energy;  //!< Energy
    TVector3 momentum;  //!< Three-momentum
  };

  /**
   * \class Event::Interaction
   * \brief All truth information associated with one neutrino interaction
   */
  class Interaction {
  public:
    /** Constructor. */
    Interaction() : nfinalstate(0) {}

    Neutrino neutrino;  //!< The neutrino
    FinalStateParticle lepton;  //!< The primary final state lepton
    size_t nfinalstate;  //!< Number of other final state particles

    /** The other final state particles. */
    FinalStateParticle finalstate[kMaxFinalState];

    /**
     * Event weights.
     *
     * This is a map from the weight calculator name to the list of weights
     * for all the sampled universes.
     */
    std::map<std::string, std::vector<double> > weights;
  };

  /** Constructor. */
  Event() : ninteractions(0) {}

  Metadata metadata;  //!< Event metadata
  size_t ninteractions;  //!< Number of interactions
  Interaction interactions[kMaxInteractions];  //!< All interactions
};

const size_t Event::kMaxInteractions;
const size_t Event::kMaxFinalState;

#endif  // __ts_core_Event__

