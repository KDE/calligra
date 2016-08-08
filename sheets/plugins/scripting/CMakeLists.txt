add_definitions(-DTRANSLATION_DOMAIN=\"krossmodulesheets\")

include_directories( 
        ${CMAKE_SOURCE_DIR}/sheets
        ${CMAKE_SOURCE_DIR}/sheets/commands
        ${CMAKE_SOURCE_DIR}/sheets/ui
        ${KOTEXT_INCLUDES}
        ${KOMAIN_INCLUDES}
        ${KOKROSS_INCLUDES}
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

add_subdirectory( scripts )

# the Sheets Scripting Plugin + Kross module
set(krossmodulesheets_PART_SRCS
    ScriptingDebug.cpp
    ScriptingModule.cpp
    ScriptingFunction.cpp
    ScriptingWidgets.cpp
    ScriptingPart.cpp
    ScriptingListener.cpp
)

qt5_wrap_cpp(krossmodulesheets_PART_SRCS
    ScriptingReader.h
    ScriptingWriter.h
)

add_library(krossmodulesheets MODULE ${krossmodulesheets_PART_SRCS})
generate_export_header(krossmodulesheets BASE_NAME sheetsscripting)
calligrasheets_viewplugin_desktop_to_json(krossmodulesheets sheetsscripting.desktop)

target_link_libraries(krossmodulesheets
    calligrasheetscommon
    kokross
)

install(TARGETS krossmodulesheets DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/extensions)
install( FILES scripting.rc  DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/viewplugins)
