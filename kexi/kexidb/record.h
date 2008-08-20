/* This file is part of the KDE project
   Copyright (C) 2003 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIDB_RECORD_H
#define KEXIDB_RECORD_H

#include <q3valuelist.h>
#include <qstring.h>

#include "field.h"

namespace KexiDB
{

/*! KexiDB::Record provides single database record.
*/

class KEXI_DB_EXPORT Record
{
public:
    Record(const QString & name);

//TODO.............
    Table();
    ~Table();
    const QString& name() const;
    void setName(const QString& name);
    unsigned int fieldCount() const;
    KexiDB::Field field(unsigned int id) const;
    QStringList primaryKeys() const;
    bool hasPrimaryKeys() const;
//js void addField(KexiDB::Field field);
//js void addPrimaryKey(const QString& key);
private:
//js QStringList m_primaryKeys;
    Q3ValueList<Field> m_fields;
    QString m_name;
    Connection* m_conn;
};


/*
class KexiDBTableFields: public QValueList<KexiDBField> {
public:
  KexiDBTable(const QString & name);
  ~KexiDBTable();
  void addField(KexiDBField);
// const QString& tableName() const;

private:
// QString m_tableName;
};
*/

} //namespace KexiDB

#endif
