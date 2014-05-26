# Try to find Asset Importer (AssImp) libraries
# Once done this will define
#
#  ASSIMP_FOUND       - System has AssImp
#  ASSIMP_INCLUDE_DIR - Include directory for AssImp
#  ASSIMP_LIBRARY     - The AssImp Library
#

find_package(PkgConfig)
pkg_check_modules(PC_ASSIMP QUIET assimp)

find_path(ASSIMP_INCLUDE_DIR
    NAMES ai_assert.h version.h Exporter.hpp
    PATH_SUFFIXES assimp
    HINTS ${PC_ASSIMP_INCLUDEDIR}
)

find_library(ASSIMP_LIBRARY
    NAMES assimp
    HINTS ${PC_ASSIMP_LIBDIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AssImp DEFAULT_MSG ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)

mark_as_advanced(ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)
