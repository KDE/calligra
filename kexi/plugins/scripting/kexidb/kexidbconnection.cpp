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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
//Added by qt3to4:
#include <Q3ValueList>

using namespace Kross::KexiDB;

KexiDBConnection::KexiDBConnection(::KexiDB::Connection* connection, KexiDBDriver* driver, KexiDBConnectionData* connectiondata)
    : Kross::Api::Class<KexiDBConnection>("KexiDBConnection", driver ? driver : new KexiDBDriver(connection->driver()))
    , m_connection(connection)
    , m_connectiondata(connectiondata ? connectiondata : new KexiDBConnectionData(connection->data()))
{
    addFunction("hadError", &KexiDBConnection::hadError);
    addFunction("lastError", &KexiDBConnection::lastError);

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
    addFunction("isReadOnly", &KexiDBConnection::isReadOnly);

    addFunction("useDatabase", &KexiDBConnection::useDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("closeDatabase", &KexiDBConnection::closeDatabase);

    addFunction("tableNames", &KexiDBConnection::tableNames);
    addFunction("queryNames", &KexiDBConnection::queryNames);

    addFunction("executeQueryString", &KexiDBConnection::executeQueryString,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("executeQuerySchema", &KexiDBConnection::executeQuerySchema,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBQuerySchema"));
    addFunction("querySingleString", &KexiDBConnection::querySingleString,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("queryStringList", &KexiDBConnection::queryStringList,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("querySingleRecord", &KexiDBConnection::querySingleRecord,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("insertRecord", &KexiDBConnection::insertRecord,
        Kross::Api::ArgumentList()
            << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema")
            << Kross::Api::Argument("Kross::Api::List"));

    addFunction("createDatabase", &KexiDBConnection::createDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));
    addFunction("dropDatabase", &KexiDBConnection::dropDatabase,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant::String"));

    addFunction("createTable", &KexiDBConnection::createTable,
        Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::KexiDB::KexiDBTableSchema"));
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
    //if(m_connection->error())
    //    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Connection error: %1").arg(m_connection->errorMsg())) );
    return m_connection;
}

Kross::Api::Object::Ptr KexiDBConnection::hadError(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(connection()->error(),0));
}

Kross::Api::Object::Ptr KexiDBConnection::lastError(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->errorMsg());
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
    return new Kross::Api::Variant(QVariant(connection()->connect(),0));
}

Kross::Api::Object::Ptr KexiDBConnection::isConnected(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(connection()->isConnected(),0));
}

Kross::Api::Object::Ptr KexiDBConnection::disconnect(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(connection()->disconnect(),0));
}

Kross::Api::Object::Ptr KexiDBConnection::databaseExists(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           QVariant(connection()->databaseExists(Kross::Api::Variant::toString(args->item(0))),0));
}

Kross::Api::Object::Ptr KexiDBConnection::currentDatabase(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->currentDatabase());
}

Kross::Api::Object::Ptr KexiDBConnection::databaseNames(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(connection()->databaseNames());
}

Kross::Api::Object::Ptr KexiDBConnection::isDatabaseUsed(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(connection()->isDatabaseUsed(),0));
}

Kross::Api::Object::Ptr KexiDBConnection::isReadOnly(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(connection()->isReadOnly(),0));
}

Kross::Api::Object::Ptr KexiDBConnection::useDatabase(Kross::Api::List::Ptr args)
{
    QString dbname = Kross::Api::Variant::toString(args->item(0));
    bool ok = connection()->databaseExists(dbname);
    if(ok) ok = m_connection->useDatabase(dbname);
    return new Kross::Api::Variant(QVariant(ok,0));
}

Kross::Api::Object::Ptr KexiDBConnection::closeDatabase(Kross::Api::List::Ptr)
{
    return new Kross::Api::Variant(QVariant(connection()->closeDatabase(),0));
}

Kross::Api::Object::Ptr KexiDBConnection::tableNames(Kross::Api::List::Ptr args)
{
    bool systables = args->count() > 0 ? Kross::Api::Variant::toBool(args->item(0)) : false;
    return new Kross::Api::Variant(connection()->tableNames(systables));
}

Kross::Api::Object::Ptr KexiDBConnection::queryNames(Kross::Api::List::Ptr)
{
    bool ok = true;
    QStringList queries = connection()->objectNames(::KexiDB::QueryObjectType, &ok);
    if(! ok)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to determinate querynames.")) );
    return new Kross::Api::Variant(queries);
}

Kross::Api::Object::Ptr KexiDBConnection::executeQueryString(Kross::Api::List::Ptr args)
{
    QString querystatement = Kross::Api::Variant::toString(args->item(0));

    // The ::KexiDB::Connection::executeQuery() method does not check if we pass
    // a valid SELECT-statement or e.g. a DROP TABLE operation. So, let's check
    // for such dangerous operations right now.
    ::KexiDB::Parser parser( connection() );
    if(! parser.parse(querystatement))
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to parse query: %1 %2").arg(parser.error().type()).arg(parser.error().error())) );
    if( parser.query() == 0 || parser.operation() != ::KexiDB::Parser::OP_Select )
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Invalid query operation \"%1\"").arg(parser.operationString()) ) );

    ::KexiDB::Cursor* cursor = connection()->executeQuery(querystatement);
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
    uint column = args->count() > 1 ? Kross::Api::Variant::toUInt(args->item(1)) : 0;
    QString value;
    if(connection()->querySingleString(sql, value, column))
        return new Kross::Api::Variant(value);
    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("The string query failed.")) );
}

Kross::Api::Object::Ptr KexiDBConnection::queryStringList(Kross::Api::List::Ptr args)
{
    QString sql = Kross::Api::Variant::toString(args->item(0));
    uint column = args->count() > 1 ? Kross::Api::Variant::toUInt(args->item(1)) : 0;
    QStringList valuelist;
    if(connection()->queryStringList(sql, valuelist, column))
        return new Kross::Api::Variant(valuelist);
    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to query stringlist.")) );
}

Kross::Api::Object::Ptr KexiDBConnection::querySingleRecord(Kross::Api::List::Ptr args)
{
    Q3ValueVector<QVariant> list;
    if(connection()->querySingleRecord(Kross::Api::Variant::toString(args->item(0)), list)) {
        Q3ValueList<QVariant> l; //FIXME isn't there a better/faster way to deal with QValueVector<QVariant> => QVariant ?!
        for(Q3ValueVector<QVariant>::Iterator it = list.begin(); it != list.end(); ++it)
            l.append(*it);
        return new Kross::Api::Variant(l);
    }
    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to query single record.")) );
}

Kross::Api::Object::Ptr KexiDBConnection::insertRecord(Kross::Api::List::Ptr args)
{
    Q3ValueList<QVariant> values = Kross::Api::Variant::toList(args->item(1));
    /*
    kDebug()<<"Kross::KexiDB::KexiDBConnection::insertRecord()"<<endl;
    for(QValueList<QVariant>::Iterator it = values.begin(); it != values.end(); ++it)
        kDebug()<<"  value="<< (*it).toString() << " type=" << (*it).typeName() << endl;
    */

    Kross::Api::Object::Ptr obj = args->item(0);
    if(obj->getClassName() == "Kross::KexiDB::KexiDBFieldList") {
        return new Kross::Api::Variant(
                   QVariant(connection()->insertRecord(
                       *Kross::Api::Object::fromObject<KexiDBFieldList>(obj)->fieldlist(),
                       values
                   ), 0));
    }
    return new Kross::Api::Variant(
               QVariant(connection()->insertRecord(
                   *Kross::Api::Object::fromObject<KexiDBTableSchema>(obj)->tableschema(),
                   values
               ), 0));
}

Kross::Api::Object::Ptr KexiDBConnection::createDatabase(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           QVariant(connection()->createDatabase(Kross::Api::Variant::toString(args->item(0))),0));
}

Kross::Api::Object::Ptr KexiDBConnection::dropDatabase(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           QVariant(connection()->dropDatabase(Kross::Api::Variant::toString(args->item(0))),0));
}

Kross::Api::Object::Ptr KexiDBConnection::createTable(Kross::Api::List::Ptr args)
{
    bool replace = args->count() > 1 ? Kross::Api::Variant::toBool(args->item(1)) : false;
    return new Kross::Api::Variant(QVariant(
               connection()->createTable(
                    Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
                    replace) // replace existing tables
               ,0));
}

Kross::Api::Object::Ptr KexiDBConnection::dropTable(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(QVariant(
                   connection()->dropTable(Kross::Api::Variant::toString(args->item(0)))
               ,0));
}

Kross::Api::Object::Ptr KexiDBConnection::alterTable(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(QVariant(
               connection()->alterTable(
                   *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
                   *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(1))->tableschema() )
               ,0));
}

Kross::Api::Object::Ptr KexiDBConnection::alterTableName(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(QVariant(
           connection()->alterTableName(
               *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
               Kross::Api::Variant::toString(args->item(1)) )
           ,0));
}

Kross::Api::Object::Ptr KexiDBConnection::tableSchema(Kross::Api::List::Ptr args)
{
    ::KexiDB::TableSchema* tableschema = connection()->tableSchema( Kross::Api::Variant::toString(args->item(0)) );
    if(! tableschema)
        return 0; //throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("No such tableschema.")) );
    return new KexiDBTableSchema(tableschema);
}

Kross::Api::Object::Ptr KexiDBConnection::isEmptyTable(Kross::Api::List::Ptr args)
{
    bool success;
    bool notempty =
        connection()->isEmpty(
            *Kross::Api::Object::fromObject<KexiDBTableSchema>(args->item(0))->tableschema(),
            success);
    return new Kross::Api::Variant(QVariant(! (success && notempty), 0));
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
    return new Kross::Api::Variant(QVariant(connection()->autoCommit(), 0));
}

Kross::Api::Object::Ptr KexiDBConnection::setAutoCommit(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(
           QVariant(connection()->setAutoCommit(Kross::Api::Variant::toBool(args->item(0))), 0) );
}

Kross::Api::Object::Ptr KexiDBConnection::beginTransaction(Kross::Api::List::Ptr)
{
    ::KexiDB::Transaction t = connection()->beginTransaction();
    return new KexiDBTransaction(this, t);
}

Kross::Api::Object::Ptr KexiDBConnection::commitTransaction(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(QVariant(
           connection()->commitTransaction(
               Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction() )
           , 0));
}

Kross::Api::Object::Ptr KexiDBConnection::rollbackTransaction(Kross::Api::List::Ptr args)
{
    return new Kross::Api::Variant(QVariant(
           connection()->rollbackTransaction(
               Kross::Api::Object::fromObject<KexiDBTransaction>(args->item(0))->transaction() )
           , 0));
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
    Q3ValueList<Kross::Api::Object::Ptr> l;
    Q3ValueList< ::KexiDB::Transaction > list = connection()->transactions();
    for(Q3ValueList< ::KexiDB::Transaction >::Iterator it = list.begin(); it != list.end(); ++it)
        l.append( new KexiDBTransaction(this, *it) );
    return new Kross::Api::List(l);
}

Kross::Api::Object::Ptr KexiDBConnection::parser(Kross::Api::List::Ptr)
{
    return new KexiDBParser(this, new ::KexiDB::Parser(connection()));
}

