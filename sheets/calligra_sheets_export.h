/*  This file is part of the KDE project
    Copyright (C) 2007 David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_EXPORT_H
#define CALLIGRA_SHEETS_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef CALLIGRA_SHEETS_ODF_EXPORT
# if defined(MAKE_CALLIGRASHEETSODF_LIB)
/* We are building this library */
#  define CALLIGRA_SHEETS_ODF_EXPORT KDE_EXPORT
# else
/* We are using this library */
#  define CALLIGRA_SHEETS_ODF_EXPORT KDE_IMPORT
# endif
#endif

#ifndef CALLIGRA_SHEETS_COMMON_EXPORT
# if defined(MAKE_CALLIGRASHEETSCOMMON_LIB)
/* We are building this library */
#  define CALLIGRA_SHEETS_COMMON_EXPORT KDE_EXPORT
# else
/* We are using this library */
#  define CALLIGRA_SHEETS_COMMON_EXPORT KDE_IMPORT
# endif
#endif

#ifndef CALLIGRA_SHEETS_SOLVER_EXPORT
# if defined(MAKE_SHEETSSOLVER_LIB)
/* We are building this library */
#  define CALLIGRA_SHEETS_SOLVER_EXPORT KDE_EXPORT
# else
/* We are using this library */
#  define CALLIGRA_SHEETS_SOLVER_EXPORT KDE_IMPORT
# endif
#endif

# ifndef CALLIGRA_SHEETS_EXPORT_DEPRECATED
#  define CALLIGRA_SHEETS_EXPORT_DEPRECATED KDE_DEPRECATED CALLIGRA_SHEETS_EXPORT
# endif

// now for tests
#ifdef COMPILING_TESTS
#if defined _WIN32 || defined _WIN64
# if defined(MAKE_CALLIGRASHEETSCOMMON_LIB)
#       define CALLIGRA_SHEETS_COMMON_TEST_EXPORT KDE_EXPORT
#   else
#       define CALLIGRA_SHEETS_COMMON_TEST_EXPORT KDE_IMPORT
#   endif
# else /* not windows */
#   define CALLIGRA_SHEETS_COMMON_TEST_EXPORT KDE_EXPORT
# endif
#else /* not compiling tests */
#   define CALLIGRA_SHEETS_COMMON_TEST_EXPORT
#endif

#ifdef COMPILING_TESTS
#if defined _WIN32 || defined _WIN64
# if defined(MAKE_CALLIGRASHEETSODF_LIB)
#       define CALLIGRA_SHEETS_ODF_TEST_EXPORT KDE_EXPORT
#   else
#       define CALLIGRA_SHEETS_ODF_TEST_EXPORT KDE_IMPORT
#   endif
# else /* not windows */
#   define CALLIGRA_SHEETS_ODF_TEST_EXPORT KDE_EXPORT
# endif
#else /* not compiling tests */
#   define CALLIGRA_SHEETS_ODF_TEST_EXPORT
#endif

#endif
