set(LIBQPRO_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libqpro")

include_directories( ${CMAKE_SOURCE_DIR}/sheets ${LIBQPRO_SOURCES_DIR}  ${KOMAIN_INCLUDES} )


########### next target ###############

set(qpro2sheets_PART_SRCS
	${LIBQPRO_SOURCES_DIR}/src/formula.cc
	${LIBQPRO_SOURCES_DIR}/src/record.cc
	${LIBQPRO_SOURCES_DIR}/src/record_factory.cc
	${LIBQPRO_SOURCES_DIR}/src/stream.cc
	${LIBQPRO_SOURCES_DIR}/src/tablenames.cc
	qproimport.cc
	qproformula.cc )

add_library(calligra_filter_qpro2sheets MODULE ${qpro2sheets_PART_SRCS})


target_link_libraries(calligra_filter_qpro2sheets calligrasheetsui)

install(TARGETS calligra_filter_qpro2sheets  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
