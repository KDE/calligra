
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

target_link_libraries(planmpxjimport komain ${JNI_LIBRARIES})

install(TARGETS planmpxjimport  DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES
    plan_msproject_import.desktop
    plan_msprojectexchange_import.desktop
    plan_planner_import.desktop
  DESTINATION ${SERVICES_INSTALL_DIR})

install(FILES calligra_planner_mpp.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
update_xdg_mimetypes(${XDG_MIME_INSTALL_DIR})
