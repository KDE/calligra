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

#include <QObject>
#include <QString>

#include <KDbConnection>
#include <KDbCursor>

#include <KReportData>
#include <KReportGroupTracker>

/**
 @author
*/
class KRScriptFunctions : public KReportGroupTracker
{
    Q_OBJECT
public:
    KRScriptFunctions(const KReportData *, KDbConnection*);

    ~KRScriptFunctions();

private:
    KDbConnection *m_connection;
    const KReportData *m_cursor;
    QString m_source;
    qreal math(const QString &, const QString &);

    QMap<QString, QVariant> m_groupData;

    KDbEscapedString where();

public Q_SLOTS:
    virtual void setGroupData(const QMap<QString, QVariant> &groupData);

    qreal sum(const QString &);
    qreal avg(const QString &);
    qreal min(const QString &);
    qreal max(const QString &);
    qreal count(const QString &);
    QVariant value(const QString &);
};

#endif
