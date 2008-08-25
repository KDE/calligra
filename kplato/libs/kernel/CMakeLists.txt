
add_subdirectory( tests )

include_directories(${KOSTORE_INCLUDES})

########### KPlato kernel library ###############

set(kplatokernel_LIB_SRCS
    kptglobal.cpp
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

)

kde4_add_library(kplatokernel SHARED ${kplatokernel_LIB_SRCS})

target_link_libraries(kplatokernel kstore ${QT_QTGUI_LIBRARY})

set_target_properties(kplatokernel PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatokernel DESTINATION ${LIB_INSTALL_DIR})

