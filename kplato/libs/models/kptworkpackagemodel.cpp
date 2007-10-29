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

#include "kptworkpackagemodel.h"

#include "kptglobal.h"
#include "kptresource.h"
#include "kptproject.h"
#include "kpttask.h"

#include <KoDocument.h>

#include <QModelIndex>

namespace KPlato
{

void WorkPackageModel::setProject( Project *project )
{
    kDebug()<<m_project<<"->"<<project<<endl;
    m_project = project;
    reset();
}

void WorkPackageModel::setTask( Task *task )
{
    m_task = task;
    reset();
}

QVariant WorkPackageModel::name( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return r->name();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::email( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return r->email();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::status( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return WorkPackage::statusToString( workPackage().status( r ), true );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::responseType( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return WorkPackage::responseTypeToString( workPackage().responseType( r ), true );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}


int WorkPackageModel::rowCount( const QModelIndex &parent ) const
{
    if ( m_task == 0 || parent.isValid() ) {
        return 0;
    }
    return workPackage().resources().count();
}

int WorkPackageModel::columnCount( const QModelIndex & ) const
{
    return 4;
}

QVariant WorkPackageModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Resource *r = resourceForIndex( index );
    switch ( index.column() ) {
        case 0: result = name( r, role ); break;
        case 1: result = email( r, role ); break;
        case 2: result = status( r, role ); break;
        case 3: result = responseType( r, role ); break;

        default:
            //kDebug()<<"Invalid column number: "<<index.column()<<endl;;
            break;
    }
    return result;
}

bool WorkPackageModel::setData( const QModelIndex &index, const QVariant & value, int role )
{
    return false;
}

QVariant WorkPackageModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Vertical ) {
        return section;
    }
    if ( role == Qt::DisplayRole ) {
        switch ( section ) {
            case 0: return i18n( "Name" );
            case 1: return i18n( "E-mail" );
            case 2: return i18n( "Status" );
            case 3: return i18n( "Response" );
            
            default: return QVariant();
        }
    }
    return QVariant();
}

QModelIndex WorkPackageModel::parent( const QModelIndex & ) const
{
    return QModelIndex();
}

QModelIndex WorkPackageModel::index( int row, int column, const QModelIndex &parent ) const
{
    if ( m_task == 0 || parent.isValid() ) {
        return QModelIndex();
    }
    Resource *r = m_task->workPackage().resources().value( row );
    if ( r == 0 ) {
        return QModelIndex();
    }
    return createIndex( row, column );
}

WorkPackage &WorkPackageModel::workPackage() const
{
    return m_task->workPackage();
}

Resource *WorkPackageModel::resourceForIndex( const QModelIndex &index ) const
{
    if ( m_task == 0 || ! index.isValid() ) {
        return 0;
    }
    return m_task->workPackage().resources().value( index.row() );
}

} //namespace KPlato

#include "kptworkpackagemodel.moc"
