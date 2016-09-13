cmake_minimum_required(VERSION 3.3)
file(GLOB_RECURSE VOTCA_HEADERS *.h)
install(FILES ${VOTCA_HEADERS} DESTINATION include/votca/kmc)
