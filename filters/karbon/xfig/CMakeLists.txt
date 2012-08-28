set( karbonxfigimport_PLUGIN_SRCS
    XFigDocument.cpp
    XFigStreamLineReader.cpp
    XFigParser.cpp
    XFigOdgWriter.cpp
    XFigImportFilter.cpp
)

kde4_add_plugin( karbonxfigimport ${karbonxfigimport_PLUGIN_SRCS} )

target_link_libraries( karbonxfigimport
    komain
)

install(TARGETS karbonxfigimport  DESTINATION ${PLUGIN_INSTALL_DIR} )

install( FILES  karbon_xfig_import.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES  karbon_xfig_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
