#ifndef __ts_ana_TruthSelection_Selections__
#define __ts_ana_TruthSelection_Selections__

/**
 * \file Selections.hh
 *
 * Event selection methods for the truth selection.
 *
 * Author: A. Mastbaum <mastbaum@uchicago.edu>
 */

#include <vector>

namespace sim {
 class MCTrack;
 class MCShower;
}

namespace ana {
  namespace TruthSelection {

    /** Event selections for the truth selection. */
    namespace selections {

/**
 * True CC nue selection.
 *
 * Uses true neutrino PDG and interaction type directly.
 *
 * \param mctruths True MC event data
 * \param mctracks True MC tracks
 * \param mcshowers True MC showers
 * \param ereco Reconstructed energy (return by reference)
 * \param weight Event weight (return by reference)
 * \returns True if the event passes the selection
 */
bool CCNueTrue(const std::vector<simb::MCTruth>& mctruths,
               const std::vector<sim::MCTrack>& mctracks,
               const std::vector<sim::MCShower>& mcshowers,
               double& ereco, double& weight);


/**
 * True CC numu selection.
 *
 * Uses true neutrino PDG and interaction type directly.
 *
 * \param mctruths True MC event data
 * \param mctracks True MC tracks
 * \param mcshowers True MC showers
 * \param ereco Reconstructed energy (return by reference)
 * \param weight Event weight (return by reference)
 * \returns True if the event passes the selection
 */
bool CCNumuTrue(const std::vector<simb::MCTruth>& mctruths,
                const std::vector<sim::MCTrack>& mctracks,
                const std::vector<sim::MCShower>& mcshowers,
                double& ereco, double& weight);


/**
 * True 1l1p0pi0 selection.
 *
 * Uses true neutrino PDG and interaction type directly.
 *
 * \param mctruths True MC event data
 * \param mctracks True MC tracks
 * \param mcshowers True MC showers
 * \param leptonPDG PDG code for the lepton to select
 * \param ereco Reconstructed energy (return by reference)
 * \param weight Event weight (return by reference)
 * \returns True if the event passes the selection
 */
bool True1l1p0pi0(const std::vector<simb::MCTruth>& mctruths,
                  const std::vector<sim::MCTrack>& mctracks,
                  const std::vector<sim::MCShower>& mcshowers,
                  int leptonPDG,
                  double& ereco, double& weight);

/**
 * CC nue selection.
 *
 * \param mctruths True MC event data
 * \param mctracks True MC tracks
 * \param mcshowers True MC showers
 * \param ereco Reconstructed energy (return by reference)
 * \param weight Event weight (return by reference)
 * \returns True if the event passes the selection
 */
bool CCNue(const std::vector<simb::MCTruth>& mctruths,
           const std::vector<sim::MCTrack>& mctracks,
           const std::vector<sim::MCShower>& mcshowers,
           double& ereco, double& weight);


/**
 * CC numu selection.
 *
 * \param mctruths True MC event data
 * \param mctracks True MC tracks
 * \param mcshowers True MC showers
 * \param ereco Reconstructed energy (return by reference)
 * \param weight Event weight (return by reference)
 * \returns True if the event passes the selection
 */
bool CCNumu(const std::vector<simb::MCTruth>& mctruths,
            const std::vector<sim::MCTrack>& mctracks,
            const std::vector<sim::MCShower>& mcshowers,
            double& ereco, double& weight);

/**
 * CCpi0 selection.
 *
 * \param mctruths True MC event data
 * \param mctracks True MC tracks
 * \param mcshowers True MC showers
 * \param ereco Reconstructed energy (return by reference)
 * \param weight Event weight (return by reference)
 * \returns True if the event passes the selection
 */
bool CCPi0(const std::vector<simb::MCTruth>& mctruths,
           const std::vector<sim::MCTrack>& mctracks,
           const std::vector<sim::MCShower>& mcshowers,
           double& ereco, double& weight);

    }  // namespace selections
  }  // namespace TruthSelection
}  // namespace ana

#endif  // __ts_ana_TruthSelection_Selections__

