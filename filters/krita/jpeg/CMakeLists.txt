

include_directories(${CMAKE_SOURCE_DIR}/filters/krita/libkisexiv2 ${KRITA_INCLUDES} ${CMAKE_SOURCE_DIR}/krita/image/metadata)

set(libkisexiv2_LIB_SRCS
    ${CMAKE_SOURCE_DIR}/filters/krita/libkisexiv2/kis_exiv2_io.cpp
    )

set(libkritaconverter_LIB_SRCS
    kis_jpeg_converter.cc
    iccjpeg.c
    ${libkisexiv2_LIB_SRCS}
    )

set(kritajpegimport_PART_SRCS
    kis_jpeg_import.cc
    ${libkritaconverter_LIB_SRCS}
    )

kde4_automoc(${kritajpegimport_PART_SRCS})

kde4_add_plugin(kritajpegimport WITH_PREFIX ${kritajpegimport_PART_SRCS})



target_link_libraries(kritajpegimport kritaui jpeg exiv2)

install(TARGETS kritajpegimport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

set(kritajpegexport_PART_SRCS
    kis_jpeg_export.cc
    ${libkritaconverter_LIB_SRCS}
    )

kde4_automoc(${kritajpegexport_PART_SRCS})

kde4_add_ui3_files(kritajpegexport_PART_SRCS kis_wdg_options_jpeg.ui )

kde4_add_plugin(kritajpegexport WITH_PREFIX ${kritajpegexport_PART_SRCS})



target_link_libraries(kritajpegexport kritaui jpeg exiv2)

install(TARGETS kritajpegexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_jpeg_import.desktop krita_jpeg_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  krita_jpeg.desktop  DESTINATION ${XDG_APPS_DIR})
