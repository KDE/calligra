include_directories(
    ${KOMAIN_INCLUDES}
    ${KOTEXT_INCLUDES}
)

set(kpr2odp_PART_SRCS Filterkpr2odf.cpp )

kde4_add_plugin(calligra_filter_kpr2odp ${kpr2odp_PART_SRCS})

target_link_libraries(calligra_filter_kpr2odp komain)

install(TARGETS calligra_filter_kpr2odp DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  Filterkpr2odf.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES  stage_kpr_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
