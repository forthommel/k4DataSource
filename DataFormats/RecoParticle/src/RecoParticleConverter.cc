#include <edm4hep/ReconstructedParticleData.h>

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

#include "DataFormats/RecoParticle/include/RecoParticle.h"
#include "k4DataSource/DataFormatter.h"
#include "k4DataSource/k4DataConverters.h"

namespace rv = ROOT::VecOps;

class RecoParticlesConverter : public DataFormatter {
public:
  RecoParticlesConverter() : DataFormatter({"ReconstructedParticles"}, {"RecoParticles"}) {}

  void convert(ROOT::RDataFrame& rdf) override {
    auto convert_particles = [](rv::RVec<edm4hep::ReconstructedParticleData> parts) {
      rv::RVec<RecoParticle> out;
      for (const auto& part : parts)
        out.emplace_back(part.momentum.x, part.momentum.y, part.momentum.z, part.energy);
      return out;
    };
    rdf.Define("RecoParticles", convert_particles, {"ReconstructedParticles"});
  }
};

REGISTER_CONVERTER("RecoParticles", RecoParticlesConverter)
