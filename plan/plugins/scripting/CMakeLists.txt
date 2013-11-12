
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
)

kde4_add_ui_files(krossmoduleplan_PART_SRCS
    ScriptingDataQueryView.ui
)


qt4_wrap_cpp(krossmoduleplan_PART_SRCS
)

kde4_add_plugin(krossmoduleplan SHARED ${krossmoduleplan_PART_SRCS})

target_link_libraries(
    krossmoduleplan
    kokross
    planprivate
    kplatokernel
    kplatomodels
    komain
)

set_target_properties(krossmoduleplan PROPERTIES DEFINE_SYMBOL MAKE_KPLATOSCRIPTING_LIB)

install(TARGETS krossmoduleplan DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES krossmoduleplan.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES planscripting.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES scripting.rc DESTINATION ${DATA_INSTALL_DIR}/plan/viewplugins)
