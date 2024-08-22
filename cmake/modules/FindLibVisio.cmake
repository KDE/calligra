# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_LibVisio QUIET libvisio-0.1 IMPORTED_TARGET)

set(LibVisio_VERSION ${PKG_LibVisio_VERSION})
set(LibVisio_PREFIX ${PKG_LibVisio_PREFIX})

find_library(LibVisio_LIBRARIES
    NAMES visio visio-0.1
    HINTS ${PKG_LibVisio_LIBRARY_DIRS}
)

find_path(LibVisio_INCLUDE_DIRS
    NAMES libvisio/libvisio.h
    HINTS ${PKG_LibVisio_INCLUDE_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibVisio
    FOUND_VAR LibVisio_FOUND
    REQUIRED_VARS LibVisio_PREFIX LibVisio_INCLUDE_DIRS
    VERSION_VAR LibVisio_VERSION
)

if(LibVisio_FOUND AND NOT TARGET LibVisio::LibVisio)
    add_library(LibVisio::LibVisio UNKNOWN IMPORTED)
    set_target_properties(LibVisio::LibVisio PROPERTIES
        IMPORTED_LOCATION "${LibVisio_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${PKG_LibVisio_CFLAGS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibVisio_INCLUDE_DIRS}"
    )
    if (TARGET PkgConfig::PKG_LibVisio)
        target_link_libraries(LibVisio::LibVisio INTERFACE PkgConfig::PKG_LibVisio)
    endif()
endif()

mark_as_advanced(LibVisio_LIBRARIES LibVisio_INCLUDE_DIRS LibVisio_VERSION)

include(FeatureSummary)
set_package_properties(LibVisio PROPERTIES
    URL "https://wiki.documentfoundation.org/DLP/Libraries/libvisio"
    DESCRIPTION "libvisio is a library and a set of tools for reading and converting MS Visio diagrams."
)
