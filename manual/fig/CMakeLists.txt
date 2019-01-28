foreach(PIC fig/logo fig/dcv2t fig/coupling fig/fragment_segment fig/monomer_parabolas fig/workflow)
  add_custom_command(OUTPUT ${PIC}.eps
    COMMAND ${INKSCAPE} --file=${CMAKE_CURRENT_SOURCE_DIR}/${PIC}.svg
      --export-eps=${CMAKE_CURRENT_BINARY_DIR}/${PIC}.eps
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fig
    DEPENDS ${PIC}.svg)
  list(APPEND manual_FIGURES "${PIC}.eps")
endforeach()
