foreach(PIC fig/logo fig/dcv2t fig/coupling fig/fragment_segment fig/monomer_parabolas fig/workflow)
  add_custom_command(OUTPUT ${PIC}.eps
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${PIC}.svg
        ${CMAKE_CURRENT_BINARY_DIR}/${PIC}.svg
    COMMAND ${INKSCAPE_EXECUTABLE} ${INKSCAPE_EXPORT_FLAG} ${PIC}.eps ${PIC}.svg
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${PIC}.svg)
  list(APPEND manual_FIGURES "${PIC}.eps")
endforeach()
