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

#ifndef KARCHIVE_EXPORT_H
#define KARCHIVE_EXPORT_H

#ifdef COMPILE_KARCHIVE_STANDALONE

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef KARCHIVE_EXPORT
# if defined(KDELIBS_STATIC_LIBS)
   /* No export/import for static libraries */
#  define KARCHIVE_EXPORT
# elif defined(MAKE_KARCHIVE_LIB)
   /* We are building this library */
#  define KARCHIVE_EXPORT KDE_EXPORT
# else
   /* We are using this library */
#  define KARCHIVE_EXPORT KDE_IMPORT
# endif
#endif

#else

/* KDE4 compatibility */

#include <kdecore_export.h>
#define KARCHIVE_EXPORT KDECORE_EXPORT

#endif

# ifndef KARCHIVE_EXPORT_DEPRECATED
#  define KARCHIVE_EXPORT_DEPRECATED KDE_DEPRECATED KARCHIVE_EXPORT
# endif


#endif
