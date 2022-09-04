# RDataSource for key4hep

![GitHub branch checks state](https://img.shields.io/github/checks-status/forthommel/k4DataSource/master)

`k4DataSource` is a prototype for the readout of `edm4hep` datasets and the recasting of their various collections into more "analysis-friendly" schemes usable by ROOT's `RDataFrame`.

For the time being, the following operations are handled/envisioned:

- [x] loading of the various `edm4hep`/`TTree` information from input files
- [x] providing the skeletton for recasting modules to build the complex output objects
  - [ ] defining an output format directly usable by users in their workflow (standard and more complex analyses cuts, ...)
  - [ ] handling references between objects in different collections (e.g. jets + their constituents, leptons + their associated reconstructed particle, ...)
- [x] handling the storage of these more complex output objects into `RDataFrame`'s "snapshot" output trees
- [ ] reproducing a few standard candle analyses in this updated scheme

## Installation instructions

The setup and installation instructions are following closely the procedure defined in [FCCAnalyses](https://github.com/HEP-FCC/FCCAnalyses).
Still being in heavy development, we advise you to build this package with the debugging flags, and with an installation path under the project's scope, i.e.:

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Debug
```
