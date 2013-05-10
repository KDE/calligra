/* This file is part of the KDE project
   Copyright (C) 2013 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXIPASSWORDPAGE_H
#define KEXIPASSWORDPAGE_H

#include <kexiutils/KexiAssistantPage.h>

namespace KexiDB {
    class ConnectionData;
}

//! An assistant page with a form for entering a password
class KexiPasswordPage : public KexiAssistantPage
{
    Q_OBJECT
public:
    explicit KexiPasswordPage(const QString &title, QWidget* parent = 0);
    explicit KexiPasswordPage(QWidget* parent = 0);
    virtual ~KexiPasswordPage();

    /**
     * Set the password.
     */
    void setPassword(const QString& password);

    /**
     * @return the password entered by the user.
     */
    QString password() const;

    /**
     * Set the username.
     */
    void setUserName(const QString& username);

    /**
     * @return the username entered by the user.
     */
    QString userName() const;

    /**
     * Set the server name. It includes port too.
     */
    void setServer(const QString& server);

    /**
     * @return the server
     */
    QString server() const;

    /**
     * Sets the database name field read-only.
     *
     * This can also be set by passing DatabaseNameReadOnly as flag in the constructor.
     */
    void setDatabaseNameReadOnly(bool readOnly);

    /**
     * Shows the database name field.
     */
    void showDatabaseName(bool show);

    /**
     * Set the database name.
     */
    void setDatabaseName(const QString& databaseName);

    /**
     * Set username, server and optionally the password based on the @a data connection data.
     */
    void setConnectionData(const KexiDB::ConnectionData &data);

    /**
     * Updates connection data @a data based on entered password.
     */
    void updateConnectionData(KexiDB::ConnectionData *data);

private:
    void init();

    class Private;
    Private* const d;
};

#endif
