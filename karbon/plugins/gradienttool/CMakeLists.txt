include_directories( ${KOFFICECORE_INCLUDES} ${FLAKE_INCLUDES} )

set(gradienttool_PART_SRCS
    KarbonGradientPlugin.cpp
    KarbonGradientTool.cpp
    KarbonGradientToolFactory.cpp
 )

kde4_automoc(${gradienttool_PART_SRCS})

kde4_add_plugin(gradienttool ${gradienttool_PART_SRCS})

target_link_libraries(gradienttool kofficeui)

kde4_install_icons( ${DATA_INSTALL_DIR}/koffice/icons  )

install(TARGETS gradienttool DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES gradienttool.desktop DESTINATION ${SERVICES_INSTALL_DIR})
