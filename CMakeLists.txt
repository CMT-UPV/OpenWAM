project (OpenWAM)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)
option(BUILD_DOCUMENTATION "Use Doxygen to create the HTML and TEX based API documentation" OFF)
if(BUILD_DOCUMENTATION)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR 
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  #-- Configure the Template Doxyfile for our specific project
  configure_file(Doxyfile.in 
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  #-- Add a custom target to run Doxygen when ever the project is built
  add_custom_target (doc ALL 
    COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
    SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
  add_custom_target (refman.pdf
    ${CMAKE_COMMAND}
    -E chdir ${PROJECT_BINARY_DIR}/doc/latex/ ${CMAKE_MAKE_PROGRAM}
    DEPENDS ${PROJECT_BINARY_DIR}/doc/latex/refman.tex)
  # IF you do NOT want the documentation to be generated EVERY time you build the project
  # then leave out the 'ALL' keyword from the above command.
endif()
option(BUILD_PARALLEL "Build using OpenMP for parallel simulations." OFF)
if(BUILD_PARALLEL)
    find_package(OpenMP)
    if (NOT OPENMP_FOUND)
        message(FATAL_ERROR 
        "OpenMP is needed to build a parallel OpenWAM.  Install it correctly.")
    endif()
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
        add_definitions(-DWITH_OPENMP)
    endif()
endif()
set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff ${CMAKE_RC_FLAGS} -i <SOURCE> -o <OBJECT>")
add_subdirectory(Source)
