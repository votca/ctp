cmake_minimum_required(VERSION 3.1)
project(votca-ctp)

set(PROJECT_VERSION "1.6-dev")
set(PROJECT_CONTACT "bugs@votca.org")
string(REGEX REPLACE "^[1-9]+\\.([1-9]+).*$" "\\1" SOVERSION "${PROJECT_VERSION}")
if (NOT ${SOVERSION} MATCHES "[1-9]+")
  message(FATAL_ERROR "Could not determind SOVERSION from ${PROJECT_VERSION}")
endif (NOT ${SOVERSION} MATCHES "[1-9]+")

# Cmake modules/macros are in a subdirectory to keep this file cleaner
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

enable_language(CXX)

######################################################################
# compiler tests
# these need ot be done early (before further tests).
#####################################################################

include(CheckCXXCompilerFlag)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" AND CMAKE_VERSION VERSION_LESS "3.6.0")
  message(FATAL_ERROR "Using the intel compiler requires cmake 3.6 or higher")
endif()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()



########################################################################
# User input options                                                   #
########################################################################

option(BUILD_SHARED_LIBS "Build shared libs" ON)
include(GNUInstallDirs)
#option(WITH_DIPRO "Build mergeorbitals and DIPRO, which use fortran" ON)

option(ENABLE_TESTING "Build and enable testing stuff" OFF)
if(ENABLE_TESTING)
  enable_testing()
endif(ENABLE_TESTING)

#for votca_ctp_config.h
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
########################################################################
#Find external packages
########################################################################
if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  find_package(Git)
endif(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.git)


find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIRS})

find_package(Boost 1.48.0 REQUIRED COMPONENTS program_options serialization filesystem system timer)
include_directories(${Boost_INCLUDE_DIRS})
set (BOOST_CFLAGS_PKG "-I${Boost_INCLUDE_DIRS}")
set(BOOST_LIBS_PKG "-L${Boost_LIBRARY_DIRS}")
foreach(_blib ${Boost_LIBRARIES})
  string(REGEX REPLACE ".*/lib([^/]*)\\.[^.]*$" "-l\\1" _blib ${_blib})
  set(BOOST_LIBS_PKG "${BOOST_LIBS_PKG} ${_blib}")
endforeach(_blib)

option(BUILD_MANPAGES "Build manpages (might lead to problem on system without rpath" OFF)
#define this target here, so that individual man pages can append to it.
if(NOT TARGET manpages)
  add_custom_target(manpages ALL)
endif()


#message(STATUS "BOOST_LIBRARIES='${BOOST_LIBRARIES}'")

find_package(VOTCA_TOOLS REQUIRED)
include_directories(${VOTCA_TOOLS_INCLUDE_DIRS})
find_package(VOTCA_CSG REQUIRED)
include_directories(${VOTCA_CSG_INCLUDE_DIRS})

########################################################################
# Basic system tests (standard libraries, headers, functions, types)   #
########################################################################
include(CheckIncludeFile)
foreach(HEADER assert.h)
  check_include_file(${HEADER} FOUND_${HEADER})
  if(NOT FOUND_${HEADER})
    message(FATAL_ERROR "Could not find needed header - ${HEADER}")
  endif(NOT FOUND_${HEADER})
endforeach(HEADER)

include(CheckIncludeFileCXX)
foreach(HEADER algorithm)
  check_include_file_cxx(${HEADER} FOUND_${HEADER})
  if(NOT FOUND_${HEADER})
    message(FATAL_ERROR "Could not find needed header - ${HEADER}")
  endif(NOT FOUND_${HEADER})
endforeach(HEADER)

set(MATH_LIBRARIES "m" CACHE STRING "math library")
mark_as_advanced( MATH_LIBRARIES )
include(CheckLibraryExists)
foreach(FUNC sqrt)
  check_library_exists(${MATH_LIBRARIES} ${FUNC} "" FOUND_${FUNC}_${MATH_LIBRARIES})
  if(NOT FOUND_${FUNC}_${MATH_LIBRARIES})
    message(FATAL_ERROR "Could not find needed math function - ${FUNC}")
  endif(NOT FOUND_${FUNC}_${MATH_LIBRARIES})
endforeach(FUNC)

######################################
# Include the following subdirectory # 
######################################
add_subdirectory(include/votca/ctp)
add_subdirectory(include/votca/moo)
add_subdirectory(include/votca/kmc)
add_subdirectory(scripts)
add_subdirectory(src/libctp)
add_subdirectory(src/libmoo)
add_subdirectory(src/libkmc)
add_subdirectory(src/tools)
add_subdirectory(share)  
add_subdirectory(share/man)

option(BUILD_CTP_MANUAL "Build manual" OFF)
if(BUILD_CTP_MANUAL)
  add_subdirectory(manual)
endif()

