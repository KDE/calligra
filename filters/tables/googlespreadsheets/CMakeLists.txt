include_directories(${CMAKE_SOURCE_DIR}/kspread ${KOMAIN_INCLUDES})

########### next target ###############

set(gspreadimport_PART_SRCS gspreadimport.cpp)

kde4_add_plugin(gspreadimport ${gspreadimport_PART_SRCS})

target_link_libraries(gspreadimport kspreadcommon)

install(TARGETS gspreadimport DESTINATION ${PLUGIN_INSTALL_DIR})

########### next target ###############

# set(gspreadexport_PART_SRCS gspreadexport.cpp)
# 
# kde4_add_ui_files(gspreadexport_PART_SRCS exportdialogui.ui )
# 
# kde4_add_plugin(gspreadexport ${gspreadexport_PART_SRCS})
# 
# target_link_libraries(gspreadexport kspreadcommon)
# 
# install(TARGETS gspreadexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kspread_gspread_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})
#install( FILES  kspread_gspread_import.desktop kspread_gspread_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
