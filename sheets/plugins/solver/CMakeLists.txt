if(GSL_FOUND)

add_definitions(-DTRANSLATION_DOMAIN=\"calligrasheets_solver\")

include_directories( ${CMAKE_SOURCE_DIR}/sheets
                     ${CMAKE_BINARY_DIR}/sheets
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
        GSL::gsl
        GSL::gslcblas
        calligrasheetspartlib
)

install(TARGETS sheetssolver  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/extensions)


########### install files ###############

install( FILES solver.rc  DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/viewplugins)

endif()
