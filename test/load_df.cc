#include <TSystem.h>

#include <catch2/catch_test_macros.hpp>

#include "k4DataSource/k4DataSource.h"

TEST_CASE("k4DataFrame", "[basics]") {
  auto df = ROOT::MakeK4DataFrame({"root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/"
                                   "spring2021/IDEA/p8_ee_ZH_ecm240/events_101027117.root"},
                                  {"RecoParticles", "RecoJets"});
  df.Snapshot("events", "test.root", {"RecoJets", "RecoParticles"});
}
