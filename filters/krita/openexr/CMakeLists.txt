

include_directories(
	${OPENEXR_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/krita
	${CMAKE_SOURCE_DIR}/krita/core/
	${CMAKE_SOURCE_DIR}/krita/sdk/
	${CMAKE_SOURCE_DIR}/krita/core/tiles/
	${CMAKE_SOURCE_DIR}/krita/kritacolor/
	${CMAKE_SOURCE_DIR}/krita/ui/
	${CMAKE_SOURCE_DIR}/krita/colorspaces/rgb_f32/
	${CMAKE_SOURCE_DIR}/krita/colorspaces/rgb_f16half/
	)


########### next target ###############

set(krita_openexr_import_PART_SRCS kis_openexr_import.cpp )

kde4_automoc(${krita_openexr_import_PART_SRCS})

kde4_add_plugin(krita_openexr_import WITH_PREFIX ${krita_openexr_import_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} krita_openexr_import )

target_link_libraries(krita_openexr_import kritaui krita_rgb_f32 krita_rgb_f16half ${OPENEXR_LIBRARIES} )

install(TARGETS krita_openexr_import  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(krita_openexr_export_PART_SRCS kis_openexr_export.cpp )

kde4_automoc(${krita_openexr_export_PART_SRCS})

kde4_add_plugin(krita_openexr_export WITH_PREFIX ${krita_openexr_export_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} krita_openexr_export )

target_link_libraries(krita_openexr_export kritaui krita_rgb_f32 krita_rgb_f16half ${OPENEXR_LIBRARIES})

install(TARGETS krita_openexr_export  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_openexr_import.desktop krita_openexr_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  krita_openexr.desktop  DESTINATION ${XDG_APPS_DIR})

