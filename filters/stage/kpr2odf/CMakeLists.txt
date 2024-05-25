include_directories(
    ${KOMAIN_INCLUDES}
    ${KOTEXT_INCLUDES}
)

set(kpr2odp_PART_SRCS Filterkpr2odf.cpp )

add_library(calligra_filter_kpr2odp MODULE ${kpr2odp_PART_SRCS})


target_link_libraries(calligra_filter_kpr2odp komain)

install(TARGETS calligra_filter_kpr2odp DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

