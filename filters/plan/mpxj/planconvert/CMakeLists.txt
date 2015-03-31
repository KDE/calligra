
set (PLANCONVERT_INSTALL_DIR ${CALLIGRA_PLUGIN_INSTALL_DIR}/planconvert)

configure_file (
  "${CMAKE_SOURCE_DIR}/filters/plan/mpxj/planconvert/mpxjconfig.h.in"
  "${CMAKE_BINARY_DIR}/filters/plan/mpxj/planconvert/mpxjconfig.h"
)


install(FILES jar/PlanConvert.jar  DESTINATION ${PLANCONVERT_INSTALL_DIR})
