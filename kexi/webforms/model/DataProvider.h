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

#ifndef KEXIWEBFORMS_DATAPROVIDER_H
#define KEXIWEBFORMS_DATAPROVIDER_H

#include <db/driver.h>
#include <db/connection.h>
#include <db/drivermanager.h>

class QString;

namespace KexiWebForms {
namespace Model {

/*!
 * Global connection object
 *
 * @note Using global object is doing something "The Wrong Way"™
 */
extern KexiDB::Connection* gConnection;

/**
 * @brief A class 'abstracting' the access to a database file
 *
 * DataProvider abstracts a bit the connection to a SQLite-based database
 * It's a singleton, it's not thread-safe, and it's a huge work-around a bug
 * I found at initialization time, when the Authenticator creates the tables
 * required to manage user authentication. That said, enjoy!
 */
class DataProvider {
public:
    static DataProvider* instance();

    bool initDatabase(const QString& name);
    bool reopenDatabase();
    KexiDB::Connection* connection();
protected:
    DataProvider() {};
private:
    static DataProvider* m_instance;
    KexiDB::Driver* m_driver;
    KexiDB::Connection* m_connection;
    KexiDB::DriverManager m_manager;
    KexiDB::ConnectionData* m_connData;

    QString m_dbName;
    bool m_initialized;
};

}
}

#endif /* KEXIWEBFORMS_DATAPROVIDER_H */
