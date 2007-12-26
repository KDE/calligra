/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Dag Andersen <kplato@kde.org>

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

#ifndef KPTRESOURCEEDITOR_H
#define KPTRESOURCEEDITOR_H

#include "kplatoui_export.h"

#include <kptviewbase.h>
#include <kptresourcemodel.h>


class KoDocument;

class QPoint;


namespace KPlato
{

class Project;
class Resource;
class ResourceGroup;


class KPLATOUI_EXPORT ResourceTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    ResourceTreeView( QWidget *parent );

    ResourceItemModel *model() const { return static_cast<ResourceItemModel*>( DoubleTreeViewBase::model() ); }

    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }

    QObject *currentObject() const;
    QList<QObject*> selectedObjects() const;
    QList<ResourceGroup*> selectedGroups() const;
    QList<Resource*> selectedResources() const;

protected slots:
    void slotActivated( const QModelIndex index );

};

class KPLATOUI_EXPORT ResourceEditor : public ViewBase
{
    Q_OBJECT
public:
    ResourceEditor( KoDocument *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();

    ResourceItemModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool readwrite );

    virtual Resource *currentResource() const;
    virtual ResourceGroup *currentResourceGroup() const;
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
signals:
    void requestPopupMenu( const QString&, const QPoint& );
    void addResource( ResourceGroup* );
    void deleteObjectList( QObjectList );
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected:
    void updateActionsEnabled(  bool on = true );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    void slotSplitView();
    void slotOptions();
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void slotEnableActions( bool on );

    void slotAddResource();
    void slotAddGroup();
    void slotDeleteSelection();

private:
    ResourceTreeView *m_view;

    KAction *actionAddResource;
    KAction *actionAddGroup;
    KAction *actionDeleteSelection;

    // View options context menu
    KAction *actionOptions;
};

}  //KPlato namespace

#endif
