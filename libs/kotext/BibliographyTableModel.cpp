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

#include <BibDbFilter.h>
#include <db/cursor.h>
#include <db/queryschema.h>
#include <db/tableschema.h>
#include <db/parser/sqlparser.h>
#include <db/roweditbuffer.h>
#include <db/tableviewdata.h>

#include <QAbstractTableModel>
#include <QString>
#include <QDebug>

BibliographyTableModel::BibliographyTableModel(KexiDB::Connection *conn, QObject * parent) :
    QAbstractTableModel(parent),
    m_conn(conn),
    m_cursor(0),
    m_data(0)
{
    Q_ASSERT(m_conn);
    const char *tableName = "bibref";
    m_schema = m_conn->tableSchema(tableName);
    if (m_schema) {
        m_cursor = m_conn->executeQuery(*m_schema);
    }
    else {
        kWarning() << tableName << "table not found";
    }
    if (m_cursor) {
        m_data = new KexiDB::TableViewData(m_cursor);
        m_data->preloadAllRows();
    }
}

BibliographyTableModel::~BibliographyTableModel()
{
    delete m_data;
    //m_conn->deleteCursor(m_cursor);
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
    return m_data ? m_data->count() : 0;
}

int BibliographyTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data ? m_data->columnsCount() : 0;
}

QVariant BibliographyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || !m_data) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        const KexiDB::TableViewColumn *col = m_data->column(section);
        return col->captionAliasOrName();
    }
    else {
        return QVariant(section + 1);
    }
}

QVariant BibliographyTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_data) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const KexiDB::RecordData *record = m_data->at(index.row());
        if (record) {
            return record->at(index.column());
        }
    }
    return QVariant();
}

bool BibliographyTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        KexiDB::RecordData *record = m_data->at(index.row());
        if (record && m_data->updateRowEditBuffer(record, index.column(), value)) {
            if (m_data->saveRowChanges(*record)) {
                emit dataChanged(index, index);
                return true;
            }
        }
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

static QMap<QString, int> g_BibliographyTableModel_relations;

static int getRelationInt(const QString &comparison)
{
    if (g_BibliographyTableModel_relations.isEmpty()) {
        g_BibliographyTableModel_relations.insert("=", '=');
        g_BibliographyTableModel_relations.insert("<>", NOT_EQUAL);
        g_BibliographyTableModel_relations.insert("<=", LESS_OR_EQUAL);
        g_BibliographyTableModel_relations.insert(">=", GREATER_OR_EQUAL);
        g_BibliographyTableModel_relations.insert("LIKE", LIKE);
        g_BibliographyTableModel_relations.insert("NULL", SQL_IS_NULL);
        g_BibliographyTableModel_relations.insert("NOT NULL", SQL_IS_NOT_NULL);
    }
    QMap<QString, int>::ConstIterator it = g_BibliographyTableModel_relations.constFind(comparison);
    return it == g_BibliographyTableModel_relations.constEnd()
            ? NOT_EQUAL // comparison left "less than", "greater than", "isn't like"
            : it.value();
}

void BibliographyTableModel::setFilter(QList<BibDbFilter *> *filters)
{
    delete m_data;
    //m_conn->deleteCursor(m_cursor);

    KexiDB::QuerySchema *query = new KexiDB::QuerySchema(*m_schema);
    if (filters) {
        foreach(BibDbFilter *filter, *filters) {
            query->addToWhereExpression(query->field(filter->m_leftOp),
                                        QVariant(filter->m_rightOp),
                                        getRelationInt(filter->m_comparison));
        }
    }

    qDebug() << query->debugString();
    m_cursor = m_conn->executeQuery(*query);
    m_data = new KexiDB::TableViewData(m_cursor);
    m_data->preloadAllRows();
}

QString BibliographyTableModel::capitalize(const QString &s) const
{
    QString tmp = s;
    tmp = tmp.toLower();
    tmp[0] = s[0].toUpper();
    return tmp;
}
