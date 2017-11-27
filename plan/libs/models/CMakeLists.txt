
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()

include_directories(
    ${PLANKERNEL_INCLUDES}
    ${PLANWIDGETS_INCLUDES}
    ${PLANODF_INCLUDES}
    ${PLANKUNDO2_INCLUDES}
)

if (KF5Contacts_FOUND)
    add_definitions(-DPLAN_KCONTACTS_FOUND)
endif ()

########### KPlato data models library ###############

set(kplatomodels_LIB_SRCS
    reportgenerator/ReportGenerator.cpp
    reportgenerator/ReportGeneratorOdt.cpp

    kptcommonstrings.cpp
    kpttreecombobox.cpp

    kcalendar/kdatetable.cpp
    kcalendar/kdatepicker.cpp
    
    kptnodechartmodel.cpp
    
    kptflatproxymodel.cpp

    kptrelationmodel.cpp
    kptworkpackagemodel.cpp
    kptdocumentmodel.cpp
    kptitemmodelbase.cpp
    kptnodeitemmodel.cpp
    kptdurationspinbox.cpp
    kpttaskstatusmodel.cpp
    kptresourcemodel.cpp
    kptcalendarmodel.cpp
    kptschedulemodel.cpp
    kptaccountsmodel.cpp
    kptpertcpmmodel.cpp
    kptresourceappointmentsmodel.cpp
    kptresourceallocationmodel.cpp

    kpttaskcompletedelegate.cpp
)

add_library(kplatomodels SHARED ${kplatomodels_LIB_SRCS})
generate_export_header(kplatomodels)

target_link_libraries(kplatomodels
    PUBLIC
        kplatokernel
        planodf
        KChart
        KGantt
        KF5::KIOWidgets
    PRIVATE
        KF5::Notifications
        KF5::TextWidgets
)
if(KF5Contacts_FOUND)
    target_link_libraries(kplatomodels  PRIVATE KF5::Contacts)
endif()

set_target_properties(kplatomodels PROPERTIES VERSION ${GENERIC_PLAN_LIB_VERSION} SOVERSION ${GENERIC_PLAN_LIB_SOVERSION} )

install(TARGETS kplatomodels ${INSTALL_TARGETS_DEFAULT_ARGS})

