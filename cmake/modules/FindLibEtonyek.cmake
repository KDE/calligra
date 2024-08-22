# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_LibEtonyek QUIET libetonyek-0.1 IMPORTED_TARGET)

set(LibEtonyek_VERSION ${PKG_LibEtonyek_VERSION})
set(LibEtonyek_PREFIX ${PKG_LibEtonyek_PREFIX})

find_path(LibEtonyek_INCLUDE_DIRS
    NAMES libetonyek/libetonyek.h
    HINTS ${PKG_LibEtonyek_INCLUDE_DIRS}
)

find_library(LibEtonyek_LIBRARIES
    NAMES etonyek etonyek-0.1
    HINTS ${PKG_LibEtonyek_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibEtonyek
    FOUND_VAR LibEtonyek_FOUND
    REQUIRED_VARS LibEtonyek_PREFIX LibEtonyek_INCLUDE_DIRS
    VERSION_VAR LibEtonyek_VERSION
)

if(LibEtonyek_FOUND AND NOT TARGET LibEtonyek::LibEtonyek)
    add_library(LibEtonyek::LibEtonyek UNKNOWN IMPORTED)
    set_target_properties(LibEtonyek::LibEtonyek PROPERTIES
        IMPORTED_LOCATION "${LibEtonyek_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${PKG_LibEtonyek_CFLAGS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibEtonyek_INCLUDE_DIRS}"
    )
    if (TARGET PkgConfig::PKG_LibEtonyek)
        target_link_libraries(LibEtonyek::LibEtonyek INTERFACE PkgConfig::PKG_LibEtonyek)
    endif()
endif()

mark_as_advanced(LibEtonyek_LIBRARIES LibEtonyek_INCLUDE_DIRS LibEtonyek_VERSION)

include(FeatureSummary)
set_package_properties(LibEtonyek PROPERTIES
    DESCRIPTION "Apple Keynote Document Library"
    URL "https://wiki.documentfoundation.org/DLP/Libraries/libetonyek"
)
