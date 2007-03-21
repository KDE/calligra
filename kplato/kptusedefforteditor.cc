/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#include "kptusedefforteditor.h"

#include "kptitemmodelbase.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QTableWidget>
#include <QTableWidgetItem>

#include <klocale.h>

#include <kdebug.h>

#include "kpttask.h"
#include "kptresource.h"

namespace KPlato
{

UsedEffortItemModel::UsedEffortItemModel ( QWidget *parent )
    : QAbstractItemModel( parent ),
    m_completion( 0 )
{
    m_headers << i18n( "Resource" )
            << i18n( "Monday" )
            << i18n( "Tuesday" )
            << i18n( "Wednesday" )
            << i18n( "Thursday" )
            << i18n( "Friday" )
            << i18n( "Saturday" )
            << i18n( "Sunday" )
            << i18n( "Total" );
}

Qt::ItemFlags UsedEffortItemModel::flags ( const QModelIndex &index ) const
{
    
    Qt::ItemFlags flags = QAbstractItemModel::flags( index );
    if ( index.column() == 0 || index.column() == 8 ) {
        return flags;
    }
    return flags | Qt::ItemIsEditable;
}

QVariant UsedEffortItemModel::data ( const QModelIndex &index, int role ) const
{
    if ( ! index.isValid() ) {
        return QVariant();
    }
    switch ( role ) {
        case Qt::DisplayRole: {
            if ( index.column() == 0 ) {
                const Resource *r = resource( index );
                //kDebug()<<k_funcinfo<<index.row()<<","<<index.column()<<" "<<r<<endl;
                if ( r == 0 ) {
                    return QVariant();
                }
                return r->name();
            }
            if ( index.column() == 8 ) {
                // Total
                //kDebug()<<k_funcinfo<<index.row()<<","<<index.column()<<" total"<<endl;
                return QVariant();
            }
            Completion::UsedEffort *ue = usedEffort( index );
            if ( ue == 0 ) {
                return QVariant();
            }
            Completion::UsedEffort::ActualEffort *e = ue->effort( m_dates.value( index.column() - 1 ) );
            double res = 0.0;
            if ( e ) {
                res = e->normalEffort().toDouble( Duration::Unit_h );
            }
            return KGlobal::locale()->formatNumber( res, 1 );
        }
        default: break;
    }
    return QVariant();
}

bool UsedEffortItemModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    switch ( role ) {
        case Qt::EditRole: {
            if ( index.column() == 0 || index.column() == 8 ) {
                return false;
            }
            Completion::UsedEffort *ue = usedEffort( index );
            if ( ue == 0 ) {
                return false;
            }
            QDate d = m_dates.value( index.column() - 1 );
            Completion::UsedEffort::ActualEffort *e = ue->effort( d );
            if ( e == 0 ) {
                e = new Completion::UsedEffort::ActualEffort();
                ue->setEffort( d, e );
            }
            e->setNormalEffort( Duration( value.toDouble(), Duration::Unit_h ) );
            emit dataChanged( index, index );
            return true;
        }
        default: break;
    }
    return false;
}

QVariant UsedEffortItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Vertical ) {
        return QVariant();
    }
    if ( role == Qt::DisplayRole ) {
        if ( section < 0 || section >= m_headers.count() ) {
            return QVariant();
        }
        return m_headers.at( section );
    }
    return QVariant();
}

int UsedEffortItemModel::columnCount(const QModelIndex & parent ) const
{
    int c = 0;
    if ( m_completion && ! parent.isValid() ) {
        c = 9;
    }
    return c;
}

int UsedEffortItemModel::rowCount(const QModelIndex & parent ) const
{
    int rows = 0;
    if ( m_completion ) {
        rows = m_completion->usedEffortMap().count();
    }
    return rows;
}

bool UsedEffortItemModel::hasChildren ( const QModelIndex &parent ) const
{
    return ! parent.isValid();
}

QModelIndex UsedEffortItemModel::index ( int row, int column, const QModelIndex &parent ) const
{
    if ( parent.isValid() ) {
        return QModelIndex();
    }
    return createIndex( row, column );
}

void UsedEffortItemModel::setCompletion( Completion *completion )
{
    m_completion = completion;
    reset();
}

const Resource *UsedEffortItemModel::resource(const QModelIndex &index ) const
{
    int row = index.row();
    if ( m_completion == 0 || row < 0 || row >= m_completion->usedEffortMap().count() ) {
        return 0;
    }
    return m_completion->usedEffortMap().uniqueKeys().at( row );
}

Completion::UsedEffort *UsedEffortItemModel::usedEffort(const QModelIndex &index ) const
{
    int row = index.row();
    if ( m_completion == 0 || row < 0 || row >= m_completion->usedEffortMap().count() ) {
        return 0;
    }
    return m_completion->usedEffortMap().values().at( row );
}

void UsedEffortItemModel::setCurrentMonday( const QDate &date )
{
    m_dates.clear();
    for ( int i = 0; i < 7; ++i ) {
        m_dates << date.addDays( i );
    }
    reset();
}

//-----------
UsedEffortEditor::UsedEffortEditor( QWidget *parent )
    : QTableView( parent )
{
    setModel( new UsedEffortItemModel( this ) );
    
    setItemDelegateForColumn ( 1, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 2, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 3, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 4, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 5, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 6, new DoubleSpinBoxDelegate( this ) );
    setItemDelegateForColumn ( 7, new DoubleSpinBoxDelegate( this ) );
    
    connect ( model(), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ), SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ) );
}

void UsedEffortEditor::setCompletion( Completion *completion )
{
    static_cast<UsedEffortItemModel*>( model() )->setCompletion( completion );
}

void UsedEffortEditor::setCurrentMonday( const QDate &date )
{
    static_cast<UsedEffortItemModel*>( model() )->setCurrentMonday( date );
}

}  //KPlato namespace

#include "kptusedefforteditor.moc"
