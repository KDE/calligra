
set(LIBRCPS_MIN_VERSION "0.3")
# macro_optional_find_package(LibRCPS)
# 
# if(LIBRCPS_FOUND AND LIBRCPS_INCLUDE_DIR AND LIBRCPS_LIBRARIES)
#     set(HAVE_LIBRCPS 1)
# endif(LIBRCPS_FOUND AND LIBRCPS_INCLUDE_DIR AND LIBRCPS_LIBRARIES)

set(HAVE_LIBRCPS 1)

macro_log_feature(HAVE_LIBRCPS "LibRCPS" "Resource Constrained Project Scheduling Library" "http://www.librcps.org" FALSE ${LIBRCPS_MIN_VERSION} "Required by Plan RCPS Plugin")

if(HAVE_LIBRCPS)
    add_subdirectory( rcps )
endif(HAVE_LIBRCPS)

add_subdirectory(tj)
