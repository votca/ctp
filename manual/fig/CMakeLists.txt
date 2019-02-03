foreach(PIC fig/logo fig/dcv2t fig/coupling fig/fragment_segment fig/monomer_parabolas fig/workflow)
  add_custom_command(OUTPUT ${PIC}.eps
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${PIC}.svg
        ${CMAKE_CURRENT_BINARY_DIR}/${PIC}.svg
    COMMAND ${INKSCAPE} -f ${PIC}.svg -E ${PIC}.eps
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${PIC}.svg)
  list(APPEND manual_FIGURES "${PIC}.eps")
endforeach()

#appendix/derivation_jortner.tex:%    \includegraphics[width=1.0\textwidth]{fig/jortner_rate/marcus_parabolas_new}
#appendix/derivation_jortner.tex:%   \includegraphics[width=\linewidth]{fig/jortner_rate/compare_rates}

