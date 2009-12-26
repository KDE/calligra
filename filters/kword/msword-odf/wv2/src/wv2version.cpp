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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "wv2version.h"

unsigned int wvWare::version()
{
    return WV2_VERSION;
}

unsigned int wvWare::versionMajor()
{
    return WV2_VERSION_MAJOR;
}

unsigned int wvWare::versionMinor()
{
    return WV2_VERSION_MINOR;
}

unsigned int wvWare::versionRelease()
{
    return WV2_VERSION_RELEASE;
}

const char *wvWare::versionString()
{
    return WV2_VERSION_STRING;
}
