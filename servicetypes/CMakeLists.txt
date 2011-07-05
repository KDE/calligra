########### install files ###############

install(
    FILES
        calligra_application.desktop
        kofilter.desktop
        koplugin.desktop
        calligrapart.desktop
        kochart.desktop
        kofilterwrapper.desktop
        calligra_deferred_plugin.desktop
    DESTINATION
        ${SERVICETYPES_INSTALL_DIR}
)
