

include_directories(
	${CMAKE_SOURCE_DIR}/kchart 
	${CMAKE_SOURCE_DIR}/kchart/kdchart1
	${CMAKE_SOURCE_DIR}/interfaces 
	${CMAKE_SOURCE_DIR}/filters/libdialogfilter 
	${KDE4_INCLUDE_DIR} 
	${QT_INCLUDES}  )


########### next target ###############

set(kchartgenericimageexport_PART_SRCS genericimageexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kchartgenericimageexport_PART_SRCS})

kde4_add_plugin(kchartgenericimageexport WITH_PREFIX ${kchartgenericimageexport_PART_SRCS})

target_link_libraries(kchartgenericimageexport  ${KDE4_KDECORE_LIBS} kchartcommon )

install(TARGETS kchartgenericimageexport  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kchart_bmp_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_jpeg_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_mng_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_png_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_xbm_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES kchart_xpm_export.desktop DESTINATION ${SERVICES_INSTALL_DIR})

