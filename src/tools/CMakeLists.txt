foreach(PROG ctp_map ctp_run ctp_tools ctp_parallel ctp_dump moo_overlap)
  file(GLOB ${PROG}_SOURCES ${PROG}*.cc)

  if( ${PROG} STREQUAL "ctp_map" )
     add_executable(ctp_map ctp_map.cc Md2QmEngine.cc)
  else()
     add_executable(${PROG} ${${PROG}_SOURCES})
  endif()

  target_link_libraries(${PROG} votca_ctp votca_moo ${BOOST_LIBRARIES})

  install(TARGETS ${PROG} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  if (BUILD_MANPAGES)
     add_custom_command(OUTPUT ${PROG}.man
       COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROG} --man > ${PROG}.man
       COMMENT "Building ${PROG} manpage"
       DEPENDS ${PROG})
     add_custom_target(${PROG}_manpage DEPENDS ${PROG}.man)
     add_dependencies(manpages ${PROG}_manpage)
     install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROG}.man DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 RENAME ${PROG}.1)
  endif (BUILD_MANPAGES)

  if(ENABLE_TESTING)
    add_test(${PROG}Help ${PROG} --help)
    set_tests_properties(${PROG}Help PROPERTIES LABELS "tools;moo;csg;ctp;votca")
  endif(ENABLE_TESTING)
endforeach(PROG)

