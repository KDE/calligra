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

#ifndef KSPREAD_EXPORT_H
#define KSPREAD_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef KSPREAD_EXPORT
# if defined(MAKE_KSPREADCOMMON_LIB)
/* We are building this library */
#  define KSPREAD_EXPORT KDE_EXPORT
# elif defined(MAKE_KSPREADSOLVER_LIB)
/* We are building this library */
#  define KSPREAD_EXPORT KDE_EXPORT
# else
/* We are using this library */
#  define KSPREAD_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KSPREAD_EXPORT_DEPRECATED
#  define KSPREAD_EXPORT_DEPRECATED KDE_DEPRECATED KSPREAD_EXPORT
# endif

// now for tests
#ifdef COMPILING_TESTS
#if defined _WIN32 || defined _WIN64
# if defined(MAKE_KSPREADCOMMON_LIB)
#       define KSPREAD_TEST_EXPORT KDE_EXPORT
#   else
#       define KSPREAD_TEST_EXPORT KDE_IMPORT
#   endif
# else /* not windows */
#   define KSPREAD_TEST_EXPORT KDE_EXPORT
# endif
#else /* not compiling tests */
#   define KSPREAD_TEST_EXPORT
#endif

#endif
