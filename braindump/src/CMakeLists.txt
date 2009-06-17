
include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${KOFFICE_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/import )

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
  modeltest.cpp
  # Layouts
  layouts/ColumnLayout.cpp
  layouts/FreeLayout.cpp
  # Commands
  commands/InsertSectionCommand.cpp
  commands/RemoveSectionCommand.cpp
  commands/RenameSectionCommand.cpp
  # Imported file
  import/DockerManager.cpp
  import/KoDocumentSectionDelegate.cpp
  import/KoDocumentSectionToolTip.cpp
  import/KoDocumentSectionView.cpp
  import/KoItemToolTip.cpp
  import/KoToolBox.cpp
  import/KoToolBoxFactory.cpp
  import/KoZoomAction.cpp
  import/KoZoomController.cpp
  import/KoZoomInput.cpp
)

kde4_add_ui_files(braindump_SRCS
  forms/wdgsectionsbox.ui
    forms/wdgsectionproperties.ui
)

kde4_add_executable(braindump ${braindump_SRCS})

target_link_libraries(braindump ${KDE4_KDEUI_LIBS} ${QT_QT3SUPPORT_LIBRARY} ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} ${FLAKE_LIBRARY} )

install(TARGETS braindump ${INSTALL_TARGETS_DEFAULT_ARGS})

### GUI files ###
install( FILES braindumpui.rc braindumpview.rc DESTINATION ${DATA_INSTALL_DIR}/braindump)

install( FILES import/zoom-width.png import/zoom-page.png import/zoom-pixels.png DESTINATION ${DATA_INSTALL_DIR}/braindump/icons/hicolor/22x22/actions)

### desktop files ###
install( FILES braindump.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
