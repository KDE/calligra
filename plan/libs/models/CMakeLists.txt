
add_subdirectory( tests )

include_directories(
    ../kernel 
    ${CMAKE_SOURCE_DIR}/libs/widgetutils
    ${KOODF_INCLUDES}
    ${KUNDO2_INCLUDES}
    ${KDCHART_INCLUDES}
    
    ${KDEPIMLIBS_INCLUDE_DIR}
)

########### KPlato data models library ###############

set(kplatomodels_LIB_SRCS
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

target_link_libraries(kplatomodels kplatokernel koodf ${KDCHART_LIBS} KGantt  KF5::KIOWidgets Qt5::Core Qt5::Gui ${KDE4_KABC_LIBS})
target_link_libraries(kplatomodels kundo2)

set_target_properties(kplatomodels PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS kplatomodels ${INSTALL_TARGETS_DEFAULT_ARGS})

