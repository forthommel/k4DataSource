#include <edm4hep/ReconstructedParticleData.h>

#include <iostream>  //FIXME

#include "DataFormats/RecoJet/include/RecoJet.h"
#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4DataConverterFactory.h"

class RecoJetsConverter : public k4DataConverter {
public:
  RecoJetsConverter()
      : h_jets_(consumes<std::vector<edm4hep::ReconstructedParticleData> >("Jet")),
        h_reco_parts_(consumes<std::vector<edm4hep::ReconstructedParticleData> >("ReconstructedParticles")) {
    produces<RecoJet>("RecoJet");
  }

  void convert() override {
    auto output = std::make_unique<std::vector<RecoJet> >();
    for (const auto& jet : *h_jets_) {
      auto& out_jet = output->emplace_back(jet.momentum.x, jet.momentum.y, jet.momentum.z, jet.energy);
      for (auto it = jet.particles_begin; it < jet.particles_end; ++it) {
        const auto& jc = h_reco_parts_->at(it);
        out_jet.addConstituent(RecoParticle(jc.momentum.x, jc.momentum.y, jc.momentum.z, jc.energy));
      }
    }
    put(std::move(output));
  }

private:
  Handle<std::vector<edm4hep::ReconstructedParticleData> > h_jets_;
  Handle<std::vector<edm4hep::ReconstructedParticleData> > h_reco_parts_;
};

REGISTER_CONVERTER("RecoJets", RecoJetsConverter)
