add_definitions(-DTRANSLATION_DOMAIN=\"calligraplan_scheduler_tj\")

set(LIBTJ_INCLUDE_DIR taskjuggler)
include_directories(${LIBTJ_INCLUDE_DIR} ${PLANODF_INCLUDES} ${PLANKERNEL_INCLUDES})

#add_subdirectory( taskjuggler )
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()

set ( TJScheduler_SRCS
    PlanTJPlugin.cpp
    PlanTJScheduler.cpp
    taskjuggler/Allocation.cpp
    taskjuggler/CoreAttributes.cpp
    taskjuggler/CoreAttributesList.cpp
    taskjuggler/Project.cpp
    taskjuggler/Task.cpp
    taskjuggler/TaskDependency.cpp
    taskjuggler/TaskList.cpp
    taskjuggler/TaskScenario.cpp
    taskjuggler/Resource.cpp
    taskjuggler/ResourceList.cpp
    taskjuggler/Scenario.cpp
    taskjuggler/ScenarioList.cpp
    taskjuggler/Shift.cpp
    taskjuggler/ShiftList.cpp
    taskjuggler/ShiftSelection.cpp
    taskjuggler/ShiftSelectionList.cpp
    taskjuggler/VacationList.cpp
    taskjuggler/TjMessageHandler.cpp
    taskjuggler/Utility.cpp
#     taskjuggler/XMLFile.cpp
#     taskjuggler/ParserElement.cpp
#     taskjuggler/ParserNode.cpp
#     taskjuggler/ParserTreeContext.cpp
    taskjuggler/Interval.cpp
)

# TODO: plugin should not be SHARED, but MODULE. Needs to be SHARED because tests link to it -> fix with util lib/objects
add_library(plantjscheduler SHARED ${TJScheduler_SRCS} )
# calligraplan_scheduler_desktop_to_json(plantjscheduler plantjscheduler.desktop)
if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(plantjscheduler plantjscheduler.desktop)
else()
    kcoreaddons_desktop_to_json(plantjscheduler plantjscheduler.desktop
        SERVICE_TYPES ${PLAN_SOURCE_DIR}/libs/kernel/plan_schedulerplugin.desktop
    )
endif()

# TODO: only export symbols for tests, not release
generate_export_header(plantjscheduler BASE_NAME kplatotj)
add_definitions(-Dplantjscheduler_EXPORTS)
target_link_libraries(
    plantjscheduler
    kplatokernel
)

set_target_properties( plantjscheduler PROPERTIES DEFINE_SYMBOL MAKE_KPLATOTJ_LIB )

install( TARGETS plantjscheduler DESTINATION ${PLUGIN_INSTALL_DIR}/calligraplan/schedulers )
