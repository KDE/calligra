
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
    KF6::TextWidgets
    KF6::ConfigWidgets
    KF6::I18n
    KF6::Archive
    Qt::Gui
    Qt::Xml
)

set_target_properties(autocorrection PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)

install(TARGETS autocorrection  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if(BUILD_TESTING)
    add_subdirectory(autotests)
    target_compile_definitions(autocorrection PRIVATE BUILD_TESTING)
endif()
