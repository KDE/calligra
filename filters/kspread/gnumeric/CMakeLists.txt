

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES}  )


########### next target ###############

set(gnumericexport_PART_SRCS gnumericexport.cc )

kde4_automoc(${gnumericexport_PART_SRCS})

kde4_add_plugin(gnumericexport WITH_PREFIX ${gnumericexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} gnumericexport )

target_link_libraries(gnumericexport  ${KDE4_KDECORE_LIBS} kspreadcommon )

install(TARGETS gnumericexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(gnumericimport_PART_SRCS gnumericimport.cc )

kde4_automoc(${gnumericimport_PART_SRCS})

kde4_add_plugin(gnumericimport WITH_PREFIX ${gnumericimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} gnumericimport )

target_link_libraries(gnumericimport  ${KDE4_KDECORE_LIBS} kspreadcommon )

install(TARGETS gnumericimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_gnumeric_export.desktop kspread_gnumeric_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


