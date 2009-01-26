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

#ifndef KEXIDB_DRIVER_ODBC_H
#define KEXIDB_DRIVER_ODBC_H

#include <kexidb/driver.h>

namespace KexiDB
{

class Connection;
class ODBCConnection;

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
    ODBCDriver(QObject *parent, const QStringList &args = QStringList());
    ~ODBCDriver();

    virtual bool isSystemDatabaseName(const QString& name) const;

    virtual QString escapeString(const QString& str) const;
    virtual QByteArray escapeString(const QByteArray& str) const;

    QString escapeBLOB(const QByteArray& array) const;

    /** 
     * Updates the driver properties/fields using the connection handle
     * \param connectionHandle The ConnectionHandle using which the driver manager needs to be queried
     * \return true if all the required information could be gathered, else false 
     */
    bool updateDriverInfo(ODBCConnection* connection);

    /**
     * Returns the query to be run to fetch the last inserted oid
     * Temporary. I've kept this till I find a better solution. Till then atleast mysql will work :)
     */
    QString getQueryForOID();

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

private:

    /**
     * Populates the type info given the ODBCConnection
     * \param connection The ODBC Connection from which the type info needs to be populated
     * \return true if the type info could be loaded successfully else false
     */
    bool populateTypeInfo(ODBCConnection* connection);

    /**
     * Populates general info given the ODBCConnection. ( ones received from executing SQLGetInfo )
     * \param connection The ODBCConnection from which the general info needs to be populated
     * \return true if the general info could be loaded successfully, else false
     */
    bool populateGeneralInfo(ODBCConnection* connection);

    /**
     * Populates the name of the psuedo column (ROWID) that exists for each table
     * \param connection the ODBC connection from which the psuedo column info needs to be populated
     * \return true if the pseudo column information could be loaded successfully, else false
     */
    bool populateROWID(ODBCConnection* connection );

    /** 
     * Populates information about auto increments and rowid.
     * \param database name. The database name for which all the info needs to be loaded
     * \return true, if we had the information, false if default info was loaded
     */
    bool populateBehaviourInfo(const QString& dbName);

    /**
     * Set the name of the type
     * \param type The KexiDB enum value for the type
     * \param typeName The string version of the type for the data store
     */
    void setTypeName( Field::Type type, const QString& typeName);

    /*
     * Flag which indicates whether the driver info has been updated for this driver
     * As the ODBCDriver class is a driver manager of sorts, and handles all ODBC connections
     * with the same connection data, this info needs to be loaded only once, per connection data
     */
    bool m_driverInfoUpdated;
    
    /*
     * The database name
     */
    QString m_dbmsName;

    class ODBCDatabaseProperties;
    ODBCDatabaseProperties* o_d;
};

}

#endif

