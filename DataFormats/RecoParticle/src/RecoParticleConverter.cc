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

  std::vector<void*> convert(std::vector<void*> input) override {
    if (input.size() != cols_in_.size())
      throw std::runtime_error("Invalid inputs multiplicity: " + std::to_string(input.size()) +
                               " != " + std::to_string(cols_in_.size()) + "!");
    auto part = *(edm4hep::ReconstructedParticleData*)input.at(0);
    std::vector<void*> out;
    out.emplace_back((void*)new RecoParticle(part.momentum.x, part.momentum.y, part.momentum.z, part.energy));
    return out;
  }
};

REGISTER_CONVERTER("RecoParticles", RecoParticlesConverter)
