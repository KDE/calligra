/*
 *  Copyright (c) 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <CalligraVersionWrapper.h>

#include <calligraversion.h>
#include <calligragitversion.h>

QString CalligraVersionWrapper::versionYear()
{
    return QStringLiteral(CALLIGRA_YEAR);
}

QString CalligraVersionWrapper::versionString(bool checkGit)
{
    QString version = QStringLiteral(CALLIGRA_VERSION_STRING);

    if (checkGit) {
#ifdef CALLIGRA_GIT_SHA1_STRING
        version = QStringLiteral("%1 (git %2)").arg(CALLIGRA_VERSION_STRING, CALLIGRA_GIT_SHA1_STRING).toLatin1();
#endif
    }
    return version;
}

