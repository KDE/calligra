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
calligrasheets_viewplugin_desktop_to_json(sheetssolver sheetssolver.desktop)


target_link_libraries( sheetssolver
        ${GSL_LIBRARIES}
        ${GSL_CBLAS_LIBRARIES}
        calligrasheetscommon
)

install(TARGETS sheetssolver  DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/extensions)


########### install files ###############

install( FILES solver.rc  DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/viewplugins)

endif()
