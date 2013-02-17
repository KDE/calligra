
macro_optional_find_package(Pstoedit)
macro_log_feature(PSTOEDIT_SVG_PLUGIN_FOUND "pstoedit with svg plugin" "Translates PostScript and PDF graphics to SVG" "http://www.pstoedit.net/" FALSE "" "Required at runtime by the Karbon EPS import filter")

if (PSTOEDIT_SVG_PLUGIN_FOUND)

set(karbonepsimport_PART_SRCS EpsImport.cpp PsCommentLexer.cpp )

kde4_add_plugin(karbonepsimport ${karbonepsimport_PART_SRCS})

target_link_libraries(karbonepsimport komain)

install(TARGETS karbonepsimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES
    karbon_eps_import.desktop
    #karbon_ps_import.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)

endif (PSTOEDIT_SVG_PLUGIN_FOUND)
