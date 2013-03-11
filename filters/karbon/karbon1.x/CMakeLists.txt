
include_directories(
    ${CMAKE_SOURCE_DIR}/karbon/common
    ${KDE4_INCLUDES}
)

########### next target ###############

set(karbon1ximport_PART_SRCS Karbon1xImport.cpp)


kde4_add_plugin(karbon1ximport ${karbon1ximport_PART_SRCS})

target_link_libraries(karbon1ximport karboncommon)

install(TARGETS karbon1ximport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES karbon_1x_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

install( FILES karbon_karbon1x_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
