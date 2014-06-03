
macro_optional_find_package(Pstoedit)
macro_log_feature(PSTOEDIT_SVG_PLUGIN_FOUND "pstoedit with svg plugin" "Translates PostScript and PDF graphics to SVG" "http://www.pstoedit.net/" FALSE "" "Required at runtime by the Karbon EPS import filter")

if (PSTOEDIT_SVG_PLUGIN_FOUND)

set(eps2svgai_PART_SRCS EpsImport.cpp PsCommentLexer.cpp )

kde4_add_plugin(calligra_filter_eps2svgai ${eps2svgai_PART_SRCS})

target_link_libraries(calligra_filter_eps2svgai komain)

install(TARGETS calligra_filter_eps2svgai  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES
    calligra_filter_eps2svgai.desktop
    #karbon_ps_import.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)

endif (PSTOEDIT_SVG_PLUGIN_FOUND)
