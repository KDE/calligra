
set(kowidgetutils_LIB_SRCS
    KoProgressBar.cpp
    KoProgressUpdater.cpp
    KoUpdater.cpp
    KoUpdaterPrivate_p.cpp
    KoFileDialog.cpp
)


add_library(planwidgetutils SHARED ${kowidgetutils_LIB_SRCS})
generate_export_header(planwidgetutils BASE_NAME kowidgetutils)

target_link_libraries(planwidgetutils
    PUBLIC
        Qt5::Widgets
        Qt5::Gui
        Qt5::Xml
        Qt5::Core
    PRIVATE
        KF5::I18n
        KF5::ConfigCore
)

set_target_properties(planwidgetutils
    PROPERTIES VERSION ${GENERIC_PLAN_LIB_VERSION} SOVERSION ${GENERIC_PLAN_LIB_SOVERSION}
)

install(TARGETS planwidgetutils ${INSTALL_TARGETS_DEFAULT_ARGS})
