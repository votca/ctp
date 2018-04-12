file(GLOB VOTCA_SOURCES *.cc *.cpp)

add_library(votca_moo  ${VOTCA_SOURCES})

target_link_libraries(votca_moo ${VOTCA_TOOLS_LIBRARIES})
set_target_properties(votca_moo PROPERTIES SOVERSION ${SOVERSION})
install(TARGETS votca_moo LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

configure_file(libvotca_moo.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libvotca_moo.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libvotca_moo.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
