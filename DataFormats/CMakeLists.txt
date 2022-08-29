macro(build_dataformat name path)
  file(GLOB sources ${path}/src/*.cc)
  file(GLOB headers ${path}/include/*.h)
  file(GLOB reflex_cl ${path}/src/classes.h)
  file(GLOB reflex_sel ${path}/src/classes_def.xml)
  set(lib_name "k4DataFormat${name}")
  set(CMAKE_ROOTTEST_NOROOTMAP OFF)
  reflex_generate_dictionary(lib${lib_name} ${headers} ${reflex_cl} SELECTION ${reflex_sel})

  add_library(${lib_name} SHARED ${sources} ${headers} lib${lib_name}.cxx)
  set_target_properties(${lib_name} PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(${lib_name} ROOT::ROOTDataFrame
                                    EDM4HEP::edm4hep EDM4HEP::edm4hepDict)
  target_include_directories(${lib_name} PRIVATE ${CMAKE_SOURCE_DIR})
  list(APPEND DATA_FORMATS ${lib_name})
  set(DATA_FORMATS ${DATA_FORMATS} PARENT_SCOPE)
  install(TARGETS ${lib_name}
          RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
          LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
          PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/${name}")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${lib_name}.rootmap"
          DESTINATION "${INSTALL_LIB_DIR}")
  if(${ROOT_VERSION} GREATER 6)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${lib_name}_rdict.pcm"
            DESTINATION "${INSTALL_LIB_DIR}")
  endif()
endmacro()

build_dataformat(RecoParticle RecoParticle)
