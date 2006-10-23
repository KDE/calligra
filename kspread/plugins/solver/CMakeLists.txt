find_package(GSL)

if(GSL_FOUND)

include_directories( ${GSL_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kspread ${KOFFICEUI_INCLUDES} )

########### next target ###############

set( solver_PART_SRCS
	Solver.cpp
	SolverDialog.cpp
)

kde4_automoc(${solver_PART_SRCS})

kde4_add_ui_files( solver_PART_SRCS
	SolverDetailsWidget.ui
	SolverMainWidget.ui
)

kde4_add_plugin( kspreadsolver WITH_PREFIX ${solver_PART_SRCS} )

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kspreadsolver )

target_link_libraries( kspreadsolver
	${GSL_LIBRARIES}
	${GSL_CBLAS_LIBRARIES}
	kspreadcommon
	${KDE4_KDECORE_LIBS}
)

install(TARGETS kspreadsolver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES solver.rc  DESTINATION ${DATA_INSTALL_DIR}/kspread/kpartplugins)

endif(GSL_FOUND)
