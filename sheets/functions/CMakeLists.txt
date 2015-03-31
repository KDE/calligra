include_directories(${CMAKE_SOURCE_DIR}/sheets ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${EIGEN3_INCLUDE_DIR})


########### next target ###############

add_library(kspreadbitopsmodule MODULE bitops.cpp)
target_link_libraries(kspreadbitopsmodule calligrasheetscommon)
install(TARGETS kspreadbitopsmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadbitopsmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES bitops.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadconversionmodule MODULE conversion.cpp)
target_link_libraries(kspreadconversionmodule calligrasheetscommon)
install(TARGETS kspreadconversionmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadconversionmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES conversion.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreaddatabasemodule MODULE database.cpp)
target_link_libraries(kspreaddatabasemodule calligrasheetscommon)
install(TARGETS kspreaddatabasemodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreaddatabasemodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES database.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreaddatetimemodule MODULE datetime.cpp)
target_link_libraries(kspreaddatetimemodule calligrasheetscommon)
install(TARGETS kspreaddatetimemodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreaddatetimemodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES datetime.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadengineeringmodule MODULE engineering.cpp)
target_link_libraries(kspreadengineeringmodule calligrasheetscommon)
install(TARGETS kspreadengineeringmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadengineeringmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES engineering.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadfinancialmodule MODULE financial.cpp)
target_link_libraries(kspreadfinancialmodule calligrasheetscommon)
install(TARGETS kspreadfinancialmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadfinancialmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES financial.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadinformationmodule MODULE information.cpp)
target_link_libraries(kspreadinformationmodule calligrasheetscommon)
install(TARGETS kspreadinformationmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadinformationmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES information.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadlogicmodule MODULE logic.cpp)
target_link_libraries(kspreadlogicmodule calligrasheetscommon)
install(TARGETS kspreadlogicmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadlogicmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES logic.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadmathmodule MODULE math.cpp)
target_link_libraries(kspreadmathmodule calligrasheetscommon)
install(TARGETS kspreadmathmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadmathmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES math.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadreferencemodule MODULE reference.cpp)
target_link_libraries(kspreadreferencemodule calligrasheetscommon)
install(TARGETS kspreadreferencemodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadreferencemodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES reference.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadstatisticalmodule MODULE statistical.cpp)
target_link_libraries(kspreadstatisticalmodule calligrasheetscommon)
install(TARGETS kspreadstatisticalmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadstatisticalmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES statistical.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadtextmodule MODULE text.cpp)
target_link_libraries(kspreadtextmodule calligrasheetscommon)
install(TARGETS kspreadtextmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadtextmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES text.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadtrigonometrymodule MODULE trig.cpp)
target_link_libraries(kspreadtrigonometrymodule calligrasheetscommon)
install(TARGETS kspreadtrigonometrymodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kspreadtrigonometrymodule.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
install(FILES trig.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)
