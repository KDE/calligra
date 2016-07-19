add_definitions(-DTRANSLATION_DOMAIN=\"calligraplan_scheduler_tj\")

set(LIBTJ_INCLUDE_DIR taskjuggler)
include_directories( ${LIBTJ_INCLUDE_DIR} ${KOODF_INCLUDES} ${CMAKE_SOURCE_DIR}/plan/libs/kernel  )

#add_subdirectory( taskjuggler )
add_subdirectory( tests )

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
kcoreaddons_desktop_to_json(plantjscheduler plantjscheduler.desktop)
# TODO: only export symbols for tests, not release
generate_export_header(plantjscheduler BASE_NAME kplatotj)

target_link_libraries(
    plantjscheduler
    kplatokernel
)

set_target_properties( plantjscheduler PROPERTIES DEFINE_SYMBOL MAKE_KPLATOTJ_LIB )

install( TARGETS plantjscheduler DESTINATION ${PLUGIN_INSTALL_DIR}/calligraplan/schedulers )
