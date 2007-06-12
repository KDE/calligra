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

#include "TableModel.h"

#include "CellStorage.h"
#include "Region.h"
#include "Sheet.h"
#include "Value.h"

#include <kdebug.h>

using namespace KSpread;

class TableModel::Private : public QSharedData
{
public:
    Region region;
};

TableModel::TableModel( QObject* parent )
    : QAbstractTableModel( parent )
    , d( new Private )
{
}

TableModel::TableModel( const TableModel& other )
    : d( other.d )
{
}

TableModel::~TableModel()
{
}

bool TableModel::isEmpty() const
{
    return d->region.isEmpty();
}

QVariant TableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    Q_UNUSED( role );
    if ( d->region.isEmpty() )
        return QVariant();
    const QPoint offset = d->region.firstRange().topLeft();
    const int col = ( orientation == Qt::Vertical ) ? offset.x() : offset.x() + section;
    const int row = ( orientation == Qt::Vertical ) ? offset.y() + section : offset.y();
    const Sheet* sheet = (*d->region.constBegin())->sheet();
    const Value value = sheet->cellStorage()->value( col, row );
    return QVariant( value.asString() );
}

int TableModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    return d->region.isEmpty() ? 0 : d->region.firstRange().height();
}

int TableModel::columnCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    return d->region.isEmpty() ? 0 : d->region.firstRange().width();
}

QVariant TableModel::data( const QModelIndex& index, int role ) const
{
    kDebug() << k_funcinfo << ": " << index.column() << ", " << index.row() << endl;
    Q_UNUSED( role );
    if ( d->region.isEmpty() )
        return QVariant();
    const QPoint offset = d->region.firstRange().topLeft();
    const Sheet* sheet = (*d->region.constBegin())->sheet();
    const Value value = sheet->cellStorage()->value( offset.x() + index.column(),
                                                     offset.y() + index.row() );
    // KoChart::Value is either:
    //  - a double (interpreted as a value)
    //  - a QString (interpreted as a label)
    //  - a QDateTime (interpreted as a date/time value)
    //  - Invalid (interpreted as empty)
    QVariant variant;
    switch ( value.type() )
    {
        case Value::Float:
        case Value::Integer:
            if ( value.format() == Value::fmt_DateTime ||
                 value.format() == Value::fmt_Date ||
                 value.format() == Value::fmt_Time )
            {
                variant.setValue<QDateTime>( value.asDateTime( sheet->doc() ) );
                break;
            }
        case Value::Boolean:
        case Value::Complex:
        case Value::Array:
            variant.setValue<double>( value.asFloat() );
            break;
        case Value::String:
        case Value::Error:
            variant.setValue<QString>( value.asString() );
            break;
        case Value::Empty:
        case Value::CellRange:
        default:
            break;
    }
    return variant;
}

void TableModel::setRegion( const Region& region )
{
    Q_ASSERT( region.isEmpty() || region.isContiguous() );
    kDebug() << "Creating TableModel for " << region.name() << endl;
    d->region = region;
}

void TableModel::operator=( const TableModel& other )
{
    d = other.d;
}

bool TableModel::operator==( const TableModel& other ) const
{
    return d == other.d;
}

bool TableModel::operator<( const TableModel& other ) const
{
    return d < other.d;
}

#include "TableModel.moc"
