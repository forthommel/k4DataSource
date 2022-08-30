#ifndef DataFormats_RecoJet_RecoJet_h
#define DataFormats_RecoJet_RecoJet_h

#include <vector>

#include "DataFormats/RecoParticle/include/RecoParticle.h"

class RecoJet : public RecoParticle {
public:
  using RecoParticle::RecoParticle;
  virtual ~RecoJet() = default;

  void addConstituent(const RecoParticle& part) { constituents_.emplace_back(part); }

private:
  std::vector<RecoParticle> constituents_;
};

#endif
