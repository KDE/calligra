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

#include <QModelIndex>

namespace KPlato
{

void WorkPackageModel::setProject( Project *project )
{
    kDebug()<<m_project<<"->"<<project;
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

QVariant WorkPackageModel::sendStatus( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return WorkPackage::sendStatusToString( workPackage().sendStatus( r ), true );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::sendTime( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return workPackage().resourceStatus().value( const_cast<Resource*>( r ) ).sendTime.dateTime();
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

QVariant WorkPackageModel::requiredTime( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return workPackage().resourceStatus().value( const_cast<Resource*>( r ) ).requiredTime.dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::responseStatus( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return WorkPackage::responseStatusToString( workPackage().responseStatus( r ), true );
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::responseTime( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return workPackage().resourceStatus().value( const_cast<Resource*>( r ) ).responseTime.dateTime();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::lastAction( const Resource *r, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return WorkPackage::actionTypeToString( workPackage().actionType( r ) );
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
    return 9;
}

QVariant WorkPackageModel::data( const QModelIndex &index, int role ) const
{
    QVariant result;
    Resource *r = resourceForIndex( index );
    switch ( index.column() ) {
        case 0: result = name( r, role ); break;
        case 1: result = email( r, role ); break;
        case 2: result = sendStatus( r, role ); break;
        case 3: result = sendTime( r, role ); break;
        case 4: result = responseType( r, role ); break;
        case 5: result = requiredTime( r, role ); break;
        case 6: result = responseStatus( r, role ); break;
        case 7: result = responseTime( r, role ); break;
        case 8: result = lastAction( r, role ); break;

        default:
            //kDebug()<<"Invalid column number: "<<index.column();;
            break;
    }
    return result;
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
            case 2: return i18n( "Send Reason" );
            case 3: return i18n( "Send Time" );
            case 4: return i18n( "Response Type" );
            case 5: return i18n( "Response Due" );
            case 6: return i18n( "Response Reason" );
            case 7: return i18n( "Response Time" );
            case 8: return i18n( "Last Action" );
            
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
