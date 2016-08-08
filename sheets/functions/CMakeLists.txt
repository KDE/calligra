include_directories(${CMAKE_SOURCE_DIR}/sheets ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${EIGEN3_INCLUDE_DIR})


########### next target ###############

add_library(kspreadbitopsmodule MODULE BitOpsModule.h bitops.cpp)
calligrasheets_plugin_desktop_to_json(kspreadbitopsmodule kspreadbitopsmodule.desktop)
target_link_libraries(kspreadbitopsmodule calligrasheetscommon)
install(TARGETS kspreadbitopsmodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES bitops.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadconversionmodule MODULE ConversionModule.h conversion.cpp)
calligrasheets_plugin_desktop_to_json(kspreadconversionmodule kspreadconversionmodule.desktop)
target_link_libraries(kspreadconversionmodule calligrasheetscommon)
install(TARGETS kspreadconversionmodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES conversion.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreaddatabasemodule MODULE DatabaseModule.h database.cpp)
calligrasheets_plugin_desktop_to_json(kspreaddatabasemodule kspreaddatabasemodule.desktop)
target_link_libraries(kspreaddatabasemodule calligrasheetscommon)
install(TARGETS kspreaddatabasemodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES database.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreaddatetimemodule MODULE DateTimeModule.h datetime.cpp)
calligrasheets_plugin_desktop_to_json(kspreaddatetimemodule kspreaddatetimemodule.desktop)
target_link_libraries(kspreaddatetimemodule calligrasheetscommon)
install(TARGETS kspreaddatetimemodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES datetime.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadengineeringmodule MODULE EngineeringModule.h engineering.cpp)
calligrasheets_plugin_desktop_to_json(kspreadengineeringmodule kspreadengineeringmodule.desktop)
target_link_libraries(kspreadengineeringmodule calligrasheetscommon)
install(TARGETS kspreadengineeringmodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES engineering.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadfinancialmodule MODULE FinancialModule.h financial.cpp)
calligrasheets_plugin_desktop_to_json(kspreadfinancialmodule kspreadfinancialmodule.desktop)
target_link_libraries(kspreadfinancialmodule calligrasheetscommon)
install(TARGETS kspreadfinancialmodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES financial.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadinformationmodule MODULE InformationModule.h information.cpp)
calligrasheets_plugin_desktop_to_json(kspreadinformationmodule kspreadinformationmodule.desktop)
target_link_libraries(kspreadinformationmodule calligrasheetscommon)
install(TARGETS kspreadinformationmodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES information.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadlogicmodule MODULE LogicModule.h logic.cpp)
calligrasheets_plugin_desktop_to_json(kspreadlogicmodule kspreadlogicmodule.desktop)
target_link_libraries(kspreadlogicmodule calligrasheetscommon)
install(TARGETS kspreadlogicmodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES logic.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadmathmodule MODULE MathModule.h math.cpp)
calligrasheets_plugin_desktop_to_json(kspreadmathmodule kspreadmathmodule.desktop)
target_link_libraries(kspreadmathmodule calligrasheetscommon)
install(TARGETS kspreadmathmodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES math.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadreferencemodule MODULE ReferenceModule.h reference.cpp)
calligrasheets_plugin_desktop_to_json(kspreadreferencemodule kspreadreferencemodule.desktop)
target_link_libraries(kspreadreferencemodule calligrasheetscommon)
install(TARGETS kspreadreferencemodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES reference.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadstatisticalmodule MODULE StatisticalModule.h statistical.cpp)
calligrasheets_plugin_desktop_to_json(kspreadstatisticalmodule kspreadstatisticalmodule.desktop)
target_link_libraries(kspreadstatisticalmodule calligrasheetscommon)
install(TARGETS kspreadstatisticalmodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES statistical.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadtextmodule MODULE TextModule.h text.cpp)
calligrasheets_plugin_desktop_to_json(kspreadtextmodule kspreadtextmodule.desktop)
target_link_libraries(kspreadtextmodule calligrasheetscommon)
install(TARGETS kspreadtextmodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES text.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadtrigonometrymodule MODULE TrigonometryModule.h trig.cpp)
calligrasheets_plugin_desktop_to_json(kspreadtrigonometrymodule kspreadtrigonometrymodule.desktop)
target_link_libraries(kspreadtrigonometrymodule calligrasheetscommon)
install(TARGETS kspreadtrigonometrymodule DESTINATION ${PLUGIN_INSTALL_DIR}/calligrasheets/functions)
install(FILES trig.xml DESTINATION ${DATA_INSTALL_DIR}/calligrasheets/functions)
