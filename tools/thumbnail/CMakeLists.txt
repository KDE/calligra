include_directories( ${KOMAIN_INCLUDES})

########### next target ###############

set(calligrathumbnail_SRCS calligracreator.cpp )
kde4_add_plugin(calligrathumbnail ${calligrathumbnail_SRCS})
target_link_libraries(calligrathumbnail komain)
install(TARGETS calligrathumbnail  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES  calligrathumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})


