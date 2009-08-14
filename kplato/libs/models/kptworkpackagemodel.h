/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version..

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#ifndef WORKPACKAGEMODEL_H
#define WORKPACKAGEMODEL_H

#include "kptitemmodelbase.h"
#include "kpttask.h"

class QModelIndex;

/// The main namespace
namespace KPlato
{

class WorkPackage;

class KPLATOMODELS_EXPORT WorkPackageModel : public QObject
{
    Q_OBJECT
    Q_ENUMS( Properties )
public:
    explicit WorkPackageModel( QObject *parent = 0 )
        : QObject( parent )
     {}
    ~WorkPackageModel() {}
    
    enum Properties {
        WPNodeName = 0,
        WPOwnerName,
        WPTransmitionStatus,
        WPTransmitionTime,

        // Completion
        WPNodeStatus,
        WPNodeCompleted,
        WPNodePlannedEffort,
        WPNodeActualEffort,
        WPNodeRemainingEffort,
        WPNodePlannedCost,
        WPNodeActualCost,
        WPNodeActualStart,
        WPNodeStarted,
        WPNodeActualFinish,
        WPNodeFinished,
        WPNodeStatusNote,
    };
    const QMetaEnum columnMap() const;
    

    QVariant data( const WorkPackage *wp, int column, int role = Qt::DisplayRole ) const;

protected:
    QVariant nodeName(const WorkPackage *wp, int role ) const;
    QVariant ownerName(const WorkPackage *wp, int role ) const;
    QVariant transmitionStatus(const WorkPackage *wp, int role ) const;
    QVariant transmitionTime(const WorkPackage *wp, int role ) const;

    QVariant completion( const WorkPackage *wp, int role ) const;
};


} //namespace KPlato

#endif //WORKPACKAGEMODEL_H
