/*  This file is part of the KDE project
    Copyright (C) 2006 David Faure <faure@kde.org>

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

#ifndef KOPROPERTY_EXPORT_H
#define KOPROPERTY_EXPORT_H

/* needed for OMG_EXPORT_MACRO and OMG_IMPORT_MACRO macros */
#include <common/omg_macros.h>

/* We use _WIN32/_WIN64 instead of Q_OS_WIN so that this header can be used from C files too */
#if defined _WIN32 || defined _WIN64

#ifndef KOPROPERTY_EXPORT
# if defined(MAKE_KOPROPERTY_LIB)
/* We are building this library */
#  define KOPROPERTY_EXPORT OMG_EXPORT_MACRO
# else
/* We are using this library */
#  define KOPROPERTY_EXPORT OMG_IMPORT_MACRO
# endif
#endif

#else /* UNIX */

#define KOPROPERTY_EXPORT OMG_EXPORT_MACRO

#endif

#endif
