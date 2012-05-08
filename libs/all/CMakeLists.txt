
set(all_TARGETS koplugin kundo2 odf/koodf pigment/pigmentcms flake kotext widgets/kowidgets textlayout main/komain kopageapp )
set(all_SOURCES)
set(all_DEPS)

foreach(_target IN LISTS all_TARGETS)
  if(_target MATCHES "/")
    string(REGEX MATCH "^(.+)/([^/]+)$" _rematch ${_target})
    set(_path ${CMAKE_MATCH_1})
    set(_targetname ${CMAKE_MATCH_2})
  else()
    set(_path ${_target})
    set(_targetname ${_target})
  endif()

  get_directory_property(_includes DIRECTORY ../${_path} INCLUDE_DIRECTORIES)
  include_directories(${_includes})

  get_target_property(_sources ${_targetname} SOURCES)

  foreach(sourceFile IN LISTS _sources)
    if(IS_ABSOLUTE ${sourceFile})
      if(NOT sourceFile MATCHES "automoc" AND NOT sourceFile MATCHES "/ui_.*h$")
        list(APPEND all_SOURCES "${sourceFile}")
      endif()
    else()
      list(APPEND all_SOURCES "../${_path}/${sourceFile}")
    endif()
  endforeach(sourceFile)

  list(APPEND all_DEPS "${_targetname}")
endforeach(_target)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../widgets)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../main)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../kfile)

kde4_add_library(calligraall SHARED ${all_SOURCES})

add_dependencies(calligraall ${all_DEPS})

target_link_libraries(calligraall ${KDE4_KIO_LIBS} ${QT_QTXML_LIBRARY})

target_link_libraries(calligraall LINK_INTERFACE_LIBRARIES ${KDE4_KIO_LIBS} )

install(TARGETS calligraall ${INSTALL_TARGETS_DEFAULT_ARGS})