/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg <adam@piggz.co.uk>
 * Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "krscriptfunctions.h"
#include <kexidb/cursor.h>
#include <kexidb/utils.h>
#include <kdebug.h>

KRScriptFunctions::KRScriptFunctions(const KoReportData* kodata, KexiDB::Connection* conn)
{
    m_cursor = kodata;
    m_connection = conn;
    m_source = kodata->sourceName();
}

KRScriptFunctions::~KRScriptFunctions()
{
}

void KRScriptFunctions::setWhere(const QString&w)
{
    m_where = w;
}

qreal KRScriptFunctions::math(const QString &function, const QString &field)
{
    qreal ret;
    QString sql;

    sql = "SELECT " + function + '(' + field + ") FROM (" + m_source + ')' ;

    if (!m_where.isEmpty()) {
        sql += " WHERE(" + m_where + ')';
    }

    kDebug() << sql;
    KexiDB::Cursor *curs = m_connection->executeQuery(sql);

    if (curs) {
        ret = curs->value(0).toDouble();
    } else {
        ret = 0.0;
    }
    delete curs;

    return ret;
}

qreal KRScriptFunctions::sum(const QString &field)
{
    return math("SUM", field);
}

qreal KRScriptFunctions::avg(const QString &field)
{
    return math("AVG", field);
}

qreal KRScriptFunctions::min(const QString &field)
{
    return math("MIN", field);
}

qreal KRScriptFunctions::max(const QString &field)
{
    return math("MAX", field);
}

qreal KRScriptFunctions::count(const QString &field)
{
    return math("COUNT", field);
}

QVariant KRScriptFunctions::value(const QString &field)
{
    QVariant val;
    if (!m_cursor) {
        kDebug() << "No cursor to get value of field " << field;
        return val;
    }

    QStringList fields = m_cursor->fieldNames();

    val = m_cursor->value(fields.indexOf(field));
    if (val.type() == QVariant::String) {
        // UTF-8 values are expected so convert this
        return val.toString().toUtf8();
    }

    return val;
}

