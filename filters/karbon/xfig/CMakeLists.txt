set( xfig2odg_PLUGIN_SRCS
    XFigDocument.cpp
    XFigStreamLineReader.cpp
    XFigParser.cpp
    XFigOdgWriter.cpp
    XFigImportFilter.cpp
)

kde4_add_plugin( calligra_filter_xfig2odg ${xfig2odg_PLUGIN_SRCS} )

target_link_libraries( calligra_filter_xfig2odg
    komain
)

install(TARGETS calligra_filter_xfig2odg  DESTINATION ${PLUGIN_INSTALL_DIR} )

install( FILES  karbon_xfig_import.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES  karbon_xfig_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
