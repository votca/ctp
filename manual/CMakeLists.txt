CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/UseLATEX.cmake)
latex_get_output_path(output_dir)

MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} ) 
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} ) 
MESSAGE( STATUS "MANUAL_SOURCES: " ${manual_SOURCES} ) 
MESSAGE( STATUS "OUTPUT_PATH: " ${output_dir} ) 

include(theory/CMakeLists.txt) # description files
#include(reference/programs/CMakeLists.txt) # 
include(reference/xml/CMakeLists.txt)
include(fig/CMakeLists.txt)
include(input/CMakeLists.txt)


# generates tex refernce for all calculators 
add_custom_command(OUTPUT calculator_refs
    COMMAND  ${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_calculators.sh 
    ARGS   ${output_dir}/reference/calculators.tex
)

# generates tex reference for all programs
add_custom_command(OUTPUT program_refs
    COMMAND  ${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_programs.sh
    ARGS   ${output_dir}/reference/programs.tex
)

# generates tex reference for xml files
add_custom_command(OUTPUT xml_refs
    COMMAND  ${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_xml.sh
    ARGS   ${CMAKE_CURRENT_SOURCE_DIR}/input/dcv2t ${output_dir}/reference/xml
)


ADD_LATEX_DOCUMENT( manual.tex
    INPUTS  newcommands.tex 
            titlepage.tex 
            reference/reference.tex 
            reference/calculators.xml
            ${manual_SOURCES} 
            ${manual_PROGRAMS}
            ${manual_XML}
            ${manual_FIGURES}
    DEPENDS calculator_refs program_refs xml_refs
    BIBFILES manual.bib
    USE_INDEX
    FORCE_PDF
)

add_custom_target(manual_pdf ALL DEPENDS manual.pdf)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manual.pdf DESTINATION ${CMAKE_INSTALL_DOCDIR})
