project(flow)

include_directories( ${KOPAGEAPP_INCLUDES})

### flowprivate ###
set(flowprivate_LIB_SRCS
  FlowFactory.cpp
  FlowPart.cpp
  FlowDocument.cpp
  FlowView.cpp
  #FlowCanvas.cpp
  #FlowPage.cpp
  #FlowLayout.cpp
)

kde4_add_library(flowprivate SHARED ${flowprivate_LIB_SRCS})

target_link_libraries(flowprivate kopageapp)
target_link_libraries(flowprivate LINK_INTERFACE_LIBRARIES kopageapp)

set_target_properties(flowprivate PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS flowprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})

### flowpart ###
set(flowpart_PART_SRCS FlowFactoryInit.cpp )

kde4_add_plugin(flowpart ${flowpart_PART_SRCS})

target_link_libraries(flowpart flowprivate)

install(TARGETS flowpart DESTINATION ${PLUGIN_INSTALL_DIR})

### kdeinit flow ###
set(flow_KDEINIT_SRCS main.cpp )

kde4_add_kdeinit_executable(calligraflow ${flow_KDEINIT_SRCS})

target_link_libraries(kdeinit_calligraflow komain)

install(TARGETS kdeinit_calligraflow ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(calligraflow kdeinit_calligraflow)
install(TARGETS calligraflow ${INSTALL_TARGETS_DEFAULT_ARGS})

### desktop files ###
install( FILES flowpart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( PROGRAMS flow.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

### GUI files ###
install( FILES flow.rc DESTINATION ${DATA_INSTALL_DIR}/flow)
install( FILES flowrc DESTINATION ${CONFIG_INSTALL_DIR} )
