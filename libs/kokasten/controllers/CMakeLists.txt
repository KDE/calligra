project( kastencontrollers )

add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1805 )

include_directories(
  ../core
  ../core/entity
  ../core/document
  ../core/io
  ../core/io/filesystem
  ../core/system
  ../gui
  ../gui/view
  ../gui/io
  ../gui/system
  ../gui/shell
  ../gui/controller
)


set( SYNCHRONIZECONTROLLER_SRCS
  io/synchronize/synchronizecontroller.cpp
)
set( SETREMOTECONTROLLER_SRCS
  io/setremote/setremotecontroller.cpp
)
set( CREATORCONTROLLER_SRCS
  documentsystem/creator/creatorcontroller.cpp
)
set( LOADERCONTROLLER_SRCS
  documentsystem/loader/loadercontroller.cpp
)
set( CLOSECONTROLLER_SRCS
  documentsystem/close/closecontroller.cpp
)
set( FILESYSTEMBROWSERCONTROLLER_SRCS
  documentsystem/filesystembrowser/filesystembrowsertool.cpp
  documentsystem/filesystembrowser/filesystembrowserview.cpp
  documentsystem/filesystembrowser/filesystembrowsertoolview.cpp
)
set( DOCUMENTSBROWSERCONTROLLER_SRCS
  documentsystem/documentsbrowser/documentlistmodel.cpp
  documentsystem/documentsbrowser/documentstool.cpp
  documentsystem/documentsbrowser/documentsview.cpp
  documentsystem/documentsbrowser/documentstoolview.cpp
)
set( TERMINALCONTROLLER_SRCS
  document/terminal/terminaltool.cpp
  document/terminal/terminalview.cpp
  document/terminal/terminaltoolview.cpp
)
set( MODIFIEDBARCONTROLLER_SRCS
  document/modified/modifiedbarcontroller.cpp
)
set( READONLYCONTROLLER_SRCS
  document/readonly/readonlycontroller.cpp
)
set( READONLYBARCONTROLLER_SRCS
  document/readonly/readonlybarcontroller.cpp
)
if(CMAKE_BUILD_TYPE_TOLOWER MATCHES release)
set( VERSIONVIEWCONTROLLER_SRCS )
else(CMAKE_BUILD_TYPE_TOLOWER MATCHES release)
set( VERSIONVIEWCONTROLLER_SRCS
  document/versionview/versiontablemodel.cpp
  document/versionview/versionview.cpp
  document/versionview/versionviewtool.cpp
  document/versionview/versionviewtoolview.cpp
)
endif(CMAKE_BUILD_TYPE_TOLOWER MATCHES release)
set( VERSIONCONTROLLER_SRCS
  view/version/versioncontroller.cpp
)
set( ZOOMCONTROLLER_SRCS
  view/zoom/zoomcontroller.cpp
)
set( ZOOMBARCONTROLLER_SRCS
  view/zoom/zoomslider.cpp
  view/zoom/zoombarcontroller.cpp
)
set( SELECTCONTROLLER_SRCS
  view/select/selectcontroller.cpp
)
set( CLIPBOARDCONTROLLER_SRCS
  io/clipboard/clipboardcontroller.cpp
)
set( insertcontroller_SRCS
  io/insert/insertdialog.cpp
  io/insert/insertcontroller.cpp
)
set( COPYASCONTROLLER_SRCS
  io/copyas/copyasdialog.cpp
  io/copyas/copyascontroller.cpp
)
set( EXPORTCONTROLLER_SRCS
  io/export/exportdialog.cpp
  io/export/exportcontroller.cpp
)
# set( CLOSECONTROLLER_SRCS
#   viewsystem/close/closecontroller.cpp
# )
set( SWITCHVIEWCONTROLLER_SRCS
  shellwindow/switchview/switchviewcontroller.cpp
)
set( VIEWLISTMENUCONTROLLER_SRCS
  shellwindow/viewlistmenu/viewlistmenucontroller.cpp
)
set( VIEWAREASPLITCONTROLLER_SRCS
  shellwindow/viewareasplit/viewareasplitcontroller.cpp
)
set( TOOLLISTMENUCONTROLLER_SRCS
  shellwindow/toollistmenu/toollistmenucontroller.cpp
)
set( FULLSCREENCONTROLLER_SRCS
  shellwindow/fullscreen/fullscreencontroller.cpp
)
set( QUITCONTROLLER_SRCS
  program/quit/quitcontroller.cpp
)

set( kastencontroller_LIB_SRCS
  ${SETREMOTECONTROLLER_SRCS}
  ${CLOSECONTROLLER_SRCS}
  ${FILESYSTEMBROWSERCONTROLLER_SRCS}
  ${DOCUMENTSBROWSERCONTROLLER_SRCS}
  ${TERMINALCONTROLLER_SRCS}
  ${VERSIONCONTROLLER_SRCS}
  ${VERSIONVIEWCONTROLLER_SRCS}
  ${MODIFIEDBARCONTROLLER_SRCS}
  ${READONLYCONTROLLER_SRCS}
  ${READONLYBARCONTROLLER_SRCS}
  ${SWITCHVIEWCONTROLLER_SRCS}
  ${VIEWLISTMENUCONTROLLER_SRCS}
  ${VIEWAREASPLITCONTROLLER_SRCS}
  ${TOOLLISTMENUCONTROLLER_SRCS}
  ${FULLSCREENCONTROLLER_SRCS}
  ${ZOOMCONTROLLER_SRCS}
  ${ZOOMBARCONTROLLER_SRCS}
  ${CREATORCONTROLLER_SRCS}
  ${LOADERCONTROLLER_SRCS}
  ${SYNCHRONIZECONTROLLER_SRCS}
  ${SELECTCONTROLLER_SRCS}
  ${CLIPBOARDCONTROLLER_SRCS}
  ${insertcontroller_SRCS}
  ${COPYASCONTROLLER_SRCS}
  ${EXPORTCONTROLLER_SRCS}
  ${QUITCONTROLLER_SRCS}
)

set( kastencontrollers_LIB_HDRS
  kastencontrollers_export.h
  document/modified/modifiedbarcontroller.h
  document/readonly/readonlybarcontroller.h
  document/readonly/readonlycontroller.h
  document/versionview/versionviewtool.h
  document/versionview/versionviewtoolview.h
  documentsystem/close/closecontroller.h
  documentsystem/documentsbrowser/documentstoolview.h
  documentsystem/documentsbrowser/documentstool.h
  documentsystem/creator/creatorcontroller.h
  documentsystem/loader/loadercontroller.h
  documentsystem/filesystembrowser/filesystembrowsertoolview.h
  documentsystem/filesystembrowser/filesystembrowsertool.h
  io/export/exportcontroller.h
  io/insert/insertcontroller.h
  io/setremote/setremotecontroller.h
  io/copyas/copyascontroller.h
  io/clipboard/clipboardcontroller.h
  io/synchronize/synchronizecontroller.h
  view/select/selectcontroller.h
  view/zoom/zoomcontroller.h
  view/zoom/zoombarcontroller.h
  view/version/versioncontroller.h
  shellwindow/fullscreen/fullscreencontroller.h
  shellwindow/toollistmenu/toollistmenucontroller.h
  shellwindow/switchview/switchviewcontroller.h
  shellwindow/viewlistmenu/viewlistmenucontroller.h
  shellwindow/viewareasplit/viewareasplitcontroller.h
  program/quit/quitcontroller.h
)

kde4_add_library( ${kastencontrollers_LIB} SHARED ${kastencontroller_LIB_SRCS} )
target_link_libraries( ${kastencontrollers_LIB}
  ${kastengui_LIB}
  ${kastencore_LIB}
  ${KDE4_KPARTS_LIBS}
  ${KDE4_KFILE_LIBS}
)
set_target_properties( ${kastencontrollers_LIB}  PROPERTIES
  OUTPUT_NAME ${kastencontrollers_LIB_NAME}
  VERSION     ${KASTEN_LIB_VERSION}
  SOVERSION   ${KASTEN_LIB_SOVERSION}
)

install( TARGETS ${kastencontrollers_LIB}  ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES ${kastencontrollers_LIB_HDRS}
  DESTINATION ${KASTEN_INCLUDE_INSTALL_DIR}
  COMPONENT   Devel
)
