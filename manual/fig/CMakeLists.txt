foreach(PIC fig/logo fig/dcv2t fig/coupling fig/fragment_segment fig/monomer_parabolas fig/workflow)
  add_custom_command(OUTPUT ${PIC}.eps
    COMMAND ${INKSCAPE} -f ${CMAKE_CURRENT_SOURCE_DIR}/${PIC}.svg
      -E ${CMAKE_CURRENT_BINARY_DIR}/${PIC}.eps
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fig
    DEPENDS ${PIC}.svg)
  list(APPEND manual_FIGURES "${PIC}.eps")
endforeach()

#appendix/derivation_jortner.tex:%    \includegraphics[width=1.0\textwidth]{fig/jortner_rate/marcus_parabolas_new}
#appendix/derivation_jortner.tex:%   \includegraphics[width=\linewidth]{fig/jortner_rate/compare_rates}

