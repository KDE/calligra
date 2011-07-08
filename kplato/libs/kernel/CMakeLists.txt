
add_subdirectory( tests )

include_directories(${KOODF_INCLUDES}
                   ${KUNDO2_INCLUDES})


########### KPlato kernel library ###############

set(kplatokernel_LIB_SRCS
    kptglobal.cpp
    kpteffortcostmap.cpp
    kptdocuments.cpp
    kptaccount.cpp
    kptappointment.cpp
    kptnode.cpp
    kptproject.cpp
    kptrelation.cpp
    kptresource.cpp
    kpttask.cpp
    kptduration.cpp
    kptdatetime.cpp
    kptcalendar.cpp
    kptschedule.cpp
    kptwbsdefinition.cpp
    kptcommand.cpp

    kptschedulerplugin.cpp

    kptconfigbase.cpp

    KPlatoXmlLoaderBase.cpp
)

kde4_add_library(kplatokernel SHARED ${kplatokernel_LIB_SRCS})

target_link_libraries(kplatokernel koodf ${QT_QTGUI_LIBRARY})
target_link_libraries(kplatokernel kundo2)

set_target_properties(kplatokernel PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )

install(TARGETS kplatokernel ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES kplato_schedulerplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )
