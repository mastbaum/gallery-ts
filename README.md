Gallery Truth Selection
========================
This package provides a lightweight event selection and analysis framework.

It is built on [gallery](http://art.fnal.gov/gallery/), a library designed
for reading [art](http://art.fnal.gov) ROOT files.

Documentation
-------------
In addition to this README, the code is fully documented with Doxygen.
To build it, run

    doxygen doc/Doxyfile

The output HTML documentation is placed in `doc/html/index.html`.

*Note to developers:* All code in in this package should be completely
documented with Doxygen-compatible comments. Please build the
documentation before committing to check for anything missing.

Building
--------
The build process is managed by [CMake](https://cmake.org). A number of
dependencies are required to read art ROOT files, which are most easily
configured using [ups](https://cdcvs.fnal.gov/redmine/projects/ups/wiki).

For example, to configure an environment for reading MicroBooNE
files on a host with CVMFS access:
    
    source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh

Then set up your favorite MCC8 release, e.g.:

    source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh
    setup uboonecode v06_26_01_20 -q e10:prof
    setup gallery v1_03_13 -q e10:prof:nu
    setup cmake

To build the software:

    mkdir build && cd build  # A directory for build products
    cmake ..  # Configure the build system
    make install  # Build
    source bin/setup_ts.sh  # Set up environment

Note that the setup script `setup_ts.sh` is created during the
build and contains absolute paths to the installation location.

The `cmake` command accepts many useful options. For example:

* `-DCMAKE_INSTALL_PREFIX=/some/path`: Install the software at  `/some/path`.
  The default is to install inside the build directory.
* `-DCMAKE_BUILD_TYPE=Debug`: Build with debugging flags enabled (i.e. `-g`)

Final binaries are placed in the `bin` directory and libraries in `lib`.

Analysis
--------
The analysis code is run over a set of files using a framework-level
executable:

    selection [-c CONFIG] [[-c CONFIG] ...] INPUT

    CONFIG - JSON configuration file
    INPUT - Input files (see note)

The configuration file contains settings which are passed to the selection
processor's `Initialize` function.

The input files can be defined as one or more ROOT files (ending in `.root`)
or one file list (ending in `.list` and containing one filename per line, with
no comments or blank lines). Support is planned for SAM dataset definitions
in future versions.

The selections are defined in the `Selections` class, and referenced by a
string name in the `TruthSelection` class. Currently available selections
include:

* `ccnue_true`
* `ccnum_true`
* `ccnue`
* `ccnumu`
* `1e1p`
* `1m1p`
* `ccpi0`

and additional selections can be implemented by adding a new funcion in the
`Selections` namespace. These functions can use any information available to
gallery.

The configuration file defines the selection to use, as well as other run
time options. For example, to run the `1e1p` selection:

    {
      "OutputFile": "output_TruthSelection_1e1p.root",
      "MCWeightTag": "mcweight",
      "TruthSelection": {
        "SelectionType": "1e1p"
      }
    }

Note that multiple configuration files can be passed on the command line
(using the `-c` flag several times), to apply multiple selections and
produce several output trees while only running over an MC sample once.

### Analyzing the Output

The output file is a ROOT file with a tree named `events` plus any additional
objects written by the selection code. The `events` tree contains a branch
called `events`; this is the standard event-level information written out
by all processors, stored in an `Event` object. See `core/Event.hh` for the
complete definition.

To read the output files in ROOT, one must load the event dictionary, which
is stored in `libts_Processor.so`. Compiled code should link to this
library, and on the ROOT command line one can run:

    .L $TS_LIB_DIR/libts_Event.so

Now, we can open the file in a `TBrowser`. One can make plots interactively,
or analyze this tree with a ROOT macro or script. For example, in the ROOT
console:

    root [0] .L $TS_LIB_DIR/libts_Event.so
    root [1] TFile f("output.root")
    root [2] events->Draw("interactions.lepton.energy")

This will produce a plot of the primary lepton energies for all neutrino
interactions.

### Covariance Matrices

A utility (`bin/covariance`) is provided for computing covariance matrices
given selected samples. This is implemented in the `Covariance` class and
`bin/CovarianceMain.cxx`, and currently makes hard-coded assumptions about
the selections. Until this is improved, modify the covariance calculator to
suit your analysis needs. The areas that will likely need changes are noted
with comments.

Authors
-------
This package is a simplified subset of the `sbncode` analysis framework,
developed by:

* A. Mastbaum, UChicago
* G. Putnam, UChicago
* J. Zennamo, UChicago
* D. Schmitz, UChicago

License
-------
This package is licensed under an MIT license. See `LICENSE.txt` for details.

