/***************************************************************************
 * kexidbconnection.cpp
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
        i18n("Return the KexiDBConnectionData object that were used to "
             "create this connection.")
    );
    addFunction("driver", &KexiDBConnection::driver,
        Kross::Api::ArgumentList(),
        i18n("Return the KexiDBDriver object this connection belongs too.")
    );
    addFunction("connect", &KexiDBConnection::connect,
        Kross::Api::ArgumentList(),
        i18n("Connect and return true if we are successfully connected now.")
    );
    addFunction("isConnected", &KexiDBConnection::isConnected,
        Kross::Api::ArgumentList(),
        i18n("Return true if we are connected.")
    );
    addFunction("disconnect", &KexiDBConnection::disconnect,
        Kross::Api::ArgumentList(),
        i18n("Disconnect and return true if we are successfully disconnected now.")
    );

    addFunction("databaseExists", &KexiDBConnection::databaseExists,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Return true if the as argument passed databasename exists.")
    );
    addFunction("currentDatabase", &KexiDBConnection::currentDatabase,
        Kross::Api::ArgumentList(),
        i18n("Return the name of currently used database for this "
             "connection or empty string if there is no used database.")
    );
    addFunction("databaseNames", &KexiDBConnection::databaseNames,
        Kross::Api::ArgumentList(),
        i18n("Return list of database names for opened connection.")
    );
    addFunction("isDatabaseUsed", &KexiDBConnection::isDatabaseUsed,
        Kross::Api::ArgumentList(),
        i18n("Return true if connection is properly established.")
    );
    addFunction("useDatabase", &KexiDBConnection::useDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Opens an existing database specified by the as argument "
             "passed databasename.")
    );
    addFunction("closeDatabase", &KexiDBConnection::closeDatabase,
        Kross::Api::ArgumentList(),
        i18n("Closes currently used database for this connection.")
    );

    addFunction("tableNames", &KexiDBConnection::disconnect,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool", Kross::Api::Variant::create(QVariant(true))),
        i18n("Return names of all table schemas stored in currently "
             "used database. If the as argument passed boolean value "
             "is true, internal KexiDB system table names (kexi__*) "
             "are also returned.")
    );

    addFunction("executeQuery", &KexiDBConnection::executeQuery,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Executes query described by the as argument passed "
             "sqlstatement-string. Returns the opened cursor "
             "created for results of this query.")
    );
    addFunction("querySingleString", &KexiDBConnection::querySingleString,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::UInt", Kross::Api::Variant::create(0)),
        i18n("Executes the as argument passed sqlquery-string and returns "
             "the first record's field's value.")
    );
    addFunction("queryStringList", &KexiDBConnection::queryStringList,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::Bool", Kross::Api::Variant::create(QVariant(true))),
        i18n("Executes the as argument passed sqlquery-string and returns "
             "a stringlist of first record's first field's values.")
    );
    addFunction("querySingleRecord", &KexiDBConnection::querySingleRecord,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Executes sql query statement and returned the first record's valuelist.")
    );

    addFunction("insertRecord", &KexiDBConnection::insertRecord,
        Kross::Api::ArgumentList(),
        i18n("Inserts the as argument passed KexiDBField object.")
    );
    addFunction("createDatabase", &KexiDBConnection::createDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Creates new database with the as argument passed databasename.")
    );
    addFunction("dropDatabase", &KexiDBConnection::dropDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Drops the as argument passed databasename.")
    );

    addFunction("createTable", &KexiDBConnection::createTable,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema"),
        i18n("Creates table defined by the as argument passed KexiTableSchema object.")
    );
    addFunction("dropTable", &KexiDBConnection::dropTable,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema"),
        i18n("Drops table defined by the as argument passed KexiDBTableSchema object.")
    );
    addFunction("alterTable", &KexiDBConnection::alterTable,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema")
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema"),
        i18n("Alters the as first argument passed KexiDBTableSchema object "
             "using the as second argument passed KexiDBTableSchema.")
    );
    addFunction("alterTableName", &KexiDBConnection::alterTableName,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema")
            << Kross::Api::Argument("Kross::Api::Variant::String"),
        i18n("Alters the tablename of the as first argument passed "
             "KexiDBTableSchema into the as second argument passed "
             "new tablename.")
    );
    addFunction("tableSchema", &KexiDBConnection::tableSchema,
        Kross::Api::ArgumentList(),
        i18n("Returns the KexiDBTableSchema object of the table matching "
             "to the as argument passed tablename.")
    );
    addFunction("isEmptyTable", &KexiDBConnection::isEmptyTable,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema"),
        i18n("Returns true if there is at least one valid record in the "
             "as argument passed tablename.")
    );

    addFunction("autoCommit", &KexiDBConnection::autoCommit,
        Kross::Api::ArgumentList(),
        i18n("Return true if the \"auto commit\" option is on.")
    );
    addFunction("setAutoCommit", &KexiDBConnection::setAutoCommit,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"),
        i18n("Set the auto commit option. This does not affect currently "
             "started transactions and can be changed even when connection "
             "is not established.")
    );
    addFunction("beginTransaction", &KexiDBConnection::beginTransaction,
        Kross::Api::ArgumentList(),
        i18n("Creates new transaction handle and starts a new transaction.")
    );
    addFunction("commitTransaction", &KexiDBConnection::commitTransaction,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTransaction"),
        i18n("Commits the as rgument passed KexiDBTransaction object.")
    );
    addFunction("rollbackTransaction", &KexiDBConnection::rollbackTransaction,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTransaction"),
        i18n("Rollback the as argument passed KexiDBTransaction object.")
    );
    addFunction("defaultTransaction", &KexiDBConnection::defaultTransaction,
        Kross::Api::ArgumentList(),
        i18n("Return the KEXIDBTransaction object for default transaction "
             "for this connection.")
    );
    addFunction("setDefaultTransaction", &KexiDBConnection::setDefaultTransaction,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTransaction"),
        i18n("Sets default transaction that will be used as context for "
             "operations on data in opened database for this connection.")
    );
    addFunction("transactions", &KexiDBConnection::transactions,
        Kross::Api::ArgumentList(),
        i18n("Return list of currently active KexiDBTransaction objects.")
    );

    addFunction("parser", &KexiDBConnection::parser,
        Kross::Api::ArgumentList(),
        i18n("Return a KexiDBParser object.")
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
                "established with given database source.");
}

::KexiDB::Connection* KexiDBConnection::connection()
{
    if(! m_connection)
        throw Kross::Api::RuntimeException(i18n("KexiDB::Connection is NULL."));
    if(m_connection->error())
        throw Kross::Api::RuntimeException(i18n("KexiDB::Connection error: %1").arg(m_connection->errorMsg()));
    return m_connection;
}

Kross::Api::Object* KexiDBConnection::data(Kross::Api::List*)
{
    if(! m_connectiondata) // caching
        m_connectiondata = new KexiDBConnectionData( connection()->data() );
    return m_connectiondata;
}

Kross::Api::Object* KexiDBConnection::driver(Kross::Api::List*)
{
    if(! getParent()) // We don't check getParent()->driver() here!
        throw Kross::Api::RuntimeException(i18n("Invalid driver cause KexiDBConnection::driver() is NULL."));
    return getParent(); // the parent object is our KexiDBDriver* instance
}

Kross::Api::Object* KexiDBConnection::connect(Kross::Api::List*)
{
    return Kross::Api::Variant::create(connection()->connect(),
           "Kross::KexiDB::Connection::connect::Bool");
}

Kross::Api::Object* KexiDBConnection::isConnected(Kross::Api::List*)
{
    return Kross::Api::Variant::create(connection()->isConnected(),
           "Kross::KexiDB::Connection::isConnected::Bool");
}

Kross::Api::Object* KexiDBConnection::disconnect(Kross::Api::List*)
{
    return Kross::Api::Variant::create(connection()->disconnect(),
           "Kross::KexiDB::Connection::disconnect::Bool");
}

Kross::Api::Object* KexiDBConnection::databaseExists(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           connection()->databaseExists(Kross::Api::Variant::toString(args->item(0))),
           "Kross::KexiDB::Connection::databaseExists::Bool");
}

Kross::Api::Object* KexiDBConnection::currentDatabase(Kross::Api::List*)
{
    return Kross::Api::Variant::create(connection()->currentDatabase(),
           "Kross::KexiDB::Connection::currentDatabase::String");
}

Kross::Api::Object* KexiDBConnection::databaseNames(Kross::Api::List*)
{
    return Kross::Api::Variant::create(connection()->databaseNames(),
           "Kross::KexiDB::Connection::databaseNames::StringList");
}

Kross::Api::Object* KexiDBConnection::isDatabaseUsed(Kross::Api::List*)
{
    return Kross::Api::Variant::create(connection()->isDatabaseUsed(),
           "Kross::KexiDB::Connection::isDatabaseUsed::Bool");
}

Kross::Api::Object* KexiDBConnection::useDatabase(Kross::Api::List* args)
{
    QString dbname = Kross::Api::Variant::toString(args->item(0));
    if(! connection()->databaseExists(dbname))
        throw Kross::Api::TypeException(i18n("There exists no database with the name '%1'.").arg(dbname));
    return Kross::Api::Variant::create(m_connection->useDatabase(dbname),
           "Kross::KexiDB::Connection::useDatabase::Bool");
}

Kross::Api::Object* KexiDBConnection::closeDatabase(Kross::Api::List*)
{
    return Kross::Api::Variant::create(connection()->closeDatabase(),
           "Kross::KexiDB::Connection::closeDatabase::Bool");
}

Kross::Api::Object* KexiDBConnection::tableNames(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           connection()->tableNames(Kross::Api::Variant::toBool(args->item(0))),
           "Kross::KexiDB::Connection::tableNames::StringList");
}

Kross::Api::Object* KexiDBConnection::executeQuery(Kross::Api::List* args)
{
    /*
    if(connection()->currentDatabase().isNull()) //TODO move condition to ::KexiDB to prevent crashes ?!
        throw Kross::Api::RuntimeException(i18n("No database selected."));
    */

    QString sql = Kross::Api::Variant::toString(args->item(0));
    ::KexiDB::Cursor* cursor = m_connection->executeQuery(sql);
    if(! cursor)
        throw Kross::Api::RuntimeException(i18n("Failed to create cursor."));
    return new KexiDBCursor(this, cursor);
}

Kross::Api::Object* KexiDBConnection::querySingleString(Kross::Api::List* args)
{
    QString sql = Kross::Api::Variant::toString(args->item(0));
    uint column = Kross::Api::Variant::toUInt(args->item(1));
    QString value;
    if(! connection()->querySingleString(sql, value, column))
        throw Kross::Api::RuntimeException(i18n("Query the string failed."));
    return Kross::Api::Variant::create(value,
           "Kross::KexiDB::Connection::querySingleString::String");
}

Kross::Api::Object* KexiDBConnection::queryStringList(Kross::Api::List* args)
{
    QString sql = Kross::Api::Variant::toString(args->item(0));
    uint column = Kross::Api::Variant::toUInt(args->item(1));
    QStringList valuelist;
    if(! connection()->queryStringList(sql, valuelist, column))
        throw Kross::Api::RuntimeException(i18n("Query the stringlist failed."));
    return Kross::Api::Variant::create(valuelist,
           "Kross::KexiDB::Connection::queryStringList::StringList");
}

Kross::Api::Object* KexiDBConnection::querySingleRecord(Kross::Api::List* args)
{
    QValueVector<QVariant> list;
    if(! connection()->querySingleRecord(Kross::Api::Variant::toString(args->item(0)), list))
        return 0;

    QValueList<QVariant> l; //FIXME isn't there a better/faster way to deal with QValueVector<QVariant> => QVariant ?!
    for(QValueVector<QVariant>::Iterator it = list.begin(); it != list.end(); ++it)
        l.append(*it);

    return Kross::Api::Variant::create(l,
           "Kross::KexiDB::Connection::querySingleRecord::ValueList");
}

Kross::Api::Object* KexiDBConnection::insertRecord(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           connection()->insertRecord(
               *Kross::Api::Object::fromObject<KexiDBFieldList>(args->item(0))->fieldlist(),
               Kross::Api::Variant::toVariant(args->item(1)) //Kross::Api::Variant::toList(args->item(1))
           ),
           "Kross::KexiDB::Connection::insertRecord::Bool");
}

Kross::Api::Object* KexiDBConnection::createDatabase(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           connection()->createDatabase(Kross::Api::Variant::toString(args->item(0))),
           "Kross::KexiDB::Connection::createDatabase::Bool");
}

Kross::Api::Object* KexiDBConnection::dropDatabase(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           connection()->dropDatabase(Kross::Api::Variant::toString(args->item(0))),
           "Kross::KexiDB::Connection::dropDatabase::Bool");
}

Kross::Api::Object* KexiDBConnection::createTable(Kross::Api::List* args)
{
    ::KexiDB::TableSchema* tableschema = Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema();
    if(! tableschema)
        kdDebug()<<"KexiDBConnection::createTable tableschema==NULL !!!"<<endl;

    return Kross::Api::Variant::create(
           connection()->createTable(
               tableschema,
               false // replace existing tables
           ),
           "Kross::KexiDB::Connection::createTable::Bool");
}

Kross::Api::Object* KexiDBConnection::dropTable(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           QVariant(connection()->dropTable(
               Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema()
           )),
           "Kross::KexiDB::Connection::dropTable::Bool");
}

Kross::Api::Object* KexiDBConnection::alterTable(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           QVariant(connection()->alterTable(
               *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
               *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(1))->tableschema()
           )),
           "Kross::KexiDB::Connection::alterTable::Bool");
}

Kross::Api::Object* KexiDBConnection::alterTableName(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           connection()->alterTableName(
               *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
               Kross::Api::Variant::toString(args->item(1))
           ),
           "Kross::KexiDB::Connection::alterTableName::Bool");
}

Kross::Api::Object* KexiDBConnection::tableSchema(Kross::Api::List* args)
{
    ::KexiDB::TableSchema* tableschema = connection()->tableSchema( Kross::Api::Variant::toString(args->item(0)) );
    if(! tableschema) return 0;
    return new KexiDBTableSchema(tableschema);
}

Kross::Api::Object* KexiDBConnection::isEmptyTable(Kross::Api::List* args)
{
    bool success;
    bool notempty =
        connection()->isEmpty(
            *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
            success);
    return Kross::Api::Variant::create(! (success && notempty),
           "Kross::KexiDB::Connection::isEmptyTable::Bool");
}

Kross::Api::Object* KexiDBConnection::autoCommit(Kross::Api::List*)
{
    return Kross::Api::Variant::create(connection()->autoCommit(),
           "Kross::KexiDB::Connection::autoCommit::Bool");
}

Kross::Api::Object* KexiDBConnection::setAutoCommit(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           connection()->setAutoCommit(Kross::Api::Variant::toBool(args->item(0))),
           "Kross::KexiDB::Connection::setAutoCommit::Bool");
}

Kross::Api::Object* KexiDBConnection::beginTransaction(Kross::Api::List*)
{
    ::KexiDB::Transaction t = connection()->beginTransaction();
    return new KexiDBTransaction(this, t);
}

Kross::Api::Object* KexiDBConnection::commitTransaction(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           connection()->commitTransaction(
               Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction()
           ),
           "Kross::KexiDB::Connection::commitTransaction::Bool");
}

Kross::Api::Object* KexiDBConnection::rollbackTransaction(Kross::Api::List* args)
{
    return Kross::Api::Variant::create(
           connection()->rollbackTransaction(
               Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction()
           ),
           "Kross::KexiDB::Connection::rollbackTransaction::Bool");
}

Kross::Api::Object* KexiDBConnection::defaultTransaction(Kross::Api::List*)
{
    return new KexiDBTransaction(this, connection()->defaultTransaction());
}

Kross::Api::Object* KexiDBConnection::setDefaultTransaction(Kross::Api::List* args)
{
    connection()->setDefaultTransaction(
        Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction()
    );
    return 0;
}

Kross::Api::Object* KexiDBConnection::transactions(Kross::Api::List*)
{
    QValueList<Object*> l;
    QValueList< ::KexiDB::Transaction > list = connection()->transactions();
    for(QValueList< ::KexiDB::Transaction >::Iterator it = list.begin(); it != list.end(); ++it)
        l.append( new KexiDBTransaction(this, *it) );
    return Kross::Api::List::create(l,
           "Kross::KexiDB::Connection::transactions::List");
}

Kross::Api::Object* KexiDBConnection::parser(Kross::Api::List*)
{
    return new KexiDBParser(this, new ::KexiDB::Parser(connection()));
}

