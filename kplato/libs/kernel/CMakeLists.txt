
#add_subdirectory( tests )

include_directories(${KOSTORE_INCLUDES})

########### KPlato kernel library ###############

set(kplatokernel_LIB_SRCS
    kptdocuments.cc
    kptaccount.cc
    kptappointment.cc
    kptnode.cc
    kptproject.cc
    kptrelation.cc
    kptresource.cc
    kpttask.cc
    kptduration.cc
    kptdatetime.cc
    kptcalendar.cc
    kptschedule.cc
    kptwbsdefinition.cc
    kptcommand.cc

)

kde4_add_library(kplatokernel SHARED ${kplatokernel_LIB_SRCS})

target_link_libraries(kplatokernel kstore)

set_target_properties(kplatokernel PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatokernel DESTINATION ${LIB_INSTALL_DIR})

