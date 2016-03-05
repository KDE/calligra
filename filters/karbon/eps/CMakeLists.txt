
macro_optional_find_package(PstoeditSvgOutput)
set_package_properties(PstoeditSvgOutput PROPERTIES
    DESCRIPTION "pstoedit with svg plugin"
    URL "http://www.pstoedit.net/"
    PURPOSE "Required at runtime by the calligra_filter_eps2svgai format filter"
    TYPE RUNTIME
)

set(eps2svgai_PART_SRCS EpsImport.cpp PsCommentLexer.cpp )

add_library(calligra_filter_eps2svgai MODULE ${eps2svgai_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_eps2svgai calligra_filter_eps2svgai.desktop)

target_link_libraries(calligra_filter_eps2svgai komain)

install(TARGETS calligra_filter_eps2svgai  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

#TODO: restore also importing from PostScript
#karbon_ps_import.desktop
