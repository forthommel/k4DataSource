#include <edm4hep/ReconstructedParticleData.h>

#include "DataFormats/RecoJet/include/RecoJet.h"
#include "DataFormats/RecoParticle/include/RecoParticle.h"
#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4DataConverterFactory.h"

class RecoJetsConverter : public k4DataConverter {
public:
  RecoJetsConverter()
      : h_jets_(consumes<std::vector<edm4hep::ReconstructedParticleData> >("Jet")),
        h_reco_parts_(consumes<std::vector<RecoParticle> >("RecoParticles")) {
    produces<RecoJet>("RecoJet");
  }

  void convert() override {
    auto output = std::make_unique<std::vector<RecoJet> >();
    for (const auto& jet : *h_jets_) {
      auto& out_jet = output->emplace_back(jet.momentum.x, jet.momentum.y, jet.momentum.z, jet.energy);
      // add in jets constituents
      //FIXME work with references instead of copies
      for (auto it = jet.particles_begin; it < jet.particles_end; ++it)
        out_jet.addConstituent(h_reco_parts_->at(it));
    }
    put(std::move(output));
  }

private:
  Handle<std::vector<edm4hep::ReconstructedParticleData> > h_jets_;
  Handle<std::vector<RecoParticle> > h_reco_parts_;
};

REGISTER_CONVERTER("RecoJets", RecoJetsConverter)
