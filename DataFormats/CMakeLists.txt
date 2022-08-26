macro(build_dataformat name path)
  file(GLOB sources ${path}/src/*.cc)
  file(GLOB headers ${path}/include/*.h)
  add_library(${name} SHARED ${sources} ${headers})
  set_target_properties(${name} PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(${name} ROOT::ROOTDataFrame
                                EDM4HEP::edm4hep EDM4HEP::edm4hepDict)
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR})
  list(APPEND DATA_FORMATS ${name})
  set(DATA_FORMATS ${DATA_FORMATS} PARENT_SCOPE)
endmacro()

build_dataformat(RecoParticle RecoParticle)
