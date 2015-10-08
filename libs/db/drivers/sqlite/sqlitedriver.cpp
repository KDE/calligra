/* This file is part of the KDE project
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

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

#include "sqlitedriver.h"

#include <db/connection.h>
#include <db/drivermanager.h>
#include <db/driver_p.h>
#include <db/utils.h>
#include <db/pluginloader.h>
#include <db/expression.h>

#include "sqlite3.h"
#include "sqliteconnection.h"
#include "sqliteconnection_p.h"
#include "sqliteadmin.h"

#include <kdebug.h>

using namespace KexiDB;

K_EXPORT_KEXIDB_DRIVER(SQLiteDriver, sqlite3)

//! driver specific private data
//! @internal
class KexiDB::SQLiteDriverPrivate
{
public:
    SQLiteDriverPrivate() : collate(" COLLATE ''")
    {
    }
    QString collate;
};

//PgSqlDB::PgSqlDB(QObject *parent, const char *name, const QStringList &)
SQLiteDriver::SQLiteDriver(QObject *parent, const QVariantList &args)
        : Driver(parent, args)
        , dp(new SQLiteDriverPrivate())
{
    d->isFileDriver = true;
    d->isDBOpenedAfterCreate = true;
    d->features = SingleTransactions | CursorForward
                  | CompactingDatabaseSupported;

    //special method for autoincrement definition
    beh->SPECIAL_AUTO_INCREMENT_DEF = true;
    beh->AUTO_INCREMENT_FIELD_OPTION = ""; //not available
    beh->AUTO_INCREMENT_TYPE = "INTEGER";
    beh->AUTO_INCREMENT_PK_FIELD_OPTION = "PRIMARY KEY";
    beh->AUTO_INCREMENT_REQUIRES_PK = true;
    beh->ROW_ID_FIELD_NAME = "OID";
    beh->_1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY = true;
    beh->QUOTATION_MARKS_FOR_IDENTIFIER = '"';
    beh->SELECT_1_SUBQUERY_SUPPORTED = true;
    initDriverSpecificKeywords(keywords);

    //predefined properties
    d->properties["client_library_version"] = sqlite3_libversion();
    d->properties["default_server_encoding"] = "UTF8"; //OK?

    d->typeNames[Field::Byte] = "Byte";
    d->typeNames[Field::ShortInteger] = "ShortInteger";
    d->typeNames[Field::Integer] = "Integer";
    d->typeNames[Field::BigInteger] = "BigInteger";
    d->typeNames[Field::Boolean] = "Boolean";
    d->typeNames[Field::Date] = "Date";       // In fact date/time types could be declared as datetext etc.
    d->typeNames[Field::DateTime] = "DateTime"; // to force text affinity..., see http://sqlite.org/datatype3.html
    d->typeNames[Field::Time] = "Time";       //
    d->typeNames[Field::Float] = "Float";
    d->typeNames[Field::Double] = "Double";
    d->typeNames[Field::Text] = "Text";
    d->typeNames[Field::LongText] = "CLOB";
    d->typeNames[Field::BLOB] = "BLOB";
}

SQLiteDriver::~SQLiteDriver()
{
    delete dp;
}


KexiDB::Connection*
SQLiteDriver::drv_createConnection(ConnectionData &conn_data)
{
    return new SQLiteConnection(this, conn_data);
}

bool SQLiteDriver::isSystemObjectName(const QString& n) const
{
    return Driver::isSystemObjectName(n) || n.startsWith(QLatin1String("sqlite_"), Qt::CaseInsensitive);
}

bool SQLiteDriver::drv_isSystemFieldName(const QString& n) const
{
    QString lcName = n.toLower();
    return (lcName == "_rowid_")
           || (lcName == "rowid")
           || (lcName == "oid");
}

QString SQLiteDriver::escapeString(const QString& str) const
{
    return QLatin1Char('\'') + QString(str).replace('\'', "''") +QLatin1Char('\'');
}

QByteArray SQLiteDriver::escapeString(const QByteArray& str) const
{
    return QByteArray("'") + QByteArray(str).replace('\'', "''") + "'";
}

QString SQLiteDriver::escapeBLOB(const QByteArray& array) const
{
    return KexiDB::escapeBLOB(array, KexiDB::BLOBEscapeXHex);
}

QString SQLiteDriver::drv_escapeIdentifier(const QString& str) const
{
    return QString(str).replace('"', "\"\"");
}

QByteArray SQLiteDriver::drv_escapeIdentifier(const QByteArray& str) const
{
    return QByteArray(str).replace('"', "\"\"");
}

AdminTools* SQLiteDriver::drv_createAdminTools() const
{
    return new SQLiteAdminTools();
}

QString SQLiteDriver::collationSQL() const
{
    return dp->collate;
}

QString SQLiteDriver::greatestOrLeastFunctionToString(const QString &name,
                                                      KexiDB::NArgExpr *args,
                                                      QuerySchemaParameterValueListIterator* params) const
{
    Q_ASSERT(args->args() >= 2);
    static QLatin1String greatestString("GREATEST");
    static QLatin1String maxString("MAX");
    static QLatin1String minString("MIN");
    const QString realName(
        name == greatestString ? maxString : minString);
    if (args->args() >= 2 && KexiDB::Field::isTextType(args->arg(0)->type())) {
        QString s;
        s.reserve(256);
        foreach(BaseExpr* e, args->list) {
            if (!s.isEmpty())
                s += ", ";
            s += QLatin1Char('(') + e->toString(this, params) + QLatin1String(") ") + collationSQL();
        }
        return realName + QLatin1Char('(') + s + QLatin1Char(')');
    }
    return FunctionExpr::toString(realName, this, args, params);
}

QString SQLiteDriver::randomFunctionToString(KexiDB::NArgExpr *args,
                                             QuerySchemaParameterValueListIterator* params) const
{
    if (!args || args->args() < 1 ) {
        static QLatin1String randomStatic("((RANDOM()+9223372036854775807)/18446744073709551615)");
        return randomStatic;
    }
    Q_ASSERT(args->args() == 2);
    const QString x(args->arg(0)->toString(this, params));
    const QString y(args->arg(1)->toString(this, params));
    static QLatin1String floorRandomStatic("+CAST(((");
    static QLatin1String floorRandomStatic2("))*(RANDOM()+9223372036854775807)/18446744073709551615 AS INT))");
    //! (X + CAST((Y - X) * (RANDOM()+9223372036854775807)/18446744073709551615 AS INT)).
    return QLatin1String("((") + x + QLatin1Char(')') + floorRandomStatic + y + QLatin1Char(')')
            + QLatin1String("-(") + x + floorRandomStatic2;
}

QString SQLiteDriver::ceilingOrFloorFunctionToString(const QString &name,
                                                     KexiDB::NArgExpr *args,
                                                     QuerySchemaParameterValueListIterator* params) const
{
    Q_ASSERT(args->args() == 1);
    static QLatin1String ceilingString("CEILING");
    QString x(args->arg(0)->toString(this, params));
    if (name == ceilingString) {
        return QLatin1String("(CASE WHEN ")
            + x + QLatin1String("=CAST(") + x + QLatin1String(" AS INT) THEN CAST(")
            + x + QLatin1String(" AS INT) WHEN ")
            + x + QLatin1String(">=0 THEN CAST(")
            + x + QLatin1String(" AS INT)+1 ELSE CAST(")
            + x + QLatin1String(" AS INT) END)");
    }
    // floor():
    return QLatin1String("(CASE WHEN ") + x + QLatin1String(">=0 OR ")
            + x + QLatin1String("=CAST(") + x + QLatin1String(" AS INT) THEN CAST(")
            + x + QLatin1String(" AS INT) ELSE CAST(")
            + x + QLatin1String(" AS INT)-1 END)");
}

#include "sqlitedriver.moc"
