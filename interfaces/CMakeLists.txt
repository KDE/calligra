if (SHOULD_BUILD_DEVEL_HEADERS)

install(FILES KoGenericRegistry.h KoID.h KoChartInterface.h KoChartModel.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel)

endif()
