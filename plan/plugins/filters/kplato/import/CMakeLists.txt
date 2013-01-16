
include_directories(
    ${CMAKE_BINARY_DIR}/plan/plugins/filters/plan/import
    ${CMAKE_SOURCE_DIR}/plan
    ${CMAKE_SOURCE_DIR}/plan/libs/kernel
    ${KOMAIN_INCLUDES}
)

set(kplatoimport_PART_SRCS 
   kplatoimport.cpp
)


kde4_add_plugin(plankplatoimport ${kplatoimport_PART_SRCS})

target_link_libraries(plankplatoimport planprivate kplatokernel komain)

install(TARGETS plankplatoimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  plan_kplato_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
