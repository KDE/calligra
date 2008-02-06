/* This file is part of the KDE project
  Copyright (C) 2006 -2007 Dag Andersen <danders@get2net.dk>

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

#ifndef DEPENDENCYEDTIOR_H
#define DEPENDENCYEDTIOR_H

#include "kplatoui_export.h"

#include "kptglobal.h"
#include "kptitemmodelbase.h"
#include "kpttaskeditor.h"
#include "kptviewbase.h"
#include "kptnode.h"

#include <QGraphicsView>
#include <QGraphicsItem>

#include <klocale.h>

#include "KoView.h"

class QModelIndex;

class KAction;

namespace KPlato
{

class Project;
//class Node;
class Relation;

class DependencyConnectorItem;
class DependencyNodeItem;
class DependencyScene;

class KPLATOUI_EXPORT DependencyLinkItemBase : public QGraphicsPathItem
{
public:
    explicit DependencyLinkItemBase ( QGraphicsItem * parent = 0 );
    DependencyLinkItemBase ( DependencyNodeItem *predecessor, DependencyNodeItem *successor, Relation *rel, QGraphicsItem * parent = 0 );
    ~DependencyLinkItemBase();
    
    enum { Type = QGraphicsItem::UserType + 10 };
    int type() const { return Type; }
    
    DependencyScene *itemScene() const;
    virtual void createPath() {}
    virtual void createPath( const QPointF &sp, int stype, const QPointF &dp, int dtype );
    virtual QPointF startPoint() const { return QPointF(); }
    virtual QPointF endPoint() const { return QPointF(); }

public:
    DependencyNodeItem *predItem;
    DependencyNodeItem *succItem;
    Relation *relation;
    QGraphicsPathItem *m_arrow;
};

class KPLATOUI_EXPORT DependencyLinkItem : public DependencyLinkItemBase
{
public:
    explicit DependencyLinkItem ( DependencyNodeItem *predecessor, DependencyNodeItem *successor, Relation *rel, QGraphicsItem * parent = 0 );
    ~DependencyLinkItem();
    
    enum { Type = QGraphicsItem::UserType + 11 };
    int type() const { return Type; }
    
    
    int newChildColumn() const;
    
    using DependencyLinkItemBase::createPath;
    virtual void createPath();
    virtual QPointF startPoint() const;
    virtual QPointF endPoint() const;
    
    void setItemVisible( bool show );

protected:
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent *event );
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent *event );
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
private:
    QPen m_pen;
};

class KPLATOUI_EXPORT DependencyCreatorItem : public DependencyLinkItemBase
{
public:
    explicit DependencyCreatorItem ( QGraphicsItem * parent = 0 );
    ~DependencyCreatorItem() {}
    
    enum { Type = QGraphicsItem::UserType + 12 };
    int type() const { return Type; }
    
    void clear();
    
    using DependencyLinkItemBase::createPath;
    virtual void createPath();
    void createPath( const QPointF &ep );

    virtual QPointF startPoint() const;
    virtual QPointF endPoint() const;
    
    void setItemVisible( bool show );

    void setPredConnector( DependencyConnectorItem *item );
    void setSuccConnector( DependencyConnectorItem *item );

public:
    DependencyConnectorItem *predConnector;
    DependencyConnectorItem *succConnector;
};

//-----------------------

class KPLATOUI_EXPORT DependencyNodeItem : public QGraphicsRectItem
{
public:
    DependencyNodeItem( Node *node, DependencyNodeItem *parent = 0 );
    ~DependencyNodeItem();
    
    enum  { Type = QGraphicsItem::UserType + 1 };
    int type() const { return Type; }
    
    DependencyNodeItem *parentItem() const { return m_parent; }
    void setParentItem( DependencyNodeItem *parent );
    
    void addChild( DependencyNodeItem *ch ) { m_children.append( ch ); }
    DependencyNodeItem *takeChild( DependencyNodeItem *ch );
    QList<DependencyNodeItem*> children() const { return m_children; }
    
    DependencyScene *itemScene() const;
    
    void setRectangle( const QRectF &rect );
    
    void setRow( int row );
    int row() const;
    void setColumn();
    void setColumn( int column );
    int column() const;
    
    Node *node() const { return m_node; }
    QString text() const { return m_text->toPlainText(); }
    void setText( const QString &text ) const { m_text->setPlainText( text ); }
    
    enum ConnectorType { Start, Finish };
    QPointF connectorPoint( ConnectorType type ) const;
    void setConnectorHoverMode( bool mode );
    
    void addParentRelation( DependencyLinkItem *r ) { m_parentrelations.append( r ); }
    DependencyLinkItem *takeParentRelation( DependencyLinkItem *r );
    QList<DependencyLinkItem*> parentRelations() const { return m_parentrelations; }

    void addChildRelation( DependencyLinkItem *r ) { m_childrelations.append( r ); }
    DependencyLinkItem *takeChildRelation( DependencyLinkItem *r );
    QList<DependencyLinkItem*> childRelations() const { return m_childrelations; }

    void setExpanded( bool mode );
    void setItemVisible( bool show );
    
    void setSymbol();
    
    int nodeLevel() const { return m_node == 0 ? 0 : m_node->level() - 1; }
    
protected:
    void moveToY( qreal y );
    void moveToX( qreal x );
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    
private:
    Node *m_node;
    DependencyConnectorItem *m_start;
    DependencyConnectorItem *m_finish;
    QGraphicsTextItem *m_text;
    QGraphicsPathItem *m_symbol;
    
    DependencyNodeItem *m_parent;
    QList<DependencyNodeItem*> m_children;
    
    QList<DependencyNodeItem*> m_dependParents;
    QList<DependencyNodeItem*> m_dependChildren;
    
    QList<DependencyLinkItem*> m_parentrelations;
    QList<DependencyLinkItem*> m_childrelations;
};

//-----------------------
class KPLATOUI_EXPORT DependencyConnectorItem : public QGraphicsRectItem
{
public:
    DependencyConnectorItem( DependencyNodeItem::ConnectorType type, DependencyNodeItem *parent );

    enum { Type = QGraphicsItem::UserType + 2 };
    int type() const { return Type; }
    
    DependencyNodeItem::ConnectorType ctype() const { return m_ctype; }
    
    DependencyScene *itemScene() const;
    Node* node() const;
    QPointF connectorPoint() const;
    
protected:
    void hoverEnterEvent ( QGraphicsSceneHoverEvent *event );
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent *event );
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
private:
    DependencyNodeItem::ConnectorType m_ctype;
};

//-----------------------
class KPLATOUI_EXPORT DependencyScene : public QGraphicsScene
{
    Q_OBJECT
public:
    DependencyScene( QWidget *parent = 0 );
    ~DependencyScene();
    
    void setProject( Project *p ) { m_project = p; }
    
    QList<QGraphicsItem*> itemList( int type ) const;
    void clear();
    
    qreal horizontalGap() const { return 40.0; }
    qreal verticalGap() const { return 6.0; }
    qreal itemWidth() const { return 40.0; }
    qreal itemHeight() const { return 20.0; }
    qreal connectorWidth() const { return 8.0; }
    
    qreal totalItemWidth() const { return connectorWidth()*2 + itemWidth(); }
    qreal columnWidth() const { return horizontalGap() + totalItemWidth(); }
    
    qreal gridHeight() const { return verticalGap() + itemHeight(); }
    qreal gridWidth() const { return horizontalGap() + totalItemWidth(); }
    
    
    qreal gridX( int col ) const { return col * gridWidth(); } 
    qreal itemX( int col=0 ) const { return ( horizontalGap() / 2 ) + ( gridWidth() * col ); }
    int column( qreal x ) const { return (int)( x / columnWidth() ); }
    
    qreal gridY( int row ) const { return row * gridHeight(); } 
    qreal itemY( int row=0 ) const { return ( verticalGap() / 2 ) + ( gridHeight() * row ); }
    int row( qreal y ) const { return (int)( y / gridHeight() ); }
    
    DependencyLinkItem *findItem( const Relation *rel ) const;
    DependencyNodeItem *findItem( const Node *node ) const;
    
    DependencyNodeItem *findPrevItem( Node *node )  const;
    DependencyNodeItem *itemBefore( DependencyNodeItem *parent, Node *node ) const;
    DependencyNodeItem *createItem( Node *node );
    
    void setItemVisible( DependencyNodeItem *item, bool show );
    
    void createLinks();
    void createLinks( DependencyNodeItem *item );
    void createLink( DependencyNodeItem *parent, Relation *rel );

    void connectorEntered( DependencyConnectorItem *item, bool entered );
    void setFromItem( DependencyConnectorItem *item );
    DependencyConnectorItem *fromItem() const { return m_connectionitem->predConnector; }
    bool connectionMode() const { return m_connectionitem->isVisible(); }
    
    void singleConnectorClicked( DependencyConnectorItem *item );
    void multiConnectorClicked( DependencyConnectorItem *item );
    bool connectionIsValid( DependencyConnectorItem *pred, DependencyConnectorItem *succ );
    void clearConnection();
    
    /// Used when a node has been moved
    void moveItem( DependencyNodeItem *item, const QList<Node*> &lst );
    QList<DependencyNodeItem*> removeChildItems( DependencyNodeItem *item );
    
signals:
    void connectorClicked( DependencyConnectorItem *item );
    void connectItems( DependencyConnectorItem *pred, DependencyConnectorItem *succ );
    void itemDoubleClicked( QGraphicsItem * );
    void contextMenuRequested( QGraphicsItem*, const QPoint& );
    
protected:
    virtual void drawBackground ( QPainter * painter, const QRectF & rect );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent );
    virtual void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );
    virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent *mouseEvent );
    virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent *contextMenuEvent );
    
private:
    Project *m_project;
    NodeItemModel *m_model;
    QList<DependencyNodeItem*> m_allItems;
    QMap<int, DependencyNodeItem*> m_visibleItems;
    QMap<int, DependencyNodeItem*> m_hiddenItems;
    DependencyCreatorItem *m_connectionitem;
    
    QList<DependencyConnectorItem*> m_clickedItems;
};


//-----------------------
class KPLATOUI_EXPORT DependencyView : public QGraphicsView
{
    Q_OBJECT
public:
    DependencyView( QWidget *parent );
    
    void setProject( Project *project );
    Project *project() const { return m_project; }
    
    void setItemExpanded( int x, bool mode );
    DependencyNodeItem *createItem( Node *node );
    void createItems( Node *node );
    void createItems();
    void createLinks();
    
    DependencyLinkItem *findItem( const Relation *rel ) const;
    DependencyNodeItem *findItem( const Node *node ) const;

    DependencyScene *itemScene() const { return static_cast<DependencyScene*>( scene() ); }
    void setItemScene(DependencyScene *scene );
    
signals:
    void selectionChanged();
    void selectionChanged( QGraphicsItem * );
    void selectionChanged( QList<QGraphicsItem*> );
    void makeConnection( DependencyConnectorItem *pred, DependencyConnectorItem *succ );
    
public slots:
    void slotNodeAdded( Node *node );
    void slotNodeRemoved( Node *node );
    void slotNodeChanged( Node *node );
    
protected slots:
    void slotSelectionChanged();
    void slotRelationAdded( Relation* rel );
    void slotRelationRemoved( Relation* rel );
    void slotRelationModified( Relation* rel );
    
    void slotSelectedItems(); // HACK due to tt bug 160653
    void slotConnectorClicked( DependencyConnectorItem *item );
    
private:
    Project *m_project;
};

//------------------------------
class KPLATOUI_EXPORT DependencyEditor : public ViewBase
{
    Q_OBJECT
public:
    DependencyEditor( KoDocument *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    virtual Node *currentNode() const;
    QList<Node*> selectedNodes() const ;
    Node *selectedNode() const;
    
    virtual Relation *currentRelation() const;

    virtual void updateReadWrite( bool /*readwrite*/ ) {}

signals:
    void requestPopupMenu( const QString&, const QPoint & );
    void openNode();
    void editNode( Node *node );
    void addTask();
    void addMilestone();
    void addSubtask();
    void deleteTaskList( QList<Node*> );

    void addRelation( Node*, Node*, int );
    void modifyRelation( Relation*, int );
    void modifyRelation( Relation* );
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );
    void slotCreateRelation( DependencyConnectorItem *pred, DependencyConnectorItem *succ );

protected:
    void updateActionsEnabled( bool on );
    int selectedNodeCount() const;
    
private slots:
    void slotItemDoubleClicked( QGraphicsItem *item );
    void slotCurrentChanged( const QModelIndex&, const QModelIndex& );
    void slotSelectionChanged(  QList<QGraphicsItem*> lst );
    void slotContextMenuRequested( QGraphicsItem *item, const QPoint& pos );
    
    void slotEnableActions();

    void slotAddTask();
    void slotAddSubtask();
    void slotAddMilestone();
    void slotDeleteTask();

private:
    void edit( QModelIndex index );

private:
    DependencyView *m_view;
    Node *m_currentnode;
    Relation *m_currentrelation;
    
    KAction *actionAddTask;
    KAction *actionAddMilestone;
    KAction *actionAddSubtask;
    KAction *actionDeleteTask;
};


} //namespace KPlato


#endif
