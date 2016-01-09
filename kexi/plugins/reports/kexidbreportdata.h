/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
* Copyright (C) 2015-2016 Jarosław Staniek <staniek@kde.org>
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

#include <db/cursor.h>
#include <db/utils.h>

#include <KoReportData.h>

class KexiReportView;

/**

*/
class KexiDBReportData : public KoReportData
{
public:
    /*!
     * @a objectClass specifies @a objectName type: a table or query.
     * Types accepted:
     * -"org.kexi-project.table"
     * -"org.kexi-project.query"
     * -empty QString() - attempt to resolve @a objectName
     */
    KexiDBReportData(const QString &objectName, const QString& objectClass, KexiDB::Connection *conn, KexiReportView *view);
    virtual ~KexiDBReportData();

    virtual QStringList fieldNames() const;
    virtual void setSorting(const QList<SortedField>& sorting);
    virtual void addExpression(const QString &field, const QVariant &value, int relation = '=');

    virtual QString sourceName() const;
    virtual QString sourceClass() const;
    virtual int fieldNumber(const QString &field) const;
    virtual QVariant value(unsigned int) const;
    virtual QVariant value(const QString &field) const;

    virtual bool open();
    virtual bool close();
    virtual bool moveNext();
    virtual bool movePrevious();
    virtual bool moveFirst();
    virtual bool moveLast();

    virtual qint64 at() const;
    virtual qint64 recordCount() const;

    //Utility Functions
    virtual QStringList scriptList(const QString& language) const;
    virtual QString scriptCode(const QString& script, const QString& language) const;
    virtual QStringList dataSources() const;
    virtual KoReportData* create(const QString &source) const;

private:
    class Private;
    Private * const d;

    bool getSchema();
};

#endif

