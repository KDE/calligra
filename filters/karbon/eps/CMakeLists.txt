
macro_optional_find_package(Pstoedit)
macro_log_feature(PSTOEDIT_SVG_PLUGIN_FOUND "pstoedit with svg plugin" "Translates PostScript and PDF graphics to SVG" "http://www.pstoedit.net/" FALSE "" "Required at runtime by the Karbon EPS import filter")

if (PSTOEDIT_SVG_PLUGIN_FOUND)

set(eps2svgai_PART_SRCS EpsImport.cpp PsCommentLexer.cpp )

add_library(calligra_filter_eps2svgai MODULE ${eps2svgai_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_eps2svgai calligra_filter_eps2svgai.desktop)

target_link_libraries(calligra_filter_eps2svgai komain)

install(TARGETS calligra_filter_eps2svgai  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

#TODO: restore also importing from PostScript
#karbon_ps_import.desktop

endif ()
