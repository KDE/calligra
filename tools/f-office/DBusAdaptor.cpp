/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "DBusAdaptor.h"
#include "Common.h"
#include <QUrl>

DBusAdaptor::DBusAdaptor(HildonApplication *aApplication) :
        QDBusAbstractAdaptor(aApplication), m_app(aApplication)
{
    bool value = QDBusConnection::sessionBus().registerObject(
                     FREOFFICE_PATH, this, QDBusConnection::ExportAllSlots);

    if (!value) {
        printf("Failure to register\n");
        printf("Try with run-standalone.sh ./xxx\n");
    }

    QDBusConnection::sessionBus().registerService(FREOFFICE_SERVICE_NAME);
    QObject::connect(this, SIGNAL(openDocument(const QString &)), m_app, SIGNAL(openDocument(const QString &)));
}

DBusAdaptor::~DBusAdaptor()
{
}

void DBusAdaptor::mime_open(QString aText)
{
    QUrl path = QUrl::fromEncoded(aText.toUtf8());

    if (!QFile::exists(path.path()))
        path = QUrl(aText.toUtf8());

    if (path.isValid() && QFile::exists(path.path()))
        emit openDocument(path.path());
}

