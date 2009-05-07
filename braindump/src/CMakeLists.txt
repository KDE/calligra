
include_directories (${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${KOFFICE_INCLUDE_DIR} )

### braindump ###
set(braindump_SRCS
  main.cpp
  BrainDumpDocument.cpp
  BrainDumpView.cpp
)

kde4_add_executable(braindump ${braindump_SRCS})

target_link_libraries(braindump komain ${KDE4_KDEUI_LIBS} ${QT_QT3SUPPORT_LIBRARY} kopageapp ${KDE4_KPARTS_LIBS} ${KDE4_KDEUI_LIBS} )

install(TARGETS braindump ${INSTALL_TARGETS_DEFAULT_ARGS})

### GUI files ###
install( FILES braindump.rc DESTINATION ${DATA_INSTALL_DIR}/braindump)

### desktop files ###
install( FILES braindump.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
