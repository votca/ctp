file(GLOB VOTCA_SOURCES *.cc *.cpp)

configure_file(votca_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/votca_config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(gitversion-kmc COMMAND ${CMAKE_COMMAND} -DTOP_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
  -DGIT_EXECUTABLE="${GIT_EXECUTABLE}"
  -P ${CMAKE_MODULE_PATH}/gitversion.cmake)
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES gitversion.h)

file(GLOB_RECURSE VOTCA_SOURCES *.cc)
file(GLOB_RECURSE NOT_VOTCA_SOURCES version_nb.cc)
list(REMOVE_ITEM VOTCA_SOURCES ${NOT_VOTCA_SOURCES})

add_library(votca_kmc  ${VOTCA_SOURCES})
add_dependencies(votca_kmc gitversion-kmc)
set_target_properties(votca_kmc PROPERTIES SOVERSION ${SOVERSION})
target_link_libraries(votca_kmc VOTCA::votca_tools ${Boost_LIBRARIES} )
install(TARGETS votca_kmc LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

configure_file(libvotca_kmc.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libvotca_kmc.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libvotca_kmc.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
