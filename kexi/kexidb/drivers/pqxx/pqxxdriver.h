/* This file is part of the KDE project
   Copyright (C) 2003 Adam Pigg <adam@piggz.co.uk>
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

#ifndef KEXIDB_DRIVER_PQXX_H
#define KEXIDB_DRIVER_PQXX_H

#include <db/driver.h>

namespace KexiDB
{

class Connection;

//! PostgreSQL database driver.
class pqxxSqlDriver : public Driver
{
    Q_OBJECT
    KEXIDB_DRIVER

public:
    explicit pqxxSqlDriver(QObject *parent, const QVariantList &args = QVariantList());
    ~pqxxSqlDriver();

    virtual bool isSystemObjectName(const QString& n)const;
    virtual bool isSystemDatabaseName(const QString& n)const;

    //! Escape a string for use as a value
    virtual QString escapeString(const QString& str) const;
    virtual QByteArray escapeString(const QByteArray& str) const;
    virtual QString sqlTypeName(int id_t, int p = 0) const;

    //! Escape BLOB value \a array
    virtual QString escapeBLOB(const QByteArray& array) const;

    /*! Escapes and converts value \a v (for type \a ftype)
     to string representation required by SQL commands.
     Reimplemented for boolean type only to use SQL compliant TRUE or FALSE */
    virtual QString valueToSQL(uint ftype, const QVariant& v) const;

    //! Generates native (driver-specific) HEX() function call.
    //! Uses UPPER(ENCODE(val, 'hex')).
    //! See http://www.postgresql.org/docs/9.3/static/functions-string.html#FUNCTIONS-STRING-OTHER */
    virtual QString hexFunctionToString(KexiDB::NArgExpr *args, QuerySchemaParameterValueListIterator* params) const;

    //! Generates native (driver-specific) IFNULL() function call.
    //! Uses COALESCE().
    virtual QString ifnullFunctionToString(KexiDB::NArgExpr *args, QuerySchemaParameterValueListIterator* params) const;

    //! Generates native (driver-specific) LENGTH() function call.
    //! For text types default LENGTH(val) is used, for BLOBs OCTET_LENGTH(val) is used because
    //! LENGTH(val) for BLOB returns number of bits.
    virtual QString lengthFunctionToString(KexiDB::NArgExpr *args, QuerySchemaParameterValueListIterator* params) const;

    //! Generates native (driver-specific) GREATEST() and LEAST() function calls.
    //! Since PostgreSQL's LEAST()/GREATEST() function ignores NULL values, it only returns NULL
    //! if all the expressions evaluate to NULL. So this is used for F(v0,..,vN):
    //! (CASE WHEN (v0) IS NULL OR .. OR (vN) IS NULL THEN NULL ELSE F(v0,..,vN) END)
    //! where F == GREATEST or LEAST.
    virtual QString greatestOrLeastFunctionToString(const QString &name,
                                                    KexiDB::NArgExpr *args,
                                                    QuerySchemaParameterValueListIterator* params) const;

    //! Generates native (driver-specific) UNICODE() function call.
    //! Uses ASCII(X).
    virtual QString unicodeFunctionToString(KexiDB::NArgExpr *args,
                                            QuerySchemaParameterValueListIterator* params) const;

protected:
    virtual QString drv_escapeIdentifier(const QString& str) const;
    virtual QByteArray drv_escapeIdentifier(const QByteArray& str) const;
    virtual Connection *drv_createConnection(ConnectionData &conn_data);
    virtual bool drv_isSystemFieldName(const QString& n)const;

private:
    static const char * const keywords[];
};

}

#endif
