
include_directories( 
    ${CMAKE_SOURCE_DIR}/kplato
    ${KPLATO_INCLUDES}
    ${KOGUIUTILS_INCLUDES}
    ${KOKROSS_INCLUDES}
)

add_subdirectory( scripts )

set(krossmodulekplato_PART_SRCS
    Project.cpp
    Module.cpp
    ScriptingPart.cpp
)

qt4_wrap_cpp(krossmodulekplato_PART_SRCS
)

kde4_add_plugin(krossmodulekplato ${krossmodulekplato_PART_SRCS})
target_link_libraries(krossmodulekplato ${KDE4_KROSSUI_LIBS} ${KDE4_KPARTS_LIBS} kokross kplatoprivate)
install(TARGETS krossmodulekplato DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES krossmodulekplato.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES scripting.rc DESTINATION ${DATA_INSTALL_DIR}/kplato/kpartplugins)
