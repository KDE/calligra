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
#include "kexidbdrivermanager.h"
#include "kexidbdriver.h"
#include "kexidbcursor.h"
#include "kexidbfieldlist.h"
#include "kexidbschema.h"
#include "kexidbtransaction.h"
#include "kexidbparser.h"

#include <api/exception.h>

#include <kdebug.h>

#include <kexidb/transaction.h>

using namespace Kross::KexiDB;

KexiDBConnection::KexiDBConnection(::KexiDB::Connection* connection, KexiDBDriver* driver, KexiDBConnectionData* connectiondata)
    : Kross::Api::Class<KexiDBConnection>("KexiDBConnection", driver ? driver : new KexiDBDriver(connection->driver()))
    , m_connection(connection)
    , m_connectiondata(connectiondata ? connectiondata : new KexiDBConnectionData(connection->data()))
{
    addFunction("data", &KexiDBConnection::data);
    addFunction("driver", &KexiDBConnection::driver);
    addFunction("connect", &KexiDBConnection::connect);
    addFunction("isConnected", &KexiDBConnection::isConnected);
    addFunction("disconnect", &KexiDBConnection::disconnect);

    addFunction("databaseExists", &KexiDBConnection::databaseExists,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("currentDatabase", &KexiDBConnection::currentDatabase);
    addFunction("databaseNames", &KexiDBConnection::databaseNames);
    addFunction("isDatabaseUsed", &KexiDBConnection::isDatabaseUsed);
    addFunction("useDatabase", &KexiDBConnection::useDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("closeDatabase", &KexiDBConnection::closeDatabase);

    addFunction("tableNames", &KexiDBConnection::tableNames,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool", new Kross::Api::Variant(bool(false))));
    addFunction("queryNames", &KexiDBConnection::queryNames);

    addFunction("executeQueryString", &KexiDBConnection::executeQueryString,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("executeQuerySchema", &KexiDBConnection::executeQuerySchema,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBQuerySchema"));
    addFunction("querySingleString", &KexiDBConnection::querySingleString,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::UInt", new Kross::Api::Variant(0)));
    addFunction("queryStringList", &KexiDBConnection::queryStringList,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::Api::Variant::String")
            << Kross::Api::Argument("Kross::Api::Variant::Bool", new Kross::Api::Variant(bool(true))));
    addFunction("querySingleRecord", &KexiDBConnection::querySingleRecord,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("insertRecord", &KexiDBConnection::insertRecord);
    addFunction("createDatabase", &KexiDBConnection::createDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("dropDatabase", &KexiDBConnection::dropDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("createTable", &KexiDBConnection::createTable,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema")
            << Kross::Api::Argument("Kross::Api::Variant::Bool", new Kross::Api::Variant(false)));
    addFunction("dropTable", &KexiDBConnection::dropTable,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("alterTable", &KexiDBConnection::alterTable,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema")
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema"));
    addFunction("alterTableName", &KexiDBConnection::alterTableName,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema")
            << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("tableSchema", &KexiDBConnection::tableSchema,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("isEmptyTable", &KexiDBConnection::isEmptyTable,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema"));

    addFunction("querySchema", &KexiDBConnection::querySchema,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("autoCommit", &KexiDBConnection::autoCommit);
    addFunction("setAutoCommit", &KexiDBConnection::setAutoCommit,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::Bool"));
    addFunction("beginTransaction", &KexiDBConnection::beginTransaction);
    addFunction("commitTransaction", &KexiDBConnection::commitTransaction,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTransaction"));
    addFunction("rollbackTransaction", &KexiDBConnection::rollbackTransaction,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTransaction"));
    addFunction("defaultTransaction", &KexiDBConnection::defaultTransaction);
    addFunction("setDefaultTransaction", &KexiDBConnection::setDefaultTransaction,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTransaction"));
    addFunction("transactions", &KexiDBConnection::transactions);

    addFunction("parser", &KexiDBConnection::parser);
}

KexiDBConnection::~KexiDBConnection()
{
}

const QString KexiDBConnection::getClassName() const
{
    return "Kross::KexiDB::KexiDBConnection";
}

::KexiDB::Connection* KexiDBConnection::connection()
{
    if(! m_connection)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Connection is NULL.")) );
    if(m_connection->error())
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Connection error: %1").arg(m_connection->errorMsg())) );
    return m_connection;
}

Kross::Api::Object::Ptr KexiDBConnection::data(Kross::Api::List::Ptr)
{
    return m_connectiondata;
}

Kross::Api::Object::Ptr KexiDBConnection::driver(Kross::Api::List::Ptr)
{
    if(! getParent()) // We don't check getParent()->driver() here!
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Invalid driver - KexiDBConnection::driver() is NULL.")) );
    return getParent(); // the parent object is our KexiDBDriver* instance
}

Kross::Api::Object::Ptr KexiDBConnection::connect(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(bool(connection()->connect()),
           "Kross::KexiDB::Connection::connect::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::isConnected(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(bool(connection()->isConnected()),
           "Kross::KexiDB::Connection::isConnected::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::disconnect(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(bool(connection()->disconnect()),
           "Kross::KexiDB::Connection::disconnect::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::databaseExists(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           bool(connection()->databaseExists(Kross::Api::Variant::toString(args->item(0)))),
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
    return new Kross::Api::Variant(bool(connection()->isDatabaseUsed()),
           "Kross::KexiDB::Connection::isDatabaseUsed::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::useDatabase(Kross::Api::List::Ptr args)
{
    QString dbname = Kross::Api::Variant::toString(args->item(0));
    if(connection()->databaseExists(dbname))
        return new Kross::Api::Variant(bool(m_connection->useDatabase(dbname)),
               "Kross::KexiDB::Connection::useDatabase::Bool");
    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("There exists no database with the name '%1'.").arg(dbname)) );
}

Kross::Api::Object::Ptr KexiDBConnection::closeDatabase(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(bool(connection()->closeDatabase()),
           "Kross::KexiDB::Connection::closeDatabase::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::tableNames(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           connection()->tableNames(Kross::Api::Variant::toBool(args->item(0))),
           "Kross::KexiDB::Connection::tableNames::StringList");
}

Kross::Api::Object::Ptr KexiDBConnection::queryNames(Kross::Api::List::Ptr)
{
    bool ok = true;
    QStringList queries = connection()->objectNames(::KexiDB::QueryObjectType, &ok);
    if(! ok)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to determinate querynames.")) );
    return new Kross::Api::Variant(queries, "Kross::KexiDB::Connection::queryNames::StringList");
}

Kross::Api::Object::Ptr KexiDBConnection::executeQueryString(Kross::Api::List::Ptr args)
{
    ::KexiDB::Cursor* cursor = connection()->executeQuery(
        Kross::Api::Variant::toString(args->item(0))
    );
    if(! cursor)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to execute querystring.")) );
    return new KexiDBCursor(this, cursor);
}

Kross::Api::Object::Ptr KexiDBConnection::executeQuerySchema(Kross::Api::List::Ptr args)
{
    ::KexiDB::Cursor* cursor = connection()->executeQuery(
        *Kross::Api::Object::fromObject<KexiDBQuerySchema>(args->item(0))->queryschema()
    );
    if(! cursor)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to execute queryschema.")) );
    return new KexiDBCursor(this, cursor);
}

Kross::Api::Object::Ptr KexiDBConnection::querySingleString(Kross::Api::List::Ptr args)
{
    QString sql = Kross::Api::Variant::toString(args->item(0));
    uint column = Kross::Api::Variant::toUInt(args->item(1));
    QString value;
    if(connection()->querySingleString(sql, value, column))
        return new Kross::Api::Variant(value,
               "Kross::KexiDB::Connection::querySingleString::String");
    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("The string query failed.")) );
}

Kross::Api::Object::Ptr KexiDBConnection::queryStringList(Kross::Api::List::Ptr args)
{
    QString sql = Kross::Api::Variant::toString(args->item(0));
    uint column = Kross::Api::Variant::toUInt(args->item(1));
    QStringList valuelist;
    if(connection()->queryStringList(sql, valuelist, column))
        return new Kross::Api::Variant(valuelist,
               "Kross::KexiDB::Connection::queryStringList::StringList");
    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to query stringlist.")) );
}

Kross::Api::Object::Ptr KexiDBConnection::querySingleRecord(Kross::Api::List::Ptr args)
{
    QValueVector<QVariant> list;
    if(connection()->querySingleRecord(Kross::Api::Variant::toString(args->item(0)), list)) {
        QValueList<QVariant> l; //FIXME isn't there a better/faster way to deal with QValueVector<QVariant> => QVariant ?!
        for(QValueVector<QVariant>::Iterator it = list.begin(); it != list.end(); ++it)
            l.append(*it);
        return new Kross::Api::Variant(l,
               "Kross::KexiDB::Connection::querySingleRecord::ValueList");
    }
    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to query single record.")) );
}

Kross::Api::Object::Ptr KexiDBConnection::insertRecord(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(bool(
               connection()->insertRecord(
                    *Kross::Api::Object::fromObject<KexiDBFieldList>(args->item(0))->fieldlist(),
                    Kross::Api::Variant::toVariant(args->item(1))
               )), "Kross::KexiDB::Connection::insertRecord::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::createDatabase(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           bool(connection()->createDatabase(Kross::Api::Variant::toString(args->item(0)))),
           "Kross::KexiDB::Connection::createDatabase::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::dropDatabase(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           bool(connection()->dropDatabase(Kross::Api::Variant::toString(args->item(0)))),
           "Kross::KexiDB::Connection::dropDatabase::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::createTable(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(bool(
               connection()->createTable(
                    Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
                    Kross::Api::Variant::toBool(args->item(1)) // replace existing tables
               )), "Kross::KexiDB::Connection::createTable::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::dropTable(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(bool(
                   connection()->dropTable(Kross::Api::Variant::toString(args->item(0)))
               ), "Kross::KexiDB::Connection::dropTable::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::alterTable(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(bool(
               connection()->alterTable(
                   *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
                   *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(1))->tableschema()
               )), "Kross::KexiDB::Connection::alterTable::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::alterTableName(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           bool(connection()->alterTableName(
               *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
               Kross::Api::Variant::toString(args->item(1))
           )), "Kross::KexiDB::Connection::alterTableName::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::tableSchema(Kross::Api::List::Ptr args)
{
    ::KexiDB::TableSchema* tableschema = connection()->tableSchema( Kross::Api::Variant::toString(args->item(0)) );
    if(! tableschema)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("No such tableschema.")) );
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

Kross::Api::Object::Ptr KexiDBConnection::querySchema(Kross::Api::List::Ptr args)
{
    ::KexiDB::QuerySchema* queryschema = connection()->querySchema( Kross::Api::Variant::toString(args->item(0)) );
    if(! queryschema)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("No such queryschema.")) );
    return new KexiDBQuerySchema(queryschema);
}

Kross::Api::Object::Ptr KexiDBConnection::autoCommit(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(bool(connection()->autoCommit()),
           "Kross::KexiDB::Connection::autoCommit::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::setAutoCommit(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           bool(connection()->setAutoCommit(Kross::Api::Variant::toBool(args->item(0)))),
           "Kross::KexiDB::Connection::setAutoCommit::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::beginTransaction(Kross::Api::List::Ptr)
{
    ::KexiDB::Transaction t = connection()->beginTransaction();
    return new KexiDBTransaction(this, t);
}

Kross::Api::Object::Ptr KexiDBConnection::commitTransaction(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(bool(
           connection()->commitTransaction(
               Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction()
           )), "Kross::KexiDB::Connection::commitTransaction::Bool");
}

Kross::Api::Object::Ptr KexiDBConnection::rollbackTransaction(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(bool(
           connection()->rollbackTransaction(
               Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction()
           )), "Kross::KexiDB::Connection::rollbackTransaction::Bool");
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
    return 0; // no returnvalue
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

