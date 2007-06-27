/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef _KEXIDB_EXPORT_H_
#define _KEXIDB_EXPORT_H_

#include <kdemacros.h>

#ifndef KEXI_DB_EXPORT
# ifdef MAKE_KEXI_DB_LIB
#  define KEXI_DB_EXPORT KDE_EXPORT
# else
#  define KEXI_DB_EXPORT KDE_IMPORT
# endif
#endif

#ifndef KEXIMIGR_EXPORT
# ifdef MAKE_KEXIMIGRATE_LIB
#  define KEXIMIGR_EXPORT  KDE_EXPORT
# else
#  define KEXIMIGR_EXPORT  KDE_IMPORT
# endif
#endif

/* -- compile-time settings -- */
#if defined(Q_WS_WIN) || defined(KEXI_OPTIONS)
/* defined in a .pro file or 'KEXI_OPTIONS' env. variable */
#else
#endif

/* Might want to add GUI defines here if widgets are to be
 * distributed as part of kexidb - mart */

#endif //KEXI_EXPORT_H
