include_directories (${CMAKE_CURRENT_BINARY_DIR} ${CALLIGRA_INCLUDE_DIR} )

########### next target ###############

set(braindumpquickstates_PART_SRCS BraindumpQuickStatesPlugin.cpp QuickStateHandler.cpp )

kde4_add_plugin(braindumpquickstates ${braindumpquickstates_PART_SRCS})

target_link_libraries(braindumpquickstates ${FLAKE_LIBRARY} ${KOTEXT_LIBRARY} ${KOBASE_LIBRARY}  braindumpcore)

install(TARGETS braindumpquickstates  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  quickstates.rc  DESTINATION ${DATA_INSTALL_DIR}/braindump/plugins)
install( FILES  braindumpquickstates.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
