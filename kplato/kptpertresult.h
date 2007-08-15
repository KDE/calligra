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
#include <kptnodeitemmodel.h>

#include <QList>
#include <QStringList>

#include <QtGui>

#include <kdebug.h>

#include "ui_kptpertresult.h"
#include "ui_kptcpmwidget.h"

class KAction;
class KIcon;

/// The main namespace
namespace KPlato
{

class DateTime;
class Duration;
class Estimate;
class Node;
class Project;
class ScheduleManager;
class Task;
class View;

typedef QList<Node*> NodeList;

class CriticalPathItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit CriticalPathItemModel( Part *part, QObject *parent = 0 );
    ~CriticalPathItemModel();
    
    virtual void setProject( Project *project );
    
    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    
    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 
    
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    Node *node( const QModelIndex &index ) const;
    void setManager( ScheduleManager *sm );
    ScheduleManager *manager() const { return m_manager; }
    
    Duration::Unit presentationUnit() const { return Duration::Unit_d; }
    
protected slots:
    void slotNodeChanged( Node* );
    void slotNodeToBeInserted( Node *node, int row );
    void slotNodeInserted( Node *node );
    void slotNodeToBeRemoved( Node *node );
    void slotNodeRemoved( Node *node );

public:
    QVariant alignment( int column ) const;
    
    QVariant name( const Node *node, int role ) const;
    QVariant name( int role ) const;
    QVariant duration( const Node *node, int role ) const;
    QVariant duration( int role ) const;
    
    QVariant variance( const Node *node, int role ) const;
    QVariant variance( int role ) const;
    QVariant variance( const Estimate *est, int role ) const;
    
    QVariant optimistic( const Node *node, int role ) const;
    QVariant optimistic( int role ) const;
    QVariant optimistic( const Estimate *est, int role ) const;
    
    QVariant expected( const Estimate *est, int role ) const;

    QVariant pessimistic( const Node *node, int role ) const;
    QVariant pessimistic( int role ) const;
    QVariant pessimistic( const Estimate *est, int role ) const;
    
    QVariant estimate( const Node *node, int role ) const;
    QVariant notUsed( int role ) const;

private:
    ScheduleManager *m_manager;
    QList<Node*> m_path;
    NodeModel m_nodemodel;
};

//--------------------

/**
 This model displays results from project scheduling.
*/
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
    QVariant negativeFloat( const Task *node, int role ) const;
    QVariant startFloat( const Task *node, int role ) const;
    QVariant finishFloat( const Task *node, int role ) const;

private:
    QStringList m_topNames;
    QList<NodeList*> m_top;
    NodeList m_cp;
    NodeList m_critical;
    NodeList m_noncritical;
    NodeList m_dummyList;
    
    ScheduleManager *m_manager;
    NodeModel m_nodemodel;
};

//--------------------
class PertResult : public ViewBase
{
    Q_OBJECT
public:
    PertResult( Part *part, QWidget *parent = 0 );
    
    void setupGui();
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

    PertResultItemModel *model() const { return static_cast<PertResultItemModel*>( widget.treeWidgetTaskResult->model() ); }

    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &/*context*/ ) const;

public slots:
    void slotScheduleSelectionChanged( ScheduleManager *sm );
    
protected slots:
    void slotProjectCalculated( ScheduleManager *sm );
    void slotScheduleManagerToBeRemoved( const ScheduleManager *sm );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    
    void slotOptions();
    
private:
    Node * m_node;
    Part * m_part;
    Project * m_project;
    bool complexGraph;
    QList<Node *> m_criticalPath;
    ScheduleManager *current_schedule;
    Ui::PertResult widget;
    
    // View options context menu
    KAction *actionOptions;

private slots:
    void slotUpdate();

};

//--------------------
class PertCpmView : public ViewBase
{
    Q_OBJECT
public:
    PertCpmView( Part *part, QWidget *parent = 0 );
    
    void setupGui();
    void setProject( Project *project );
    void draw();
    
    CriticalPathItemModel *model() const { return static_cast<CriticalPathItemModel*>( widget.cpmTable->model() ); }

    double probability( double z ) const;
    double valueZ( double p ) const;
    
    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &/*context*/ ) const;

public slots:
    void slotScheduleSelectionChanged( ScheduleManager *sm );
    
protected slots:
    void slotProjectCalculated( ScheduleManager *sm );
    void slotScheduleManagerToBeRemoved( const ScheduleManager *sm );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    
    void slotOptions();
    
    void slotFinishTimeChanged( const QDateTime &dt );
    void slotProbabilityChanged( int value );

private slots:
    void slotUpdate();

private:
    Project * m_project;
    QList<Node *> m_criticalPath;
    ScheduleManager *current_schedule;
    Ui::CpmWidget widget;
    
    // View options context menu
    KAction *actionOptions;

    bool block;
};

}  //KPlato namespace

#endif
