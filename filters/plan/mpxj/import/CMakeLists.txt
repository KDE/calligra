
include_directories(
    ${CMAKE_BINARY_DIR}/filters/plan/mpxj/planconvert
    ${CMAKE_BINARY_DIR}/filters/plan/mpxj/import
    ${KOMAIN_INCLUDES}
    ${JNI_INCLUDE_DIRS}
)

set(mpxjimport_PART_SRCS
   mpxjimport.cpp
)


kde4_add_plugin(planmpxjimport ${mpxjimport_PART_SRCS})

target_link_libraries(planmpxjimport komain)
# Note: no need to link to ${JNI_LIBRARIES} currently, JNI not used,
# cmp. "NOTE:" in mpxjimport.cpp

install(TARGETS planmpxjimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES
    plan_msproject_import.desktop
    plan_planner_import.desktop
  DESTINATION ${SERVICES_INSTALL_DIR})
