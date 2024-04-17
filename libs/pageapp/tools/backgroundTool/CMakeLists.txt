include_directories( ../.. )

set(kopabackgroundtool_PART_SRCS
    Plugin.cpp
    KoPABackgroundToolFactory.cpp
    KoPAMasterPageDocker.cpp
)


add_library(kopabackgroundtool MODULE ${kopabackgroundtool_PART_SRCS})

target_link_libraries(
    kopabackgroundtool
    kopageapp
)

install(TARGETS kopabackgroundtool  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/pageapptools)


########### install files ###############

if (SHOULD_BUILD_DEVEL_HEADERS)
install(FILES
    KoPABackgroundTool.h
    DESTINATION
    ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel
)
endif()

ecm_install_icons( ICONS
    22-actions-backgroundtool.png
    DESTINATION ${KDE_INSTALL_DATADIR}/calligra/icons
    THEME hicolor
)
