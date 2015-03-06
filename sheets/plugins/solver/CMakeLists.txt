if(GSL_FOUND)

include_directories( ${CMAKE_SOURCE_DIR}/sheets
                     ${CMAKE_SOURCE_DIR}/sheets/ui
                     ${KOMAIN_INCLUDES} 
                     ${GSL_INCLUDE_DIR} )

########### next target ###############

set( solver_PART_SRCS
	Solver.cpp
	SolverDialog.cpp
)

ki18n_wrap_ui( solver_PART_SRCS
	SolverDetailsWidget.ui
	SolverMainWidget.ui
)

add_library(sheetssolver MODULE ${solver_PART_SRCS} )



target_link_libraries( sheetssolver
	${GSL_LIBRARIES}
        calligrasheetscommon
)

install(TARGETS sheetssolver  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES solver.rc  DESTINATION ${DATA_INSTALL_DIR}/sheets/viewplugins)
install( FILES sheetssolver.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)


endif()
