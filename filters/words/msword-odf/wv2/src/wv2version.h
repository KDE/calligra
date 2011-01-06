/* This file is part of the wvWare 2 project
   Copyright (C) 2003 KOffice Team
   Copyright (C) 2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#ifndef WV2_VERSION_H
#define WV2_VERSION_H

#define WV2_VERSION_STRING "0.4.2"
#define WV2_VERSION_MAJOR 0
#define WV2_VERSION_MINOR 4
#define WV2_VERSION_RELEASE 2
#define WV2_MAKE_VERSION( a, b, c ) ( ( ( a ) << 16 ) | ( ( b ) << 8 ) | ( c ) )

#define WV2_VERSION \
  WV2_MAKE_VERSION( WV2_VERSION_MAJOR, WV2_VERSION_MINOR, WV2_VERSION_RELEASE )

#define WV2_IS_VERSION( a, b, c ) ( WV2_VERSION >= WV2_MAKE_VERSION( a, b, c ) )

namespace wvWare
{
    /**
     * Returns the encoded number of wv2's version, see the WV2_VERSION macro.
     * In contrary to that macro this function returns the number of the actually
     * installed wv2 version, not the number of the wv2 version that was
     * installed when the program was compiled.
     * @return the version number, encoded in a single uint
     */
    unsigned int version();
    /**
     * Returns the major number of wv2's version, e.g.
     * 0 for wv2 0.2.5.
     * @return the major version number
     */
    unsigned int versionMajor();
    /**
     * Returns the minor number of wv2's version, e.g.
     * 2 for wv2 0.2.5.
     * @return the minor version number
     */
    unsigned int versionMinor();
    /**
     * Returns the release of wv2's version, e.g.
     * 5 for wv2 0.2.5.
     * @return the release number
     */
    unsigned int versionRelease();
    /**
     * Returns the wv2 version as string, e.g. "0.2.5".
     * @return the wv2 version. You can keep the string forever
     */
    const char *versionString();
}

#endif // WV2_VERSION_H
