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

#include "kexidbreportdata.h"
#include <kdebug.h>
#include <db/queryschema.h>
#include <core/kexipart.h>
#include <QDomDocument>


class KexiDBReportData::Private
{
public:
    Private(KexiDB::Connection *pDb)
      : cursor(0), connection(pDb), originalSchema(0), copySchema(0)
    {
    }
    ~Private()
    {
        delete copySchema;
        delete originalSchema;
        delete cursor;
    }


    QString qstrQuery;

    KexiDB::Cursor *cursor;
    KexiDB::Connection *connection;
    KexiDB::QuerySchema *originalSchema;
    KexiDB::QuerySchema *copySchema;
};

KexiDBReportData::KexiDBReportData (const QString &qstrSQL,
				    KexiDB::Connection * pDb)
        : d(new Private(pDb))
{
    d->qstrQuery = qstrSQL;
    getSchema();
}

void KexiDBReportData::setSorting(const QList<SortedField>& sorting)
{
    if (d->copySchema) {
        if (sorting.isEmpty())
            return;
        KexiDB::OrderByColumnList order;
        for (int i = 0; i < sorting.count(); i++) {
            order.appendField(*d->copySchema, sorting[i].field, sorting[i].order == Qt::AscendingOrder);
        }
        d->copySchema->setOrderByColumnList(order);
    } else {
        kDebug() << "Unable to sort null schema";
    }
}

void KexiDBReportData::addExpression(const QString& field, const QVariant& value, int relation)
{
    if (d->copySchema) {
        KexiDB::Field *fld = d->copySchema->findTableField(field);
        if (fld) {
            d->copySchema->addToWhereExpression(fld, value, relation);
        }
    } else {
        kDebug() << "Unable to add expresstion to null schema";
    }
}

KexiDBReportData::~KexiDBReportData()
{
    close();
    delete d;
}

bool KexiDBReportData::open()
{
    if ( d->connection && d->cursor == 0 )
    {
        if ( d->qstrQuery.isEmpty() )
        {
            d->cursor = d->connection->executeQuery ( "SELECT '' AS expr1 FROM kexi__db WHERE kexi__db.db_property = 'kexidb_major_ver'" );
        }
        else if ( d->copySchema)
        {
            kDebug() << "Opening cursor.." << d->copySchema->debugString();
            d->cursor = d->connection->executeQuery ( *d->copySchema, 1 );
        }


        if ( d->cursor )
        {
            kDebug() << "Moving to first record..";
            return d->cursor->moveFirst();
        }
        else
            return false;
    }
    return false;
}

bool KexiDBReportData::close()
{
    if ( d->cursor )
    {
        d->cursor->close();
        delete d->cursor;
        d->cursor = 0;
    }

    return true;
}

bool KexiDBReportData::getSchema()
{
    if ( d->connection )
    {
        delete d->originalSchema;
        delete d->copySchema;

        if ( d->connection->tableSchema ( d->qstrQuery ) )
        {
            kDebug() << d->qstrQuery <<  " is a table..";
            d->originalSchema = new KexiDB::QuerySchema ( *(d->connection->tableSchema ( d->qstrQuery )) );
        }
        else if ( d->connection->querySchema ( d->qstrQuery ) )
        {
            kDebug() << d->qstrQuery <<  " is a query..";
            d->connection->querySchema(d->qstrQuery)->debug();
            d->originalSchema = new KexiDB::QuerySchema(*(d->connection->querySchema ( d->qstrQuery )));
        }

        if (d->originalSchema) {
            kDebug() << "Original:" << d->connection->selectStatement(*d->originalSchema);
            d->originalSchema->debug();

            d->copySchema = new KexiDB::QuerySchema(*d->originalSchema);
            d->copySchema->debug();
            kDebug() << "Copy:" << d->connection->selectStatement(*d->copySchema);
        }

        return true;
    }
    return false;
}

QString KexiDBReportData::sourceName() const
{
    return d->qstrQuery;
}

int KexiDBReportData::fieldNumber ( const QString &fld ) const
{

    if (!d->cursor || !d->cursor->query()) {
        return -1;
    }
    const KexiDB::QueryColumnInfo::Vector fieldsExpanded(
        d->cursor->query()->fieldsExpanded(KexiDB::QuerySchema::Unique));
    for (int i = 0; i < fieldsExpanded.size() ; ++i) {
        if (0 == QString::compare(fld, fieldsExpanded[i]->aliasOrName(), Qt::CaseInsensitive)) {
            return i;
        }
    }
    return -1;
}

QStringList KexiDBReportData::fieldNames() const
{
    if (!d->originalSchema) {
        return QStringList();
    }
    QStringList names;
    const KexiDB::QueryColumnInfo::Vector fieldsExpanded(
        d->originalSchema->fieldsExpanded(KexiDB::QuerySchema::Unique));
    for (int i = 0; i < fieldsExpanded.size(); i++) {
//! @todo in some Kexi mode captionOrAliasOrName() would be used here (more user-friendly)
        names.append(fieldsExpanded[i]->aliasOrName());
    }
    return names;
}

QVariant KexiDBReportData::value ( unsigned int i ) const
{
    if ( d->cursor )
        return d->cursor->value ( i );

    return QVariant();
}

QVariant KexiDBReportData::value ( const QString &fld ) const
{
    int i = fieldNumber ( fld );

    if ( d->cursor )
        return d->cursor->value ( i );

    return QVariant();
}

bool KexiDBReportData::moveNext()
{
    if ( d->cursor )
        return d->cursor->moveNext();

    return false;
}

bool KexiDBReportData::movePrevious()
{
    if ( d->cursor ) return d->cursor->movePrev();

    return false;
}

bool KexiDBReportData::moveFirst()
{
    if ( d->cursor ) return d->cursor->moveFirst();

    return false;
}

bool KexiDBReportData::moveLast()
{
    if ( d->cursor )
        return d->cursor->moveLast();

    return false;
}

qint64 KexiDBReportData::at() const
{
    if ( d->cursor )
        return d->cursor->at();

    return 0;
}

qint64 KexiDBReportData::recordCount() const
{
    if ( d->copySchema )
    {
        return KexiDB::rowCount ( *d->copySchema );
    }
    else
    {
        return 1;
    }
}

QStringList KexiDBReportData::scriptList(const QString& interpreter) const
{
    QStringList scripts;

    if( d->connection) {
        QList<int> scriptids = d->connection->objectIds(KexiPart::ScriptObjectType);
        QStringList scriptnames = d->connection->objectNames(KexiPart::ScriptObjectType);
        QString script;

        int i;
        i = 0;

        kDebug() << scriptids << scriptnames;
        kDebug() << interpreter;

        //A blank entry
        scripts << "";


        foreach(int id, scriptids) {
            kDebug() << "ID:" << id;
            tristate res;
            res = d->connection->loadDataBlock(id, script, QString());
            if (res == true) {
                QDomDocument domdoc;
                bool parsed = domdoc.setContent(script, false);

                QDomElement scriptelem = domdoc.namedItem("script").toElement();
                if (parsed && !scriptelem.isNull()) {
                    if (interpreter == scriptelem.attribute("language") && scriptelem.attribute("scripttype") == "object") {
                        scripts << scriptnames[i];
                    }
                } else {
                    kDebug() << "Unable to parse script";
                }
            } else {
                kDebug() << "Unable to loadDataBlock";
            }
            ++i;
        }

        kDebug() << scripts;
    }

    return scripts;
}

QString KexiDBReportData::scriptCode(const QString& scriptname, const QString& language) const
{
    QString scripts;

    if (d->connection) {
        QList<int> scriptids = d->connection->objectIds(KexiPart::ScriptObjectType);
        QStringList scriptnames = d->connection->objectNames(KexiPart::ScriptObjectType);

        int i = 0;
        QString script;

        foreach(int id, scriptids) {
            kDebug() << "ID:" << id;
            tristate res;
            res = d->connection->loadDataBlock(id, script, QString());
            if (res == true) {
                QDomDocument domdoc;
                bool parsed = domdoc.setContent(script, false);

                if (! parsed) {
                    kDebug() << "XML parsing error";
                    return QString();
                }

                QDomElement scriptelem = domdoc.namedItem("script").toElement();
                if (scriptelem.isNull()) {
                    kDebug() << "script domelement is null";
                    return QString();
                }

                QString interpretername = scriptelem.attribute("language");
                kDebug() << language << interpretername;
                kDebug() << scriptelem.attribute("scripttype");
                kDebug() << scriptname << scriptnames[i];

                if (language == interpretername && (scriptelem.attribute("scripttype") == "module" || scriptname == scriptnames[i])) {
                    scripts += '\n' + scriptelem.text().toUtf8();
                }
                ++i;
            } else {
                kDebug() << "Unable to loadDataBlock";
            }
        }
    }
    return scripts;
}

QStringList KexiDBReportData::dataSources() const
{
    //Get the list of queries in the database
    QStringList qs;
    if (d->connection && d->connection->isConnected()) {
        QList<int> tids = d->connection->tableIds();
        qs << "";
        for (int i = 0; i < tids.size(); ++i) {
            KexiDB::TableSchema* tsc = d->connection->tableSchema(tids[i]);
            if (tsc)
                qs << tsc->name();
        }

        QList<int> qids = d->connection->queryIds();
        qs << "";
        for (int i = 0; i < qids.size(); ++i) {
            KexiDB::QuerySchema* qsc = d->connection->querySchema(qids[i]);
            if (qsc)
                qs << qsc->name();
        }
    }

    return qs;
}

KoReportData* KexiDBReportData::data(const QString& source)
{
    return new KexiDBReportData(source, d->connection);
}
