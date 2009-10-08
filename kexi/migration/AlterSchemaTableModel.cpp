/* This file is part of the KDE project
   Copyright (C) 2009 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004-2006 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "AlterSchemaTableModel.h"
#include <kexidb/tableschema.h>
#include <kdebug.h>

AlterSchemaTableModel::AlterSchemaTableModel ( QObject* parent ) : QAbstractTableModel ( parent )
{
    kDebug();
    m_schema = 0;
}


AlterSchemaTableModel::~AlterSchemaTableModel()
{
    kDebug();
}

QVariant AlterSchemaTableModel::data ( const QModelIndex& index, int role ) const
{
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= (int)m_schema->fieldCount())
        return QVariant();
    
    if (role == Qt::DisplayRole) {
        if (m_data.length() > index.row()) {
            const KexiDB::RecordData r( m_data[index.row()] );
            if (r.size() <= index.column())
                return QVariant();
            return r[index.column()];
        }
        else {
            return QVariant();
        }
    }
    else
        return QVariant();
}

QVariant AlterSchemaTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (m_schema) {
            KexiDB::Field *fld = m_schema->field(section);
            if (fld)
                return m_schema->field(section)->captionOrName();
        }
        return QString("Column %1").arg(section);
    }
    return QString("Row %1").arg(section);
}
                                     

int AlterSchemaTableModel::columnCount ( const QModelIndex& parent ) const
{
    if (m_schema) {
        return m_schema->fieldCount();
    }
    return 0;
}

int AlterSchemaTableModel::rowCount ( const QModelIndex& parent ) const
{
    return 3;
}

void AlterSchemaTableModel::setSchema(KexiDB::TableSchema *ts)
{
    m_schema = ts;
    kDebug() << m_schema->fieldCount();

    beginInsertColumns(QModelIndex(), 0, m_schema->fieldCount() - 1);
    endInsertColumns();
}

void AlterSchemaTableModel::setData(const QList<KexiDB::RecordData>& data)
{
    m_data = data;
}
