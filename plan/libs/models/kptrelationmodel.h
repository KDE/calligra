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

#ifndef RELATIONMODEL_H
#define RELATIONMODEL_H

#include "kptitemmodelbase.h"
#include "kptschedule.h"


class QMimeData;
class QModelIndex;
class KUndo2Command;

namespace KPlato
{

class Project;
class Node;
class Relation;

class KPLATOMODELS_EXPORT RelationModel : public QObject
{
    Q_OBJECT
public:
    RelationModel()
        : QObject()
    {}
    ~RelationModel() {}
    
    QVariant data( const Relation *relation, int property, int role = Qt::DisplayRole ) const; 
    
    static QVariant headerData( int section, int role = Qt::DisplayRole );

    static int propertyCount();
    
    QVariant parentName( const Relation *r, int role ) const;
    QVariant childName( const Relation *r, int role ) const;
    QVariant type( const Relation *r, int role ) const;
    QVariant lag( const Relation *r, int role ) const;

};

class KPLATOMODELS_EXPORT RelationItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit RelationItemModel( QObject *parent = 0 );
    ~RelationItemModel();
    
    virtual void setProject( Project *project );
    virtual void setNode( Node *node );
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    
    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 
    
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    Relation *relation( const QModelIndex &index ) const;
    QAbstractItemDelegate *createDelegate( int column, QWidget *parent ) const;

signals:
    void executeCommand( KUndo2Command* );
    
protected slots:
    void slotNodeChanged( Node* );
    void slotNodeToBeRemoved( Node *node );
    void slotNodeRemoved( Node *node );
    void slotRelationToBeRemoved( Relation *r );
    void slotRelationRemoved( Relation *r );
    void slotRelationToBeAdded( Relation *r, int, int );
    void slotRelationAdded( Relation *r );
    void slotRelationModified( Relation *r );

    void slotLayoutChanged();
    
protected:
    bool setType( Relation *r, const QVariant &value, int role );
    bool setLag( Relation *r, const QVariant &value, int role );
    
private:
    Node *m_node;
    RelationModel m_relationmodel;
    
    Relation *m_removedRelation; // to control endRemoveRows()
};

} //namespace KPlato

#endif
