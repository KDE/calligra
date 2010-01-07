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

#ifndef DBUSADAPTOR_H
#define DBUSADAPTOR_H

#include <QtDBus>
#include "HildonApplication.h"

/// \brief DBusAdaptor class
///
/// This class creates a dbus-interface for File browser to open the document.
/// The available dbus functions can be found from public slots
/// section.
class DBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.FreOffice")

public:

    DBusAdaptor(HildonApplication *aApplication);

    /// \brief The destructor
    ~DBusAdaptor();

public slots:
    //slot for mime_open DBus message
    Q_NOREPLY void mime_open(QString aText);

signals:
    //Signal triggered when there is valid filename in mime_open message
    void openDocument(const QString& fileName);

private:

    /// \brief Pointer to the qApp object, not owned
    HildonApplication *m_app;
};

#endif
