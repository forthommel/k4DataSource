#include <edm4hep/ReconstructedParticleData.h>

#include "DataFormats/RecoParticle/include/RecoParticle.h"
#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4DataConverterFactory.h"

class RecoParticlesConverter : public k4DataConverter {
public:
  RecoParticlesConverter(const k4Parameters& params)
      : k4DataConverter(params),
        h_reco_parts_(consumes<std::vector<edm4hep::ReconstructedParticleData> >(
            params.get<std::string>("src", "ReconstructedParticles"))) {
    produces<std::vector<RecoParticle> >();
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
