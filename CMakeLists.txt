cmake_minimum_required(VERSION 3.16)
project(matrix-viewer)
set(CMAKE_CXX_STANDARD 17)

# Forbid in-source build
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(
    FATAL_ERROR
      "In-source builds are not permitted. Make a separate folder for building:\n"
      "mkdir build; cd build; cmake ..\n" "Before that, remove the files already created:\n"
      "rm -rf CMakeCache.txt CMakeFiles")
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.17")
  cmake_policy(SET CMP0100 NEW) # Let AUTOMOC and AUTOUIC process .hh files
endif()

# Defines the CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_BINDIR and many other useful macros. See
# https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)

# sources
set(PROJECT_SOURCES
    src/main.cc
    src/main-window.cc
    src/preferences.cc
    src/progress-bar.cc
    src/empty-icon-provider.cc
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
    src/operations-widget.cc
    src/common-widgets.cc
    src/parser.cc
    src/mfe.cc
    src/edf.cc
    src/double-spinbox.cc
    src/new-matrix-dialog.cc
    src/toggle-button.cc)

# profiles
set(PROJECT_PROFILES
    profiles/cameras.xml
    profiles/projectors.xml
    profiles/dataBand.xml
    profiles/planes.xml
    profiles/tool.xml
    profiles/lines.xml
    profiles/dataRelocalization.xml
    profiles/measures.xml)

# resources
set(PROJECT_RCS icons/matrix-viewer/matrix.qrc icons/tango/tango.qrc matrix.rc)

# Additional CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})
find_package(ECM)

if(ECM_FOUND)
  set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Version information
include(GetGitRevisionDescription)
git_describe(VERSION --tags)

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message(STATUS "${PROJECT_NAME} version from latest git tag: ${VERSION_SHORT}")

# Configure a header file to pass some of the CMake settings to the source code
set(PROJECT_APPLICATION_NAME ${PROJECT_NAME})
set(PROJECT_VERSION ${VERSION_SHORT})
set(PROJECT_COMPILE_MACHINE ${CMAKE_SYSTEM_PROCESSOR})
set(PROJECT_COMPILE_HOSTNAME ${BUILDHOSTNAME})
set(PROJECT_COMPILE_BY $ENV{USER})
set(PROJECT_DATA_PATH ${PREFIX}/share/${MATRIX_VIEWER_APPLICATION_NAME})

configure_file("${PROJECT_SOURCE_DIR}/config.hh.in" "${PROJECT_BINARY_DIR}/config.hh")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_BINARY_DIR ON)

# Instruct CMake to run moc/rcc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find external dependencies
find_package(
  OpenCV
  COMPONENTS core imgcodecs
  REQUIRED)
list(APPEND LIBRARIES ${OpenCV_LIBS})

find_package(Qt6 COMPONENTS Core Widgets)

if(NOT Qt6_FOUND)
  find_package(
    Qt5 5.15
    COMPONENTS Core Widgets
    REQUIRED)
endif()

list(APPEND LIBRARIES Qt::Core Qt::Widgets)

find_package(LibExiv2)

if(LibExiv2_FOUND)
  list(APPEND LIBRARIES LibExiv2::LibExiv2)
  list(APPEND DEFINITIONS LIBEXIV2_ENABLED)
endif()

# Compiler flags
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Release")
  set(FLAGS -O3 -march=native -DNDEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(FLAGS
      -g
      -Werror
      -Wall
      -Wextra
      -Wshadow
      -Wnon-virtual-dtor
      -pedantic)
endif()

include(FeatureSummary)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES)

message(STATUS "Compile with ${CMAKE_CXX_COMPILER} in ${CMAKE_BUILD_TYPE} configuration")
message(STATUS "Compile options: ${FLAGS}")
message(STATUS "Compile definitions: ${DEFINITIONS}")
message(STATUS "Link with libraries: ${LIBRARIES}")

# Build the main executable
add_executable(${PROJECT_NAME} WIN32 ${PROJECT_RCS} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${FLAGS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${DEFINITIONS})

# Installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${PROJECT_NAME}.desktop DESTINATION share/applications)
install(FILES icons/matrix-viewer/256x256/matrix-viewer.png DESTINATION share/icons)
install(FILES ${PROJECT_PROFILES} DESTINATION ${PROJECT_DATA_PATH}/profiles)
