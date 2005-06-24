/***************************************************************************
 * kexidbconnection.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "kexidbconnection.h"
#include "kexidbconnectiondata.h"
#include "kexidbdriver.h"
#include "kexidbcursor.h"
#include "kexidbfieldlist.h"
#include "kexidbschema.h"
#include "kexidbtransaction.h"
#include "kexidbparser.h"
#include "../api/exception.h"

//#include <qvaluelist.h>
#include <klocale.h>
#include <kdebug.h>

#include <kexidb/transaction.h>

using namespace Kross::KexiDB;

KexiDBConnection::KexiDBConnection(KexiDBDriver* driver, ::KexiDB::Connection* connection, KexiDBConnectionData* connectiondata)
    : Kross::Api::Class<KexiDBConnection>("KexiDBConnection", driver)
    , m_connection(connection)
    , m_connectiondata(connectiondata)
{
    addFunction("data", &KexiDBConnection::data,
        Kross::Api::ArgumentList(),
        i18n("Returns the KexiDBConnectionData object that was used to "
             "create this connection.")
    );
    addFunction("driver", &KexiDBConnection::driver,
        Kross::Api::ArgumentList(),
        i18n("Returns the KexiDBDriver object to which this connection belongs.")
    );
    addFunction("connect", &KexiDBConnection::connect,
        Kross::Api::ArgumentList(),
        i18n("Connects and return true if a connection was established.")
    );
    addFunction("isConnected", &KexiDBConnection::isConnected,
        Kross::Api::ArgumentList(),
        i18n("Returns true if a connection is established.")
    );
    addFunction("disconnect", &KexiDBConnection::disconnect,
        Kross::Api::ArgumentList(),
        i18n("Disconnects and return true if disconnection was successful.")
    );

    addFunction("databaseExists", &KexiDBConnection::databaseExists,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Returns true if a database with the specified name exists.")
    );
    addFunction("currentDatabase", &KexiDBConnection::currentDatabase,
        Kross::Api::ArgumentList(),
        i18n("Returns the name of currently used database for this "
             "connection, or an empty string if no database is used.")
    );
    addFunction("databaseNames", &KexiDBConnection::databaseNames,
        Kross::Api::ArgumentList(),
        i18n("Returns a list of database names for the currently opened connection.")
    );
    addFunction("isDatabaseUsed", &KexiDBConnection::isDatabaseUsed,
        Kross::Api::ArgumentList(),
        i18n("Returns true if the connection is properly established.")
    );
    addFunction("useDatabase", &KexiDBConnection::useDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Opens an existing database specified by database name passed "
             "as an argument.")
    );
    addFunction("closeDatabase", &KexiDBConnection::closeDatabase,
        Kross::Api::ArgumentList(),
        i18n("Closes the currently used database for this connection.")
    );

    addFunction("tableNames", &KexiDBConnection::tableNames,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool", new Kross::Api::Variant(QVariant(true))),
        i18n("Returns the names of all table schemata stored in the currently "
             "used database. If the boolean value passed as an argument "
             "is true, internal KexiDB system table names (kexi__*) "
             "are also returned.")
    );

    addFunction("executeQueryString", &KexiDBConnection::executeQueryString,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Executes the query specified by the SQL statement passed "
             "as a string. Returns the cursor "
             "created for results of this query.")
    );
    addFunction("executeQuerySchema", &KexiDBConnection::executeQuerySchema,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBQuerySchema"),
        i18n("Executes the query specified by the SQL statement passed "
             "as a KexiDBQuerySchema object. Returns the cursor "
             "created for results of this query.")
    );
    addFunction("querySingleString", &KexiDBConnection::querySingleString,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::UInt", new Kross::Api::Variant(0)),
        i18n("Executes the query specified by the SQL statement passed "
             "as a string, and returns the first record's field's value.")
    );
    addFunction("queryStringList", &KexiDBConnection::queryStringList,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::Bool", new Kross::Api::Variant(QVariant(true))),
        i18n("Executes the query specified by the SQL statement passed "
             "as a string, and returns a stringlist of first record's first field's values.")
    );
    addFunction("querySingleRecord", &KexiDBConnection::querySingleRecord,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Executes the query specified by the SQL statement passed "
             "as a string, and returns the first record's valuelist.")
    );

    addFunction("insertRecord", &KexiDBConnection::insertRecord,
        Kross::Api::ArgumentList(),
        i18n("Inserts the KexiDBField object passed as an argument.")
    );
    addFunction("createDatabase", &KexiDBConnection::createDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Creates new database, named by the passed string argument.")
    );
    addFunction("dropDatabase", &KexiDBConnection::dropDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Drops the database specified by the passed string argument.")
    );

    addFunction("createTable", &KexiDBConnection::createTable,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema")
            << Kross::Api::Argument("Kross::Api::Variant::Bool", new Kross::Api::Variant(false)),
        i18n("Creates a table as defined by the KexiTableSchema object passed as an argument.")
    );
    addFunction("dropTable", &KexiDBConnection::dropTable,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Drops the table specified by the KexiDBTableSchema object passed as an argument.")
    );
    addFunction("alterTable", &KexiDBConnection::alterTable,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema")
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema"),
        i18n("Alters the KexiDBTableSchema object passed as the first argument "
             "using the KexiDBTableSchema object passed as the second argument.")
    );
    addFunction("alterTableName", &KexiDBConnection::alterTableName,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema")
            << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Changes the table name of the KexiDBTableSchema object "
             "passed as the first argument to the string passed "
             "as the second argument.")
    );
    addFunction("tableSchema", &KexiDBConnection::tableSchema,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Returns the KexiDBTableSchema object of the table matching "
             "to the table name passed as an argument.")
    );
    addFunction("isEmptyTable", &KexiDBConnection::isEmptyTable,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema"),
        i18n("Returns true if there is at least one valid record in the "
             "as KexiDBTable Schema passed as argument.")
    );

    addFunction("autoCommit", &KexiDBConnection::autoCommit,
        Kross::Api::ArgumentList(),
        i18n("Returns true if the \"auto commit\" option is on.")
    );
    addFunction("setAutoCommit", &KexiDBConnection::setAutoCommit,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"),
        i18n("Sets the auto commit option. This does not affect currently "
             "started transactions and can be changed even when a connection "
             "is not established.")
    );
    addFunction("beginTransaction", &KexiDBConnection::beginTransaction,
        Kross::Api::ArgumentList(),
        i18n("Creates a new transaction handle and starts a new transaction.")
    );
    addFunction("commitTransaction", &KexiDBConnection::commitTransaction,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTransaction"),
        i18n("Commits the transaction specified by the passed KexiDBTransaction object.")
    );
    addFunction("rollbackTransaction", &KexiDBConnection::rollbackTransaction,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTransaction"),
        i18n("Rollback the transaction specified by the passed KexiDBTransaction object.")
    );
    addFunction("defaultTransaction", &KexiDBConnection::defaultTransaction,
        Kross::Api::ArgumentList(),
        i18n("Return the KEXIDBTransaction object for the default transaction "
             "for this connection.")
    );
    addFunction("setDefaultTransaction", &KexiDBConnection::setDefaultTransaction,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTransaction"),
        i18n("Sets the default transaction that will be used as context for "
             "operations on data in the opened database for this connection.")
    );
    addFunction("transactions", &KexiDBConnection::transactions,
        Kross::Api::ArgumentList(),
        i18n("Returns a list of currently active KexiDBTransaction objects.")
    );

    addFunction("parser", &KexiDBConnection::parser,
        Kross::Api::ArgumentList(),
        i18n("Returns a KexiDBParser object.")
    );
}

KexiDBConnection::~KexiDBConnection()
{
}

const QString KexiDBConnection::getClassName() const
{
    return "Kross::KexiDB::KexiDBConnection";
}

const QString KexiDBConnection::getDescription() const
{
    return i18n("KexiDB::Connection wrapper for the database "
                "connection API to enable queries and updates. "
                "This class represents a database connection "
                "established with a given database source.");
}

::KexiDB::Connection* KexiDBConnection::connection()
{
    if(! m_connection)
        throw Kross::Api::RuntimeException(i18n("KexiDB::Connection is NULL."));
    if(m_connection->error())
        throw Kross::Api::RuntimeException(i18n("KexiDB::Connection error: %1").arg(m_connection->errorMsg()));
    return m_connection;
}

Kross::Api::Object::Ptr KexiDBConnection::data(Kross::Api::List::Ptr)
{
    if(! m_connectiondata) // caching
        m_connectiondata = new KexiDBConnectionData( connection()->data() );
    return m_connectiondata;
}

Kross::Api::Object::Ptr KexiDBConnection::driver(Kross::Api::List::Ptr)
{
    if(! getParent()) // We don't check getParent()->driver() here!
        throw Kross::Api::RuntimeException(i18n("Invalid driver - KexiDBConnection::driver() is NULL."));
    return getParent(); // the parent object is our KexiDBDriver* instance
}

Kross::Api::Object::Ptr KexiDBConnection::connect(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->connect(),
           "Kross::KexiDB::Connection::connect::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::isConnected(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->isConnected(),
           "Kross::KexiDB::Connection::isConnected::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::disconnect(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->disconnect(),
           "Kross::KexiDB::Connection::disconnect::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::databaseExists(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->databaseExists(Kross::Api::Variant::toString(args->item(0))),
           "Kross::KexiDB::Connection::databaseExists::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::currentDatabase(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->currentDatabase(),
           "Kross::KexiDB::Connection::currentDatabase::String");
}

Kross::Api::Object::Ptr KexiDBConnection::databaseNames(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->databaseNames(),
           "Kross::KexiDB::Connection::databaseNames::StringList");
}

Kross::Api::Object::Ptr KexiDBConnection::isDatabaseUsed(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->isDatabaseUsed(),
           "Kross::KexiDB::Connection::isDatabaseUsed::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::useDatabase(Kross::Api::List::Ptr args)
{
    QString dbname = Kross::Api::Variant::toString(args->item(0));
    if(! connection()->databaseExists(dbname))
        throw Kross::Api::TypeException(i18n("There exists no database with the name '%1'.").arg(dbname));
    return new Kross::Api::Variant(m_connection->useDatabase(dbname),
           "Kross::KexiDB::Connection::useDatabase::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::closeDatabase(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->closeDatabase(),
           "Kross::KexiDB::Connection::closeDatabase::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::tableNames(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->tableNames(Kross::Api::Variant::toBool(args->item(0))),
           "Kross::KexiDB::Connection::tableNames::StringList");
}

Kross::Api::Object::Ptr KexiDBConnection::executeQueryString(Kross::Api::List::Ptr args)
{
    /*TODO move condition to ::KexiDB to prevent crashes ?!
    if(connection()->currentDatabase().isNull())
        throw Kross::Api::RuntimeException(i18n("No database selected."));
    */

    ::KexiDB::Cursor* cursor = connection()->executeQuery(
        Kross::Api::Variant::toString(args->item(0))
    );
    return cursor ? new KexiDBCursor(this, cursor) : 0;
}

Kross::Api::Object::Ptr KexiDBConnection::executeQuerySchema(Kross::Api::List::Ptr args)
{
    ::KexiDB::Cursor* cursor = connection()->executeQuery(
        *Kross::Api::Object::fromObject<KexiDBQuerySchema>(args->item(0))->queryschema()
    );
    return cursor ? new KexiDBCursor(this, cursor) : 0;
}

Kross::Api::Object::Ptr KexiDBConnection::querySingleString(Kross::Api::List::Ptr args)
{
    QString sql = Kross::Api::Variant::toString(args->item(0));
    uint column = Kross::Api::Variant::toUInt(args->item(1));
    QString value;
    if(! connection()->querySingleString(sql, value, column))
        throw Kross::Api::RuntimeException(i18n("The string query failed."));
    return new Kross::Api::Variant(value,
           "Kross::KexiDB::Connection::querySingleString::String");
}

Kross::Api::Object::Ptr KexiDBConnection::queryStringList(Kross::Api::List::Ptr args)
{
    QString sql = Kross::Api::Variant::toString(args->item(0));
    uint column = Kross::Api::Variant::toUInt(args->item(1));
    QStringList valuelist;
    if(! connection()->queryStringList(sql, valuelist, column))
        throw Kross::Api::RuntimeException(i18n("The stringlist query failed."));
    return new Kross::Api::Variant(valuelist,
           "Kross::KexiDB::Connection::queryStringList::StringList");
}

Kross::Api::Object::Ptr KexiDBConnection::querySingleRecord(Kross::Api::List::Ptr args)
{
    QValueVector<QVariant> list;
    if(! connection()->querySingleRecord(Kross::Api::Variant::toString(args->item(0)), list))
        return 0;

    QValueList<QVariant> l; //FIXME isn't there a better/faster way to deal with QValueVector<QVariant> => QVariant ?!
    for(QValueVector<QVariant>::Iterator it = list.begin(); it != list.end(); ++it)
        l.append(*it);

    return new Kross::Api::Variant(l,
           "Kross::KexiDB::Connection::querySingleRecord::ValueList");
}

Kross::Api::Object::Ptr KexiDBConnection::insertRecord(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->insertRecord(
               *Kross::Api::Object::fromObject<KexiDBFieldList>(args->item(0))->fieldlist(),
               Kross::Api::Variant::toVariant(args->item(1)) //Kross::Api::Variant::toList(args->item(1))
           ),
           "Kross::KexiDB::Connection::insertRecord::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::createDatabase(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->createDatabase(Kross::Api::Variant::toString(args->item(0))),
           "Kross::KexiDB::Connection::createDatabase::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::dropDatabase(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->dropDatabase(Kross::Api::Variant::toString(args->item(0))),
           "Kross::KexiDB::Connection::dropDatabase::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::createTable(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->createTable(
               Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
               Kross::Api::Variant::toBool(args->item(1)) // replace existing tables
           ),
           "Kross::KexiDB::Connection::createTable::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::dropTable(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           QVariant(connection()->dropTable(Kross::Api::Variant::toString(args->item(0)))),
           "Kross::KexiDB::Connection::dropTable::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::alterTable(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           QVariant(connection()->alterTable(
               *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
               *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(1))->tableschema()
           )),
           "Kross::KexiDB::Connection::alterTable::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::alterTableName(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->alterTableName(
               *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
               Kross::Api::Variant::toString(args->item(1))
           ),
           "Kross::KexiDB::Connection::alterTableName::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::tableSchema(Kross::Api::List::Ptr args)
{
    ::KexiDB::TableSchema* tableschema = connection()->tableSchema( Kross::Api::Variant::toString(args->item(0)) );
    if(! tableschema) return 0;
    return new KexiDBTableSchema(tableschema);
}

Kross::Api::Object::Ptr KexiDBConnection::isEmptyTable(Kross::Api::List::Ptr args)
{
    bool success;
    bool notempty =
        connection()->isEmpty(
            *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
            success);
    return new Kross::Api::Variant(! (success && notempty),
           "Kross::KexiDB::Connection::isEmptyTable::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::autoCommit(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->autoCommit(),
           "Kross::KexiDB::Connection::autoCommit::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::setAutoCommit(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->setAutoCommit(Kross::Api::Variant::toBool(args->item(0))),
           "Kross::KexiDB::Connection::setAutoCommit::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::beginTransaction(Kross::Api::List::Ptr)
{
    ::KexiDB::Transaction t = connection()->beginTransaction();
    return new KexiDBTransaction(this, t);
}

Kross::Api::Object::Ptr KexiDBConnection::commitTransaction(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->commitTransaction(
               Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction()
           ),
           "Kross::KexiDB::Connection::commitTransaction::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::rollbackTransaction(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->rollbackTransaction(
               Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction()
           ),
           "Kross::KexiDB::Connection::rollbackTransaction::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::defaultTransaction(Kross::Api::List::Ptr)
{
    return new KexiDBTransaction(this, connection()->defaultTransaction());
}

Kross::Api::Object::Ptr KexiDBConnection::setDefaultTransaction(Kross::Api::List::Ptr args)
{
    connection()->setDefaultTransaction(
        Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction()
    );
    return 0;
}

Kross::Api::Object::Ptr KexiDBConnection::transactions(Kross::Api::List::Ptr)
{
    QValueList<Kross::Api::Object::Ptr> l;
    QValueList< ::KexiDB::Transaction > list = connection()->transactions();
    for(QValueList< ::KexiDB::Transaction >::Iterator it = list.begin(); it != list.end(); ++it)
        l.append( new KexiDBTransaction(this, *it) );
    return new Kross::Api::List(l,
           "Kross::KexiDB::Connection::transactions::List");
}

Kross::Api::Object::Ptr KexiDBConnection::parser(Kross::Api::List::Ptr)
{
    return new KexiDBParser(this, new ::KexiDB::Parser(connection()));
}

