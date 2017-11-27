
include_directories(
    ${CMAKE_SOURCE_DIR}/plan
    ${CMAKE_SOURCE_DIR}/plan/libs/kernel
    ${KOMAIN_INCLUDES}
)

set(kplatoimport_PART_SRCS 
   kplatoimport.cpp
)


add_library(plankplatoimport MODULE ${kplatoimport_PART_SRCS})
# calligraplan_filter_desktop_to_json(plankplatoimport plan_kplato_import.desktop)
if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(plankplatoimport plan_kplato_import.desktop)
else()
    kcoreaddons_desktop_to_json(plankplatoimport plan_kplato_import.desktop
#         SERVICE_TYPES ${PLAN_SOURCE_DIR}/servicetypes/calligraplan_filter.desktop
    )
endif()

target_link_libraries(plankplatoimport planprivate kplatokernel planmain)

install(TARGETS plankplatoimport  DESTINATION ${PLUGIN_INSTALL_DIR}/calligraplan/formatfilters)
