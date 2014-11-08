cmake_minimum_required(VERSION 2.8)
set(CMAKE_COLOR_MAKEFILE ON)
#set(CMAKE_VERBOSE_MAKEFILE OFF)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH "${SOURCE_DIR}/cmake/")

if(NOT EXISTS ${CMAKE_MODULE_PATH}/matrix-viewer-config.cmake)
  message(FATAL_ERROR "Cannot find: ${CMAKE_MODULE_PATH}/matrix-viewer-config.cmake")
endif()

include(${CMAKE_MODULE_PATH}/matrix-viewer-config.cmake)

# {{{ Sources
set(MATRIX_VIEWER_SOURCES
  src/main.cc
  src/main-window.cc
  src/preferences.cc
  src/progress-bar.cc
  src/file-chooser.cc
  src/matrix-model.cc
  src/matrix-view.cc
  src/image-view.cc
  src/matrix-converter.cc
  src/compare-dialog.cc
  src/properties-dialog.cc
  src/tab-widget.cc
  src/tab.cc
  src/delegate.cc
  src/position.cc
  src/histogram.cc
  src/histogram-dialog.cc
  src/mfe.cc
  )

# header (moc)
set(MATRIX_VIEWER_QT_HEADER
  src/main-window.hh
  src/preferences.hh
  src/progress-bar.hh
  src/file-chooser.hh
  src/matrix-model.hh
  src/matrix-view.hh
  src/image-view.hh
  src/matrix-converter.hh
  src/compare-dialog.hh
  src/properties-dialog.hh
  src/tab-widget.hh
  src/tab.hh
  src/delegate.hh
  src/position.hh
  src/histogram.hh
  src/histogram-dialog.hh
  )

# matrix profiles
set(MATRIX_VIEWER_PROFILES
  profiles/cameras.xml
  profiles/projectors.xml
  profiles/planes.xml
  profiles/tool.xml
  profiles/lines.xml
  )


# uis
set(MATRIX_VIEWER_UIS
  )

# resources (rcs)
set(MATRIX_VIEWER_RCS
  icons/matrix-viewer/matrix.qrc
  icons/tango/tango.qrc
  )

# }}}

find_package(OpenCV REQUIRED)

find_package(Qt4 REQUIRED)
set(QT_USE_QTSVG true)

include(${QT_USE_FILE})

include_directories(
  ${SOURCE_DIR}
  ${SOURCE_DIR}/src/
  ${BUILD_DIR}
  )

# process Qt special files
qt4_wrap_cpp(MATRIX_VIEWER_MOCS ${MATRIX_VIEWER_QT_HEADER})
qt4_add_resources(MATRIX_VIEWER_RESSOURCES ${MATRIX_VIEWER_RCS})
qt4_wrap_ui(MATRIX_VIEWER_UI_HDRS ${MATRIX_VIEWER_UIS})

# generating executable
add_executable(${MATRIX_VIEWER_APPLICATION_NAME} WIN32
  ${MATRIX_VIEWER_SOURCES}
  ${MATRIX_VIEWER_MOCS}
  ${MATRIX_VIEWER_RESSOURCES}
  ${COMPILED_TRANSLATIONS}
  ${MATRIX_VIEWER_UI_HDRS}
  )

if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-enable-auto-import")
endif(WIN32)

list(APPEND LIBRARIES ${QT_LIBRARIES} ${OpenCV_LIBS})

target_link_libraries(${MATRIX_VIEWER_APPLICATION_NAME} ${LIBRARIES})

# {{{ Internationalization
set (TRANSLATIONS
  lang/matrix-viewer_fr.ts)

set (COMPILED_TRANSLATIONS
  lang/matrix-viewer_fr.qm)

add_custom_target(
  translations_target
  COMMAND "${QT_LRELEASE_EXECUTABLE}" ${TRANSLATIONS}
  DEPENDS ${TRANSLATIONS}
  COMMENT "Updating translations"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
#uncomment and run `make translations_target` to update translations ts files from sources
#warning, if uncommented, running make clean will remove ts files
#qt4_create_translation(COMPILED_TRANSLATIONS ${MATRIX_VIEWER_SOURCES} ${TRANSLATIONS} )
# }}}

# {{{ Manpages
if(GENERATE_MANPAGES)

  set(MATRIX_VIEWER_MAN_SRCS
    ${SOURCE_DIR}/manpages/matrix-viewer.1.txt
    )

  set(MATRIX_VIEWER_MAN_LANGS default fr)

  foreach(lang ${MATRIX_VIEWER_MAN_LANGS})

    foreach(txtfile ${MATRIX_VIEWER_MAN_SRCS})
      # figure the base name of the file (ie "matrix-viewer.1")
      GET_FILENAME_COMPONENT(tmpname ${txtfile} NAME)
      string(REGEX REPLACE ".txt\$" "" basename ${tmpname})

      # figure the relative path of the file
      GET_FILENAME_COMPONENT(tmppath ${txtfile} PATH)
      string(REPLACE ${SOURCE_DIR}/ "" relpath ${tmppath})

      # figure the manpage section to install to from filename
      string(REGEX REPLACE "^.*\\.([0-9])$" "\\1" section ${basename})

      # construct the language specific versions of the basename and path
      if (lang STREQUAL default)
        set(basename2 ${basename})
        set(relpath2 ${relpath}/man${section})
      else()
        set(basename2 ${basename}.${lang})
        set(relpath2 ${relpath}/${lang}/man${section})
      endif()

      # create the build directory (if it does not exist)
      file(MAKE_DIRECTORY ${BUILD_DIR}/${relpath2})

      # set the final filenames
      set(txtfile ${SOURCE_DIR}/${relpath}/${basename2}.txt)
      set(xmlfile ${BUILD_DIR}/${relpath2}/${basename}.xml)
      set(gzfile  ${BUILD_DIR}/${relpath2}/${basename}.gz)
      set(manfile ${BUILD_DIR}/${relpath2}/${basename})

      add_custom_command(
        COMMAND ${ASCIIDOC_EXECUTABLE} -d manpage -b docbook -o ${xmlfile} - < ${txtfile}
        WORKING_DIRECTORY ${BUILD_DIR}/${relpath2}
        OUTPUT  ${xmlfile}
        DEPENDS ${txtfile}
        VERBATIM)
      add_custom_command(
        COMMAND ${XMLTO_EXECUTABLE} --skip-validation man ${xmlfile}
        OUTPUT  ${manfile}
        WORKING_DIRECTORY ${BUILD_DIR}/${relpath2}
        DEPENDS ${xmlfile})

      if(COMPRESS_MANPAGES)
        add_custom_command(
          COMMAND ${GZIP_EXECUTABLE} < ${manfile} > ${gzfile}
          OUTPUT  ${gzfile}
          WORKING_DIRECTORY ${BUILD_DIR}/${relpath2}
          DEPENDS ${manfile}
          VERBATIM)

        set(MAN_FILES ${MAN_FILES} ${gzfile})
      else()
        set(MAN_FILES ${MAN_FILES} ${manfile})
      endif()
    endforeach()

  endforeach()

  add_custom_target(man ALL DEPENDS ${MAN_FILES})
endif()
# }}}

# {{{ Installation
install(TARGETS ${MATRIX_VIEWER_APPLICATION_NAME} RUNTIME DESTINATION bin)
if(GENERATE_MANPAGES)
  if(COMPRESS_MANPAGES)
    set(regex "\\.(xml|txt|[0-9])$")
  else()
    set(regex "\\.(xml|txt|gz)$")
  endif()
  install(DIRECTORY ${BUILD_DIR}/${relpath}/ DESTINATION ${MATRIX_VIEWER_MAN_PATH}
    REGEX ${regex} EXCLUDE)
endif()
install(FILES ${MATRIX_VIEWER_APPLICATION_NAME}.desktop DESTINATION share/applications)
install(FILES icons/matrix-viewer/256x256/matrix-viewer.png DESTINATION share/icons)
install(FILES ${COMPILED_TRANSLATIONS} DESTINATION ${MATRIX_VIEWER_DATA_PATH}/lang)
install(FILES ${MATRIX_VIEWER_PROFILES} DESTINATION ${MATRIX_VIEWER_DATA_PATH}/profiles)
# }}}
