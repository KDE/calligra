# Configure checks for the caching subdir
include(CheckIncludeFiles)
check_include_files("sys/types.h;sys/mman.h" HAVE_SYS_MMAN_H)
configure_file(caching/config-caching.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-caching.h)

set(kcoreaddons_OPTIONAL_SRCS )
set(kcoreaddons_OPTIONAL_LIBS )

if (FAM_FOUND)
   include_directories(${FAM_INCLUDE_DIR})
   set(kcoreaddons_OPTIONAL_LIBS ${kcoreaddons_OPTIONAL_LIBS} ${FAM_LIBRARIES})
endif ()

if(NOT WIN32)
    set(kcoreaddons_OPTIONAL_SRCS caching/kshareddatacache.cpp)
    set(kcoreaddons_OPTIONAL_LIBS ${kcoreaddons_OPTIONAL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

    set_source_files_properties(caching/kshareddatacache.cpp
        PROPERTIES COMPILE_FLAGS -fexceptions)

else(NOT WIN32)
    set(kcoreaddons_OPTIONAL_SRCS
        caching/kshareddatacache_win.cpp
        io/kdirwatch_win.cpp
    )
endif(NOT WIN32)

if (WIN32)
    set(kcoreaddons_OPTIONAL_SRCS
        ${kcoreaddons_OPTIONAL_SRCS}
        text/kmacroexpander_win.cpp
        util/kshell_win.cpp
    )
    if(NOT WINCE)
        LIST(APPEND kcoreaddons_OPTIONAL_SRCS
           util/kuser_win.cpp
        )
    else(NOT WINCE)
        LIST(APPEND kcoreaddons_OPTIONAL_SRCS
           util/kuser_wince.cpp
        )
    endif(NOT WINCE)
endif ()

if (UNIX)
    set(kcoreaddons_OPTIONAL_SRCS
        ${kcoreaddons_OPTIONAL_SRCS}
        text/kmacroexpander_unix.cpp
        util/kuser_unix.cpp
        util/kshell_unix.cpp
    )
endif ()

set(libkcoreaddons_SRCS
    kaboutdata.cpp
    io/kdirwatch.cpp
    io/kfilesystemtype_p.cpp
    io/kmessage.cpp
    io/kprocess.cpp
    io/kbackup.cpp
    io/kurlmimedata.cpp
    jobs/kcompositejob.cpp
    jobs/kjob.cpp
    jobs/kjobtrackerinterface.cpp
    jobs/kjobuidelegate.cpp
    randomness/krandom.cpp
    randomness/krandomsequence.cpp
    text/kmacroexpander.cpp
    text/kstringhandler.cpp
    util/kshell.cpp
    ${kcoreaddons_OPTIONAL_SRCS}
)


set(kcoreaddons_INCLUDE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}/.. # for kcoreaddons_version.h
    ${CMAKE_CURRENT_SOURCE_DIR}/caching/
    ${CMAKE_CURRENT_BINARY_DIR}/io/
    ${CMAKE_CURRENT_SOURCE_DIR}/io/
    ${CMAKE_CURRENT_SOURCE_DIR}/jobs/
    ${CMAKE_CURRENT_SOURCE_DIR}/randomness/
    ${CMAKE_CURRENT_SOURCE_DIR}/text/
    ${CMAKE_CURRENT_SOURCE_DIR}/util/
)

add_library(KCoreAddons ${libkcoreaddons_SRCS})
generate_export_header(KCoreAddons)

target_include_directories(KCoreAddons PUBLIC "$<BUILD_INTERFACE:${kcoreaddons_INCLUDE_DIRS}>")
target_link_libraries(KCoreAddons
  LINK_PUBLIC
    Qt5::Core
  LINK_PRIVATE
    ${kcoreaddons_OPTIONAL_LIBS}
)
if(NOT WIN32)
    target_link_libraries(KCoreAddons LINK_PRIVATE pthread)
endif()

if(IS_ABSOLUTE "${INCLUDE_INSTALL_DIR}")
  target_include_directories(KCoreAddons INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>" )
else()
  target_include_directories(KCoreAddons INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}>" )
endif()

set_target_properties(KCoreAddons PROPERTIES VERSION   ${KCOREADDONS_VERSION_STRING}
                                             SOVERSION ${KCOREADDONS_SOVERSION}
)

install(TARGETS KCoreAddons EXPORT KCoreAddonsTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    kaboutdata.h
    caching/kshareddatacache.h
    io/kautosavefile.h
    io/kdirwatch.h
    io/kmessage.h
    io/kprocess.h
    io/kbackup.h
    io/kurlmimedata.h
    jobs/kcompositejob.h
    jobs/kjob.h
    jobs/kjobtrackerinterface.h
    jobs/kjobuidelegate.h
    randomness/krandom.h
    randomness/krandomsequence.h
    text/kmacroexpander.h
    text/kstringhandler.h
    util/kuser.h
    util/kshell.h
    ${CMAKE_CURRENT_BINARY_DIR}/kcoreaddons_export.h
    DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)
