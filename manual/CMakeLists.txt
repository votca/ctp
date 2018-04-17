CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
INCLUDE(${CMAKE_MODULE_PATH}/UseLATEX.cmake)
latex_get_output_path(output_dir)

MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} ) 
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} ) 
MESSAGE( STATUS "MANUAL_SOURCES: " ${manual_SOURCES} ) 
MESSAGE( STATUS "OUTPUT_PATH: " ${output_dir} ) 

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  find_package(Git)
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.git)

add_custom_target(ctp-gitid COMMAND ${CMAKE_COMMAND}
  -DTOP_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
  -DPROJECT_VERSION="${PROJECT_VERSION}"
  -DGIT_EXECUTABLE="${GIT_EXECUTABLE}"
  -P ${CMAKE_MODULE_PATH}/gitid.cmake)
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES gitid.tex)
add_custom_command(OUTPUT gitid_tex DEPENDS ctp-gitid)

include(theory/CMakeLists.txt)        # manual_SOURCES
include(reference/xml/CMakeLists.txt) # manual_PROGRAMS
include(fig/CMakeLists.txt)           # manual_FIGURES
include(input/CMakeLists.txt)         # manual_XML


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
    DEPENDS calculator_refs program_refs xml_refs gitid_tex
    IMAGE_DIRS fig
    BIBFILES manual.bib
    USE_INDEX
    FORCE_PDF
)

add_custom_target(manual_pdf ALL DEPENDS manual.pdf)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manual.pdf DESTINATION ${CMAKE_INSTALL_DOCDIR})
