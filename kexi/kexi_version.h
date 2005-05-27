/* This file is part of the KDE project
   Copyright (c) 2003-2005 Kexi Team
    
   Version information based on kofficeversion.h,
   Copyright (c) 2003 KOffice Team

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

#ifndef _KEXI_VERSION_
#define _KEXI_VERSION_

#ifdef OOPL_VERSION /* user-friendly version info */
# include "oopl_global.h"
#else /* default */
# define KEXI_APP_NAME "Kexi"
#endif

#ifndef KEXI_VERSION_STRING
# define KEXI_VERSION_STRING "0.9 Beta 1"
#endif

#define KEXI_VERSION_MAJOR 0
#define KEXI_VERSION_MINOR 8 /* 0.9 */
#define KEXI_VERSION_RELEASE 91

#define KEXI_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))

#define KEXI_VERSION \
  KEXI_MAKE_VERSION(KEXI_VERSION_MAJOR,KEXI_VERSION_MINOR,KEXI_VERSION_RELEASE)

#define KEXI_IS_VERSION(a,b,c) ( KEXI_VERSION >= KEXI_MAKE_VERSION(a,b,c) )

/**
 * Namespace for general Kexi functions.
 */
namespace Kexi
{
    /**
     * Returns the encoded number of Kexi's version, see the KEXI_VERSION macro.
     * In contrary to that macro this function returns the number of the actually
     * installed Kexi version, not the number of the Kexi version that was
     * installed when the program was compiled.
     * @return the version number, encoded in a single uint
     */
    KEXICORE_EXPORT unsigned int version();
    /**
     * Returns the major number of Kexi's version, e.g.
     * 1 for Kexi 1.2.3.
     * @return the major version number
     */
    KEXICORE_EXPORT unsigned int versionMajor();
    /**
     * Returns the minor number of Kexi's version, e.g.
     * 2 for Kexi 1.2.3.
     * @return the minor version number
     */
    KEXICORE_EXPORT unsigned int versionMinor();
    /**
     * Returns the release of Kexi's version, e.g.
     * 3 for Kexi 1.2.3.
     * @return the release number
     */
    KEXICORE_EXPORT unsigned int versionRelease();
    /**
     * Returns the Kexi version as string, e.g. "1.2.3"
     * Sometimes it may be even something like "1.2.3 beta 2"
     * @return the Kexi version. You can keep the string forever
     */
    KEXICORE_EXPORT const char *versionString();
}

/*
 * this is the version a part has to be only increase it if the 
 * interface isn't binary compatible anymore
 
 * @todo
 */
 
#define KEXI_PART_VERSION 1

#endif /* _KEXI_VERSION_ */

