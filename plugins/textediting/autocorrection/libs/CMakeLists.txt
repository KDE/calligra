
set(autocorrection_LIB_SRCS
    AutoCorrectionDebug.cpp
    autocorrection.cpp
    autocorrectionwidget.cpp
    autocorrectionlistwidget.cpp
    autocorrectiontreewidget.cpp
    autocorrectionlanguage.cpp
    autocorrectiondialog.cpp
    import/importlibreofficeautocorrection.cpp
    import/importkmailautocorrection.cpp
    import/importabstractautocorrection.cpp
    widgets/lineeditwithautocorrection.cpp
    widgets/richtexteditwithautocorrection.cpp
    widgets/selectspecialchardialog.cpp
    widgets/spellchecklineedit.cpp
)

ki18n_wrap_ui(autocorrection_LIB_SRCS
    ui/autocorrectionwidget.ui
    ui/autocorrectiondialog.ui
)

add_library(autocorrection SHARED ${autocorrection_LIB_SRCS})
generate_export_header(autocorrection)

target_link_libraries(autocorrection
    PRIVATE
    KF5::TextWidgets
    KF5::ConfigWidgets
    KF5::I18n
    KF5::Archive
    Qt::Gui
)

set_target_properties(autocorrection PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)

install(TARGETS autocorrection  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if(BUILD_TESTING)
    add_subdirectory(autotests)
    target_compile_definitions(autocorrection PRIVATE BUILD_TESTING)
endif()
