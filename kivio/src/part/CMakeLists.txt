project(kivio)

include_directories(${KOFFICEUI_INCLUDES})

### kivioprivate ###
set(kivioprivate_LIB_SRCS
  KivioFactory.cpp
  KivioDocument.cpp
  KivioView.cpp
  KivioCanvas.cpp
  KivioAbstractPage.cpp
  KivioMasterPage.cpp
  KivioPage.cpp
)

kde4_automoc(${kivioprivate_LIB_SRCS})

kde4_add_library(kivioprivate SHARED ${kivioprivate_LIB_SRCS})

target_link_libraries(kivioprivate  ${KDE4_KDECORE_LIBS} kofficeui flake)

set_target_properties(kivioprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kivioprivate  DESTINATION ${LIB_INSTALL_DIR})

### kiviopart ###
set(kiviopart_PART_SRCS KivioFactoryInit.cpp )

kde4_automoc(${kiviopart_PART_SRCS})

kde4_add_plugin(kiviopart WITH_PREFIX ${kiviopart_PART_SRCS})

kde4_install_libtool_file(${PLUGIN_INSTALL_DIR} kiviopart)

target_link_libraries(kiviopart  ${KDE4_KDECORE_LIBS} kivioprivate)

install(TARGETS kiviopart DESTINATION ${PLUGIN_INSTALL_DIR})

### kdeinit kivio ###
set(kivio_KDEINIT_SRCS main.cpp )

kde4_automoc(${kivio_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kivio ${kivio_KDEINIT_SRCS})

target_link_libraries(kdeinit_kivio  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kivio DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries(kivio kdeinit_kivio)
install(TARGETS kivio DESTINATION ${BIN_INSTALL_DIR})

### desktop files ###
install( FILES kiviopart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kivio.desktop DESTINATION ${XDG_APPS_DIR})

### GUI files ###
install( FILES kivio.rc DESTINATION ${DATA_INSTALL_DIR}/kivio)
