if (SHOULD_BUILD_DEVEL_HEADERS)

install(FILES KoGenericRegistry.h KoID.h KoChartInterface.h KoChartModel.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/calligra COMPONENT Devel)

endif()
