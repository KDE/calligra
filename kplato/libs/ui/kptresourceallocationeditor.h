/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <kplato@kde.org>

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

#ifndef KPTRESOURCEALLOCATIONEDITOR_H
#define KPTRESOURCEALLOCATIONEDITOR_H

#include "kplatoui_export.h"

#include "kptviewbase.h"
#include "kptresourceallocationmodel.h"
#include "kpttask.h"

class KoDocument;

class QPoint;


namespace KPlato
{

class Project;
class Resource;
class ResourceGroup;


class KPLATOUI_EXPORT ResourceAllocationTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    explicit ResourceAllocationTreeView( QWidget *parent );

    ResourceAllocationItemModel *model() const { return static_cast<ResourceAllocationItemModel*>( DoubleTreeViewBase::model() ); }

    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }

    Task *task() const { return model()->task(); }
    void setTask( Task *task ) { model()->setTask( task ); }

    QObject *currentObject() const;

    const QMap<const Resource*, ResourceRequest*> &resourceCache() const { return model()->resourceCache(); }
    const QMap<const ResourceGroup*, ResourceGroupRequest*> &groupCache() const { return model()->groupCache(); }

signals:
    void dataChanged();

};

class KPLATOUI_EXPORT ResourceAllocationEditor : public ViewBase
{
    Q_OBJECT
public:
    ResourceAllocationEditor( KoDocument *part, QWidget *parent );
    
    void setupGui();
    Project *project() const { return m_view->project(); }
    virtual void setProject( Project *project ) { m_view->setProject( project ); }

    ResourceAllocationItemModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool readwrite );

    virtual Resource *currentResource() const;
    virtual ResourceGroup *currentResourceGroup() const;
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
    KoPrintJob *createPrintJob();

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected slots:
    virtual void slotOptions();

protected:
    void updateActionsEnabled(  bool on = true );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    void slotSplitView();
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void slotEnableActions( bool on );

private:
    ResourceAllocationTreeView *m_view;

};

}  //KPlato namespace

#endif
