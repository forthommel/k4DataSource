macro(build_dataformat name path)
  file(GLOB sources ${path}/src/*.cc)
  file(GLOB headers ${path}/include/*.h)
  file(GLOB reflex_cl ${path}/src/classes.h)
  file(GLOB reflex_sel ${path}/src/classes_def.xml)
  set(CMAKE_ROOTTEST_NOROOTMAP OFF)
  reflex_generate_dictionary(libk4DataFormat${name} ${headers} ${reflex_cl} SELECTION ${reflex_sel})

  add_library(${name} SHARED ${sources} ${headers} libk4DataFormat${name}.cxx)
  set_target_properties(${name} PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(${name} ROOT::ROOTDataFrame
                                EDM4HEP::edm4hep EDM4HEP::edm4hepDict)
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR})
  list(APPEND DATA_FORMATS ${name})
  set(DATA_FORMATS ${DATA_FORMATS} PARENT_SCOPE)
endmacro()

build_dataformat(RecoParticle RecoParticle)
