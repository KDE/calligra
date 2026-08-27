add_library(interfaces INTERFACE)
target_include_directories(interfaces INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/calligra>
)
install(TARGETS interfaces EXPORT CalligraQt6Targets)

if (SHOULD_BUILD_DEVEL_HEADERS)

install(FILES KoGenericRegistry.h KoID.h KoChartInterface.h KoChartModel.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel)

endif()
