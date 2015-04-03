include_directories(${CMAKE_SOURCE_DIR}/sheets ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${EIGEN3_INCLUDE_DIR})


########### next target ###############

add_library(kspreadbitopsmodule MODULE BitOpsModule.h bitops.cpp)
kcoreaddons_desktop_to_json(kspreadbitopsmodule kspreadbitopsmodule.desktop)
target_link_libraries(kspreadbitopsmodule calligrasheetscommon)
install(TARGETS kspreadbitopsmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES bitops.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadconversionmodule MODULE ConversionModule.h conversion.cpp)
kcoreaddons_desktop_to_json(kspreadconversionmodule kspreadconversionmodule.desktop)
target_link_libraries(kspreadconversionmodule calligrasheetscommon)
install(TARGETS kspreadconversionmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES conversion.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreaddatabasemodule MODULE DatabaseModule.h database.cpp)
kcoreaddons_desktop_to_json(kspreaddatabasemodule kspreaddatabasemodule.desktop)
target_link_libraries(kspreaddatabasemodule calligrasheetscommon)
install(TARGETS kspreaddatabasemodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES database.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreaddatetimemodule MODULE DateTimeModule.h datetime.cpp)
kcoreaddons_desktop_to_json(kspreaddatetimemodule kspreaddatetimemodule.desktop)
target_link_libraries(kspreaddatetimemodule calligrasheetscommon)
install(TARGETS kspreaddatetimemodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES datetime.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadengineeringmodule MODULE EngineeringModule.h engineering.cpp)
kcoreaddons_desktop_to_json(kspreadengineeringmodule kspreadengineeringmodule.desktop)
target_link_libraries(kspreadengineeringmodule calligrasheetscommon)
install(TARGETS kspreadengineeringmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES engineering.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadfinancialmodule MODULE FinancialModule.h financial.cpp)
kcoreaddons_desktop_to_json(kspreadfinancialmodule kspreadfinancialmodule.desktop)
target_link_libraries(kspreadfinancialmodule calligrasheetscommon)
install(TARGETS kspreadfinancialmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES financial.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadinformationmodule MODULE InformationModule.h information.cpp)
kcoreaddons_desktop_to_json(kspreadinformationmodule kspreadinformationmodule.desktop)
target_link_libraries(kspreadinformationmodule calligrasheetscommon)
install(TARGETS kspreadinformationmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES information.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadlogicmodule MODULE LogicModule.h logic.cpp)
kcoreaddons_desktop_to_json(kspreadlogicmodule kspreadlogicmodule.desktop)
target_link_libraries(kspreadlogicmodule calligrasheetscommon)
install(TARGETS kspreadlogicmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES logic.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadmathmodule MODULE MathModule.h math.cpp)
kcoreaddons_desktop_to_json(kspreadmathmodule kspreadmathmodule.desktop)
target_link_libraries(kspreadmathmodule calligrasheetscommon)
install(TARGETS kspreadmathmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES math.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadreferencemodule MODULE ReferenceModule.h reference.cpp)
kcoreaddons_desktop_to_json(kspreadreferencemodule kspreadreferencemodule.desktop)
target_link_libraries(kspreadreferencemodule calligrasheetscommon)
install(TARGETS kspreadreferencemodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES reference.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadstatisticalmodule MODULE StatisticalModule.h statistical.cpp)
kcoreaddons_desktop_to_json(kspreadstatisticalmodule kspreadstatisticalmodule.desktop)
target_link_libraries(kspreadstatisticalmodule calligrasheetscommon)
install(TARGETS kspreadstatisticalmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES statistical.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadtextmodule MODULE TextModule.h text.cpp)
kcoreaddons_desktop_to_json(kspreadtextmodule kspreadtextmodule.desktop)
target_link_libraries(kspreadtextmodule calligrasheetscommon)
install(TARGETS kspreadtextmodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES text.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)

########### next target ###############

add_library(kspreadtrigonometrymodule MODULE TrigonometryModule.h trig.cpp)
kcoreaddons_desktop_to_json(kspreadtrigonometrymodule kspreadtrigonometrymodule.desktop)
target_link_libraries(kspreadtrigonometrymodule calligrasheetscommon)
install(TARGETS kspreadtrigonometrymodule DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES trig.xml DESTINATION ${DATA_INSTALL_DIR}/sheets/functions)
