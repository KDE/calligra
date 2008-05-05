/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXI_WEBFORMS_DBMODEL_H
#define KEXI_WEBFORMS_DBMODEL_H

#include <QString>
#include <QStringList>
#include <QPointer>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/field.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/indexschema.h>

namespace KexiWebForms {

    class DBModel {
    public:
        DBModel(QString);
        virtual ~DBModel() {};
        QStringList getDatabases();
        QStringList getTables(const QString&);
    private:
        KexiDB::ConnectionData m_connData;
        KexiDB::Connection* m_connection;
        KexiDB::Driver* m_driver;
        KexiDB::DriverManager* m_driverManager;
    };

}

#endif
