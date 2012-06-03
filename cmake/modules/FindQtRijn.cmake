
INCLUDE(FindPkgConfig)

pkg_check_modules(QTRIJN QtRijn>=0.9.2)

if (QTRIJN_FOUND)
    message(STATUS "QtRijn >= 0.9.2 was found")
    set(HAVE_QTRIJN TRUE)
    if (NOT QTRIJN_FIND_QUIETLY)
        message(STATUS "Found QtRijn: ${QTRIJN_LIBRARIES}")
    endif ()
else ()
    if (NOT QtRijn_FIND_QUIETLY)
        message(STATUS "QtRijn was NOT found.")
    endif ()
    if (QtRijn_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find QtRijn")
    endif ()
endif ()
