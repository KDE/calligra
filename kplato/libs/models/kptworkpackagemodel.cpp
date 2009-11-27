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
#include "kptcommonstrings.h"

#include <QModelIndex>

namespace KPlato
{

const QMetaEnum WorkPackageModel::columnMap() const
{
    return metaObject()->enumerator( metaObject()->indexOfEnumerator("Properties") );
}

QVariant WorkPackageModel::nodeName( const WorkPackage *wp, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return wp->parentTask() ? wp->parentTask()->name() : "";
        case Qt::EditRole:
            return wp->parentTask() ? wp->parentTask()->name() : "";
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}


QVariant WorkPackageModel::ownerName( const WorkPackage *wp, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return wp->ownerName();
        case Qt::EditRole:
            return wp->ownerName();
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::transmitionStatus( const WorkPackage *wp, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return wp->transmitionStatusToString( wp->transmitionStatus(), true );
        case Qt::EditRole:
            return wp->transmitionStatus();
        case Qt::ToolTipRole: {
            int sts = wp->transmitionStatus();
            if ( sts == WorkPackage::TS_Send ) {
                return i18n( "Sent to %1 at %2", wp->ownerName(), transmitionTime( wp, Qt::DisplayRole ).toString() );
            }
            if ( sts == WorkPackage::TS_Receive ) {
                return i18n( "Received from %1 at %2", wp->ownerName(), transmitionTime( wp, Qt::DisplayRole ).toString() );
            }
            return i18n( "Not available" );
        }
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::transmitionTime( const WorkPackage *wp, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            return KGlobal::locale()->formatDateTime( wp->transmitionTime() );
        case Qt::EditRole:
            return wp->transmitionTime().dateTime();
        case Qt::ToolTipRole: {
            int sts = wp->transmitionStatus();
            QString t = KGlobal::locale()->formatDateTime( wp->transmitionTime(), KLocale::LongDate, KLocale::TimeZone );
            if ( sts == WorkPackage::TS_Send ) {
                return i18n( "Work package sent at: %1", t );
            }
            if ( sts == WorkPackage::TS_Receive ) {
                return i18n( "Work package transmission received at: %1", t );
            }
            return i18n( "Not available" );
        }

        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::completion( const WorkPackage *wp, int role ) const
{
    switch ( role ) {
        case Qt::DisplayRole:
            if ( wp->transmitionStatus() == WorkPackage::TS_Receive ) {
                return wp->completion().percentFinished();
            }
            break;
        case Qt::EditRole:
            if ( wp->transmitionStatus() == WorkPackage::TS_Receive ) {
                return wp->completion().percentFinished();
            }
            break;
        case Qt::ToolTipRole:
            if ( wp->transmitionStatus() == WorkPackage::TS_Receive ) {
                return i18n( "Task reported %1% completed", wp->completion().percentFinished() );
            }
            break;
        case Qt::StatusTipRole:
        case Qt::WhatsThisRole:
            return QVariant();
    }
    return QVariant();
}

QVariant WorkPackageModel::data( const WorkPackage *wp, int column, int role ) const
{
    switch ( column ) {
        case WPNodeName: return nodeName( wp, role );
        case WPOwnerName: return ownerName( wp, role );
        case WPTransmitionStatus: return transmitionStatus( wp, role );
        case WPTransmitionTime: return transmitionTime( wp, role );

        case WPNodeCompleted: return completion( wp, role );
        case WPNodePlannedEffort:
        case WPNodeActualEffort:
        case WPNodeRemainingEffort:
        case WPNodePlannedCost:
        case WPNodeActualCost:
        case WPNodeActualStart:
        case WPNodeStarted:
        case WPNodeActualFinish:
        case WPNodeFinished:
        case WPNodeStatusNote:

        default: break;
    }
    return QVariant();
}


} //namespace KPlato

#include "kptworkpackagemodel.moc"
