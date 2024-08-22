if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

set(kowidgetutils_LIB_SRCS
    KoGroupButton.cpp
    KoProgressBar.cpp
    KoProgressUpdater.cpp
    KoUpdater.cpp
    KoUpdaterPrivate_p.cpp
    KoProperties.cpp
    KoFileDialog.cpp
    KoKineticScroller.cpp
)


add_library(kowidgetutils SHARED ${kowidgetutils_LIB_SRCS})
generate_export_header(kowidgetutils BASE_NAME kowidgetutils)

target_link_libraries(kowidgetutils
    PUBLIC
        Qt6::Widgets
        Qt6::Gui
        Qt6::Xml
        Qt6::Core
    PRIVATE
        KF6::I18n
        KF6::ConfigCore
)

set_target_properties(kowidgetutils
    PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)

install(TARGETS kowidgetutils ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if (SHOULD_BUILD_DEVEL_HEADERS)

install(FILES
    KoGroupButton.h
    KoProgressBar.h
    KoProgressProxy.h
    KoProgressUpdater.h
    KoUpdater.h
    KoProperties.h
    KoFileDialog.h
    KoKineticScroller.h
    ${CMAKE_CURRENT_BINARY_DIR}/kowidgetutils_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra
    COMPONENT Devel
)

endif()
