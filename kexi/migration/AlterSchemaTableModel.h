/* This file is part of the KDE project
   Copyright (C) 2009 Adam Pigg <adam@piggz.co.uk>

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

#ifndef ALTERSCHEMATABLEMODEL_H
#define ALTERSCHEMATABLEMODEL_H

#include <QModelIndex>
#include <QList>

namespace KexiDB {
class TableSchema;
}

class AlterSchemaTableModel : public QAbstractTableModel {
    public:
        
        AlterSchemaTableModel ( QObject* parent = 0 );
        ~AlterSchemaTableModel ( );
        
        virtual QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        virtual int columnCount ( const QModelIndex& parent = QModelIndex() ) const;
        virtual int rowCount ( const QModelIndex& parent = QModelIndex() ) const;

        void setSchema(KexiDB::TableSchema*);
        void setData(QList< QList<QVariant> > dat);
    private:
        KexiDB::TableSchema *m_schema;
        QList< QList<QVariant> > m_data; //Small amount of data to display to user
};

#endif // ALTERSCHEMATABLEMODEL_H
