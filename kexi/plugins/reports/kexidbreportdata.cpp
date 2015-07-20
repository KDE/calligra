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
#include <core/kexipart.h>

#include <KDbQuerySchema>
#include <KDbNativeStatementBuilder>

#include <QDomDocument>
#include <QDebug>

class KexiDBReportData::Private
{
public:
    explicit Private(KDbConnection *pDb)
      : cursor(0), connection(pDb), originalSchema(0), copySchema(0)
    {
    }
    ~Private()
    {
        delete copySchema;
        delete originalSchema;
        delete cursor;
    }


    QString objectName;

    KDbCursor *cursor;
    KDbConnection *connection;
    KDbQuerySchema *originalSchema;
    KDbQuerySchema *copySchema;
};

KexiDBReportData::KexiDBReportData (const QString &objectName,
                                    KDbConnection * pDb)
        : d(new Private(pDb))
{
    d->objectName = objectName;
    getSchema();
}

KexiDBReportData::KexiDBReportData(const QString& objectName,
                                   const QString& pluginId,
                                   KDbConnection* pDb)
        : d(new Private(pDb))
{
    d->objectName = objectName;
    getSchema(pluginId);
}

void KexiDBReportData::setSorting(const QList<SortedField>& sorting)
{
    if (d->copySchema) {
        if (sorting.isEmpty())
            return;
        KDbOrderByColumnList order;
        for (int i = 0; i < sorting.count(); i++) {
            order.appendField(d->copySchema, sorting[i].field, sorting[i].order == Qt::AscendingOrder);
        }
        d->copySchema->setOrderByColumnList(order);
    } else {
        qDebug() << "Unable to sort null schema";
    }
}

void KexiDBReportData::addExpression(const QString& field, const QVariant& value, char relation)
{
    if (d->copySchema) {
        KDbField *fld = d->copySchema->findTableField(field);
        if (fld) {
            d->copySchema->addToWhereExpression(fld, value, KDbToken(relation));
        }
    } else {
        qDebug() << "Unable to add expresstion to null schema";
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
        if ( d->objectName.isEmpty() )
        {
            return false;
        }
        else if ( d->copySchema)
        {
            qDebug() << "Opening cursor.." << *d->copySchema;
            d->cursor = d->connection->executeQuery ( d->copySchema, 1 );
        }


        if ( d->cursor )
        {
            qDebug() << "Moving to first record..";
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
        const bool ok = d->cursor->close();
        delete d->cursor;
        d->cursor = 0;
        return ok;
    }

    return true;
}

bool KexiDBReportData::getSchema(const QString& pluginId)
{
    if (d->connection)
    {
        delete d->originalSchema;
        d->originalSchema = 0;
        delete d->copySchema;
        d->copySchema = 0;

        if ((pluginId.isEmpty() || pluginId == "org.kexi-project.table")
                && d->connection->tableSchema(d->objectName))
        {
            qDebug() << d->objectName <<  "is a table..";
            d->originalSchema = new KDbQuerySchema(d->connection->tableSchema(d->objectName));
        }
        else if ((pluginId.isEmpty() || pluginId == "org.kexi-project.query")
                 && d->connection->querySchema(d->objectName))
        {
            qDebug() << d->objectName <<  "is a query..";
            qDebug() << *d->connection->querySchema(d->objectName);
            d->originalSchema = new KDbQuerySchema(*(d->connection->querySchema(d->objectName)));
        }

        if (d->originalSchema) {
            const KDbNativeStatementBuilder builder(d->connection);
            KDbEscapedString sql;
            if (builder.generateSelectStatement(&sql, d->originalSchema)) {
                qDebug() << "Original:" << sql;
            } else {
                qDebug() << "Original: ERROR";
            }
            qDebug() << *d->originalSchema;

            d->copySchema = new KDbQuerySchema(*d->originalSchema);
            qDebug() << *d->copySchema;
            if (builder.generateSelectStatement(&sql, d->copySchema)) {
                qDebug() << "Copy:" << sql;
            } else {
                qDebug() << "Copy: ERROR";
            }
        }
        return true;
    }
    return false;
}

QString KexiDBReportData::sourceName() const
{
    return d->objectName;
}

int KexiDBReportData::fieldNumber ( const QString &fld ) const
{

    if (!d->cursor || !d->cursor->query()) {
        return -1;
    }
    const KDbQueryColumnInfo::Vector fieldsExpanded(
        d->cursor->query()->fieldsExpanded(KDbQuerySchema::Unique));
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
    const KDbQueryColumnInfo::Vector fieldsExpanded(
        d->originalSchema->fieldsExpanded(KDbQuerySchema::Unique));
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
        return KDb::recordCount ( d->copySchema );
    }

    return 1;
}

#ifdef KREPORT_SCRIPTING
QStringList KexiDBReportData::scriptList(const QString& interpreter) const
{
    QStringList scripts;

    if( d->connection) {
        QList<int> scriptids = d->connection->objectIds(KexiPart::ScriptObjectType);
        QStringList scriptnames = d->connection->objectNames(KexiPart::ScriptObjectType);

        qDebug() << scriptids << scriptnames;
        qDebug() << interpreter;

        //A blank entry
        scripts << "";
        int i = 0;
        foreach(int id, scriptids) {
            qDebug() << "ID:" << id;
            tristate res;
            QString script;
            res = d->connection->loadDataBlock(id, &script, QString());
            if (res == true) {
                QDomDocument domdoc;
                bool parsed = domdoc.setContent(script, false);

                QDomElement scriptelem = domdoc.namedItem("script").toElement();
                if (parsed && !scriptelem.isNull()) {
                    if (interpreter == scriptelem.attribute("language") && scriptelem.attribute("scripttype") == "object") {
                        scripts << scriptnames[i];
                    }
                } else {
                    qDebug() << "Unable to parse script";
                }
            } else {
                qDebug() << "Unable to loadDataBlock";
            }
            ++i;
        }

        qDebug() << scripts;
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
        foreach(int id, scriptids) {
            qDebug() << "ID:" << id;
            tristate res;
            QString script;
            res = d->connection->loadDataBlock(id, &script, QString());
            if (res == true) {
                QDomDocument domdoc;
                bool parsed = domdoc.setContent(script, false);

                if (! parsed) {
                    qDebug() << "XML parsing error";
                    return QString();
                }

                QDomElement scriptelem = domdoc.namedItem("script").toElement();
                if (scriptelem.isNull()) {
                    qDebug() << "script domelement is null";
                    return QString();
                }

                QString interpretername = scriptelem.attribute("language");
                qDebug() << language << interpretername;
                qDebug() << scriptelem.attribute("scripttype");
                qDebug() << scriptname << scriptnames[i];

                if (language == interpretername && (scriptelem.attribute("scripttype") == "module" || scriptname == scriptnames[i])) {
                    scripts += '\n' + scriptelem.text().toUtf8();
                }
                ++i;
            } else {
                qDebug() << "Unable to loadDataBlock";
            }
        }
    }
    return scripts;
}
#endif

QStringList KexiDBReportData::dataSources() const
{
    //Get the list of queries in the database
    QStringList qs;
    if (d->connection && d->connection->isConnected()) {
        QList<int> tids = d->connection->tableIds();
        qs << "";
        for (int i = 0; i < tids.size(); ++i) {
            KDbTableSchema* tsc = d->connection->tableSchema(tids[i]);
            if (tsc)
                qs << tsc->name();
        }

        QList<int> qids = d->connection->queryIds();
        qs << "";
        for (int i = 0; i < qids.size(); ++i) {
            KDbQuerySchema* qsc = d->connection->querySchema(qids[i]);
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
