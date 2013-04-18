// -*- mode: c++; c-basic-offset: 2 -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2006 David Faure <faure@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KRUN_H
#define KRUN_H

#include <QtCore/QString>
#include <QtCore/QUrl>

/**
 * To open files with their associated applications in KDE, use KRun.
 *
 * It can execute any desktop entry, as well as any file, using
 * the default application or another application "bound" to the file type
 * (or URL protocol).
 *
 * In that example, the mimetype of the file is not known by the application,
 * so a KRun instance must be created. It will determine the mimetype by itself.
 * If the mimetype is known, or if you even know the service (application) to
 * use for this file, use one of the static methods.
 *
 * By default KRun uses auto deletion. It causes the KRun instance to delete
 * itself when the it finished its task. If you allocate the KRun
 * object on the stack you must disable auto deletion, otherwise it will crash.
 *
 * @short Opens files with their associated applications in KDE
 */
namespace KRun 
{
    /**
     * Returns whether @p serviceType refers to an executable program instead
     * of a data file.
     */
    static bool isExecutable(const QString& serviceType);

    /**
     * Returns whether the @p url of @p mimetype is executable.
     * To be executable the file must pass the following rules:
     * -# Must reside on the local filesystem.
     * -# Must be marked as executable for the user by the filesystem.
     * -# The mime type must inherit application/x-executable or application/x-executable-script.
     * To allow a script to run when the above rules are satisfied add the entry
     * @code
     * X-KDE-IsAlso=application/x-executable-script
     * @endcode
     * to the mimetype's desktop file.
     */
    static bool isExecutableFile(const QUrl& url, const QString &mimetype);

};

#endif
