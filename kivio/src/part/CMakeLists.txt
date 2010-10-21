project(kivio)

include_directories( ${KOPAGEAPP_INCLUDES})

### kivioprivate ###
set(kivioprivate_LIB_SRCS
  KivioFactory.cpp
  KivioDocument.cpp
  KivioView.cpp
)

kde4_add_library(kivioprivate SHARED ${kivioprivate_LIB_SRCS})

target_link_libraries(kivioprivate kopageapp)
target_link_libraries(kivioprivate LINK_INTERFACE_LIBRARIES kopageapp)

set_target_properties(kivioprivate PROPERTIES
    VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION}
)
install(TARGETS kivioprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})

### kiviopart ###
set(kiviopart_PART_SRCS KivioFactoryInit.cpp )

kde4_add_plugin(kiviopart WITH_PREFIX ${kiviopart_PART_SRCS})

target_link_libraries(kiviopart kivioprivate)

install(TARGETS kiviopart DESTINATION ${PLUGIN_INSTALL_DIR})

### kdeinit kivio ###
set(kivio_KDEINIT_SRCS main.cpp )

kde4_add_kdeinit_executable( kivio ${kivio_KDEINIT_SRCS})

target_link_libraries(kdeinit_kivio komain)

install(TARGETS kdeinit_kivio ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(kivio kdeinit_kivio)
install(TARGETS kivio ${INSTALL_TARGETS_DEFAULT_ARGS})

### desktop files ###
install( FILES kiviopart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( PROGRAMS kivio.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

### GUI files ###
install( FILES kivio.rc DESTINATION ${DATA_INSTALL_DIR}/kivio)
