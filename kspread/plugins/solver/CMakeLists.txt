macro_optional_find_package(GSL)
macro_log_feature(GSL_FOUND "GSL" "GNU Scientific Library" "http://www.gnu.org/software/gsl"
                  FALSE "1.7" "Required by KSpread's solver plugin")

if(GSL_FOUND)

include_directories( ${CMAKE_SOURCE_DIR}/kspread
                     ${CMAKE_SOURCE_DIR}/kspread/ui
                     ${KOMAIN_INCLUDES} 
                     ${GSL_INCLUDE_DIR} )

########### next target ###############

set( solver_PART_SRCS
	Solver.cpp
	SolverDialog.cpp
)

kde4_add_ui_files( solver_PART_SRCS
	SolverDetailsWidget.ui
	SolverMainWidget.ui
)

kde4_add_plugin( kspreadsolver WITH_PREFIX ${solver_PART_SRCS} )



target_link_libraries( kspreadsolver
	${GSL_LIBRARIES}
	${GSL_CBLAS_LIBRARIES}
	kspreadcommon
)

install(TARGETS kspreadsolver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES solver.rc  DESTINATION ${DATA_INSTALL_DIR}/kspread/kpartplugins)

endif(GSL_FOUND)
