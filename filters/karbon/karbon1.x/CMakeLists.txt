
include_directories(
    ${CMAKE_SOURCE_DIR}/karbon/common
    ${KDE4_INCLUDES}
)

########### next target ###############

set(karbon1x2karbon_PART_SRCS Karbon1xImport.cpp)


kde4_add_plugin(calligra_filter_karbon1x2karbon ${karbon1x2karbon_PART_SRCS})

target_link_libraries(calligra_filter_karbon1x2karbon karboncommon)

install(TARGETS calligra_filter_karbon1x2karbon DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES calligra_filter_karbon1x2karbon.desktop DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES karbon_karbon1x_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
