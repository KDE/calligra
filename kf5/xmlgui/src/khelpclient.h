/*  This file is part of the KDE libraries
 *  Copyright 2012 David Faure <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License or ( at
 *  your option ) version 3 or, at the discretion of KDE e.V. ( which shall
 *  act as a proxy as in section 14 of the GPLv3 ), any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KHELPCLIENT_H
#define KHELPCLIENT_H

#include "xmlgui_export.h"
#include <QtCore/QString>
#include <QtCore/QByteArray>

namespace KHelpClient
{
    /**
     * Invokes the KHelpCenter HTML help viewer from docbook sources.
     *
     * @param anchor      This has to be a defined anchor in your
     *                    docbook sources. If empty the main index
     *                    is loaded
     * @param appname     This allows you to show the help of another
     *                    application. If empty the current name() is
     *                    used
     * @param startup_id for app startup notification, "0" for none,
     *           "" ( empty string ) is the default
     * @since 5.0
     */
    XMLGUI_EXPORT void invokeHelp(const QString& anchor = QString(), const QString& appname = QString());
}


#endif /* KHELPCLIENT_H */

