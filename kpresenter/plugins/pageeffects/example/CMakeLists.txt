include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${KOGUIUTILS_INCLUDES} )

set(kpr_pageeffect_example_PART_SRCS Plugin.cpp )

kde4_add_plugin(kpr_pageeffect_example ${kpr_pageeffect_example_PART_SRCS})

target_link_libraries(kpr_pageeffect_example kpresenterprivate flake koguiutils)

install(TARGETS kpr_pageeffect_example  DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES kpr_pageeffect_example.desktop DESTINATION ${SERVICES_INSTALL_DIR})


