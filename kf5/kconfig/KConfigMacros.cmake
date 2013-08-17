#  KCONFIG_ADD_KCFG_FILES (SRCS_VAR [GENERATE_MOC] [USE_RELATIVE_PATH] file1.kcfgc ... fileN.kcfgc)
#    Use this to add KDE config compiler files to your application/library.
#    Use optional GENERATE_MOC to generate moc if you use signals in your kcfg files.
#    Use optional USE_RELATIVE_PATH to generate the classes in the build following the given
#    relative path to the file.
#
# Copyright (c) 2006-2009 Alexander Neundorf, <neundorf@kde.org>
# Copyright (c) 2006, 2007, Laurent Montel, <montel@kde.org>
# Copyright (c) 2007 Matthias Kretz <kretz@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro (KCONFIG_ADD_KCFG_FILES _sources )
   foreach (_current_ARG ${ARGN})
      if( ${_current_ARG} STREQUAL "GENERATE_MOC" )
         set(_kcfg_generatemoc TRUE)
      endif()

      if( ${_current_ARG} STREQUAL "USE_RELATIVE_PATH" )
         set(_kcfg_relativepath TRUE)
      endif()
   endforeach ()

   foreach (_current_FILE ${ARGN})

     if(NOT ${_current_FILE} STREQUAL "GENERATE_MOC" AND NOT ${_current_FILE} STREQUAL "USE_RELATIVE_PATH")
       get_filename_component(_tmp_FILE ${_current_FILE} ABSOLUTE)
       get_filename_component(_abs_PATH ${_tmp_FILE} PATH)

       if (_kcfg_relativepath) # Process relative path only if the option was set
           # Get relative path
           get_filename_component(_rel_PATH ${_current_FILE} PATH)

           if (IS_ABSOLUTE ${_rel_PATH})
               # We got an absolute path
               set(_rel_PATH "")
           endif ()
       endif ()

       get_filename_component(_basename ${_tmp_FILE} NAME_WE)
       # If we had a relative path and we're asked to use it, then change the basename accordingly
       if(NOT ${_rel_PATH} STREQUAL "")
           set(_basename ${_rel_PATH}/${_basename})
       endif()

       file(READ ${_tmp_FILE} _contents)
       string(REGEX REPLACE "^(.*\n)?File=([^\n]+kcfg).*\n.*$" "\\2"  _kcfg_FILENAME "${_contents}")
       set(_src_FILE    ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.cpp)
       set(_header_FILE ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.h)
       set(_moc_FILE    ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.moc)
       set(_kcfg_FILE   ${_abs_PATH}/${_kcfg_FILENAME})
       # Maybe the .kcfg is a generated file?
       if(NOT EXISTS "${_kcfg_FILE}")
           set(_kcfg_FILE   ${CMAKE_CURRENT_BINARY_DIR}/${_kcfg_FILENAME})
       endif()

       if(NOT EXISTS "${_kcfg_FILE}")
           message(ERROR "${_kcfg_FILENAME} not found; tried in ${_abs_PATH} and ${CMAKE_CURRENT_BINARY_DIR}")
       endif()

       # make sure the directory exist in the build directory
       if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${_rel_PATH}")
           file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${_rel_PATH})
       endif()

       # the command for creating the source file from the kcfg file
       add_custom_command(OUTPUT ${_header_FILE} ${_src_FILE}
                          COMMAND ${KConfig_KCFGC_EXECUTABLE}
                          ARGS ${_kcfg_FILE} ${_tmp_FILE} -d ${CMAKE_CURRENT_BINARY_DIR}/${_rel_PATH}
                          MAIN_DEPENDENCY ${_tmp_FILE}
                          DEPENDS ${_kcfg_FILE} ${_KDE4_KCONFIG_COMPILER_DEP} )

       if(_kcfg_generatemoc)
          qt5_generate_moc(${_header_FILE} ${_moc_FILE} )
          set_source_files_properties(${_src_FILE} PROPERTIES SKIP_AUTOMOC TRUE)  # don't run automoc on this file
          list(APPEND ${_sources} ${_moc_FILE})
       endif()

       list(APPEND ${_sources} ${_src_FILE} ${_header_FILE})
     endif(NOT ${_current_FILE} STREQUAL "GENERATE_MOC" AND NOT ${_current_FILE} STREQUAL "USE_RELATIVE_PATH")
   endforeach (_current_FILE)

endmacro (KCONFIG_ADD_KCFG_FILES)
