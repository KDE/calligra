/***************************************************************************
 * kexidbconnection.cpp
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

#include "kexidbconnection.h"
#include "kexidbconnectiondata.h"
#include "kexidbdriver.h"
#include "kexidbcursor.h"
#include "kexidbfieldlist.h"
#include "kexidbschema.h"
#include "kexidbparser.h"

#include <kexidb/transaction.h>

#include <kdebug.h>

using namespace Scripting;

KexiDBConnection::KexiDBConnection(::KexiDB::Connection* connection, KexiDBDriver* driver, KexiDBConnectionData* connectiondata)
        : QObject()
        , m_connection(connection)
        , m_connectiondata(connectiondata ? connectiondata : new KexiDBConnectionData(this, connection->data(), false))
        , m_driver(driver ? driver : new KexiDBDriver(this, connection->driver()))
{
    setObjectName("KexiDBConnection");
}

KexiDBConnection::~KexiDBConnection()
{
}

bool KexiDBConnection::hadError() const
{
    return m_connection->error();
}
const QString KexiDBConnection::lastError() const
{
    return m_connection->errorMsg();
}

QObject* KexiDBConnection::data()
{
    return m_connectiondata;
}
QObject* KexiDBConnection::driver()
{
    return m_driver;
}

bool KexiDBConnection::connect()
{
    return m_connection->connect();
}
bool KexiDBConnection::isConnected()
{
    return m_connection->isConnected();
}
bool KexiDBConnection::disconnect()
{
    return m_connection->disconnect();
}

bool KexiDBConnection::isReadOnly() const
{
    return m_connection->isReadOnly();
}

bool KexiDBConnection::databaseExists(const QString& dbname)
{
    return m_connection->databaseExists(dbname);
}
const QString KexiDBConnection::currentDatabase() const
{
    return m_connection->currentDatabase();
}
const QStringList KexiDBConnection::databaseNames() const
{
    return m_connection->databaseNames();
}
bool KexiDBConnection::isDatabaseUsed() const
{
    return m_connection->isDatabaseUsed();
}
bool KexiDBConnection::useDatabase(const QString& dbname)
{
    return m_connection->databaseExists(dbname) && m_connection->useDatabase(dbname);
}
bool KexiDBConnection::closeDatabase()
{
    return m_connection->closeDatabase();
}

const QStringList KexiDBConnection::allTableNames() const
{
    return m_connection->tableNames(true);
}
const QStringList KexiDBConnection::tableNames() const
{
    return m_connection->tableNames(false);
}

const QStringList KexiDBConnection::queryNames() const
{
    bool ok = true;
    QStringList queries = m_connection->objectNames(::KexiDB::QueryObjectType, &ok);
    if (! ok) {
        kDebug() << QString("Failed to determinate querynames.");
        return QStringList();
    }
    return queries;
}

QObject* KexiDBConnection::executeQueryString(const QString& sqlquery)
{
    // The ::KexiDB::Connection::executeQuery() method does not check if we pass a valid SELECT-statement
    // or e.g. a DROP TABLE operation. So, let's check for such dangerous operations right now.
    ::KexiDB::Parser parser(m_connection);
    if (! parser.parse(sqlquery)) {
        kDebug() << QString("Failed to parse query: %1 %2").arg(parser.error().type()).arg(parser.error().error());
        return 0;
    }
    if (parser.query() == 0 || parser.operation() != ::KexiDB::Parser::OP_Select) {
        kDebug() << QString("Invalid query operation \"%1\"").arg(parser.operationString());
        return 0;
    }
    ::KexiDB::Cursor* cursor = m_connection->executeQuery(sqlquery);
    return cursor ? new KexiDBCursor(this, cursor, true) : 0;
}

QObject* KexiDBConnection::executeQuerySchema(KexiDBQuerySchema* queryschema)
{
    ::KexiDB::Cursor* cursor = m_connection->executeQuery(*queryschema->queryschema());
    return cursor ? new KexiDBCursor(this, cursor, true) : 0;
}

bool KexiDBConnection::insertRecord(QObject* obj, const QVariantList& values)
{
    KexiDBFieldList* fieldlist = dynamic_cast< KexiDBFieldList* >(obj);
    if (fieldlist)
        return m_connection->insertRecord(*fieldlist->fieldlist(), values);
    KexiDBTableSchema* tableschema = dynamic_cast< KexiDBTableSchema* >(obj);
    if (tableschema)
        return m_connection->insertRecord(*tableschema->tableschema(), values);
    return false;
}

bool KexiDBConnection::createDatabase(const QString& dbname)
{
    return m_connection->createDatabase(dbname);
}
bool KexiDBConnection::dropDatabase(const QString& dbname)
{
    return m_connection->dropDatabase(dbname);
}

bool KexiDBConnection::createTable(KexiDBTableSchema* tableschema)
{
    return m_connection->createTable(tableschema->tableschema(), false);
}
bool KexiDBConnection::dropTable(const QString& tablename)
{
    return true == m_connection->dropTable(tablename);
}
bool KexiDBConnection::alterTable(KexiDBTableSchema* fromschema, KexiDBTableSchema* toschema)
{
    return true == m_connection->alterTable(*fromschema->tableschema(), *toschema->tableschema());
}
bool KexiDBConnection::alterTableName(KexiDBTableSchema* tableschema, const QString& newtablename)
{
    return m_connection->alterTableName(*tableschema->tableschema(), newtablename);
}

QObject* KexiDBConnection::tableSchema(const QString& tablename)
{
    ::KexiDB::TableSchema* tableschema = m_connection->tableSchema(tablename);
    return tableschema ? new KexiDBTableSchema(this, tableschema, false) : 0;
}

bool KexiDBConnection::isEmptyTable(KexiDBTableSchema* tableschema) const
{
    bool success;
    bool notempty = m_connection->isEmpty(*tableschema->tableschema(), success);
    return (!(success && notempty));
}

QObject* KexiDBConnection::querySchema(const QString& queryname)
{
    ::KexiDB::QuerySchema* queryschema = m_connection->querySchema(queryname);
    return queryschema ? new KexiDBQuerySchema(this, queryschema, false) : 0;
}

bool KexiDBConnection::autoCommit() const
{
    return m_connection->autoCommit();
}
bool KexiDBConnection::setAutoCommit(bool enabled)
{
    return m_connection->setAutoCommit(enabled);
}

#if 0
KexiDBTransaction* KexiDBConnection::beginTransaction()
{
    ::KexiDB::Transaction t = m_connection->beginTransaction();
    return new KexiDBTransaction(t);
}

bool KexiDBConnection::commitTransaction(KexiDBTransaction* transaction)
{
    return m_connection->commitTransaction(transaction->transaction());
}
bool KexiDBConnection::rollbackTransaction(KexiDBTransaction* transaction)
{
    return m_connection->rollbackTransaction(transaction->transaction());
}
KexiDBTransaction* KexiDBConnection::defaultTransaction()
{
    return new KexiDBTransaction(m_connection->defaultTransaction());
}
void KexiDBConnection::setDefaultTransaction(KexiDBTransaction* transaction)
{
    m_connection->setDefaultTransaction(transaction->transaction());
}

Kross::Api::List* KexiDBConnection::transactions()
{
    return new Kross::Api::ListT<KexiDBTransaction>(m_connection->transactions());
}
#endif

QObject* KexiDBConnection::parser()
{
    return new KexiDBParser(this, new ::KexiDB::Parser(m_connection), true);
}

#include "kexidbconnection.moc"
