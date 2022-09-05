#include <catch2/catch_test_macros.hpp>

#include "k4DataSource/k4DataSource.h"

TEST_CASE("k4DataFrame", "[basics]") {
  auto df = MakeK4DataFrame({"root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/"
                             "p8_ee_ZH_ecm240/events_101027117.root"},
                            std::vector<std::string>{"RecoParticles:recoParticles", "RecoJets:jets"})
                ->Range(0, 1000);
  df.Snapshot("events", "test.root", {"recoParticles", "jets"});
}

TEST_CASE("k4DataFrameMT", "[basics]") {
  ROOT::EnableImplicitMT();
  auto df = MakeK4DataFrame({"root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/"
                             "p8_ee_ZH_ecm240/events_101027117.root"},
                            std::vector<std::string>{"RecoParticles:recoParticles", "RecoJets:jets"})
                ->Snapshot("events", "test.root", {"recoParticles", "jets"});
}
