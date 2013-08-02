/* This file is part of the KDE libraries
    Copyright (C) 2000 Torben Weis <weis@kde.org>
    Copyright (C) 2006 David Faure <faure@kde.org>
    Copyright (C) 2009 Michael Pyne <michael.pyne@kdemail.net>

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

#include "krun.h"

#include <QFileInfo>
#include <QMimeDatabase>

bool KRun::isExecutableFile(const QUrl& url, const QString &mimetype)
{
    if (!url.isLocalFile()) {
        return false;
    }
    QFileInfo file(url.toLocalFile());
    if (file.isExecutable()) {    // Got a prospective file to run
        QMimeDatabase db;
        QMimeType mimeType = db.mimeTypeForName(mimetype);
        if (mimeType.inherits(QLatin1String("application/x-executable")) ||
#ifdef Q_OS_WIN
            mimeType.inherits(QLatin1String("application/x-ms-dos-executable")) ||
#endif
            mimeType.inherits(QLatin1String("application/x-executable-script"))
           ) {
            return true;
        }
    }
    return false;
}

bool KRun::isExecutableFile(const QString& url, const QString &mimetype)
{
    return isExecutableFile(QUrl(url), mimetype);
}

bool KRun::isExecutable(const QString& serviceType)
{
    return (serviceType == "application/x-desktop" ||
            serviceType == "application/x-executable" ||
            serviceType == "application/x-ms-dos-executable" ||
            serviceType == "application/x-shellscript");
}

