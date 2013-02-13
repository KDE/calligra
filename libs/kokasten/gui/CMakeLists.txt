project( kastengui )

add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1804 )

include_directories(
  ../core
  ../core/document
  ../core/io
  ../core/system
  view
  io
  system
  shell
  controller
)


set( kastengui_view_SRCS
  view/abstractview.cpp
  view/abstractselectionview.cpp
  view/abstracttoolinlineview.cpp
  view/abstracttoolview.cpp
)
set( kastengui_view_HDRS
  view/abstractviewfactory.h
  view/selecteddatawriteable.h
  view/abstracttoolinlineview.h
  view/abstractview.h
  view/abstractselectionview.h
  view/abstracttoolview.h
  view/abstracttoolviewfactory.h
  view/dataselectable.h
  view/zoomable.h
)

set( kastengui_io_configeditor_SRCS
  io/filesystem/modelencoderfilesystemexporterconfigeditorfactory.cpp
  io/filesystem/modelencoderfilesystemexporterconfigeditor.cpp
  io/abstractmodelstreamencoderconfigeditor.cpp
  io/abstractmodelexporterconfigeditor.cpp
  io/abstractmodeldatageneratorconfigeditor.cpp
)
set( kastengui_io_SRCS
  ${kastengui_io_configeditor_SRCS}
)
set( kastengui_io_HDRS
  io/filesystem/modelencoderfilesystemexporterconfigeditor.h
  io/filesystem/modelencoderfilesystemexporterconfigeditorfactory.h
  io/abstractmodelstreamencoderconfigeditor.h
  io/abstractmodelexporterconfigeditor.h
  io/abstractmodeldatageneratorconfigeditor.h
  io/abstractmodelstreamencoderconfigeditorfactory.h
  io/abstractmodelexporterconfigeditorfactory.h
  io/abstractmodeldatageneratorconfigeditorfactory.h
)


set( kastengui_system_SRCS
  system/abstractdocumentstrategy.cpp
  system/createdialog.cpp
  system/dialoghandler.cpp
  system/dummyview.cpp
  system/modelcodecviewmanager.cpp
  system/multidocumentstrategy_p.cpp
  system/multidocumentstrategy.cpp
  system/singledocumentstrategy_p.cpp
  system/singledocumentstrategy.cpp
  system/viewmanager.cpp
)
set( kastengui_system_HDRS
  system/abstractdocumentstrategy.h
  system/dialoghandler.h
  system/dummyview.h
  system/modelcodecviewmanager.h
  system/multidocumentstrategy.h
  system/singledocumentstrategy.h
  system/viewmanager.h
)

set( kastengui_shell_SRCS
  shell/viewbox.cpp
  shell/toolinlineviewwidget.cpp
  shell/toolviewdockwidget.cpp
  shell/abstractviewarea.cpp
  shell/abstractgroupedviews.cpp
  shell/viewareabox.cpp
  shell/singleviewarea_p.cpp
  shell/singleviewarea.cpp
  shell/tabbedviews_p.cpp
  shell/tabbedviews.cpp
  shell/multiviewareas_p.cpp
  shell/multiviewareas.cpp
  shell/singleviewwindow_p.cpp
  shell/singleviewwindow.cpp
  shell/shellwindow_p.cpp
  shell/shellwindow.cpp
  shell/statusbar.cpp
  shell/statusbarlayout.cpp
)
set( kastengui_shell_HDRS
  shell/shellwindow.h
  shell/abstractviewarea.h
  shell/abstractgroupedviews.h
  shell/statusbar.h
  shell/viewareabox.h
  shell/viewbox.h
  shell/singleviewarea.h
  shell/singleviewwindow.h
  shell/tabbedviews.h
  shell/multiviewareas.h
  shell/toolinlineviewwidget.h
  shell/toolviewdockwidget.h
  shell/statusbarlayout.h
  shell/toolinlineviewable.h
  shell/widgetsdockable.h
  shell/viewareasplitable.h
)

set( kastengui_controller_widgets_SRCS
  controller/togglebutton_p.cpp
  controller/togglebutton.cpp
  controller/abstractlinepopup_p.cpp
  controller/abstractlinepopup.cpp
  controller/abstracttoolwidget_p.cpp
  controller/abstracttoolwidget.cpp
)
set( kastengui_controller_SRCS
  ${kastengui_controller_widgets_SRCS}
  controller/abstractcontroller.cpp
  controller/abstractxmlguicontroller.cpp
)
set( kastengui_controller_HDRS
  controller/abstractcontroller.h
  controller/abstractxmlguicontroller.h
  controller/abstractlinepopup.h
  controller/togglebutton.h
  controller/abstracttoolwidget.h
)

set( kastengui_LIB_SRCS
  ${kastengui_view_SRCS}
  ${kastengui_io_SRCS}
  ${kastengui_system_SRCS}
  ${kastengui_shell_SRCS}
  ${kastengui_controller_SRCS}
)

set( kastengui_LIB_HDRS
  kastengui_export.h
  kastengui.h
  ${kastengui_view_HDRS}
  ${kastengui_io_HDRS}
  ${kastengui_system_HDRS}
  ${kastengui_shell_HDRS}
  ${kastengui_controller_HDRS}
)

kde4_add_library( ${kastengui_LIB} SHARED ${kastengui_LIB_SRCS} )
target_link_libraries( ${kastengui_LIB}
  ${kastencore_LIB}
  ${KDE4_KDEUI_LIBS}
)
set_target_properties( ${kastengui_LIB}  PROPERTIES
  OUTPUT_NAME ${kastengui_LIB_NAME}
  VERSION     ${KASTEN_LIB_VERSION}
  SOVERSION   ${KASTEN_LIB_SOVERSION}
)

install( TARGETS ${kastengui_LIB}  ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES ${kastengui_LIB_HDRS}
  DESTINATION ${KASTEN_INCLUDE_INSTALL_DIR}
  COMPONENT   Devel
)
