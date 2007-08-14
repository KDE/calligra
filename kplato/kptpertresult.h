/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>
  Copyright (C) 2007 Dag Andersen <kplato@kde.org>

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

#ifndef KPTPERTRESULT_H
#define KPTPERTRESULT_H

#include <kptviewbase.h>
#include <kptitemmodelbase.h>

#include <QList>
#include <QStringList>

#include <QtGui>

#include <kdebug.h>


namespace KPlato
{
#include "ui_kptpertresult.h"

class DateTime;
class Duration;
class Node;
class Project;
class ScheduleManager;
class Task;
class View;

typedef QList<Node*> NodeList;

class PertResultItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit PertResultItemModel( Part *part, QObject *parent = 0 );
    ~PertResultItemModel();
    
    virtual void setProject( Project *project );
    
    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;
    
    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
//    virtual QModelIndex index( const Node *node ) const;
    virtual QModelIndex index( const NodeList *lst ) const;
    
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 
    
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual QStringList mimeTypes () const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );

    NodeList *list( const QModelIndex &index ) const;
    Node *node( const QModelIndex &index ) const;
    QItemDelegate *createDelegate( int column, QWidget *parent ) const;
    
    NodeList nodeList( QDataStream &stream );
    static NodeList removeChildNodes( const NodeList nodes );
    bool dropAllowed( Node *on, const QMimeData *data );
    
    void clear();
    void refresh();
    
    void setManager( ScheduleManager *sm );
    ScheduleManager *manager() const { return m_manager; }
    
protected slots:
    void slotAboutToBeReset();
    void slotReset();

    void slotNodeChanged( Node* );
    void slotNodeToBeInserted( Node *node, int row );
    void slotNodeInserted( Node *node );
    void slotNodeToBeRemoved( Node *node );
    void slotNodeRemoved( Node *node );

protected:
    QVariant alignment( int column ) const;
    
    QVariant name( int row, int role ) const;
    QVariant name( const Node *node, int role ) const;
    QVariant earlyStart( const Task *node, int role ) const;
    QVariant earlyFinish( const Task *node, int role ) const;
    QVariant lateStart( const Task *node, int role ) const;
    QVariant lateFinish( const Task *node, int role ) const;
    QVariant positiveFloat( const Task *node, int role ) const;
    QVariant freeFloat( const Task *node, int role ) const;

private:
    QStringList m_topNames;
    QList<NodeList*> m_top;
    NodeList m_cp;
    NodeList m_critical;
    NodeList m_noncritical;
    
    ScheduleManager *m_manager;
};

//--------------------
class PertResult : public ViewBase
{
    Q_OBJECT
public:

    PertResult( Part *part, QWidget *parent );
    void setProject( Project *project );
    void draw( Project &project );
    void draw();

    QList<Node*> criticalPath();

    DateTime getStartEarlyDate(Node * currentNode);
    DateTime getFinishEarlyDate(Node * currentNode);
    DateTime getStartLateDate(Node * currentNode);
    DateTime getFinishLateDate(Node * currentNode);
    Duration getProjectFloat(Project *project);
    Duration getFreeMargin(Node * currentNode);
    Duration getTaskFloat(Node * currentNode);
    void testComplexGraph();

    PertResultItemModel *model() const { return static_cast<PertResultItemModel*>( widget.treeWidgetTaskResult->itemModel() ); }
    
public slots:
    void slotScheduleSelectionChanged( ScheduleManager *sm );
    
protected slots:
    void slotProjectCalculated( ScheduleManager *sm );
    void slotScheduleManagerToBeRemoved( const ScheduleManager *sm );
    
private:
    Node * m_node;
    Part * m_part;
    Project * m_project;
    bool complexGraph;
    QList<Node *> m_criticalPath;
    ScheduleManager *current_schedule;
    Ui::PertResult widget;
    
private slots:
    void slotUpdate();

};

}  //KPlato namespace

#endif
