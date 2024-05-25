
include_directories(
    ${CMAKE_SOURCE_DIR}/karbon/common
    
)

########### next target ###############

set(karbon1x2karbon_PART_SRCS Karbon1xImport.cpp)


add_library(calligra_filter_karbon1x2karbon MODULE ${karbon1x2karbon_PART_SRCS})

target_link_libraries(calligra_filter_karbon1x2karbon karboncommon)

install(TARGETS calligra_filter_karbon1x2karbon DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)
