add_subdirectory(3rdparty)

include_directories(
    3rdparty/rtf-qt/src/
)

set(rtf2odt_PART_SRCS rtfimport.cpp )
add_library(calligra_filter_rtf2odt MODULE ${rtf2odt_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_filter_rtf2odt calligra_filter_rtf2odt.desktop)
target_link_libraries(calligra_filter_rtf2odt RtfReader komain KF5::KDELibs4Support)

install(TARGETS calligra_filter_rtf2odt  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES  words_rtf_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif()
