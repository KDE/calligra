
include_directories(
    ${CMAKE_SOURCE_DIR}/karbon/common
    
)

########### next target ###############

set(karbon1x2karbon_PART_SRCS Karbon1xImport.cpp)


add_library(calligra_filter_karbon1x2karbon MODULE ${karbon1x2karbon_PART_SRCS})

target_link_libraries(calligra_filter_karbon1x2karbon karboncommon)

install(TARGETS calligra_filter_karbon1x2karbon DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

########### install files ###############

#[[if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES karbon_karbon1x_thumbnail.desktop DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/thumbcreator)
endif()]]
