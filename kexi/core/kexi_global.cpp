/* This file is part of the KDE project
    Copyright (C) 2003-2012 Jarosław Staniek <staniek@kde.org>

    (version information based on calligraversion.h)

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

#include "kexi_version.h"

#include <QString>

KEXICORE_EXPORT unsigned int Kexi::version()
{
    return KEXI_VERSION;
}

KEXICORE_EXPORT unsigned int Kexi::versionMajor()
{
    return KEXI_VERSION_MAJOR;
}

KEXICORE_EXPORT unsigned int Kexi::versionMinor()
{
    return KEXI_VERSION_MINOR;
}

KEXICORE_EXPORT unsigned int Kexi::versionRelease()
{
    return KEXI_VERSION_RELEASE;
}

KEXICORE_EXPORT const char *Kexi::versionString()
{
    return KEXI_VERSION_STRING;
}

KEXICORE_EXPORT unsigned int Kexi::stableVersionMajor()
{
    return (versionRelease() > 50 && versionMinor() == 9) 
        ? (versionMajor() + 1) // e.g. 2.9.70 -> 3.0.0
        : versionMajor(); // e.g. 2.3.70 -> 2.4.0 or // e.g. 2.4.0 -> 2.4.0
}

KEXICORE_EXPORT unsigned int Kexi::stableVersionMinor()
{
    if (versionRelease() > 50) {
        return (versionMinor() == 9)
            ? 0 // e.g. 2.9.70 -> 3.0.0 
            : (versionMinor() + 1); // e.g. 2.3.70 -> 2.4.0
    }
    return versionMinor(); // e.g. 2.4.0 -> 2.4.0
}

KEXICORE_EXPORT unsigned int Kexi::stableVersionRelease()
{
    return (versionRelease() > 50)
        ? 0 // 2.9.70 -> 3.0.0 or 2.3.70 -> 2.4.0
        : versionRelease(); // e.g. 2.4.0 -> 2.4.0
}

KEXICORE_EXPORT QString Kexi::stableVersionString()
{
    return QString::number(stableVersionMajor()) + '.'
           + QString::number(stableVersionMinor()) + '.'
           + QString::number(stableVersionRelease());
}
