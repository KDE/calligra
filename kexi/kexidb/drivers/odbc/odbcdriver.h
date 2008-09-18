/* This file is part of the KDE project
   Copyright (C) 2004 Matt Rogers <matt.rogers@kdemail.net>

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

#ifndef KEXIDB_DRIVER_ODBC_H
#define KEXIDB_DRIVER_ODBC_H

#include <kexidb/driver.h>

namespace KexiDB
{

class Connection;

//! ODBC database driver.
/*!
 * This is the ODBC Driver for Kexi.
 * @author Matt Rogers <matt.rogers@kdemail.net>
 */
class ODBCDriver : public Driver
{
    Q_OBJECT
    KEXIDB_DRIVER

public:
    ODBCDriver(QObject *parent, const char *name, const QStringList &args = QStringList());
    ~ODBCDriver();

    virtual bool isSystemDatabaseName(const QString& name) const;
    /** \return true if n is a system object name;
     * \todo Find out what is a system object name and what isn't
     */
    virtual bool isSystemObjectName(const QString& name);

    /**
     * \return true if \a n is a system field name;
     * There aren't any system fields per tables, unless the table
     * is a system table
     */
    virtual bool isSystemFieldName(const QString& name) const;

    virtual QString escapeString(const QString& str) const;
    virtual QByteArray escapeString(const QByteArray& str) const;

protected:
    virtual Connection *drv_createConnection(ConnectionData &conn_data);
};

}

#endif

