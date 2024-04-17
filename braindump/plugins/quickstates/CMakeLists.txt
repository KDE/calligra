include_directories (${CMAKE_CURRENT_BINARY_DIR} ${CALLIGRA_INCLUDE_DIR} )

########### next target ###############

set(braindumpquickstates_PART_SRCS BraindumpQuickStatesPlugin.cpp QuickStateHandler.cpp )

add_library(braindumpquickstates MODULE ${braindumpquickstates_PART_SRCS})

target_link_libraries(braindumpquickstates ${FLAKE_LIBRARY} ${KOTEXT_LIBRARY} ${KOBASE_LIBRARY}  braindumpcore)

install(TARGETS braindumpquickstates  DESTINATION ${KDE_INSTALL_PLUGINDIR}/braindump/extensions)


########### install files ###############

install( FILES  quickstates.rc  DESTINATION ${KDE_INSTALL_DATADIR}/braindump/plugins)
install( FILES  braindumpquickstates.desktop  DESTINATION ${KDE_INSTALL_KSERVICES5DIR}/calligra)
