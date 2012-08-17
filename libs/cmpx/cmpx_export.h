/*
 *  Copyright (c) 2012 Joseph Simon <jsimon383@gmail.com>
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

#ifndef CMPX_EXPORT_H
#define CMPX_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef CMPX_EXPORT
# if defined(KDELIBS_STATIC_LIBS)
   /* No export/import for static libraries */
#  define CMPX_EXPORT
# elif defined(MAKE_CMPX_LIB)
   /* We are building this library */ 
#  define CMPX_EXPORT KDE_EXPORT
# else
   /* We are using this library */ 
#  define CMPX_EXPORT KDE_IMPORT
# endif
#endif

# ifndef CMPX_EXPORT_DEPRECATED
#  define CMPX_EXPORT_DEPRECATED KDE_DEPRECATED CMPX_EXPORT
# endif

#endif
