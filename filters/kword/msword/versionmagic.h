/* This file is part of the KOffice project
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>
   Copyright (C) 2003 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef VERSIONMAGIC_H
#define VERSIONMAGIC_H

#include <wv2/global.h>  // ###### FIXME: #include wv2version.h here

#if defined(WV2_IS_VERSION)
#  if WV2_IS_VERSION( 0, 2, 5 )
#    define IMAGE_IMPORT 1
#  endif
#endif

#endif // VERSIONMAGIC_H
