#include <edm4hep/ReconstructedParticleData.h>


#include "DataFormats/RecoParticle/include/RecoParticle.h"
#include "k4DataSource/DataFormatter.h"
#include "k4DataSource/k4DataConverters.h"

class RecoParticlesConverter : public DataFormatter {
public:
  RecoParticlesConverter() : DataFormatter({"ReconstructedParticles"}, {"RecoParticle"}) {}

  std::vector<void*> convert() override {
    //std::vector<edm4hep::ReconstructedParticleData> parts;
    //auto* parts = dynamic_cast<std::vector<edm4hep::ReconstructedParticleData>*>(input.at(0));
    auto* parts = reinterpret_cast<std::vector<edm4hep::ReconstructedParticleData>*>(input_data_.at(0));
    //auto* parts = (std::vector<edm4hep::ReconstructedParticleData>*)(input.at(0));
    //auto parts = *(edm4hep::ReconstructedParticleData*)(input.at(0));
    auto output = new std::vector<RecoParticle>();
    //for (const auto& part : *parts)
    //  output->emplace_back(part.momentum.x, part.momentum.y, part.momentum.z, part.energy);
    return {(void*)output};
  }
};

REGISTER_CONVERTER("RecoParticles", RecoParticlesConverter)
