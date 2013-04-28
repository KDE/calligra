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

class KexiMigrateReportData::Private
{
public:
    Private()
      : schema(0)
      , kexiMigrate(0)
      , position(0)
    {
    }

    ~Private()
    {
        if (kexiMigrate) {
            delete kexiMigrate;
            kexiMigrate = 0;
        }

        if (schema) {
            delete schema;
            schema = 0;
        }
    }

    QString qstrName;
    QString qstrQuery;
    bool valid;
    KexiDB::TableSchema TableSchema;
    KexiDB::TableOrQuerySchema *schema;
    KexiMigration::KexiMigrate *kexiMigrate;
    qint64 position;
};

//!Connect to an external data source
//!connStr is in the form driver|connection_string|table
KexiMigrateReportData::KexiMigrateReportData(const QString & connStr)
        : d(new Private)
{
    QStringList extConn = connStr.split('|');

    if (extConn.size() == 3) {
        KexiMigration::MigrateManager mm;

        d->kexiMigrate = mm.driver(extConn[0]);
        KexiDB::ConnectionData cd;
        KexiMigration::Data dat;
        cd.setFileName(extConn[1]);
        dat.source = &cd;
        d->kexiMigrate->setData(&dat);
        d->valid = d->kexiMigrate->connectSource();
        QStringList names;

        if (d->valid) {
            d->valid = d->kexiMigrate->readTableSchema(extConn[2], d->TableSchema);
        }
        if (d->valid) {
            d->schema = new KexiDB::TableOrQuerySchema(d->TableSchema);
        }
        d->valid = d->kexiMigrate->tableNames(names);
        if (d->valid && names.contains(extConn[2])) {
            d->valid = d->kexiMigrate->readFromTable(extConn[2]);
        }
    }
}

KexiMigrateReportData::~KexiMigrateReportData()
{
    delete d;
}

int KexiMigrateReportData::fieldNumber(const QString &fld) const
{
    KexiDB::QueryColumnInfo::Vector flds;

    uint x = -1;

    if (d->schema) {
        flds = d->schema->columns();

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

    if (d->schema) {
        flds = d->schema->columns();

        for (int i = 0; i < flds.size() ; ++i) {
            names << flds[i]->field->name();
        }
    }
    return names;
}

QVariant KexiMigrateReportData::value(unsigned int i) const
{
    if (!d->valid)
        return QVariant();

    return d->kexiMigrate->value(i);

}

QVariant KexiMigrateReportData::value(const QString &fld) const
{
    if (!d->valid)
        return QVariant();

    int i = fieldNumber(fld);

    return d->kexiMigrate->value(i);
}

bool KexiMigrateReportData::moveNext()
{
    if (!d->valid)
        return false;

    d->position++;

    return d->kexiMigrate->moveNext();

}

bool KexiMigrateReportData::movePrevious()
{
    if (!d->valid)
        return false;

    if (d->position > 0) d->position--;

    return d->kexiMigrate->movePrevious();
}

bool KexiMigrateReportData::moveFirst()
{
    if (!d->valid)
        return false;

    d->position = 1;

    return d->kexiMigrate->moveFirst();

}

bool KexiMigrateReportData::moveLast()
{
    if (!d->valid)
        return false;

    return d->kexiMigrate->moveLast();
}

qint64 KexiMigrateReportData::at() const
{
    return d->position;
}

qint64 KexiMigrateReportData::recordCount() const
{
    return 1;
}
