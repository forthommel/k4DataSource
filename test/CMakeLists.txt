find_catch_instance()

include(Catch)
set(filter_tests)

file(GLOB cc_tests *.cc)
foreach(_t ${cc_tests})
  get_filename_component(test_bin ${_t} NAME_WE)
  add_executable(${test_bin} ${_t})
  target_link_libraries(${test_bin} PUBLIC k4DataSource
                                    PRIVATE Catch2::Catch2WithMain)
  catch_discover_tests(${test_bin} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                                   TEST_SPEC ${filter_tests})
endforeach()

file(GLOB py_tests *.py)
foreach(_t ${py_tests})
  set(test_name "py_${_t}")
  add_test(NAME ${test_name} COMMAND python ${_t})
  set_tests_properties(${test_name}
                       PROPERTIES
                       WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endforeach()
