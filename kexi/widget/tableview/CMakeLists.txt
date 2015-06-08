

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs   )


########### next target ###############

set(kexidatatable_LIB_SRCS
   KexiTableScrollArea.cpp
   KexiTableScrollArea_p.cpp
   KexiTableScrollAreaHeader.cpp
   KexiTableScrollAreaHeaderModel.cpp
   KexiDataTableView.cpp
   KexiDataTableScrollArea.cpp
   KexiTableScrollAreaWidget.cpp
   kexicelleditorfactory.cpp
   kexitableedit.cpp
   kexiinputtableedit.cpp
   kexiblobtableedit.cpp
   kexibooltableedit.cpp
   kexicomboboxbase.cpp
   kexicomboboxtableedit.cpp
   kexicomboboxpopup.cpp
   kexidatetableedit.cpp
   kexitimetableedit.cpp
   kexidatetimetableedit.cpp
   kexitextformatter.cpp
)

add_library(kexidatatable SHARED ${kexidatatable_LIB_SRCS})

generate_export_header(kexidatatable)

target_link_libraries(
    kexidatatable
    kexidataviewcommon
    kexicore
    kexiutils
    kexiguiutils

    KDb
    KProperty

    KF5::KIOWidgets
)

set_target_properties(kexidatatable PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kexidatatable  ${INSTALL_TARGETS_DEFAULT_ARGS})
