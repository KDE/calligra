set( xfig2odg_PLUGIN_SRCS
    XFigDocument.cpp
    XFigStreamLineReader.cpp
    XFigParser.cpp
    XFigOdgWriter.cpp
    XFigImportFilter.cpp
)

add_library(calligra_filter_xfig2odg MODULE ${xfig2odg_PLUGIN_SRCS} )
kcoreaddons_desktop_to_json(calligra_filter_xfig2odg calligra_filter_xfig2odg.desktop)

target_link_libraries( calligra_filter_xfig2odg
    komain
    KF5::KDELibs4Support
)

install(TARGETS calligra_filter_xfig2odg  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR} )

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES  karbon_xfig_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()
