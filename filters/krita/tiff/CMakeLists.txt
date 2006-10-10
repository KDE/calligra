

include_directories(${KRITA_INCLUDES})

set(libkritatiffconverter_LIB_SRCS
    kis_tiff_converter.cc
    kis_tiff_stream.cc
    kis_tiff_writer_visitor.cpp
    kis_tiff_reader.cc
    kis_tiff_ycbcr_reader.cc
    )

########## next target ###############

set(kritatiffimport_PART_SRCS
    ${libkritatiffconverter_LIB_SRCS}
    kis_tiff_import.cc
    )

kde4_automoc(${kritatiffimport_PART_SRCS})

kde4_add_plugin(kritatiffimport WITH_PREFIX ${kritatiffimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritatiffimport )

target_link_libraries(kritatiffimport kritaui tiff)

install(TARGETS kritatiffimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kritatiffexport_PART_SRCS
    ${libkritatiffconverter_LIB_SRCS}
    kis_tiff_export.cc
    kis_dlg_options_tiff.cpp
    )

kde4_automoc(${kritatiffexport_PART_SRCS})

kde4_add_ui3_files(kritatiffexport_PART_SRCS kis_wdg_options_tiff.ui )

kde4_add_plugin(kritatiffexport WITH_PREFIX ${kritatiffexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritatiffexport )

target_link_libraries(kritatiffexport kritaui tiff)

install(TARGETS kritatiffexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############


########### install files ###############

install( FILES  krita_tiff_export.desktop krita_tiff_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  krita_tiff.desktop  DESTINATION ${XDG_APPS_DIR})

