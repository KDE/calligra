/* This file is part of the KDE project
   Copyright (C) 2009 Sharan Rao <sharanrao@gmail.com>

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

#ifndef KEXIDB_DRIVER_KODBC_H
#define KEXIDB_DRIVER_KODBC_H

#include <kexidb/driver.h>

namespace KexiDB
{

class Connection;
class ODBCDriver;

//! ODBC database driver.
/*!
 * This is the KODBC Driver for Kexi.
 */
class KODBCDriver : public Driver
{
    Q_OBJECT
    KEXIDB_DRIVER

public:
    KODBCDriver(QObject *parent, const QStringList &args = QStringList());
    ~KODBCDriver();

    virtual bool isSystemDatabaseName(const QString& name) const;
    /** \return true if n is a system object name;
     * \todo Find out what is a system object name and what isn't
     */
    virtual bool isSystemObjectName(const QString& name) const;


    virtual QString escapeString(const QString& str) const;
    virtual QByteArray escapeString(const QByteArray& str) const;

    QString escapeBLOB(const QByteArray& array) const;

protected:

    virtual Connection* drv_createConnection(ConnectionData &conn_data);
    virtual QString drv_escapeIdentifier(const QString& str) const;
    virtual QByteArray drv_escapeIdentifier(const QByteArray& str) const;
    /**
     * \return true if \a n is a system field name;
     * There aren't any system fields per tables, unless the table
     * is a system table
     */
    virtual bool drv_isSystemFieldName(const QString& name) const;

    /**
     *  Returns the ODBCDriver ( driver manager ) for the given connection data
     * \param connData The ConnectionData for which we need the ODBCDriver
     * \return The required ODBCDriver ( manager ) if it exists in the map, else NULL 
     */
    ODBCDriver* getODBCDriver(const ConnectionData& connData);

private:

    class KODBCDriverPrivate;
    KODBCDriverPrivate* k_d;
};

}

#endif

