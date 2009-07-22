 /* This file is part of the KDE project
   Copyright (C) 2005 - 2007 Dag Andersen <kplato@kde.org>

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

#ifndef KPTRESOURCEAPPOINTMENTSVIEW_H
#define KPTRESOURCEAPPOINTMENTSVIEW_H

#include "kplatoui_export.h"

#include "ui_kptresourceappointmentsdisplayoptions.h"

#include "kptviewbase.h"
#include "kptresourceappointmentsmodel.h"

#include <kpagedialog.h>

class KoDocument;

class QPoint;
class QTreeWidgetItem;
class QSplitter;
class QDropEvent;
class QDragMoveEvent;
class QDragEnterEvent;

class KToggleAction;

namespace KPlato
{

class View;
class Project;
class Appointment;
class Resource;
class ResourceGroup;
class ScheduleManager;
class ResourceAppointmentsItemModel;

//-------------------------------------------------
class ResourceAppointmentsDisplayOptionsPanel : public QWidget, public Ui::ResourceAppointmentsDisplayOptions
{
    Q_OBJECT
public:
    explicit ResourceAppointmentsDisplayOptionsPanel( ResourceAppointmentsItemModel *model, QWidget *parent = 0 );

    void setValues( const ResourceAppointmentsItemModel &del );

public slots:
    void slotOk();
    void setDefault();

signals:
    void changed();

private:
    ResourceAppointmentsItemModel *m_model;
};

class ResourceAppointmentsSettingsDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit ResourceAppointmentsSettingsDialog( ResourceAppointmentsItemModel *model, QWidget *parent = 0 );

};

//------------------------
class KPLATOUI_EXPORT ResourceAppointmentsTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    ResourceAppointmentsTreeView( QWidget *parent );

    ResourceAppointmentsItemModel *model() const { return static_cast<ResourceAppointmentsItemModel*>( DoubleTreeViewBase::model() ); }

    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }
    void setScheduleManager( ScheduleManager *sm ) { model()->setScheduleManager( sm ); }

    QModelIndex currentIndex() const;
    
    /// Load context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    using DoubleTreeViewBase::loadContext;
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;
    using DoubleTreeViewBase::saveContext;

protected slots:
    void slotRefreshed();
};

class KPLATOUI_EXPORT ResourceAppointmentsView : public ViewBase
{
    Q_OBJECT
public:
    ResourceAppointmentsView( KoDocument *part, QWidget *parent );
    
    void setupGui();
    virtual void setProject( Project *project );
    Project *project() const { return m_view->project(); }
    virtual void draw( Project &project );
    virtual void draw();

    ResourceAppointmentsItemModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool /*readwrite*/ ) {};

    virtual Node *currentNode() const;
    virtual Resource *currentResource() const;
    virtual ResourceGroup *currentResourceGroup() const;
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
    KoPrintJob *createPrintJob();
    
signals:
    void requestPopupMenu( const QString&, const QPoint& );
    void addResource( ResourceGroup* );
    void deleteObjectList( QObjectList );
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );
    
    void setScheduleManager( ScheduleManager *sm );

protected slots:
    virtual void slotOptions();

protected:
    void updateActionsEnabled(  bool on = true );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void slotEnableActions( bool on );

    void slotAddResource();
    void slotAddGroup();
    void slotDeleteSelection();

private:
    ResourceAppointmentsTreeView *m_view;

    KAction *actionAddResource;
    KAction *actionAddGroup;
    KAction *actionDeleteSelection;

};

}  //KPlato namespace

#endif // KPTRESOURCEAPPOINTMENTSVIEW_H
