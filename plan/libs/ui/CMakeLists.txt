

include_directories(
    ${CMAKE_SOURCE_DIR}/plan/libs/kernel
    ${CMAKE_SOURCE_DIR}/plan/libs/models
    ${CMAKE_BINARY_DIR}/plan/libs/models
    ${CMAKE_BINARY_DIR}/plan/libs/ui

    ${KOMAIN_INCLUDES}
    ${KDEPIMLIBS_INCLUDE_DIR}
)

#add_subdirectory( tests )

########### KPlato private library ###############

if (PLAN_BUILD_REPORTS)
    message(STATUS "-- Building plan with reports capability")
    add_subdirectory(reports/items)

    set(planreports_LIB_SRC
        reports/reportview.cpp
        reports/reportdata.cpp
        reports/reportsourceeditor.cpp
        reports/reportscripts.cpp
    )
    set(planreports_ui_LIB_SRCS
        reports/reportsourceeditor.ui
        reports/reportnavigator.ui
        reports/reportsectionswidget.ui
        reports/reportgroupsectionswidget.ui
        reports/reporttoolswidget.ui
    )

endif()

set(kplatoui_LIB_SRCS
    ${planreports_LIB_SRC}

    WhatsThis.cpp

    TasksEditController.cpp
    TasksEditDialog.cpp

    welcome/WelcomeView.cpp

    reportsgenerator/ReportsGeneratorView.cpp

    kptganttitemdelegate.cpp

    kptworkpackagesendpanel.cpp
    kptworkpackagesenddialog.cpp
    
    kptdocumentseditor.cpp
    kptdocumentspanel.cpp
    
    kptitemviewsettup.cpp
    kptsplitterview.cpp
    kptrelationeditor.cpp
    kptdependencyeditor.cpp
    kptusedefforteditor.cpp
    kpttaskstatusview.cpp
    kptcalendareditor.cpp
    kptviewbase.cpp
    kptaccountseditor.cpp
    kptperteditor.cpp
    kptpertresult.cpp
    kpttaskeditor.cpp
    kptresourceeditor.cpp
    kptscheduleeditor.cpp
    kptsummarytaskdialog.cpp
    kptsummarytaskgeneralpanel.cpp
    kptresourceappointmentsview.cpp
    kptaccountsviewconfigdialog.cpp
    kptaccountsview.cpp
    kpttaskcostpanel.cpp
    kptmilestoneprogresspanel.cpp
    kptmilestoneprogressdialog.cpp
    kpttaskdialog.cpp
    kptmainprojectdialog.cpp
    kptmainprojectpanel.cpp
    kptganttview.cpp
    kptrelationdialog.cpp
    kptrequestresourcespanel.cpp
    kptresourcedialog.cpp
    kptstandardworktimedialog.cpp
    kptintervaledit.cpp
    kpttaskgeneralpanel.cpp
    kpttaskprogresspanel.cpp
    kpttaskprogressdialog.cpp
    kpttaskdescriptiondialog.cpp
    kptwbsdefinitiondialog.cpp
    kptwbsdefinitionpanel.cpp
    kptresourceassignmentview.cpp
    kptperformancetablewidget.cpp
    kptresourceallocationeditor.cpp
    kptworkpackagemergedialog.cpp
    kptrecalculatedialog.cpp

    kpthtmlview.cpp

    locale/localemon.cpp
    kptlocaleconfigmoneydialog.cpp

    ResourceAllocationView.cpp
)

ki18n_wrap_ui(kplatoui_LIB_SRCS
    ${planreports_ui_LIB_SRCS}

    welcome/WelcomeView.ui

    kptresourceappointmentsdisplayoptions.ui
    kptganttchartdisplayoptions.ui

    kptprintingheaderfooter.ui
    kptganttprintingoptions.ui
    
    kptworkpackagesendpanel.ui
    
    kptdocumentspanel.ui
    
    kptperformancestatus.ui
    
    kptcpmwidget.ui
    kptitemviewsettings.ui
    kptpertresult.ui
    standardworktimedialogbase.ui
    kptwbsdefinitionpanelbase.ui
    kptaccountsviewconfigurepanelbase.ui
    kptintervaleditbase.ui
    kpttaskcostpanelbase.ui
    kpttaskdescriptionpanelbase.ui
    kptsummarytaskgeneralpanelbase.ui
    kptmilestoneprogresspanelbase.ui
    resourcedialogbase.ui
    kptmainprojectpanelbase.ui
    relationpanel.ui
    kpttaskgeneralpanelbase.ui
    kpttaskprogresspanelbase.ui
    kptperteditor.ui
    kptresourceassignmentview.ui
    kpttaskstatusviewsettingspanel.ui
    kptperformancestatusviewsettingspanel.ui
    kptworkpackagemergepanel.ui
    kptrecalculatedialog.ui
    kptscheduleeditor.ui

    locale/localemon.ui
)

add_library(kplatoui SHARED ${kplatoui_LIB_SRCS})
generate_export_header(kplatoui)

target_link_libraries(kplatoui
    PUBLIC
        komain
        kplatokernel
        kplatomodels
        KGantt
        KF5::KHtml
    PRIVATE
        KChart
        KF5::ItemViews
        KF5::IconThemes
        KF5::Archive
        Qt5::Sql
)
if (PLAN_BUILD_REPORTS)
    target_link_libraries(kplatoui PUBLIC KReport PRIVATE KPropertyWidgets)
endif()

if(KF5AkonadiContact_FOUND)
    target_link_libraries(kplatoui  PRIVATE KF5::AkonadiContact)
endif()

set_target_properties(kplatoui PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS kplatoui ${INSTALL_TARGETS_DEFAULT_ARGS})

#
