include_directories(
    ${KREPORT_INCLUDES}
    ${KPROPERTY_INCLUDES}
)

ecm_create_qm_loader(planreport_textplugin_QM_LOADER planreport_textplugin_qt)

#build a shared library
set(planreport_textplugin_LIB_SRCS
    PlanReportDesignerItemText.cpp
    PlanReportItemText.cpp
    PlanReportTextPlugin.cpp
    PlanReportScriptText.cpp
    ${planreport_textplugin_QM_LOADER}
)

add_library(planreport_textplugin MODULE ${planreport_textplugin_LIB_SRCS})

target_link_libraries(planreport_textplugin
    PUBLIC
        KReport
)

########### install files ###############

install(TARGETS planreport_textplugin DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})

#add_subdirectory(pics)
