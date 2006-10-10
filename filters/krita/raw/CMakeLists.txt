




########### next target ###############

set(krita_raw_import_PART_SRCS kis_raw_import.cpp )

kde4_automoc(${krita_raw_import_PART_SRCS})

kde4_add_ui_files(krita_raw_import_PART_SRCS wdgrawimport.ui )

kde4_add_plugin(krita_raw_import WITH_PREFIX ${krita_raw_import_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} krita_raw_import )

target_link_libraries(krita_raw_import kritaui)

install(TARGETS krita_raw_import  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_raw_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  krita_raw.desktop  DESTINATION ${XDG_APPS_DIR})

