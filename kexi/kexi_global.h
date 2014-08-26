/* This file is part of the KDE project
   Copyright (c) 2003-2007 Kexi Team <kexi@kde.org>

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

#ifndef _KEXI_GLOBAL_
#define _KEXI_GLOBAL_

#include <kexi_export.h>
#include <config-kexi.h>

#ifdef _MSC_VER
#include <kdewin_export.h> // for WARNING
#endif

#ifndef futureI18n
# ifdef USE_FUTURE_I18N
#  define futureI18n(a) i18n(a)
#  define futureI18nc(a, b) i18nc(b)
#  define futureI18nc2(a, b, c) i18nc(a, b, c)
# else
#  define futureI18n(a) QString(a)
#  define futureI18nc(a, b) QString(b)
#  define futureI18nc2(a, b, c) QString(b)
# endif
#endif

#ifndef FUTURE_I18N_NOOP
# define FUTURE_I18N_NOOP(x) (x)
#endif

#endif /* _KEXI_GLOBAL_ */
