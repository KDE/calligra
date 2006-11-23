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

#include <kdebug.h>

#include <kexidb/transaction.h>
//Added by qt3to4:
//#include <Q3ValueList>

using namespace Kross::KexiDB;

KexiDBConnection::KexiDBConnection(::KexiDB::Connection* connection, KexiDBDriver* driver, KexiDBConnectionData* connectiondata)
    : QObject()
    //, m_connection(connection)
    //, m_connectiondata(connectiondata ? connectiondata : new KexiDBConnectionData(connection->data()))
    //, m_driver(driver ? driver : new KexiDBDriver(connection->driver()))
{
    setObjectName("KexiDBConnection");

/*
    this->addFunction0< Kross::Api::Variant >("hadError", this, &KexiDBConnection::hadError);
    this->addFunction0< Kross::Api::Variant >("lastError", this, &KexiDBConnection::lastError);

    this->addFunction0< KexiDBConnectionData >("data", this, &KexiDBConnection::data);
    this->addFunction0< KexiDBDriver >("driver", this, &KexiDBConnection::driver);

    this->addFunction0< Kross::Api::Variant >("connect", this, &KexiDBConnection::connect);
    this->addFunction0< Kross::Api::Variant >("isConnected", this, &KexiDBConnection::isConnected);
    this->addFunction0< Kross::Api::Variant >("disconnect", this, &KexiDBConnection::disconnect);

    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("databaseExists", this, &KexiDBConnection::databaseExists);
    this->addFunction0< Kross::Api::Variant >("currentDatabase", this, &KexiDBConnection::currentDatabase);
    this->addFunction0< Kross::Api::Variant >("databaseNames", this, &KexiDBConnection::databaseNames);
    this->addFunction0< Kross::Api::Variant >("isDatabaseUsed", this, &KexiDBConnection::isDatabaseUsed);
    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("useDatabase", this, &KexiDBConnection::useDatabase);
    this->addFunction0< Kross::Api::Variant >("closeDatabase", this, &KexiDBConnection::closeDatabase);

    this->addFunction0< Kross::Api::Variant >("tableNames", this, &KexiDBConnection::tableNames);
    this->addFunction0< Kross::Api::Variant >("queryNames", this, &KexiDBConnection::queryNames);

    this->addFunction1< KexiDBCursor, Kross::Api::Variant >("executeQueryString", this, &KexiDBConnection::executeQueryString);
    this->addFunction1< KexiDBCursor, KexiDBQuerySchema >("executeQuerySchema", this, &KexiDBConnection::executeQuerySchema);

    addFunction("insertRecord", &KexiDBConnection::insertRecord);

    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("createDatabase", this, &KexiDBConnection::createDatabase);
    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("dropDatabase", this, &KexiDBConnection::dropDatabase);

    this->addFunction1< Kross::Api::Variant, KexiDBTableSchema >("createTable", this, &KexiDBConnection::createTable);
    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("dropTable", this, &KexiDBConnection::dropTable);
    this->addFunction2< Kross::Api::Variant, KexiDBTableSchema, KexiDBTableSchema >("alterTable", this, &KexiDBConnection::alterTable);
    this->addFunction2< Kross::Api::Variant, KexiDBTableSchema, Kross::Api::Variant >("alterTableName", this, &KexiDBConnection::alterTableName);

    this->addFunction1< KexiDBTableSchema, Kross::Api::Variant >("tableSchema", this, &KexiDBConnection::tableSchema);
    this->addFunction1< Kross::Api::Variant, KexiDBTableSchema >("isEmptyTable", this, &KexiDBConnection::isEmptyTable);
    this->addFunction1< KexiDBQuerySchema, Kross::Api::Variant >("querySchema", this, &KexiDBConnection::querySchema);

    this->addFunction0< Kross::Api::Variant >("autoCommit", this, &KexiDBConnection::autoCommit);
    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("setAutoCommit", this, &KexiDBConnection::setAutoCommit);

    this->addFunction0< KexiDBTransaction >("beginTransaction", this, &KexiDBConnection::beginTransaction);
    this->addFunction1< Kross::Api::Variant, KexiDBTransaction >("commitTransaction", this, &KexiDBConnection::commitTransaction);
    this->addFunction1< Kross::Api::Variant, KexiDBTransaction >("rollbackTransaction", this, &KexiDBConnection::rollbackTransaction);
    this->addFunction0< KexiDBTransaction >("defaultTransaction", this, &KexiDBConnection::defaultTransaction);
    this->addFunction1< void, KexiDBTransaction >("setDefaultTransaction", this, &KexiDBConnection::setDefaultTransaction);
    this->addFunction0<Kross::Api::List>("transactions", this, &KexiDBConnection::transactions);

    this->addFunction0< KexiDBParser >("parser", this, &KexiDBConnection::parser);
*/
}

KexiDBConnection::~KexiDBConnection() {
}

#if 0
::KexiDB::Connection* KexiDBConnection::connection() const {
    if(! m_connection)
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Connection is NULL.")) );
    //if(m_connection->error())
    //    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("KexiDB::Connection error: %1").arg(m_connection->errorMsg())) );
    return m_connection;
}

bool KexiDBConnection::hadError() const { return connection()->error(); }
const QString KexiDBConnection::lastError() const { return connection()->errorMsg(); }

KexiDBConnectionData* KexiDBConnection::data() { return m_connectiondata.data(); }
KexiDBDriver* KexiDBConnection::driver() { return m_driver.data(); }

bool KexiDBConnection::connect() { return connection()->connect(); }
bool KexiDBConnection::isConnected() { return connection()->isConnected(); }
bool KexiDBConnection::disconnect() { return connection()->disconnect(); }

bool KexiDBConnection::isReadOnly() const { return connection()->isReadOnly(); }

bool KexiDBConnection::databaseExists(const QString& dbname) { return connection()->databaseExists(dbname); }
const QString KexiDBConnection::currentDatabase() const { return connection()->currentDatabase(); }
const QStringList KexiDBConnection::databaseNames() const { return connection()->databaseNames(); }
bool KexiDBConnection::isDatabaseUsed() const { return connection()->isDatabaseUsed(); }
bool KexiDBConnection::useDatabase(const QString& dbname) { return connection()->databaseExists(dbname) && m_connection->useDatabase(dbname); }
bool KexiDBConnection::closeDatabase() { return connection()->closeDatabase(); }

const QStringList KexiDBConnection::allTableNames() const { return connection()->tableNames(true); }
const QStringList KexiDBConnection::tableNames() const { return connection()->tableNames(false); }

const QStringList KexiDBConnection::queryNames() const {
    bool ok = true;
    QStringList queries = connection()->objectNames(::KexiDB::QueryObjectType, &ok);
    if(! ok) throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to determinate querynames.")) );
    return queries;
}

KexiDBCursor* KexiDBConnection::executeQueryString(const QString& sqlquery) {
    // The ::KexiDB::Connection::executeQuery() method does not check if we pass a valid SELECT-statement
    // or e.g. a DROP TABLE operation. So, let's check for such dangerous operations right now.
    ::KexiDB::Parser parser( connection() );
    if(! parser.parse(sqlquery))
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Failed to parse query: %1 %2").arg(parser.error().type()).arg(parser.error().error())) );
    if( parser.query() == 0 || parser.operation() != ::KexiDB::Parser::OP_Select )
        throw Kross::Api::Exception::Ptr( new Kross::Api::Exception(QString("Invalid query operation \"%1\"").arg(parser.operationString()) ) );
    ::KexiDB::Cursor* cursor = connection()->executeQuery(sqlquery);
    return cursor ? new KexiDBCursor(cursor) : 0;
}

KexiDBCursor* KexiDBConnection::executeQuerySchema(KexiDBQuerySchema* queryschema) {
    ::KexiDB::Cursor* cursor = connection()->executeQuery( *queryschema->queryschema() );
    return cursor ? new KexiDBCursor(cursor) : 0;
}

/*TODO
bool KexiDBConnection::insertRecordIntoFieldlist(KexiDBFieldList* fieldlist, Q3ValueList<QVariant> values) {
    return connection()->insertRecord(*fieldlist->fieldlist(), values);
}

bool KexiDBConnection::insertRecordIntoTable(KexiDBTableSchema* tableschema, Q3ValueList<QVariant> values) {
    return connection()->insertRecord(*tableschema->tableschema(), values);
}
*/
Kross::Api::Object::Ptr KexiDBConnection::insertRecord(Kross::Api::List::Ptr args) {
    Q3ValueList<QVariant> values = Kross::Api::Variant::toList(args->item(1));
    Kross::Api::Object::Ptr obj = args->item(0);
    if(obj->getClassName() == "Kross::KexiDB::KexiDBFieldList")
        return new Kross::Api::Variant(
                   QVariant(connection()->insertRecord(
                       *Kross::Api::Object::fromObject<KexiDBFieldList>(obj)->fieldlist(),
                       values
                   ), 0));
    return new Kross::Api::Variant(
               QVariant(connection()->insertRecord(
                   *Kross::Api::Object::fromObject<KexiDBTableSchema>(obj)->tableschema(),
                   values
               ), 0));
}

bool KexiDBConnection::createDatabase(const QString& dbname) { return connection()->createDatabase(dbname); }
bool KexiDBConnection::dropDatabase(const QString& dbname) { return connection()->dropDatabase(dbname); }

bool KexiDBConnection::createTable(KexiDBTableSchema* tableschema) { return connection()->createTable(tableschema->tableschema(), false); }
bool KexiDBConnection::dropTable(const QString& tablename) { return true == connection()->dropTable(tablename); }
bool KexiDBConnection::alterTable(KexiDBTableSchema* fromschema, KexiDBTableSchema* toschema) { return true == connection()->alterTable(*fromschema->tableschema(), *toschema->tableschema()); }
bool KexiDBConnection::alterTableName(KexiDBTableSchema* tableschema, const QString& newtablename) { return connection()->alterTableName(*tableschema->tableschema(), newtablename); }

KexiDBTableSchema* KexiDBConnection::tableSchema(const QString& tablename) const {
    ::KexiDB::TableSchema* tableschema = connection()->tableSchema(tablename);
    return tableschema ? new KexiDBTableSchema(tableschema) : 0;
}

bool KexiDBConnection::isEmptyTable(KexiDBTableSchema* tableschema) const {
    bool success;
    bool notempty = connection()->isEmpty(*tableschema->tableschema(), success);
    return (! (success && notempty));
}

KexiDBQuerySchema* KexiDBConnection::querySchema(const QString& queryname) const {
    ::KexiDB::QuerySchema* queryschema = connection()->querySchema(queryname);
    return queryschema ? new KexiDBQuerySchema(queryschema) : 0;
}

bool KexiDBConnection::autoCommit() const { return connection()->autoCommit(); }
bool KexiDBConnection::setAutoCommit(bool enabled) { return connection()->setAutoCommit(enabled); }

KexiDBTransaction* KexiDBConnection::beginTransaction() {
    ::KexiDB::Transaction t = connection()->beginTransaction();
    return new KexiDBTransaction(t);
}

bool KexiDBConnection::commitTransaction(KexiDBTransaction* transaction) { return connection()->commitTransaction( transaction->transaction() ); }
bool KexiDBConnection::rollbackTransaction(KexiDBTransaction* transaction) { return connection()->rollbackTransaction( transaction->transaction() ); }
KexiDBTransaction* KexiDBConnection::defaultTransaction() { return new KexiDBTransaction( connection()->defaultTransaction() ); }
void KexiDBConnection::setDefaultTransaction(KexiDBTransaction* transaction) { connection()->setDefaultTransaction( transaction->transaction() ); }

Kross::Api::List* KexiDBConnection::transactions() {
    return new Kross::Api::ListT<KexiDBTransaction>( connection()->transactions() );
}

KexiDBParser* KexiDBConnection::parser() { return new KexiDBParser(this, new ::KexiDB::Parser(connection())); }
#endif
