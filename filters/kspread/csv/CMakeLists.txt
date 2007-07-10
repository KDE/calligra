

include_directories( 
	${KOTEXT_INCLUDES}
	${CMAKE_SOURCE_DIR}/kspread 
	${KDE4_INCLUDE_DIR} 
	${QT_INCLUDES}  )


########### next target ###############

set(csvimport_PART_SRCS csvimport.cc xmltree.cc csvdialog.cpp )


kde4_add_ui_files(csvimport_PART_SRCS dialogui.ui )

kde4_add_plugin(csvimport WITH_PREFIX ${csvimport_PART_SRCS})



target_link_libraries(csvimport  kspreadcommon )

install(TARGETS csvimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(csvexport_PART_SRCS csvexport.cc csvexportdialog.cpp )

kde4_add_ui_files(csvexport_PART_SRCS exportdialogui.ui )

kde4_add_plugin(csvexport WITH_PREFIX ${csvexport_PART_SRCS})



target_link_libraries(csvexport  kspreadcommon )

install(TARGETS csvexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_csv_import.desktop kspread_csv_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

