/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
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

#ifndef NODEITEMMODEL_H
#define NODEITEMMODEL_H

#include "kptitemmodelbase.h"

#include <QItemDelegate>

class QMimeData;
class QModelIndex;

namespace KPlato
{

class Project;
class Node;

class NodeItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit NodeItemModel( Part *part, QObject *parent = 0 );
    ~NodeItemModel();
    
    virtual void setProject( Project *project );
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    
    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual bool hasChildren( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex index( const Node *node ) const;
    
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 
    
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );

    Node *node( const QModelIndex &index ) const;
    QItemDelegate *createDelegate( int column, QWidget *parent ) const;

    QModelIndex insertTask( Node *node, Node *after );
    QModelIndex insertSubtask( Node *node, Node *parent );
    
    QList<Node*> nodeList( QDataStream &stream );
    static QList<Node*> removeChildNodes( const QList<Node*> nodes );
    bool dropAllowed( Node *on, const QMimeData *data );
    
    virtual bool dropAllowed( const QModelIndex &index, int dropIndicatorPosition, const QMimeData *data );
    
protected slots:
    void slotNodeChanged( Node* );
    void slotNodeToBeInserted( Node *node, int row );
    void slotNodeInserted( Node *node );
    void slotNodeToBeRemoved( Node *node );
    void slotNodeRemoved( Node *node );

protected:
    QVariant name( const Node *node, int role ) const;
    bool setName( Node *node, const QVariant &value, int role );
    QVariant leader( const Node *node, int role ) const;
    bool setLeader( Node *node, const QVariant &value, int role );
    QVariant allocation( const Node *node, int role ) const;
    bool setAllocation( Node *node, const QVariant &value, int role );
    QVariant description( const Node *node, int role ) const;
    bool setDescription( Node *node, const QVariant &value, int role );
    QVariant type( const Node *node, int role ) const;
    bool setType( Node *node, const QVariant &value, int role );
    QVariant constraint( const Node *node, int role ) const;
    bool setConstraint( Node *node, const QVariant &value, int role );
    QVariant constraintStartTime( const Node *node, int role ) const;
    bool setConstraintStartTime( Node *node, const QVariant &value, int role );
    QVariant constraintEndTime( const Node *node, int role ) const;
    bool setConstraintEndTime( Node *node, const QVariant &value, int role );
    QVariant estimateType( const Node *node, int role ) const;
    bool setEstimateType( Node *node, const QVariant &value, int role );
    QVariant estimate( const Node *node, int role ) const;
    bool setEstimate( Node *node, const QVariant &value, int role );
    QVariant optimisticRatio( const Node *node, int role ) const;
    bool setOptimisticRatio( Node *node, const QVariant &value, int role );
    QVariant pessimisticRatio( const Node *node, int role ) const;
    bool setPessimisticRatio( Node *node, const QVariant &value, int role );
    QVariant riskType( const Node *node, int role ) const;
    bool setRiskType( Node *node, const QVariant &value, int role );
    QVariant runningAccount( const Node *node, int role ) const;
    bool setRunningAccount( Node *node, const QVariant &value, int role );
    QVariant startupAccount( const Node *node, int role ) const;
    bool setStartupAccount( Node *node, const QVariant &value, int role );
    QVariant startupCost( const Node *node, int role ) const;
    bool setStartupCost( Node *node, const QVariant &value, int role );
    QVariant shutdownAccount( const Node *node, int role ) const;
    bool setShutdownAccount( Node *node, const QVariant &value, int role );
    QVariant shutdownCost( const Node *node, int role ) const;
    bool setShutdownCost( Node *node, const QVariant &value, int role );
    
    QVariant startTime( const Node *node, int role ) const;
    QVariant endTime( const Node *node, int role ) const;
    
    QVariant test( const Node *node, int role ) const;
    bool setTest( Node *node, const QVariant &value, int role );

private:
    Node *m_node; // for sanety check
};

} //namespace KPlato

#endif //NODEITEMMODEL_H
