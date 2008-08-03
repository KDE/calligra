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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#ifndef KRSCRIPTFUNCTIONS_H
#define KRSCRIPTFUNCTIONS_H
#include <QScriptEngine>
#include <QObject>
#include <QString>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

/**
 @author
*/
class KRScriptFunctions : public QObject
{
    Q_OBJECT
public:
    KRScriptFunctions(const KexiDB::Cursor*);

    ~KRScriptFunctions();
    void setWhere(const QString&);
    void setSource(const QString&);
private:
    KexiDB::Connection *_conn;
    const KexiDB::Cursor *_curs;
    QString _source;
    qreal math(const QString &, const QString &);

    QString _where;
public slots:
    qreal sum(const QString &);
    qreal avg(const QString &);
    qreal min(const QString &);
    qreal max(const QString &);
    qreal count(const QString &);
    QVariant value(const QString &);
};

#endif
