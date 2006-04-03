/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 * copyright (C) 2006 by Tobi Krebs (tobi.krebs@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KOMACRO_EXPORT_H_
#define KOMACRO_EXPORT_H_

#ifdef __cplusplus
# include <kdeversion.h> /* this will also include <kdelibs_export.h>, if available */
#endif 

/* KDE_EXPORT will be defined multiple times without this on kdelibs 3.3 (tested on 3.3.1) */
#include <kdemacros.h>

/* workaround for KDElibs < 3.2 on !win32 */
#ifndef KDE_EXPORT
# define KDE_EXPORT
#endif

#define KOMACRO_EXPORT KDE_EXPORT

#endif
