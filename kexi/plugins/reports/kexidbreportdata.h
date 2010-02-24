/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef __KEXIDBREPORTDATA_H__
#define __KEXIDBREPORTDATA_H__

#include <QString>
#include <QStringList>

#include <kexidb/cursor.h>
#include <kexidb/utils.h>

#include "KoReportData.h"

/**

*/
class KexiDBReportData : public KoReportData
{
private:
    QString m_qstrQuery;

    KexiDB::Cursor *m_cursor;
    KexiDB::Connection *m_connection;
    KexiDB::QuerySchema *m_schema;

    bool getSchema();

public:
    KexiDBReportData(const QString &qstrSQL, KexiDB::Connection *conn);
    virtual ~KexiDBReportData();

    //virtual void* connection() const;

    virtual QStringList fieldNames() const;
    virtual void setSorting(const QList<SortedField>& sorting);

    virtual QString sourceName() const;
    virtual unsigned int fieldNumber(const QString &field) const;
    virtual QVariant value(unsigned int) const;
    virtual QVariant value(const QString &field) const;

    virtual bool open();
    virtual bool close();
    virtual bool moveNext();
    virtual bool movePrevious();
    virtual bool moveFirst();
    virtual bool moveLast();

    virtual long at() const;
    virtual long recordCount() const;

    //Utility Functions
    virtual QStringList scriptList(const QString& language) const;
    virtual QString scriptCode(const QString& script, const QString& language) const;
    virtual QStringList dataSources() const;
    virtual KoReportData* data(const QString&);
};

#endif

