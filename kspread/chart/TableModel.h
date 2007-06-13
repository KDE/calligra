/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_TABLE_MODEL
#define KSPREAD_TABLE_MODEL

#include <QAbstractTableModel>
#include <QSharedDataPointer>
#include <QVariant>

namespace KSpread
{
class Region;

/**
 * Abstracts the access to the ValueStorage.
 * Useful for KChart (or other apps, that want to access KSpread's data).
 */
class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableModel( QObject* parent = 0 );
    TableModel( const TableModel& other );
    ~TableModel();

    bool isEmpty() const;

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

    void setRegion( const Region& region );

    void operator=( const TableModel& other );
    bool operator==( const TableModel& other ) const;
    bool operator<( const TableModel& other ) const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace KSpread

Q_DECLARE_TYPEINFO( KSpread::TableModel, Q_MOVABLE_TYPE );

#endif // KSPREAD_TABLE_MODEL
