find_catch_instance()

include(Catch)
set(filter_tests)

file(GLOB tests *.cc)
foreach(_t ${tests})
  get_filename_component(test_bin ${_t} NAME_WE)
  add_executable(${test_bin} ${_t})
  target_link_libraries(${test_bin} PUBLIC k4DataSource
                                    PRIVATE Catch2::Catch2WithMain)
  catch_discover_tests(${test_bin} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                                   TEST_SPEC ${filter_tests})
endforeach()
