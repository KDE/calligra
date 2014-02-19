/* This file is part of the KDE project
 * Copyright (C) 2014 Stuart Dickson <stuartmd@kogmbh.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "pathconverter.h"

#ifdef Q_OS_WIN
#include <windows.h>

QString WindowsTools::correctPathForCase(const QString& directory) {
	// Corrects for mismatched case errors in path (qtdeclarative fails to load)
    wchar_t buffer[1024];
    DWORD rv = ::GetShortPathName((wchar_t*)directory.utf16(), buffer, 1024);
    rv = ::GetLongPathName(buffer, buffer, 1024);
    return QString((QChar *)buffer);
}

#endif 