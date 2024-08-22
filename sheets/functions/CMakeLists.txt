########### next target ###############

add_library(kspreadbitopsmodule MODULE BitOpsModule.h bitops.cpp)
target_include_directories( kspreadbitopsmodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadbitopsmodule calligrasheetsengine)
install(TARGETS kspreadbitopsmodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES bitops.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadconversionmodule MODULE ConversionModule.h conversion.cpp)
target_include_directories( kspreadconversionmodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadconversionmodule calligrasheetsengine)
install(TARGETS kspreadconversionmodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES conversion.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreaddatabasemodule MODULE DatabaseModule.h database.cpp)
target_include_directories( kspreaddatabasemodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreaddatabasemodule calligrasheetsengine)
install(TARGETS kspreaddatabasemodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES database.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreaddatetimemodule MODULE DateTimeModule.h datetime.cpp)
target_include_directories( kspreaddatetimemodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreaddatetimemodule calligrasheetsengine)
install(TARGETS kspreaddatetimemodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES datetime.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadengineeringmodule MODULE EngineeringModule.h engineering.cpp)
target_include_directories( kspreadengineeringmodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadengineeringmodule calligrasheetsengine)
install(TARGETS kspreadengineeringmodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES engineering.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadfinancialmodule MODULE FinancialModule.h financial.cpp)
target_include_directories( kspreadfinancialmodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadfinancialmodule calligrasheetsengine)
install(TARGETS kspreadfinancialmodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES financial.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadinformationmodule MODULE InformationModule.h information.cpp)
target_include_directories( kspreadinformationmodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadinformationmodule calligrasheetsengine)
install(TARGETS kspreadinformationmodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES information.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadlogicmodule MODULE LogicModule.h logic.cpp)
target_include_directories( kspreadlogicmodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadlogicmodule calligrasheetsengine)
install(TARGETS kspreadlogicmodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES logic.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadmathmodule MODULE MathModule.h math.cpp)
target_include_directories( kspreadmathmodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadmathmodule calligrasheetsengine Eigen3::Eigen)
install(TARGETS kspreadmathmodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES math.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadreferencemodule MODULE ReferenceModule.h reference.cpp)
target_include_directories( kspreadreferencemodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadreferencemodule calligrasheetsengine)
install(TARGETS kspreadreferencemodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES reference.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadstatisticalmodule MODULE StatisticalModule.h statistical.cpp)
target_include_directories( kspreadstatisticalmodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadstatisticalmodule calligrasheetsengine)
install(TARGETS kspreadstatisticalmodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES statistical.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadtextmodule MODULE TextModule.h text.cpp)
target_include_directories( kspreadtextmodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadtextmodule calligrasheetsengine)
install(TARGETS kspreadtextmodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES text.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

########### next target ###############

add_library(kspreadtrigonometrymodule MODULE TrigonometryModule.h trig.cpp)
target_include_directories( kspreadtrigonometrymodule PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ )
target_link_libraries(kspreadtrigonometrymodule calligrasheetsengine)
install(TARGETS kspreadtrigonometrymodule DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrasheets/functions)
install(FILES trig.xml DESTINATION ${KDE_INSTALL_DATADIR}/calligrasheets/functions)

