/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
Daniel Molkentin <molkentin@kde.org>
Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>
Copyright (C) 2003-2016 Jarosław Staniek <staniek@kde.org>

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

#ifndef MYSQLDB_H
#define MYSQLDB_H

#include <db/driver.h>

namespace KexiDB
{

//! MySQL database driver.
class MySqlDriver : public Driver
{
    Q_OBJECT
    KEXIDB_DRIVER

public:
    explicit MySqlDriver(QObject *parent, const QVariantList &args = QVariantList());
    virtual ~MySqlDriver();

    virtual bool isSystemDatabaseName(const QString &n) const;

    //! Escape a string for use as a value
    virtual QString escapeString(const QString& str) const;
    virtual QByteArray escapeString(const QByteArray& str) const;

    //! Escape BLOB value \a array
    virtual QString escapeBLOB(const QByteArray& array) const;

    //! Generates native (driver-specific) LENGTH() function call.
    //! char_length(val) is used because length(val) in mysql returns number of bytes,
    //! what is not right for multibyte (unicode) encodings. */
    virtual QString lengthFunctionToString(KexiDB::NArgExpr *args, QuerySchemaParameterValueListIterator* params) const;

    //! Generates native (driver-specific) GREATEST() and LEAST() function call.
    //! Since MySQL's LEAST()/GREATEST() function ignores NULL values, it only returns NULL
    //! if all the expressions evaluate to NULL. So this is used for F(v0,..,vN):
    //! (CASE WHEN (v0) IS NULL OR .. OR (vN) IS NULL THEN NULL ELSE F(v0,..,vN) END)
    //! where F == GREATEST or LEAST.
    virtual QString greatestOrLeastFunctionToString(const QString &name,
                                                    KexiDB::NArgExpr *args,
                                                    QuerySchemaParameterValueListIterator* params) const;

    //! Generates native (driver-specific) UNICODE() function call.
    //! Uses ORD(CONVERT(X USING UTF16)).
    virtual QString unicodeFunctionToString(KexiDB::NArgExpr *args,
                                            QuerySchemaParameterValueListIterator* params) const;

    //! Generates native (driver-specific) function call for concatenation of two strings.
    //! Uses CONCAT().
    virtual QString concatenateFunctionToString(KexiDB::BinaryExpr *args,
                                                QuerySchemaParameterValueListIterator* params) const;

protected:
    virtual QString drv_escapeIdentifier(const QString& str) const;
    virtual QByteArray drv_escapeIdentifier(const QByteArray& str) const;
    virtual Connection *drv_createConnection(ConnectionData &conn_data);
    virtual bool drv_isSystemFieldName(const QString& n) const;

private:
    static const char * const keywords[];
};
}

#endif
