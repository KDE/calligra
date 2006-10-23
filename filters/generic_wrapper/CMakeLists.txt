

include_directories(
    ${KOFFICECORE_INCLUDES}
    )




########### next target ###############

set(generickofilter_PART_SRCS generic_filter.cc )

kde4_automoc(${generickofilter_PART_SRCS})

kde4_add_plugin(generickofilter WITH_PREFIX ${generickofilter_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} generickofilter )

target_link_libraries(generickofilter  ${KDE4_KIO_LIBS} kofficecore )

install(TARGETS generickofilter  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  generic_filter.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

