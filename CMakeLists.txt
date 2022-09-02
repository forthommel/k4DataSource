cmake_minimum_required(VERSION 3.16.9)

project(k4DataSource CXX)

include(CTest)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE BOOL "RPATH USE LINK PATH")
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install" CACHE PATH "default install path" FORCE)
endif()
include(GNUInstallDirs)
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")

find_package(ROOT COMPONENTS ROOTVecOps ROOTDataFrame REQUIRED)
find_package(EDM4HEP REQUIRED)

file(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${CMAKE_SOURCE_DIR}/k4DataSource/*.h)

file(GLOB reflex_cl ${CMAKE_SOURCE_DIR}/src/classes.h)
file(GLOB reflex_sel ${CMAKE_SOURCE_DIR}/src/classes_def.xml)
set(CMAKE_ROOTTEST_NOROOTMAP OFF)
reflex_generate_dictionary(libk4DataSource ${headers} ${reflex_cl} SELECTION ${reflex_sel})

include_directories(${CMAKE_SOURCE_DIR})

set(DATA_FORMATS)
add_subdirectory(DataFormats)
message(STATUS ${DATA_FORMATS})

add_library(k4DataSource SHARED ${sources} ${headers} libk4DataSource.cxx)
target_include_directories(k4DataSource PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(k4DataSource ROOT::ROOTDataFrame
                                   EDM4HEP::edm4hep EDM4HEP::edm4hepDict
                                   ${DATA_FORMATS})
set_target_properties(k4DataSource PROPERTIES PUBLIC_HEADER "${headers}")
install(TARGETS k4DataSource
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/k4DataSource")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libk4DataSource.rootmap"
              DESTINATION "${INSTALL_LIB_DIR}")
if(${ROOT_VERSION} GREATER 6)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libk4DataSource_rdict.pcm"
          DESTINATION "${INSTALL_LIB_DIR}")
endif()

if(BUILD_TESTING)
  include(CatchInclude)
  add_subdirectory(test)
endif()
