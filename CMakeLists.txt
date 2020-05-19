cmake_minimum_required(VERSION 2.8.11)

project(matrix-viewer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# sources
set(PROJECT_SOURCES
  src/main.cc
  src/main-window.cc
  src/preferences.cc
  src/progress-bar.cc
  src/file-chooser.cc
  src/matrix-model.cc
  src/matrix-view.cc
  src/image-view.cc
  src/matrix-converter.cc
  src/operation.cc
  src/operations-dialog.cc
  src/benchmark-task.cc
  src/benchmark-result.cc
  src/benchmark-dialog.cc
  src/properties-dialog.cc
  src/tab-widget.cc
  src/tab.cc
  src/position.cc
  src/histogram.cc
  src/histogram-widget.cc
  src/mfe.hh
  src/operations-widget.cc
  src/common-widgets.cc
  src/parser.cc
  src/mfe.cc
  src/double-spinbox.cc
  src/new-matrix-dialog.cc
)

# profiles
set(PROJECT_PROFILES
  profiles/cameras.xml
  profiles/projectors.xml
  profiles/dataBand.xml
  profiles/planes.xml
  profiles/tool.xml
  profiles/lines.xml
  profiles/dataRelocalization.xml
  profiles/measures.xml
)

# resources
set(PROJECT_RCS
  icons/matrix-viewer/matrix.qrc
  icons/tango/tango.qrc
  matrix.rc
)

# Additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Version information
include(GetGitRevisionDescription)
git_describe(VERSION --tags)

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message(STATUS "Git version ${VERSION_SHORT}")

# Configure a header file to pass some of the CMake settings to the source code
set(PROJECT_APPLICATION_NAME ${PROJECT_NAME})
set(PROJECT_VERSION          ${VERSION})
set(PROJECT_COMPILE_MACHINE  ${CMAKE_SYSTEM_PROCESSOR})
set(PROJECT_COMPILE_HOSTNAME ${BUILDHOSTNAME})
set(PROJECT_COMPILE_BY       $ENV{USER})
set(PROJECT_DATA_PATH        ${PREFIX}/share/${MATRIX_VIEWER_APPLICATION_NAME})

configure_file(
  "${PROJECT_SOURCE_DIR}/config.hh.in"
  "${PROJECT_BINARY_DIR}/config.hh"
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_BINARY_DIR ON)

# Instruct CMake to run moc/rcc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find external dependencies
find_package(OpenCV REQUIRED)
find_package(Qt5Widgets REQUIRED)

list(APPEND LIBRARIES Qt5::Widgets)
list(APPEND LIBRARIES ${OpenCV_LIBS})

# Compiler flags
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_BUILD_TYPE MATCHES "Release")
  set(FLAGS -Ofast -march=native -DNDEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
  set(FLAGS -g -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)
endif ()

message (STATUS "Using ${CMAKE_CXX_COMPILER} compiler")
message (STATUS "Compile in ${CMAKE_BUILD_TYPE} configuration")
message (STATUS "Compile options: ${FLAGS}")

# Build the main executable
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES} ${PROJECT_RCS})
target_compile_options(${PROJECT_NAME} PRIVATE ${FLAGS})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

# Installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${PROJECT_NAME}.desktop DESTINATION share/applications)
install(FILES icons/matrix-viewer/256x256/matrix-viewer.png DESTINATION share/icons)
install(FILES ${PROJECT_PROFILES} DESTINATION ${PROJECT_DATA_PATH}/profiles)

