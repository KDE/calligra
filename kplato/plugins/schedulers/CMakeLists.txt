
#TODO check version
find_path(LIBRCPS_INCLUDE_DIR librcps.h)
find_library(LIBRCPS_LIBRARIES librcps.so)
if(LIBRCPS_INCLUDE_DIR AND LIBRCPS_LIBRARIES)
    set(HAVE_LIBRCPS 1)
endif(LIBRCPS_INCLUDE_DIR AND LIBRCPS_LIBRARIES)
macro_log_feature(HAVE_LIBRCPS "LibRCPS" "Resource Conflict Project Scheduling Library" "http://www.librcps.org" FALSE "0.2" "Required by KPlato RCPS Plugin")


if(HAVE_LIBRCPS)
    add_subdirectory( rcps )
endif(HAVE_LIBRCPS)
