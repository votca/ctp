
#configure_file(votca_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/votca_config.h)
#include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(gitversion-ctp COMMAND ${CMAKE_COMMAND} -DTOP_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
  -DGIT_EXECUTABLE="${GIT_EXECUTABLE}"
  -P ${CMAKE_MODULE_PATH}/gitversion.cmake)
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES gitversion.h)


file(GLOB_RECURSE VOTCA_SOURCES *.cc)
file(GLOB_RECURSE NOT_VOTCA_SOURCES version_nb.cc segmenttype.cc)
list(REMOVE_ITEM VOTCA_SOURCES ${NOT_VOTCA_SOURCES})

add_library(votca_ctp  ${VOTCA_SOURCES})
add_dependencies(votca_ctp gitversion-ctp)
set_target_properties(votca_ctp PROPERTIES SOVERSION ${SOVERSION})
target_link_libraries(votca_ctp ${VOTCA_CSG_LIBRARIES} ${GSL_LIBRARIES} votca_moo ${VOTCA_TOOLS_LIBRARIES} ${Boost_LIBRARIES} )
install(TARGETS votca_ctp LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

configure_file(libvotca_ctp.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libvotca_ctp.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libvotca_ctp.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
