
include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${KOTEXT_INCLUDES} ${KOMAIN_INCLUDES} )

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
  SectionPropertiesDock.cpp
  SectionShapeContainerModel.cpp
  SectionsIO.cpp
  TreeSortFilter.cpp
  Utils.cpp
  View.cpp
  ViewManager.cpp
  # Layouts
  layouts/ColumnLayout.cpp
  layouts/FreeLayout.cpp
  # Commands
  commands/ChangeLayoutCommand.cpp
  commands/MoveSectionCommand.cpp
  commands/InsertSectionCommand.cpp
  commands/RememberPositionCommand.cpp
  commands/RemoveSectionCommand.cpp
  commands/RenameSectionCommand.cpp
  # Imported file
  import/DockerManager.cpp
  import/ToolDocker.cpp
)

kde4_add_ui_files(braindump_SRCS
  forms/wdgsectionsbox.ui
    forms/wdgsectionproperties.ui
)

kde4_add_app_icon( braindump_SRCS "../data/icons/hi*-app-braindump.png" )

kde4_add_executable(braindump ${braindump_SRCS})

target_link_libraries(braindump ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} flake kotext kowidgets braindumpcore komain)

install(TARGETS braindump ${INSTALL_TARGETS_DEFAULT_ARGS})
