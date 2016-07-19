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

add_library(flowprivate SHARED ${flowprivate_LIB_SRCS})

target_link_libraries(flowprivate kopageapp)
target_link_libraries(flowprivate LINK_INTERFACE_LIBRARIES kopageapp)

set_target_properties(flowprivate PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS flowprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})

### flowpart ###
set(flowpart_PART_SRCS FlowFactoryInit.cpp )

add_library(flowpart MODULE ${flowpart_PART_SRCS})

target_link_libraries(flowpart flowprivate)

install(TARGETS flowpart DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/parts)

### kdeinit flow ###
set(flow_KDEINIT_SRCS main.cpp )

file(GLOB ICONS_SRCS "../pics/hi*-app-calligraflow.png")
ecm_add_app_icon(flow_KDEINIT_SRCS ICONS ${ICONS_SRCS})

kf5_add_kdeinit_executable(calligraflow ${flow_KDEINIT_SRCS})

target_link_libraries(kdeinit_calligraflow komain)

install(TARGETS kdeinit_calligraflow ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(calligraflow kdeinit_calligraflow)
install(TARGETS calligraflow ${INSTALL_TARGETS_DEFAULT_ARGS})

### desktop files ###
install( FILES flowpart.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install( PROGRAMS flow.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES flow.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

### GUI files ###
install( FILES flow.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/flow)
install( FILES flowrc DESTINATION ${CONFIG_INSTALL_DIR} )
if(APPLE)
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/flow_KDEINIT_SRCS.icns DESTINATION ${BUNDLE_INSTALL_DIR}/calligraflow.app/Contents/Resources)
endif()
