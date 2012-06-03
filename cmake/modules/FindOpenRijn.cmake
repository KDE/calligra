
INCLUDE(FindPkgConfig)

pkg_check_modules(OPENRIJN OpenRijn>=0.9.16)

if (OPENRIJN_FOUND)
    set(HAVE_OPENRIJN TRUE)
    message(STATUS "OpenRijn >= 0.9.16 was found")
    if (NOT OPENRIJN_FIND_QUIETLY)
        message(STATUS "Found OpenRijn: ${OPENRIJN_LIBRARIES}")
    endif ()
else ()
    if (NOT OPENRIJN_FIND_QUIETLY)
        message(STATUS "OpenRijn was NOT found.")
    endif ()
    if (OPENRIJN_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find OpenRijn")
    endif ()
endif ()
