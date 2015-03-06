set(LIBQPRO_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libqpro")

include_directories( ${CMAKE_SOURCE_DIR}/sheets ${LIBQPRO_SOURCES_DIR} ${KDE4_INCLUDES} ${KOMAIN_INCLUDES} )


########### next target ###############

set(qpro2sheets_PART_SRCS
	${LIBQPRO_SOURCES_DIR}/src/formula.cc
	${LIBQPRO_SOURCES_DIR}/src/record.cc
	${LIBQPRO_SOURCES_DIR}/src/record_factory.cc
	${LIBQPRO_SOURCES_DIR}/src/stream.cc
	${LIBQPRO_SOURCES_DIR}/src/tablenames.cc
	qproimport.cc
	qproformula.cc )

kde4_add_plugin(calligra_filter_qpro2sheets ${qpro2sheets_PART_SRCS})



target_link_libraries(calligra_filter_qpro2sheets calligrasheetscommon)

install(TARGETS calligra_filter_qpro2sheets  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  calligra_filter_qpro2sheets.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

