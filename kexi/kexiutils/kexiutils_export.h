/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef _KEXIUTILS_EXPORT_H_
#define _KEXIUTILS_EXPORT_H_

#include <kdelibs_export.h>

#ifdef MAKE_KEXIUTILS_LIB
# define KEXIUTILS_EXPORT KDE_EXPORT
#elif defined(KDE_MAKE_LIB)
# define KEXIUTILS_EXPORT KDE_IMPORT
#else
# define KEXIUTILS_EXPORT
#endif

// temporary, remove after porting to Qt4
#undef foreach3
#define foreach3(_class, _variable, _list) \
	for (_class _variable = (_list).constBegin(); _variable!=(_list).constEnd(); ++_variable)

#endif
