/* This file is part of the KDE project
   Copyright (C) 2011 Yue Liu <yue.liu@mail.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef WRITERPERFECT_EXPORT_H
#define WRITERPERFECT_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef WRITERPERFECT_EXPORT
# if defined(MAKE_WRITERPERFECT_LIB)
/* We are building this library */
#  define WRITERPERFECT_EXPORT KDE_EXPORT
# else
/* We are using this library */
#  define WRITERPERFECT_EXPORT KDE_IMPORT
# endif
#endif

# ifndef WRITERPERFECT_EXPORT_DEPRECATED
#  define WRITERPERFECT_EXPORT_DEPRECATED KDE_DEPRECATED WRITERPERFECT_EXPORT
# endif

#endif
