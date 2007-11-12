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

#include <kptviewbase.h>
#include <kptitemmodelbase.h>
#include "kpteffortcostmap.h"

#include <QTreeWidget>

class KoDocument;

class QPoint;
class QTreeWidgetItem;
class QSplitter;
class QDropEvent;
class QDragMoveEvent;
class QDragEnterEvent;

class KToggleAction;
class KPrinter;

namespace KPlato
{

class View;
class Project;
class Appointment;
class Resource;
class ResourceGroup;
class ScheduleManager;

class KPLATOUI_EXPORT ResourceAppointmentsItemModel : public ItemModelBase
{
    Q_OBJECT
public:
    explicit ResourceAppointmentsItemModel( QObject *parent = 0 );
    ~ResourceAppointmentsItemModel();

    virtual void setProject( Project *project );
    virtual void setScheduleManager( ScheduleManager *sm );

    virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

    virtual QModelIndex parent( const QModelIndex & index ) const;
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex index( const ResourceGroup *group ) const;
    QModelIndex index( const Resource *resource ) const;

    virtual int columnCount( const QModelIndex & parent = QModelIndex() ) const; 
    virtual int rowCount( const QModelIndex & parent = QModelIndex() ) const; 

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const; 
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );


    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    virtual QStringList mimeTypes () const;
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );
  

    QObject *object( const QModelIndex &index ) const;
    Appointment *appointment( const QModelIndex &index ) const;
    QModelIndex createAppointmentIndex( int row, int col, void *ptr ) const;
    Resource *resource( const QModelIndex &index ) const;
    QModelIndex createResourceIndex( int row, int col, void *ptr ) const;
    ResourceGroup *resourcegroup( const QModelIndex &index ) const;
    QModelIndex createGroupIndex( int row, int col, void *ptr ) const;

signals:
    void refreshed();
      
protected slots:
    void slotResourceChanged( Resource* );
    void slotResourceGroupChanged( ResourceGroup * );
    void slotResourceGroupToBeInserted( const ResourceGroup *group, int row );
    void slotResourceGroupInserted( const ResourceGroup *group );
    void slotResourceGroupToBeRemoved( const ResourceGroup *group );
    void slotResourceGroupRemoved( const ResourceGroup *group );
    void slotResourceToBeInserted( const ResourceGroup *group, int row );
    void slotResourceInserted( const Resource *resource );
    void slotResourceToBeRemoved( const Resource *resource );
    void slotResourceRemoved( const Resource *resource );
    void slotCalendarChanged( Calendar* cal );
    void slotProjectCalculated( ScheduleManager *sm );
    
protected:
    QVariant notUsed( const ResourceGroup *res, int role ) const;
    
    QVariant name( const Resource *res, int role ) const;
    QVariant name( const ResourceGroup *res, int role ) const;
    QVariant name( const Node *node, int role ) const;
    
    QVariant total( const Resource *res, int role ) const;
    QVariant total( const Resource *res, const QDate &date, int role ) const;
    QVariant total( const Appointment *a, int role ) const;
    
    QVariant assignment( const Appointment *a, const QDate &date, int role ) const;
    
    void refresh();
      
  private:
    int m_columnCount;
    QMap<const Appointment*, EffortCostMap> m_effortMap;
      QDate m_start;
      QDate m_end;
    
    QList<const void*> m_groups;
    QList<const void*> m_resources;
    QList<const void*> m_appointments;
    
    ResourceGroup *m_group; // Used for sanity checks
    Resource *m_resource; // Used for sanity checks
    
    ScheduleManager *m_manager;
  };
  
class KPLATOUI_EXPORT ResourceAppointmentsTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    ResourceAppointmentsTreeView( QWidget *parent );

    ResourceAppointmentsItemModel *model() const { return static_cast<ResourceAppointmentsItemModel*>( DoubleTreeViewBase::model() ); }

    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }
    void setScheduleManager( ScheduleManager *sm ) { model()->setScheduleManager( sm ); }

protected slots:
    void slotActivated( const QModelIndex index );

    void slotColumnsInserted( const QModelIndex&, int c1, int c2 );
    void slotRefreshed();
};

class KPLATOUI_EXPORT ResourceAppointmentsView : public ViewBase
{
    Q_OBJECT
public:
    ResourceAppointmentsView( KoDocument *part, QWidget *parent );
    
    void setupGui();
    virtual void setProject( Project *project );
    virtual void draw( Project &project );
    virtual void draw();

    ResourceAppointmentsItemModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool /*readwrite*/ ) {};

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
    
    void setScheduleManager( ScheduleManager *sm );

protected:
    void updateActionsEnabled(  bool on = true );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    void slotOptions();
    
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

    // View options context menu
    KAction *actionOptions;
};

}  //KPlato namespace

#endif // KPTRESOURCEAPPOINTMENTSVIEW_H
