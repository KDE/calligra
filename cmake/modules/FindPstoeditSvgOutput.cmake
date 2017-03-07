# - Try to find Pstoedit with svg output plugin
# Once done this will define
#
#  PstoeditSvgOutput_FOUND - system has pstoedit with svg output plugin
#  PSTOEDIT_EXECUTABLE - path of the pstoedit executable
#  PSTOEDIT_VERSION - the version string, like "3.45"

find_program(PSTOEDIT_EXECUTABLE NAMES pstoedit)

include(MacroEnsureVersion)

if(PSTOEDIT_EXECUTABLE)
    execute_process(COMMAND ${PSTOEDIT_EXECUTABLE} -help
        ERROR_VARIABLE _PSTOEDIT_VERSION OUTPUT_VARIABLE _PSTOEDIT_PLUGINS
    )
    string (REGEX MATCH "[0-9]\\.[0-9]+" PSTOEDIT_VERSION "${_PSTOEDIT_VERSION}")
    string (REGEX MATCH "plot-svg" PSTOEDIT_SVG_PLUGIN "${_PSTOEDIT_PLUGINS}")
    string (COMPARE EQUAL "plot-svg" "${PSTOEDIT_SVG_PLUGIN}" PstoeditSvgOutput_FOUND )
endif()

if(PstoeditSvgOutput_FOUND)
  if(NOT Pstoedit_FIND_QUIETLY)
    message(STATUS "Found pstoedit version ${PSTOEDIT_VERSION}: ${PSTOEDIT_EXECUTABLE}")
    message(STATUS "Found pstoedit svg-plugin: ${PSTOEDIT_SVG_PLUGIN}")
  endif()
else()
  if(Pstoedit_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find pstoedit or the svg output plugin")
  endif()
endif()

