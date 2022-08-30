#include <edm4hep/ReconstructedParticleData.h>

#include "DataFormats/RecoParticle/include/RecoParticle.h"
#include "k4DataSource/DataFormatter.h"
#include "k4DataSource/k4DataConverters.h"

class RecoParticlesConverter : public DataFormatter {
public:
  RecoParticlesConverter()
      : h_reco_parts_(consumes<std::vector<edm4hep::ReconstructedParticleData> >("ReconstructedParticles")) {
    produces<RecoParticle>("RecoParticle");
  }

  void convert() override {
    auto output = std::make_unique<std::vector<RecoParticle> >();
    const auto& parts = *h_reco_parts_;
    for (const auto& part : *h_reco_parts_)
      output->emplace_back(part.momentum.x, part.momentum.y, part.momentum.z, part.energy);
    put(std::move(output));
  }

private:
  Handle<std::vector<edm4hep::ReconstructedParticleData> > h_reco_parts_;
};

REGISTER_CONVERTER("RecoParticles", RecoParticlesConverter)
