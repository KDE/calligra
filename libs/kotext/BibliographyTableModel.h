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

#ifndef BIBLIOGRAPHYTABLEMODEL_H
#define BIBLIOGRAPHYTABLEMODEL_H

#include "kotext_export.h"

#include <QAbstractTableModel>

#include <db/connection.h>

class QString;
class BibDbFilter;

class KOTEXT_EXPORT BibliographyTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    BibliographyTableModel(KexiDB::Connection *conn, QObject * parent = 0);
    ~BibliographyTableModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setFilter(QList<BibDbFilter*> *filters);
    void clearFilter();

    static int getRelationInt(QString comparison);

private:
    KexiDB::Connection *m_conn;
    KexiDB::TableSchema *m_schema;
    KexiDB::Cursor *m_cursor;

    QString capitalize(QString s) const;
};

#endif // BIBLIOGRAPHYTABLEMODEL_H
