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

#ifndef KPLATO_RELATIONEDTIOR_H
#define KPLATO_RELATIONEDTIOR_H

#include "kplatoui_export.h"

#include "kptglobal.h"
#include "kptviewbase.h"
#include "kptrelationmodel.h"


class KoDocument;

class QModelIndex;
class QItemDelegate;

class KAction;

namespace KPlato
{

class Project;
class Node;
class RelationItemModel;
class Relation;

class KPLATOUI_EXPORT RelationTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    explicit RelationTreeView( QWidget *parent = 0 );
    
    RelationItemModel *model() const { return static_cast<RelationItemModel*>( DoubleTreeViewBase::model() ); }
    
    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }
    
    void setNode( Node *node ) { model()->setNode( node ); }
    Relation *currentRelation() const { return model()->relation( selectionModel()->currentIndex() ); }
signals:
    void currentColumnChanged( QModelIndex, QModelIndex );
    
protected slots:
    void slotCurrentChanged(const QModelIndex &curr, const QModelIndex& );
};

class KPLATOUI_EXPORT RelationEditor : public ViewBase
{
    Q_OBJECT
public:
    /// Create a relation editor
    RelationEditor( KoDocument *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    virtual Relation *currentRelation() const;
    Relation *selectedRelation() const;

    virtual void updateReadWrite( bool readwrite );

    RelationItemModel *model() const { return m_view->model(); }

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
    KoPrintJob *createPrintJob();

signals:
    void openNode();
    void addRelation();
    void deleteRelation( Relation * );

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected slots:
    virtual void slotOptions();

protected:
    void updateActionsEnabled( bool on );

private slots:
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex&, const QModelIndex& );
    void slotContextMenuRequested( const QModelIndex &index, const QPoint& pos );
    
    void slotEnableActions();

    void slotAddRelation();
    void slotDeleteRelation( Relation *r );

    void slotSplitView();
    
    void slotHeaderContextMenuRequested( const QPoint& );
    
private:
    void edit( QModelIndex index );

private:
    RelationTreeView *m_view;
};


} //namespace KPlato

#endif
