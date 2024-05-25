set( xfig2odg_PLUGIN_SRCS
    XFigDocument.cpp
    XFigStreamLineReader.cpp
    XFigParser.cpp
    XFigOdgWriter.cpp
    XFigImportFilter.cpp
)

add_library(calligra_filter_xfig2odg MODULE ${xfig2odg_PLUGIN_SRCS} )

target_link_libraries( calligra_filter_xfig2odg
    komain
)

install(TARGETS calligra_filter_xfig2odg  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters )
