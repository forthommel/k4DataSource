macro(find_catch_instance)
  if(USE_EXTERNAL_CATCH2)
    find_package(Catch2 REQUIRED)
  else()
    message(STATUS "Fetching local copy of Catch2 library for unit-tests...")
    # Build Catch2 with the default flags, to avoid generating warnings when we
    # build it
    set(CXX_FLAGS_CMAKE_USED ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS ${CXX_FLAGS_CMAKE_DEFAULTS})
    include(FetchContent)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG        037ddbc75cc5e58b93cf5a010a94b32333ad824d
      )
    FetchContent_MakeAvailable(Catch2)
    set(CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras ${CMAKE_MODULE_PATH})
    # Disable clang-tidy on external contents
    set_target_properties(Catch2 PROPERTIES CXX_CLANG_TIDY "")

    # Hack around the fact, that the include directories are not declared as
    # SYSTEM for the targets defined this way. Otherwise warnings can still occur
    # in Catch2 code when templates are evaluated (which happens quite a bit)
    get_target_property(CATCH2_IF_INC_DIRS Catch2 INTERFACE_INCLUDE_DIRECTORIES)
    set_target_properties(Catch2 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CATCH2_IF_INC_DIRS}")

    # Reset the flags
    set(CMAKE_CXX_FLAGS ${CXX_FLAGS_CMAKE_USED})
  endif()
endmacro()
