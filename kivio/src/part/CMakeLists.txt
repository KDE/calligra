kde4_header()

project(kivio)

include_directories(${KOFFICEUI_INCLUDES})

### kivioprivate ###
set(kivioprivate_LIB_SRCS
  KivioFactory.cpp
  KivioDocument.cpp
  KivioView.cpp
  KivioCanvas.cpp
)

kde4_automoc(${kivioprivate_LIB_SRCS})

kde4_add_library(kivioprivate SHARED ${kivioprivate_LIB_SRCS})

target_link_libraries(kivioprivate  ${KDE4_KDECORE_LIBS} kofficeui)

set_target_properties(kivioprivate PROPERTIES VERSION 2.0.0 SOVERSION 2 )
install_targets(${LIB_INSTALL_DIR} kivioprivate )

### kiviopart ###
set(kiviopart_PART_SRCS KivioFactoryInit.cpp )

kde4_automoc(${kiviopart_PART_SRCS})

kde4_add_plugin(kiviopart WITH_PREFIX ${kiviopart_PART_SRCS})

kde4_install_libtool_file(${PLUGIN_INSTALL_DIR} kiviopart)

target_link_libraries(kiviopart  ${KDE4_KDECORE_LIBS} kivioprivate)

install_targets(${LIB_INSTALL_DIR}/kde4 kiviopart)

### kdeinit kivio ###
set(kivio_KDEINIT_SRCS main.cpp )

kde4_automoc(${kivio_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kivio ${kivio_KDEINIT_SRCS})

target_link_libraries(kdeinit_kivio  ${KDE4_KDECORE_LIBS} kofficecore )

install_targets(${LIB_INSTALL_DIR} kdeinit_kivio)

target_link_libraries(kivio kdeinit_kivio)
install_targets(/bin kivio)

### desktop files ###
install_files(${SERVICES_INSTALL_DIR} FILES  kiviopart.desktop)
install_files(${XDG_APPS_DIR} FILES  kivio.desktop)

kde4_footer()
