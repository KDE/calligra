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
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "kexidbreportdata.h"
#include <kdebug.h>
#include <kexidb/queryschema.h>
#include <core/kexipart.h>
#include <QDomDocument>

KexiDBReportData::KexiDBReportData ( const QString &qstrSQL,
                                     KexiDB::Connection * pDb )
{
    m_cursor = 0;
    m_connection = pDb;
    m_schema = 0;
    m_qstrQuery = qstrSQL;
    getSchema();
}

void KexiDBReportData::setSorting ( KoReportData::SortList sl )
{
    if ( sl.count() > 0 )
    {
        KexiDB::OrderByColumnList order;
        
        for ( unsigned int i = 0; i < sl.count(); ++i )
        {
            order.appendField(*m_schema, sl[i].field, sl[i].order == KoReportData::Sort::Ascending);
        }
        m_schema->setOrderByColumnList(order);
    }
}

KexiDBReportData::~KexiDBReportData()
{
    close();
}

bool KexiDBReportData::open()
{
    if ( m_connection && m_cursor == 0 )
    {
        if ( m_qstrQuery.isEmpty() )
        {
            m_cursor = m_connection->executeQuery ( "SELECT '' AS expr1 FROM kexi__db WHERE kexi__db.db_property = 'kexidb_major_ver'" );
        }
        else if ( m_schema)
        {
            kDebug() << "Opening cursor..";
            m_cursor = m_connection->executeQuery ( *m_schema, 1 );
        }

        if ( m_cursor )
        {
            kDebug() << "Moving to first row..";
            return m_cursor->moveFirst();
        }
        else
            return false;
    }
    return false;
}

bool KexiDBReportData::close()
{
    if ( m_cursor )
    {
        m_cursor->close();
        delete m_cursor;
        m_cursor = 0;
    }

    return true;
}

bool KexiDBReportData::getSchema()
{
    if ( m_connection )
    {
        if ( m_connection->tableSchema ( m_qstrQuery ) )
        {
            kDebug() << m_qstrQuery <<  " is a table..";
            m_schema = new KexiDB::QuerySchema ( *(m_connection->tableSchema ( m_qstrQuery )) );
        }
        else if ( m_connection->querySchema ( m_qstrQuery ) )
        {
            kDebug() << m_qstrQuery <<  " is a query..";
            m_schema = m_connection->querySchema ( m_qstrQuery );
        }
        return true;
    }
    return false;
}

#if 0
void* KexiDBReportData::connection() const
{
    return m_connection;
}
#endif

QString KexiDBReportData::sourceName() const
{
    return m_qstrQuery;
}

uint KexiDBReportData::fieldNumber ( const QString &fld ) const
{
    KexiDB::QueryColumnInfo::Vector flds;
    
    uint x = -1;
    if ( m_cursor && m_cursor->query() )
    {
        flds = m_cursor->query()->fieldsExpanded();
    }

    for ( int i = 0; i < flds.size() ; ++i )
    {
        if ( fld.toLower() == flds[i]->aliasOrName().toLower() )
        {
            x = i;
        }
    }
    return x;
}

QStringList KexiDBReportData::fieldNames() const
{
    QStringList names;

    if ( m_schema )
    {
        for(unsigned int i = 0; i < m_schema->fieldCount(); ++i)
        {
            names << m_schema->field(i)->name();
        }
    }

    return names;
}

QVariant KexiDBReportData::value ( unsigned int i ) const
{
    if ( m_cursor )
        return m_cursor->value ( i );

    return QVariant();
}

QVariant KexiDBReportData::value ( const QString &fld ) const
{
    int i = fieldNumber ( fld );

    if ( m_cursor )
        return m_cursor->value ( i );

    return QVariant();
}

bool KexiDBReportData::moveNext()
{
    if ( m_cursor )
        return m_cursor->moveNext();

    return false;
}

bool KexiDBReportData::movePrevious()
{
    if ( m_cursor ) return m_cursor->movePrev();

    return false;
}

bool KexiDBReportData::moveFirst()
{
    if ( m_cursor ) return m_cursor->moveFirst();

    return false;
}

bool KexiDBReportData::moveLast()
{
    if ( m_cursor )
        return m_cursor->moveLast();

    return false;
}

long KexiDBReportData::at() const
{
    if ( !m_cursor )
        return m_cursor->at();

    return 0;
}

long KexiDBReportData::recordCount() const
{
    if ( m_schema )
    {
        return KexiDB::rowCount ( *m_schema );
    }
    else
    {
        return 1;
    }
}

QStringList KexiDBReportData::scriptList(const QString& interpreter) const
{
    QStringList scripts;

    if( m_connection) {
        QList<int> scriptids = m_connection->objectIds(KexiPart::ScriptObjectType);
        QStringList scriptnames = m_connection->objectNames(KexiPart::ScriptObjectType);
        QString script;

        int id, i;
        id = i = 0;

        kDebug() << scriptids << scriptnames;
        kDebug() << interpreter;

        //A blank entry
        scripts << "";

       
        foreach(id, scriptids) {
            kDebug() << "ID:" << id;
            tristate res;
            res = m_connection->loadDataBlock(id, script, QString());
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

    if (m_connection) {
        QList<int> scriptids = m_connection->objectIds(KexiPart::ScriptObjectType);
        QStringList scriptnames = m_connection->objectNames(KexiPart::ScriptObjectType);

        int id;
        int i = 0;
        QString script;

        foreach(id, scriptids) {
            kDebug() << "ID:" << id;
            tristate res;
            res = m_connection->loadDataBlock(id, script, QString());
            if (res == true) {
                QDomDocument domdoc;
                bool parsed = domdoc.setContent(script, false);

                if (! parsed) {
                    kDebug() << "XML parsing error";
                    return false;
                }

                QDomElement scriptelem = domdoc.namedItem("script").toElement();
                if (scriptelem.isNull()) {
                    kDebug() << "script domelement is null";
                    return false;
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
    if (m_connection && m_connection->isConnected()) {
        QList<int> tids = m_connection->tableIds();
        qs << "";
        for (int i = 0; i < tids.size(); ++i) {
            KexiDB::TableSchema* tsc = m_connection->tableSchema(tids[i]);
            if (tsc)
                qs << tsc->name();
        }

        QList<int> qids = m_connection->queryIds();
        qs << "";
        for (int i = 0; i < qids.size(); ++i) {
            KexiDB::QuerySchema* qsc = m_connection->querySchema(qids[i]);
            if (qsc)
                qs << qsc->name();
        }
    }

    return qs;
}

KoReportData* KexiDBReportData::data(const QString& source)
{
return this;
}