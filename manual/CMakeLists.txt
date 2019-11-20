CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
INCLUDE(${CMAKE_MODULE_PATH}/UseLATEX.cmake)
latex_get_output_path(CMAKE_BUILD_DIR)

MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} ) 
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} ) 
MESSAGE( STATUS "MANUAL_SOURCES: " ${manual_SOURCES} ) 
MESSAGE( STATUS "OUTPUT_PATH: " ${CMAKE_BUILD_DIR} ) 

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  find_package(Git)
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.git)

add_custom_target(ctp-gitid COMMAND ${CMAKE_COMMAND}
  -DTOP_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
  -DPROJECT_VERSION="${PROJECT_VERSION}"
  -DGIT_EXECUTABLE="${GIT_EXECUTABLE}"
  -P ${CMAKE_MODULE_PATH}/gitid.cmake)
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES gitid.tex)
add_custom_command(OUTPUT gitid.tex DEPENDS ctp-gitid)

foreach(prog votca_property inkscape dvips ps2pdf)
  string(TOUPPER "${prog}" PROG)
  if(TARGET ${prog})
    set(${PROG} $<TARGET_FILE:${prog}>)
  endif()
  find_program(${PROG} ${prog})
  find_package_handle_standard_args(${PROG} REQUIRED_VARS ${PROG})
  if(NOT ${PROG}_FOUND)
    message(FATAL_ERROR "Could not find ${prog}")
  endif()
endforeach()

include(theory/CMakeLists.txt)        # manual_SOURCES
include(fig/CMakeLists.txt)           # manual_FIGURES
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fig)
include(input/CMakeLists.txt)         # manual_XML

set(CTP_PROGS_FULL)
foreach(prog ${CTP_PROGS})
  string(TOUPPER "${prog}" PROG)
  if(TARGET ${prog})
    set(${PROG} $<TARGET_FILE:${prog}>)
  endif()
  find_program(${PROG} ${prog})
  find_package_handle_standard_args(${PROG} REQUIRED_VARS ${PROG})
  if(NOT ${PROG})
    message(FATAL_ERROR "Could not find ${prog}")
  endif()
  set(CTP_PROGS_FULL "${CTP_PROGS_FULL} ${${PROG}}")
endforeach()


set(VOTCASHARE ${CMAKE_CURRENT_SOURCE_DIR}/../share)
# generates tex refernce for all calculators 
add_custom_command(OUTPUT ${CMAKE_BUILD_DIR}/reference/calculators.tex
    COMMAND VOTCASHARE=${VOTCASHARE} VOTCA_PROPERTY=${VOTCA_PROPERTY}
      ${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_calculators.sh
    ARGS   ${CMAKE_BUILD_DIR}/reference/calculators.tex ${CTP_PROGS_FULL} 
    DEPENDS ${CTP_PROGS_FULL} ${VOTCA_PROPERTY} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_calculators.sh
)

# generates tex reference for all programs
add_custom_command(OUTPUT ${CMAKE_BUILD_DIR}/reference/programs.tex
    COMMAND VOTCASHARE=${VOTCASHARE}
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_programs.sh
    ARGS   ${CMAKE_BUILD_DIR}/reference/programs.tex ${CTP_PROGS_FULL}
    DEPENDS ${CTP_PROGS_FULL} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_programs.sh
)

# generates tex reference for xml files
#add_custom_command(OUTPUT xml_refs
#    COMMAND  ${CMAKE_CURRENT_SOURCE_DIR}/scripts/extract_xml.sh
#    ARGS   ${CMAKE_CURRENT_SOURCE_DIR}/input/dcv2t ${CMAKE_BUILD_DIR}/reference/xml
#)


ADD_LATEX_DOCUMENT( ctp-manual.tex
    INPUTS  newcommands.tex 
            titlepage.tex 
            reference/reference.tex 
            reference/calculators.xml
            ${manual_SOURCES} 
            ${manual_PROGRAMS}
            ${manual_XML}
    DEPENDS reference/calculators.tex reference/programs.tex  gitid.tex ${manual_FIGURES}
    TARGET_NAME ctp-manual
    BIBFILES manual.bib
    USE_INDEX
    FORCE_DVI
)

add_custom_command(OUTPUT ctp-manual.pdf
  COMMAND ${DVIPS} ctp-manual.dvi
  COMMAND ${PS2PDF} ctp-manual.ps ctp-manual.pdf
  DEPENDS ctp-manual_dvi
)
add_custom_target(ctp-manual_pdf ALL DEPENDS ctp-manual.pdf)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ctp-manual.pdf DESTINATION ${CMAKE_INSTALL_DOCDIR})
