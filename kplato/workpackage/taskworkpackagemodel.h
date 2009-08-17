/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#ifndef TASKWORKPACKAGEMODEL_H
#define TASKWORKPACKAGEMODEL_H

#include "kplatowork_export.h"

#include "kptitemmodelbase.h"
#include "kptnodeitemmodel.h"
#include "kptschedule.h"

#include <QMetaEnum>

class KoDocument;

class QMimeData;
class QModelIndex;
class QAbstractItemDelegate;

namespace KPlato
{

class Project;
class Node;
class Resource;
class Document;

}

using namespace KPlato;

/// The main namespace
namespace KPlatoWork
{

class Part;
class WorkPackage;

/**
 * The TaskWorkPackageModel class gives access to workpackage status
 * for the resources assigned to the task in this package.
 *
 * The model stores a nodes parentNode() in the index's internalPointer().
 */
class KPLATOWORK_EXPORT TaskWorkPackageModel : public ItemModelBase
{
    Q_OBJECT
    Q_ENUMS( Properties )
public:
    explicit TaskWorkPackageModel( Part *part, QObject *parent = 0 );
    ~TaskWorkPackageModel() {}

    enum Properties {
        NodeName = 0,
        NodeType,
        NodeResponsible,
        NodeDescription,

        // After scheduling
        NodeStartTime,
        NodeEndTime,
        NodeAssigments,

        // Completion
        NodeCompleted,
        NodePlannedEffort,
        NodeActualEffort,
        NodeRemainingEffort,
        NodeActualStart,
        NodeStarted,
        NodeActualFinish,
        NodeFinished,
        NodeStatusNote,

        ProjectName,
        ProjectManager
    };
    const QMetaEnum columnMap() const
    {
        return metaObject()->enumerator( metaObject()->indexOfEnumerator("Properties") );
    }

    WorkPackage *workPackage( int index ) const;

    virtual Qt::ItemFlags flags( const QModelIndex &index ) const;
    virtual QModelIndex parent( const QModelIndex &index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex &index = QModelIndex() ) const;
    
    Node *nodeForIndex( const QModelIndex &index ) const;
    QModelIndex indexForNode( Node *node ) const;

    QAbstractItemDelegate *createDelegate( int column, QWidget *parent ) const;

    Document *documentForIndex( const QModelIndex &idx ) const;

    WorkPackage *ptrToWorkPackage( const QModelIndex &idx ) const;
    Node *ptrToNode( const QModelIndex &idx ) const;

    bool isNode( const QModelIndex &idx ) const;
    bool isDocument( const QModelIndex &idx ) const;

public slots:
    void addWorkPackage( WorkPackage *package, int row );
    void removeWorkPackage( WorkPackage *package, int row );

protected slots:
    void slotNodeChanged( Node* );
    void slotNodeToBeInserted( Node *node, int row );
    void slotNodeInserted( Node *node );
    void slotNodeToBeRemoved( Node *node );
    void slotNodeRemoved( Node *node );

protected:
    QVariant nodeData( Node *node, int column, int role ) const; 
    QVariant documentData( Document *doc, int column, int role ) const; 

    QVariant name( const Resource *r, int role ) const;
    QVariant email( const Resource *r, int role ) const;
    QVariant sendStatus( const Resource *r, int role ) const;
    QVariant sendTime( const Resource *r, int role ) const;
    QVariant responseType( const Resource *r, int role ) const;
    QVariant requiredTime( const Resource *r, int role ) const;
    QVariant responseStatus( const Resource *r, int role ) const;
    QVariant responseTime( const Resource *r, int role ) const;
    QVariant lastAction( const Resource *r, int role ) const;
    QVariant projectName( const Node *n, int role ) const;
    QVariant projectManager( const Node *n, int role ) const;
    
    bool setCompletion( Node *node, const QVariant &value, int role );
    bool setRemainingEffort( Node *node, const QVariant &value, int role );
    bool setActualEffort( Node *node, const QVariant &value, int role );
    bool setStartedTime( Node *node, const QVariant &value, int role );
    bool setFinishedTime( Node *node, const QVariant &value, int role );

private:
    NodeModel m_nodemodel;
    Part *m_part;
};


} //namespace KPlato

#endif //WORKPACKAGEMODEL_H
