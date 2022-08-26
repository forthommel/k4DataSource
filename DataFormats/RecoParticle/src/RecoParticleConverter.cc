#include <edm4hep/ReconstructedParticleData.h>

#include "k4DataSource/k4DataConverters.h"

namespace converter {
  ROOT::RDataFrame* recoParticles(const ROOT::RDataFrame* rdf) { return const_cast<ROOT::RDataFrame*>(rdf); }
}  // namespace converter

REGISTER_CONVERTER("RecoParticles", recoParticles)
