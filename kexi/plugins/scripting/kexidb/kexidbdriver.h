/***************************************************************************
 * kexidbdriver.h
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef SCRIPTING_KEXIDBDRIVER_H
#define SCRIPTING_KEXIDBDRIVER_H

#include <QString>
#include <QObject>

#include <db/driver.h>
#include <kexi_global.h>

namespace Scripting
{

/**
 * Drivers are the implementations Kexi uses to access the
 * driver-backends.
 *
 * Example (in Python) ;
 * @code
 * # Import the Kross module and fetch the kexidb module.
 * import Kross
 * kexidb = Kross.module("kexidb")
 * # Create the driver now.
 * driver = kexidb.driver("SQLite3")
 * # Check if the driver is valid.
 * if not driver.isValid(): raise "Invalid driver"
 * # Create a connectiondata object.
 * connectiondata = kexidb.createConnectionData()
 * # Fill the new connectiondata object with what we need to connect.
 * connectiondata.setFileName("/home/user/kexisqlite3file.kexi")
 * # Print the list of connections before.
 * print driver.connectionsList()
 * # Create the connection now.
 * connection = driver.createConnection(connectiondata)
 * # Print the list of connections again. This includes our just created connection now.
 * for i in range(driver.connectionCount()): print driver.connection(i)
 * @endcode
 */
class KexiDBDriver : public QObject
{
    Q_OBJECT
public:
    KexiDBDriver(QObject* parent, ::KexiDB::Driver* driver);
    virtual ~KexiDBDriver();

public slots:

    /** Returns true if this driver is valid else false is returned. */
    bool isValid();
    /** The drivers major versionnumber. */
//2.0    int versionMajor();
    /** The drivers minor versionnumber. */
//2.0    int versionMinor();
    /** Driver-specific SQL string escaping. For example the " or ' char may
    need to be escaped for values used within SQL-statements. */
    QString escapeString(const QString& s);
    /** Returns true if this driver is file-based. */
    bool isFileDriver();
    /** Return a name of MIME type of files handled by this driver if it is a
    file-based database's driver otherwise returns null string. */
    QString fileDBDriverMimeType();
    /** Returns true if the passed string is a system object's name, eg. name
    of build-in system table that cannot be used or created by a user. */
    bool isSystemObjectName(const QString& name);
    /** Returns true if the passed string is a system database's name, eg. name
    of build-in, system database that cannot be used or created by a user. */
    bool isSystemDatabaseName(const QString& name);
    /** Returns true if the passed string is a system field's name, build-in
    system field that cannot be used or created by a user. */
    bool isSystemFieldName(const QString& name);
    /** The as second argument passed string got escaped to be usable within
    a SQL-statement and those escaped string got returned by the method.
    The first argument defines the fieldtype to what we should escape the
    second argument to. */
    QString valueToSQL(const QString& fieldtype, const QVariant& value);

    /** Create and return new \a KexiDBConnection object that uses the as
    argument passed \a KexiDBConnectionData object or NULL if the connection
    could not be created. */
    QObject* createConnection(QObject* data);
    /** Returns the number of connections. */
    uint connectionCount();
    /** Return the \a KexiDBConnection specified by the index-number passed as an argument. */
#ifdef __GNUC__
#warning TODO QSet<Connection*> is available now
#else
#pragma WARNING( TODO QSet<Connection*> is available now )
#endif
    /* TODO
        QObject* connection(uint index);*/

private:
    ::KexiDB::Driver* m_driver;
};

}

#endif

