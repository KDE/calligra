
include_directories(
    ${CMAKE_SOURCE_DIR}/karbon/common
    
)

########### next target ###############

set(karbon1x2karbon_PART_SRCS Karbon1xImport.cpp)


add_library(calligra_filter_karbon1x2karbon MODULE ${karbon1x2karbon_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_karbon1x2karbon calligra_filter_karbon1x2karbon.desktop)

target_link_libraries(calligra_filter_karbon1x2karbon karboncommon KF5::KDELibs4Support)

install(TARGETS calligra_filter_karbon1x2karbon DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

########### install files ###############

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES karbon_karbon1x_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif()
