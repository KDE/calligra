include_directories( ${CMAKE_SOURCE_DIR}/sheets ${CMAKE_CURRENT_SOURCE_DIR}/libqpro ${KDE4_INCLUDES} ${KOMAIN_INCLUDES} )


########### next target ###############

set(qpro2sheets_PART_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/formula.cc
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/record.cc
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/record_factory.cc
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/stream.cc
	${CMAKE_CURRENT_SOURCE_DIR}/libqpro/src/tablenames.cc
	qproimport.cc
	qproformula.cc )

kde4_add_plugin(calligra_filter_qpro2sheets ${qpro2sheets_PART_SRCS})



target_link_libraries(calligra_filter_qpro2sheets calligrasheetscommon)

install(TARGETS calligra_filter_qpro2sheets  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  calligra_filter_qpro2sheets.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

