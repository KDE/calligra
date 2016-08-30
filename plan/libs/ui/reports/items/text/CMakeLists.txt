include_directories(
    ${KREPORT_INCLUDES}
    ${KPROPERTY_INCLUDES}
)

ecm_create_qm_loader(planreport_richtextplugin_QM_LOADER planreport_richtextplugin_qt)

#build a shared library
set(planreport_richtextplugin_LIB_SRCS
    PlanReportDesignerItemRichText.cpp
    PlanReportItemRichText.cpp
    PlanReportRichTextPlugin.cpp
    PlanReportScriptRichText.cpp
    ${planreport_richtextplugin_QM_LOADER}
)

add_library(planreport_richtextplugin MODULE ${planreport_richtextplugin_LIB_SRCS})

target_link_libraries(planreport_richtextplugin
    PUBLIC
        KReport
)

########### install files ###############

install(TARGETS planreport_richtextplugin DESTINATION ${KREPORT_PLUGIN_INSTALL_DIR})

#add_subdirectory(pics)
