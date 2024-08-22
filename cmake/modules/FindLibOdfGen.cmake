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

if(LibOdfGen_FOUND AND NOT TARGET LibOdfGen::LibOdfGen)
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
