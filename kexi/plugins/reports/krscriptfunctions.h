/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
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
#ifndef KRSCRIPTFUNCTIONS_H
#define KRSCRIPTFUNCTIONS_H
#include <QScriptEngine>
#include <QObject>
#include <QString>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include "KoReportData.h"

/**
 @author
*/
class KRScriptFunctions : public QObject
{
    Q_OBJECT
public:
    KRScriptFunctions(const KoReportData *, KexiDB::Connection*);

    ~KRScriptFunctions();
    
private:
    KexiDB::Connection *m_connection;
    const KoReportData *m_cursor;
    QString m_source;
    qreal math(const QString &, const QString &);

    QString m_where;
public slots:
    void setWhere(const QString&);
    
    qreal sum(const QString &);
    qreal avg(const QString &);
    qreal min(const QString &);
    qreal max(const QString &);
    qreal count(const QString &);
    QVariant value(const QString &);
};

#endif
