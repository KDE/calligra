## Disable librcps for now
## It has some bugs, and should be extended to be really useful
# set(LIBRCPS_MIN_VERSION "0.3")
# # macro_optional_find_package(LibRCPS)
# # 
# # if(LIBRCPS_FOUND AND LIBRCPS_INCLUDE_DIR AND LIBRCPS_LIBRARIES)
# #     set(HAVE_LIBRCPS 1)
# # endif()
# 
# set(HAVE_LIBRCPS 1)
# 
# set_package_properties(LibRCPS PROPERTIES
#     DESCRIPTION "Resource Constrained Project Scheduling Library"
#     URL "http://www.librcps.org"
#     PURPOSE "Required by Plan RCPS Plugin"
#     TYPE OPTIONAL
# )
# 
# if(HAVE_LIBRCPS)
#     add_subdirectory( rcps )
# endif()

add_subdirectory(tj)
