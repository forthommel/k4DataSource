#include <TSystem.h>

#include <catch2/catch_test_macros.hpp>

#include "k4DataSource/k4DataSource.h"

TEST_CASE("k4DataFrame", "[basics]") {
  gSystem->Load("libTestDataModel.so");
  //gSystem->Load((std::string(getenv("PODIO_PATH")) + "/build/tests/libTestDataModelDict.so").c_str());
  auto df = MakeK4DataFrame({"/afs/cern.ch/user/l/lforthom/public/forFCC/example_frame.root"},
                            //std::vector<std::string>{"RecoParticles:recoParticles", "RecoJets:jets"})
                            std::vector<std::string>{})
                ->Range(0, 1000);
  //df.Snapshot("events", "test.root", {"recoParticles", "jets"});
  df.Snapshot("events", "test.root", {});
}

/*TEST_CASE("k4DataFrameMT", "[basics]") {
  ROOT::EnableImplicitMT();
  auto df = MakeK4DataFrame({"root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/"
                             "p8_ee_ZH_ecm240/events_101027117.root"},
                            std::vector<std::string>{"RecoParticles:recoParticles", "RecoJets:jets"})
                ->Snapshot("events", "test.root", {"recoParticles", "jets"});
}*/
