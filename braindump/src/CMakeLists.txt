
include_directories ( ${CMAKE_CURRENT_BINARY_DIR} ${KOTEXT_INCLUDES} ${KOMAIN_INCLUDES} )

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

ki18n_wrap_ui(braindump_SRCS
  forms/wdgsectionsbox.ui
    forms/wdgsectionproperties.ui
)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../data/icons/*-apps-braindump.png")
ecm_add_app_icon( braindump_SRCS ICONS ${ICONS_SRCS})

add_executable(braindump ${braindump_SRCS})

target_link_libraries(braindump braindumpcore komain kowidgets kotext flake koplugin KF6::IconThemes)

install(TARGETS braindump ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
