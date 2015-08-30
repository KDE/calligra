add_definitions(-DTRANSLATION_DOMAIN=\"plantjplugin\")

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

add_library(plantjscheduler MODULE ${TJScheduler_SRCS} )
kcoreaddons_desktop_to_json(plantjscheduler plantjscheduler.desktop)
# TODO: only export symbols for tests, not release
generate_export_header(plantjscheduler BASE_NAME kplatotj)

target_link_libraries(
    plantjscheduler
    kplatokernel
    ${KDE4_KDECORE_LIBS}
    ${QT_QTXML_LIBRARY}
)

set_target_properties( plantjscheduler PROPERTIES DEFINE_SYMBOL MAKE_KPLATOTJ_LIB )

install( TARGETS plantjscheduler DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR} )
