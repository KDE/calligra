if(BUILD_TESTING)
    add_subdirectory( tests )
endif()

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

target_link_libraries(calligra_filter_ppt2odp ppttoodplib komain)

install(TARGETS calligra_filter_ppt2odp  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/formatfilters)

if(SHOULD_BUILD_OKULAR_GENERATOR_PPT)
    kcoreaddons_add_plugin(okularGenerator_powerpoint_calligra
        INSTALL_NAMESPACE "okular_generators"
        SOURCES OkularPptGeneratorPlugin.cpp
    )

    target_link_libraries( okularGenerator_powerpoint_calligra
        kookularGenerator_odp
    )

    install( PROGRAMS okularApplication_powerpoint_calligra.desktop DESTINATION ${KDE_INSTALL_APPDIR} )
endif()
