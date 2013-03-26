/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KOMVC_EXPORT_H
#define KOMVC_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef KOMVC_EXPORT
# if defined(MAKE_KOMVC_LIB)
/* We are building this library */
#  define KOMVC_EXPORT KDE_EXPORT
#  define KOMVC_TEST_EXPORT KDE_EXPORT
# else
/* We are using this library */
#  define KOMVC_EXPORT KDE_IMPORT
#  define KOMVC_TEST_EXPORT KDE_IMPORT
# endif
#endif

#endif
