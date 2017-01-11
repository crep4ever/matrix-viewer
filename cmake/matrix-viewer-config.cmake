set(PROJECT_NAME matrix-viewer)

# If ${SOURCE_DIR} is a git repo, VERSION is set to `git describe --tags` later.
set(VERSION devel)

set(CODENAME "")

#-------------------------------------------------------------------------------
# CMake build options
#-------------------------------------------------------------------------------

option(GENERATE_MANPAGES "generate manpages" OFF)
option(COMPRESS_MANPAGES "compress manpages" OFF)

#-------------------------------------------------------------------------------
# Compiler options
#-------------------------------------------------------------------------------

# Default build type
if (NOT DEFINED CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif ()

# Compiler flags
if (CMAKE_BUILD_TYPE MATCHES "Release")
  set(FLAGS
  -Ofast -march=native)
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(FLAGS
    -g -O -Wall -pedantic -Werror
    -pedantic-errors -Wextra -Wcast-align
    -Wchar-subscripts -Wcomment
    -Wdisabled-optimization
    -Wformat -Wformat=2
    -Wformat-nonliteral -Wformat-security
    -Wformat-y2k
    -Wimport  -Winit-self
    -Winvalid-pch -Wlong-long -Wmissing-braces
    -Wmissing-field-initializers -Wmissing-format-attribute
    -Wmissing-include-dirs -Wmissing-noreturn
    -Wpacked  -Wparentheses  -Wpointer-arith
    -Wredundant-decls -Wreturn-type
    -Wsequence-point  -Wshadow -Wsign-compare  -Wstack-protector
    -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch
    -Wswitch-enum -Wtrigraphs  -Wuninitialized
    -Wunreachable-code -Wunused
    -Wunused-function -Wunused-label -Wunused-parameter
    -Wunused-value -Wunused-variable -Wvariadic-macros
    -Wvolatile-register-var  -Wwrite-strings
    )
endif ()

if (CMAKE_CXX_COMPILER MATCHES "clang")
endif()

add_definitions(${FLAGS})
message (STATUS "Using ${CMAKE_CXX_COMPILER} compiler")
message (STATUS "Compile in ${CMAKE_BUILD_TYPE} configuration")
message (STATUS "Compile options: ${FLAGS}")

#-------------------------------------------------------------------------------
# Find external utilities
#-------------------------------------------------------------------------------

macro(a_find_program var prg req)
  set(required ${req})
  find_program(${var} ${prg})
  if(NOT ${var})
    message(STATUS "${prg} not found.")
    if(required)
      message(FATAL_ERROR "${prg} is required to build ${PROJECT_NAME}")
    endif()
  else()
    message(STATUS "${prg} -> ${${var}}")
  endif()
endmacro()

a_find_program(GIT_EXECUTABLE git FALSE)
a_find_program(HOSTNAME_EXECUTABLE hostname FALSE)
# programs needed for man pages
a_find_program(ASCIIDOC_EXECUTABLE asciidoc FALSE)
a_find_program(XMLTO_EXECUTABLE xmlto FALSE)
a_find_program(GZIP_EXECUTABLE gzip FALSE)

#-------------------------------------------------------------------------------
# Documentation
#-------------------------------------------------------------------------------

# Check if documentation can be build
if (GENERATE_MANPAGES)
  if (NOT ASCIIDOC_EXECUTABLE OR NOT XMLTO_EXECUTABLE
      OR (COMPRESS_MANPAGES AND NOT GZIP_EXECUTABLE)
      OR NOT EXISTS ${SOURCE_DIR}/manpages/)
    if (NOT ASCIIDOC_EXECUTABLE)
      set(missing "asciidoc")
    endif ()
    if (NOT XMLTO_EXECUTABLE)
      set(missing ${missing} " xmlto")
    endif ()
    if (COMPRESS_MANPAGES AND NOT GZIP_EXECUTABLE)
      set(missing ${missing} " gzip")
    endif ()

    message(STATUS "Skip manpages generation. Missing: " ${missing})
    set(GENERATE_MANPAGES OFF)
  endif()
endif()

# Version stamp
if (EXISTS ${SOURCE_DIR}/.git/HEAD AND GIT_EXECUTABLE)
  # get current version
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags
    WORKING_DIRECTORY ${SOURCE_DIR}
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(BUILD_FROM_GIT TRUE)
endif()
#

# Hostname
execute_process(
  COMMAND ${HOSTNAME_EXECUTABLE}
  WORKING_DIRECTORY ${SOURCE_DIR}
  OUTPUT_VARIABLE BUILDHOSTNAME
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Install path and configuration variables
if (DEFINED PREFIX)
  set(PREFIX ${PREFIX} CACHE PATH "install prefix")
  set(CMAKE_INSTALL_PREFIX ${PREFIX})
else ()
  set(PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "install prefix")
endif ()

# set man path
if (DEFINED MATRIX_VIEWER_MAN_PATH)
  set(MATRIX_VIEWER_MAN_PATH ${MATRIX_VIEWER_MAN_PATH} CACHE PATH "matrix-viewer manpage directory")
else ()
  set(MATRIX_VIEWER_MAN_PATH ${PREFIX}/share/man CACHE PATH "matrix-viewer manpage directory")
endif ()

# Hide to avoid confusion
mark_as_advanced(CMAKE_INSTALL_PREFIX)

set(MATRIX_VIEWER_APPLICATION_NAME ${PROJECT_NAME})
set(MATRIX_VIEWER_VERSION          ${VERSION})
set(MATRIX_VIEWER_COMPILE_MACHINE  ${CMAKE_SYSTEM_PROCESSOR})
set(MATRIX_VIEWER_COMPILE_HOSTNAME ${BUILDHOSTNAME})
set(MATRIX_VIEWER_COMPILE_BY       $ENV{USER})
set(MATRIX_VIEWER_RELEASE          ${CODENAME})
set(MATRIX_VIEWER_DATA_PATH        ${PREFIX}/share/${MATRIX_VIEWER_APPLICATION_NAME})
# }}}

# Configure files
set(MATRIX_VIEWER_CONFIGURE_FILES
  config.hh.in
  )

macro(a_configure_file file)
  string(REGEX REPLACE ".in\$" "" outfile ${file})
  message(STATUS "Configuring ${outfile}")
  configure_file(${SOURCE_DIR}/${file}
    ${BUILD_DIR}/${outfile}
    @ONLY)
endmacro()

foreach(file ${MATRIX_VIEWER_CONFIGURE_FILES})
  a_configure_file(${file})
endforeach()
