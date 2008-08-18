include_directories(${CMAKE_SOURCE_DIR}/kspread ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${EIGEN2_INCLUDE_DIR})


########### next target ###############

kde4_add_plugin(kspreadbitopsmodule bitops.cpp)
target_link_libraries(kspreadbitopsmodule kspreadcommon)
install(TARGETS kspreadbitopsmodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadbitopsmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES bitops.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadconversionmodule conversion.cpp)
target_link_libraries(kspreadconversionmodule kspreadcommon)
install(TARGETS kspreadconversionmodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadconversionmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES conversion.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreaddatabasemodule database.cpp)
target_link_libraries(kspreaddatabasemodule kspreadcommon)
install(TARGETS kspreaddatabasemodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreaddatabasemodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES database.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreaddatetimemodule datetime.cpp)
target_link_libraries(kspreaddatetimemodule kspreadcommon)
install(TARGETS kspreaddatetimemodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreaddatetimemodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES datetime.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadengineeringmodule engineering.cpp)
target_link_libraries(kspreadengineeringmodule kspreadcommon)
install(TARGETS kspreadengineeringmodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadengineeringmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES engineering.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadfinancialmodule financial.cpp)
target_link_libraries(kspreadfinancialmodule kspreadcommon)
install(TARGETS kspreadfinancialmodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadfinancialmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES financial.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadinformationmodule information.cpp)
target_link_libraries(kspreadinformationmodule kspreadcommon)
install(TARGETS kspreadinformationmodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadinformationmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES information.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadlogicmodule logic.cpp)
target_link_libraries(kspreadlogicmodule kspreadcommon)
install(TARGETS kspreadlogicmodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadlogicmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES logic.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadmathmodule math.cpp)
target_link_libraries(kspreadmathmodule kspreadcommon)
install(TARGETS kspreadmathmodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadmathmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES math.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadreferencemodule reference.cpp)
target_link_libraries(kspreadreferencemodule kspreadcommon)
install(TARGETS kspreadreferencemodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadreferencemodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES reference.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadstatisticalmodule statistical.cpp)
target_link_libraries(kspreadstatisticalmodule kspreadcommon)
install(TARGETS kspreadstatisticalmodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadstatisticalmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES statistical.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadtextmodule text.cpp)
target_link_libraries(kspreadtextmodule kspreadcommon)
install(TARGETS kspreadtextmodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadtextmodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES text.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)

########### next target ###############

kde4_add_plugin(kspreadtrigonometrymodule trig.cpp)
target_link_libraries(kspreadtrigonometrymodule kspreadcommon)
install(TARGETS kspreadtrigonometrymodule DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kspreadtrigonometrymodule.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES trig.xml DESTINATION ${DATA_INSTALL_DIR}/kspread/functions)
