#include <TSystem.h>

#include <catch2/catch_test_macros.hpp>

#include "k4DataSource/k4DataSource.h"

TEST_CASE("k4DataSource", "[basics]") {
  gSystem->Load("libedm4hep");
  k4DataSource ds("events",
                  "root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/"
                  "p8_ee_ZH_ecm240/events_101027117.root");
  for (const auto& col : ds.GetColumnNames())
    std::cout << "??? " << col << std::endl;
}
