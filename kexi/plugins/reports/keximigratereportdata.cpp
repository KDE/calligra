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

#include "keximigratereportdata.h"
#include <kdebug.h>

//!Connect to an external data source
//!connStr is in the form driver|connection_string|table
KexiMigrateReportData::KexiMigrateReportData(const QString & connStr) : m_schema(0), m_kexiMigrate(0)
{
    QStringList extConn = connStr.split('|');

    if (extConn.size() == 3) {
        KexiMigration::MigrateManager mm;

        m_kexiMigrate = mm.driver(extConn[0]);
        KexiDB::ConnectionData cd;
        KexiMigration::Data dat;
        cd.setFileName(extConn[1]);
        dat.source = &cd;
        m_kexiMigrate->setData(&dat);
        m_valid = m_kexiMigrate->connectSource();
        QStringList names;

        if (m_valid) {
            m_valid = m_kexiMigrate->readTableSchema(extConn[2], m_TableSchema);
        }
        if (m_valid) {
            m_schema = new KexiDB::TableOrQuerySchema(m_TableSchema);
        }
        m_valid = m_kexiMigrate->tableNames(names);
        if (m_valid && names.contains(extConn[2])) {
            m_valid = m_kexiMigrate->readFromTable(extConn[2]);
        }
        m_position = 0;
    }
}

KexiMigrateReportData::~KexiMigrateReportData()
{
    if (m_kexiMigrate) {
        delete m_kexiMigrate;
        m_kexiMigrate = 0;
    }

    if (m_schema) {
        delete m_schema;
        m_schema = 0;
    }
}

int KexiMigrateReportData::fieldNumber(const QString &fld) const
{
    KexiDB::QueryColumnInfo::Vector flds;

    uint x = -1;

    if (m_schema) {
        flds = m_schema->columns();

        for (int i = 0; i < flds.size() ; ++i) {
            if (fld.toLower() == flds[i]->aliasOrName().toLower()) {
                x = i;
            }
        }
    }
    return x;
}

QStringList KexiMigrateReportData::fieldNames() const
{
    KexiDB::QueryColumnInfo::Vector flds;
    QStringList names;

    if (m_schema) {
        flds = m_schema->columns();

        for (int i = 0; i < flds.size() ; ++i) {
            names << flds[i]->field->name();
        }
    }
    return names;
}

QVariant KexiMigrateReportData::value(unsigned int i) const
{
    if (!m_valid)
        return QVariant();

    return m_kexiMigrate->value(i);

}

QVariant KexiMigrateReportData::value(const QString &fld) const
{
    if (!m_valid)
        return QVariant();

    int i = fieldNumber(fld);

    return m_kexiMigrate->value(i);
}

bool KexiMigrateReportData::moveNext()
{
    if (!m_valid)
        return false;

    m_position++;
    
    return m_kexiMigrate->moveNext();

}

bool KexiMigrateReportData::movePrevious()
{
    if (!m_valid)
        return false;

    if (m_position > 0) m_position--;

    return m_kexiMigrate->movePrevious();
}

bool KexiMigrateReportData::moveFirst()
{
    if (!m_valid)
        return false;

    m_position = 1;

    return m_kexiMigrate->moveFirst();

}

bool KexiMigrateReportData::moveLast()
{
    if (!m_valid)
        return false;

    return m_kexiMigrate->moveLast();
}

qint64 KexiMigrateReportData::at() const
{
    return m_position;
}

qint64 KexiMigrateReportData::recordCount() const
{
    return 1;
}
