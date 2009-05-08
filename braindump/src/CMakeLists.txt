
include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${KOFFICE_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kopageapp )

### braindump ###
set(braindump_SRCS
  main.cpp
  Canvas.cpp
  Document.cpp
  DocumentModel.cpp
  SectionsBoxDock.cpp
  Section.cpp
  SectionGroup.cpp
  TreeSortFilter.cpp
  View.cpp
)

kde4_add_ui_files(braindump_SRCS
  forms/wdgsectionsbox.ui
)

kde4_add_executable(braindump ${braindump_SRCS})

target_link_libraries(braindump ${KOMAIN_LIBRARY} ${KDE4_KDEUI_LIBS} ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} flake koguiutils )

install(TARGETS braindump ${INSTALL_TARGETS_DEFAULT_ARGS})

### GUI files ###
install( FILES braindump.rc DESTINATION ${DATA_INSTALL_DIR}/braindump)

### desktop files ###
install( FILES braindump.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
