#include <TSystem.h>

#include <catch2/catch_test_macros.hpp>

#include "k4DataSource/k4DataSource.h"

TEST_CASE("k4DataFrame", "[basics]") {
  auto df = ROOT::Experimental::MakeK4DataFrame("events",
                                                {"root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/"
                                                 "DelphesEvents/spring2021/IDEA/p8_ee_ZH_ecm240/events_101027117.root"},
                                                {"RecoJets", "RecoParticles"});
  df.Snapshot("events", "test.root", {"RecoJets", "RecoParticles"});
}
