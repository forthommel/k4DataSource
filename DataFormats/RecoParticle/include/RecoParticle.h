#ifndef DataFormats_RecoParticle_RecoParticle_h
#define DataFormats_RecoParticle_RecoParticle_h

class RecoParticle {
public:
  RecoParticle() = default;
  explicit RecoParticle(double px, double py, double pz, double e) : px_(px), py_(py), pz_(pz), e_(e) {}
  virtual ~RecoParticle() = default;

  double px() const { return px_; }
  double py() const { return py_; }
  double pz() const { return pz_; }
  double e() const { return e_; }

protected:
  double px_{0.};
  double py_{0.};
  double pz_{0.};
  double e_{0.};
};

#endif
