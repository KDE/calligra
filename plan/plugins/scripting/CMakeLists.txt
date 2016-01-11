add_definitions(-DTRANSLATION_DOMAIN=\"krossmoduleplan\")

include_directories( 
    ${CMAKE_SOURCE_DIR}/plan
    ${KPLATO_INCLUDES}
    ${KOKROSS_INCLUDES}
)

add_subdirectory( scripts )
add_subdirectory( tests )

set(krossmoduleplan_PART_SRCS
    Account.cpp
    Calendar.cpp
    Node.cpp
    Resource.cpp
    ResourceGroup.cpp
    Schedule.cpp
    Project.cpp
    Module.cpp
    ScriptingPart.cpp
    ScriptingWidgets.cpp
    ScriptingDebug.cpp
)

ki18n_wrap_ui(krossmoduleplan_PART_SRCS
    ScriptingDataQueryView.ui
)

add_library(krossmoduleplan MODULE ${krossmoduleplan_PART_SRCS})
generate_export_header(krossmoduleplan
    BASE_NAME kplatoscripting
    EXPORT_FILE_NAME kplatoscripting_generated_export.h
)
kcoreaddons_desktop_to_json(krossmoduleplan planscripting.desktop)

target_link_libraries(krossmoduleplan
    planprivate
    kplatokernel
    kplatomodels
    komain
    kokross
)

install(TARGETS krossmoduleplan DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES krossmoduleplan.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES scripting.rc DESTINATION ${DATA_INSTALL_DIR}/plan/viewplugins)
