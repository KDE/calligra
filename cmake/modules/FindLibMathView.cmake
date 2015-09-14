# - Try to find Libmathview and Qt backend
# Once done this will define
#
#  LIBMATHVIEW_FOUND        - Libmathview is available
#  LIBMATHVIEW_INCLUDE_DIRS - include directory, e.g. /usr/include/mathview
#  LIBMATHVIEW_LIBRARIES    - the libraries needed to use Libmathview
#
# Copyright (C) 2015 Yue Liu <yue.liu@mail.com>
# Redistribution and use is allowed according to the terms of the BSD license.

include(LibFindMacros)
libfind_pkg_check_modules(LIBMATHVIEW_CORE_PKGCONF mathview-core)
libfind_pkg_check_modules(LIBMATHVIEW_BACKEND_QT_PKGCONF mathview-backend-qt)
libfind_pkg_check_modules(LIBMATHVIEW_FRONTEND_LIBXML2_PKGCONF mathview-mathview-frontend-libxml2)
libfind_pkg_check_modules(LIBMATHVIEW_WIDGET_QT_PKGCONF mathview-widget-qt)

find_path(LIBMATHVIEW_INCLUDE_DIR
    NAMES QMathView.hh
    HINTS ${LIBMATHVIEW_PKGCONF_INCLUDE_DIRS} ${LIBMATHVIEW_PKGCONF_INCLUDEDIR}
    PATH_SUFFIXES mathview
)

find_library(LIBMATHVIEW_CORE_LIBRARY
    NAMES mathview
    HINTS ${LIBMATHVIEW_CORE_PKGCONF_LIBRARY_DIRS} ${LIBMATHVIEW_PKGCONF_LIBDIR}
)
find_library(LIBMATHVIEW_BACKEND_QT_LIBRARY
    NAMES mathview_backend_qt
    HINTS ${LIBMATHVIEW_BACKEND_QT_PKGCONF_LIBRARY_DIRS} ${LIBMATHVIEW_BACKEND_QT_PKGCONF_LIBDIR}
)

find_library(LIBMATHVIEW_FRONTEND_LIBXML2_LIBRARY
    NAMES mathview_frontend_libxml2
    HINTS ${LIBMATHVIEW_FRONTEND_LIBXML2_PKGCONF_LIBRARY_DIRS} ${LIBMATHVIEW_FRONTEND_LIBXML2_PKGCONF_LIBDIR}
)

find_library(LIBMATHVIEW_WIDGET_QT_LIBRARY
    NAMES mathview_widget_qt
    HINTS ${LIBMATHVIEW_WIDGET_QT_PKGCONF_LIBRARY_DIRS} ${LIBMATHVIEW_WIDGET_QT_PKGCONF_LIBDIR}
)

set(LIBMATHVIEW_PROCESS_LIBS
    LIBMATHVIEW_CORE_LIBRARY LIBMATHVIEW_BACKEND_QT_LIBRARY LIBMATHVIEW_FRONTEND_LIBXML2_LIBRARY LIBMATHVIEW_WIDGET_QT_LIBRARY
)
set(LIBMATHVIEW_PROCESS_INCLUDES LIBMATHVIEW_INCLUDE_DIR)
libfind_process(LIBMATHVIEW)
