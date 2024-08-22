# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_LibOdfGen QUIET libodfgen-0.1 IMPORTED_TARGET)

set(LibOdfGen_VERSION ${PKG_LibOdfGen_VERSION})
set(LibOdfGen_PREFIX ${PKG_LibOdfGen_PREFIX})

find_path(LibOdfGen_INCLUDE_DIRS
    NAMES libodfgen/libodfgen.hxx
    HINTS ${PKG_LibOdfGen_INCLUDE_DIRS}
)

find_library(LibOdfGen_LIBRARIES
    NAMES odfgen-0.1
    HINTS ${PKG_LibOdfGen_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibOdfGen
    FOUND_VAR LibOdfGen_FOUND
    REQUIRED_VARS LibOdfGen_PREFIX LibOdfGen_INCLUDE_DIRS
    VERSION_VAR LibOdfGen_VERSION
)

if(LibOdfGen_FOUND AND NOT TARGET LibRevenge::LibOdfGen)
    add_library(LibOdfGen::LibOdfGen UNKNOWN IMPORTED)
    set_target_properties(LibOdfGen::LibOdfGen PROPERTIES
        IMPORTED_LOCATION "${LibOdfGen_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${PKG_LibOdfGen_CFLAGS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibOdfGen_INCLUDE_DIRS}"
    )
    if (TARGET PkgConfig::PKG_LibOdfGen)
        target_link_libraries(LibOdfGen::LibOdfGen INTERFACE PkgConfig::PKG_LibOdfGen)
    endif()
endif()

mark_as_advanced(LibOdfGen_LIBRARIES LibOdfGen_INCLUDE_DIRS LibOdfGen_VERSION)

include(FeatureSummary)
set_package_properties(LibOdfGen PROPERTIES
    URL "http://sf.net/p/libwpd/libodfgen/"
    DESCRIPTION "Library to generate ODF documents from librevenge's api calls"
)


# - Try to find LibOdfGen
# Once done this will define
#
#  LIBODFGEN_FOUND       - libodfgen is available
#  LIBODFGEN_INCLUDE_DIRS - include directory, e.g. /usr/include
#  LIBODFGEN_LIBRARIES   - the libraries needed to use LibOdfGen
#
# SPDX-FileCopyrightText: 2013 Yue Liu <yue.liu@mail.com>
# Redistribution and use is allowed according to the terms of the BSD license.

include(LibFindMacros)
libfind_pkg_check_modules(LIBODFGEN_PKGCONF libodfgen-0.1)

find_path(LIBODFGEN_INCLUDE_DIR
    NAMES libodfgen/libodfgen.hxx
    HINTS ${LIBODFGEN_PKGCONF_INCLUDE_DIRS} ${LIBODFGEN_PKGCONF_INCLUDEDIR}
    PATH_SUFFIXES libodfgen-0.1
)

find_library(LIBODFGEN_LIBRARY
    NAMES odfgen-0.1
    HINTS ${LIBODFGEN_PKGCONF_LIBRARY_DIRS} ${LIBODFGEN_PKGCONF_LIBDIR}
)

set(LIBODFGEN_PROCESS_LIBS LIBODFGEN_LIBRARY)
set(LIBODFGEN_PROCESS_INCLUDES LIBODFGEN_INCLUDE_DIR)
libfind_process(LIBODFGEN)
