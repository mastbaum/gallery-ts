MCC8 Datasets
=============
MCC8 MC datasets have been created for truth-level sensitivity and systematics
studies which include truth-level information and a nominal set of
`MCEventWeight`s. Data products beyond `mcreco` have been dropped to minimize
file size and processing time.

Signal
------
The signal dataset is simulated based on the Mark Ross-Lonergan LEE signal
model, which is an extrapolation from the MiniBooNE low-energy excess.

* SAM definition: `mastbaum_mcc8_signal_mc_eventweight_v2`
* Size: 140 GB, 47 files
* Total POT: 8.8125e20

Intrinsic nue
-------------
The intrinsic nue sample includes nue and nuebar CC interactions only, to
enhance the signal statistics. Use care when combining this with the BNB
cocktail to ensure correct POT accounting.

* SAM definition: `mastbaum_mcc8_nue_mc_eventweight_v2`
* Size: 202 GB, 8195 files
* Total POT: 5.03153e22

Based on dataset `prodgenie_bnb_intrinsic_nue_cosmic_uboone_mcc8_detsim`.

BNB Inclusive
-------------
The BNB inclusive sample uses the nominal flux for all flavors (and is thus
primarily numus).

* SAM definition: `mastbaum_mcc8_bnb_mc_eventweight_v2`
* Size: 842 GB, 34109 files
* Total POT: TBD

Based on dataset `prodgenie_bnb_nu_cosmic_uboone_mcc8_detsim`.

