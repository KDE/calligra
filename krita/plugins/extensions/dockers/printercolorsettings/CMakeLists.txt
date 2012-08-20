
include_directories(${CMPX_INCLUDES})

set(KRITA_PRINTERCOLORSETTINGS_PART_SRCS 
    printercolorsettings.cpp 
    printercolorsettings_dock.cpp
    kis_printer_color_manager.cpp
    kis_printer_profile_selector.cpp
    kis_cmpx.cpp
)

kde4_add_ui_files(KRITA_PRINTERCOLORSETTINGS_PART_SRCS wdgprintcolorsettings.ui wdgprintcolorspaceselector.ui)

kde4_add_plugin(kritaprintercolorsettings ${KRITA_PRINTERCOLORSETTINGS_PART_SRCS})

target_link_libraries(kritaprintercolorsettings kritaui kocmpx)

install(TARGETS kritaprintercolorsettings DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES krita_printercolorsettings.desktop DESTINATION ${SERVICES_INSTALL_DIR})