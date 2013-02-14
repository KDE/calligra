/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "BibliographyTableModel.h"

#include <db/cursor.h>
#include <db/queryschema.h>

#include <QAbstractTableModel>
#include <QString>
#include <QDebug>

BibliographyTableModel::BibliographyTableModel(KexiDB::Connection *conn, QObject * parent) :
    QAbstractTableModel(parent),
    m_conn(conn),
    m_schema(m_conn->tableSchema("bibref"))
{
    Q_ASSERT(m_conn);
    m_cursor = m_conn->prepareQuery(*m_schema);//, KexiDB::Cursor::Buffered);
    m_cursor->open();
    qDebug() << "model created\n\n";
}

QModelIndex BibliographyTableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || column > columnCount()) {
        return QModelIndex();
    }

    if (!parent.isValid()) {
        if (row >= rowCount()){
            return QModelIndex();
        }
        return createIndex(row, column);
    }
    return QModelIndex();
}

int BibliographyTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    int count = 1;
    m_cursor->moveFirst();

    if (!m_cursor) {
        return 0;
    }
    while(m_cursor->moveNext()) {
        count++;
    }
    return count;
}

int BibliographyTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_schema->fieldCount() - 1;
}

QVariant BibliographyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        return capitalize(m_schema->fields()->at(section)->captionOrName().replace('_', ' '));
    } else {
        return QVariant(section + 1);
    }
}

QVariant BibliographyTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int row = index.row(), count = 0;
        m_cursor->moveFirst();

        while(count < row) {
            m_cursor->moveNext();
            count++;
        }
        return m_cursor->value(index.column());
    }
    return QVariant();
}

bool BibliographyTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        //TODO: set value to (row, col) index using cursor and conn
//        QuerySchema query;
        //m_conn->executeQuery();
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags BibliographyTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | defaultFlags;
    } else {
        return defaultFlags;
    }
}

QString BibliographyTableModel::capitalize(QString s) const
{
    QString tmp = s;
    tmp = tmp.toLower();
    tmp[0] = s[0].toUpper();
    return tmp;
}

BibliographyTableModel::~BibliographyTableModel()
{
//    m_conn->deleteCursor(m_cursor);
}
