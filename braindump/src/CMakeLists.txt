
include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${KOFFICE_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/kopageapp )

### braindump ###
set(braindump_SRCS
  main.cpp
  Canvas.cpp
  DocumentModel.cpp
  Layout.cpp
  LayoutFactory.cpp
  LayoutFactoryRegistry.cpp
  MainWindow.cpp
  RootSection.cpp
  SectionsBoxDock.cpp
  Section.cpp
  SectionContainer.cpp
  SectionGroup.cpp
  SectionsIO.cpp
  TreeSortFilter.cpp
  View.cpp
  ViewManager.cpp
  modeltest.cpp
  # Layouts
  layouts/FreeLayout.cpp
  # Commands
  commands/InsertSectionCommand.cpp
  commands/RemoveSectionCommand.cpp
  commands/RenameSectionCommand.cpp
  # Imported file
  import/DockerManager.cpp
)

kde4_add_ui_files(braindump_SRCS
  forms/wdgsectionsbox.ui
)

kde4_add_executable(braindump ${braindump_SRCS})

target_link_libraries(braindump ${KOMAIN_LIBRARY} ${KDE4_KDEUI_LIBS} ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} flake koguiutils )

install(TARGETS braindump ${INSTALL_TARGETS_DEFAULT_ARGS})

### GUI files ###
install( FILES braindumpui.rc braindumpview.rc DESTINATION ${DATA_INSTALL_DIR}/braindump)

### desktop files ###
install( FILES braindump.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
