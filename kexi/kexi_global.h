/* This file is part of the KDE project
   Copyright (c) 2003-2004 Kexi Team

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _KEXI_GLOBAL_
#define _KEXI_GLOBAL_

#include <kexi_export.h>

#define kexidbg  kdDebug(44010)   //! General debug area for Kexi
#define kexicoredbg  kdDebug(44020)   //! Debug area for Kexi Core
#define kexipluginsdbg kdDebug(44021) //! Debug area for Kexi Plugins
#define kexiwarn  kdWarning(44010)
#define kexicorewarn kdWarning(44020)
#define kexipluginswarn kdWarning(44021)

#endif /* _KEXI_GLOBAL_ */

