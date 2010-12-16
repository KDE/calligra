

include_directories( ${CMAKE_SOURCE_DIR}/flow/flowpart ${CMAKE_SOURCE_DIR}/flow/flowpart/flowsdk ${CMAKE_SOURCE_DIR}/flow/flowpart/config ${KDE4_INCLUDES}  )


########### next target ###############

set(flowimageexport_PART_SRCS flow_imageexport.cpp flow_imageexportdialog.cpp )


kde4_add_ui3_files(flowimageexport_PART_SRCS flow_imageexportwidget.ui )

kde4_add_plugin(flowimageexport ${flowimageexport_PART_SRCS})



target_link_libraries(flowimageexport  flowcommon )

install(TARGETS flowimageexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  flow_image_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
