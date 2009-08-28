include_directories( ${CMAKE_SOURCE_DIR}/kspread ${CMAKE_CURRENT_SOURCE_DIR}/libqpro ${KDE4_INCLUDES} ${KOMAIN_INCLUDES} )


########### next target ###############

set(qproimport_PART_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/formula.cc
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/record.cc
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/record_factory.cc
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/stream.cc
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/tablenames.cc
	qproimport.cc
	qproformula.cc )

kde4_add_plugin(qproimport WITH_PREFIX ${qproimport_PART_SRCS})



target_link_libraries(qproimport kspreadcommon)

install(TARGETS qproimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kspread_qpro_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

