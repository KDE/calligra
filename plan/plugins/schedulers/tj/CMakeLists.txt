
set(LIBTJ_INCLUDE_DIR taskjuggler)
include_directories( ${LIBTJ_INCLUDE_DIR} ${KOODF_INCLUDES} ${CMAKE_SOURCE_DIR}/plan/libs/kernel ${KDE4_INCLUDES} )

#add_subdirectory( taskjuggler )
add_subdirectory( tests )

SET ( TJScheduler_SRCS
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

kde4_add_plugin( plantjscheduler SHARED ${TJScheduler_SRCS} )

target_link_libraries(
    plantjscheduler
    kplatokernel
    koodf
)

set_target_properties( plantjscheduler PROPERTIES DEFINE_SYMBOL MAKE_KPLATOTJ_LIB )

install( TARGETS plantjscheduler DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES plantjscheduler.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
