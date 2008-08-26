

include_directories(
    ${KOMAIN_INCLUDES}
    )




########### next target ###############

set(generickofilter_PART_SRCS generic_filter.cc )


kde4_add_plugin(generickofilter WITH_PREFIX ${generickofilter_PART_SRCS})



target_link_libraries(generickofilter kstore komain ${KDE4_KIO_LIBS} ${QT_QT3SUPPORT_LIBRARY} )

install(TARGETS generickofilter  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  generic_filter.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

