

include_directories( ${CMAKE_SOURCE_DIR}/karbon  )


########### next target ###############

set(karbon2image_PART_SRCS ImageExport.cpp ImageExportOptionsWidget.cpp )

ki18n_wrap_ui( karbon2image_PART_SRCS ImageExportOptionsWidget.ui )

add_library(calligra_filter_karbon2image MODULE ${karbon2image_PART_SRCS})

target_link_libraries(calligra_filter_karbon2image karbonui komain)

install(TARGETS calligra_filter_karbon2image  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  calligra_filter_karbon2image.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
