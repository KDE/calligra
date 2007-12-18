

include_directories( ${KRITA_INCLUDES} ${CMAKE_SOURCE_DIR}/krita/image/metadata ${EXIV2_INCLUDE_DIR})

set(libkritaconverter_LIB_SRCS
    kis_jpeg_converter.cc
    iccjpeg.c
    )

set(kritajpegimport_PART_SRCS
    kis_jpeg_import.cc
    ${libkritaconverter_LIB_SRCS}
    )

kde4_add_plugin(kritajpegimport WITH_PREFIX ${kritajpegimport_PART_SRCS})



target_link_libraries(kritajpegimport kritaui jpeg kisexiv2) # link against kisexiv2 to ensure the availability of reading/writing EXIF and IPTC tags

install(TARGETS kritajpegimport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(kritajpegexport_PART_SRCS
    kis_jpeg_export.cc
    ${libkritaconverter_LIB_SRCS}
    )

kde4_add_ui3_files(kritajpegexport_PART_SRCS kis_wdg_options_jpeg.ui )

kde4_add_plugin(kritajpegexport WITH_PREFIX ${kritajpegexport_PART_SRCS})



target_link_libraries(kritajpegexport kritaui jpeg kisexiv2) # link against kisexiv2 to ensure the availability of reading/writing EXIF and IPTC tags

install(TARGETS kritajpegexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_jpeg_import.desktop krita_jpeg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  krita_jpeg.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
