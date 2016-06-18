add_subdirectory( tests )

include_directories(
    ../../libmso
    ${KOMAIN_INCLUDES}
    ${KOTEXT_INCLUDES}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS} -fPIC")

add_library(ppttoodplib STATIC PptDebug.cpp PptToOdp.cpp ParsedPresentation.cpp DateTimeFormat.cpp pptstyle.cpp)
target_link_libraries(ppttoodplib PUBLIC koodf mso)

add_executable(ppttoodp ppttoodpmain.cpp)
target_link_libraries(ppttoodp ppttoodplib)

add_library(calligra_filter_ppt2odp MODULE PowerPointImport.cpp)
kcoreaddons_desktop_to_json(calligra_filter_ppt2odp calligra_filter_ppt2odp.desktop)

target_link_libraries(calligra_filter_ppt2odp ppttoodplib komain)

install(TARGETS calligra_filter_ppt2odp  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/formatfilters)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES  stage_powerpoint_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif()

if(SHOULD_BUILD_OKULAR_GENERATOR_PPT)
    kcoreaddons_add_plugin(okularGenerator_powerpoint_calligra
        JSON libokularGenerator_ppt.json
        INSTALL_NAMESPACE "okular/generators"
        SOURCES OkularPptGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_powerpoint_calligra
        kookularGenerator_odp
    )

    install( FILES okularPowerpoint_calligra.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
    install( PROGRAMS okularApplication_powerpoint_calligra.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
endif()
