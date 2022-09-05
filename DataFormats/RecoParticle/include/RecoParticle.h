#ifndef DataFormats_RecoParticle_RecoParticle_h
#define DataFormats_RecoParticle_RecoParticle_h

#include <Math/Vector4D.h>

class RecoParticle {
public:
  RecoParticle() = default;
  explicit RecoParticle(double px, double py, double pz, double e) : momentum_(px, py, pz, e) {}
  virtual ~RecoParticle() = default;

  const ROOT::Math::XYZTVector& momentum() const { return momentum_; }

protected:
  ROOT::Math::XYZTVector momentum_;
};

#endif
