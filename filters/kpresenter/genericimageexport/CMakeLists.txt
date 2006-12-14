
include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${CMAKE_SOURCE_DIR}/filters/libdialogfilter )


########### next target ###############

set(kpresentergenericimageexport_PART_SRCS genericimageexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kpresentergenericimageexport_PART_SRCS})

kde4_add_plugin(kpresentergenericimageexport WITH_PREFIX ${kpresentergenericimageexport_PART_SRCS})

target_link_libraries(kpresentergenericimageexport  ${KDE4_KDECORE_LIBS} kpresenterprivate )

install(TARGETS kpresentergenericimageexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kpresenter_bmp_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kpresenter_jpeg_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kpresenter_mng_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kpresenter_png_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kpresenter_xbm_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kpresenter_xpm_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})

