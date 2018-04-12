file(GLOB VOTCA_SOURCES *.cc *.cpp)

configure_file(votca_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/votca_config.h)

add_library(votca_moo  ${VOTCA_SOURCES})
set_target_properties(votca_moo PROPERTIES SOVERSION ${SOVERSION})
target_link_libraries(votca_moo ${VOTCA_TOOLS_LIBRARIES})

add_custom_target(gitversion-moo COMMAND ${CMAKE_COMMAND} -DTOP_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
  -DGIT_EXECUTABLE="${GIT_EXECUTABLE}"
  -P ${CMAKE_MODULE_PATH}/gitversion.cmake)
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES gitversion.h)

add_dependencies(votca_moo gitversion-moo)
install(TARGETS votca_moo LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

configure_file(libvotca_moo.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libvotca_moo.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libvotca_moo.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
