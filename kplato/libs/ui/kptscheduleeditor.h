/* This file is part of the KDE project
  Copyright (C) 2006-2008 Dag Andersen <kplato@kde.org>
  Copyright (C) 2006-2007 Menard Alexis <kplato@kde.org>
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

#ifndef KPTSCHEDULEEDITOR_H
#define KPTSCHEDULEEDITOR_H

#include "kplatoui_export.h"

#include <kptviewbase.h>
#include "kptsplitterview.h"
#include "kptschedulemodel.h"

#include <KoXmlReaderForward.h>


class KoDocument;

class QPoint;


namespace KPlato
{

class View;
class Project;
class ScheduleManager;
class MainSchedule;
class Schedule;

class KPLATOUI_EXPORT ScheduleTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    ScheduleTreeView( QWidget *parent );

    ScheduleItemModel *model() const { return static_cast<ScheduleItemModel*>( TreeViewBase::model() ); }

    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }

    ScheduleManager *currentManager() const;

signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );

    void contextMenuRequested( QModelIndex, const QPoint& );
    
protected slots:
    void headerContextMenuRequested( const QPoint &pos );
    void slotActivated( const QModelIndex index );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    
protected:

private:
    bool m_arrowKeyNavigation;
};

class KPLATOUI_EXPORT ScheduleEditor : public ViewBase
{
    Q_OBJECT
public:
    ScheduleEditor( KoDocument *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Project &project );
    virtual void draw();
    
    ScheduleItemModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool readwrite );

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
signals:
    void requestPopupMenu( const QString&, const QPoint& );
    void calculateSchedule( Project*, ScheduleManager* );
    void baselineSchedule( Project*, ScheduleManager* );
    void addScheduleManager( Project* );
    void deleteScheduleManager( Project*, ScheduleManager* );
    void SelectionScheduleChanged();

    /**
     * Emitted when schedule selection changes.
     * @param sm is the new schedule manager. If @p is 0, no schedule is selected.
    */
    void scheduleSelectionChanged( ScheduleManager *sm );
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void updateActionsEnabled( const QModelIndex &index );
    void slotEnableActions( const ScheduleManager *sm );

    void slotCalculateSchedule();
    void slotBaselineSchedule();
    void slotAddSchedule();
    void slotAddSubSchedule();
    void slotDeleteSelection();
    
    void slotOptions();
    
private:
    ScheduleTreeView *m_view;

    KAction *actionCalculateSchedule;
    KAction *actionBaselineSchedule;
    KAction *actionAddSchedule;
    KAction *actionAddSubSchedule;
    KAction *actionDeleteSelection;
    
    KAction *actionOptions;
};


//-----------------------------
class KPLATOUI_EXPORT ScheduleLogTreeView : public QTreeView
{
    Q_OBJECT
public:
    ScheduleLogTreeView( QWidget *parent );

    Project *project() const { return logModel()->project(); }
    void setProject( Project *project ) { logModel()->setProject( project ); }

    ScheduleLogItemModel *logModel() const { return m_model; }
    
    ScheduleManager *scheduleManager() const { return logModel()->manager(); }
    void setScheduleManager( ScheduleManager *manager ) { logModel()->setManager( manager ); }

signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );

    void contextMenuRequested( QModelIndex, const QPoint& );

protected slots:
    void headerContextMenuRequested( const QPoint &pos );
    void slotActivated( const QModelIndex index );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );

private:
    ScheduleLogItemModel *m_model;
};

//----------------------------------------------
class KPLATOUI_EXPORT ScheduleLogView : public ViewBase
{
    Q_OBJECT
public:
    ScheduleLogView( KoDocument *part, QWidget *parent );

    void setupGui();
    virtual void setProject( Project *project );
    virtual void draw( Project &project );

    ScheduleLogItemModel *model() const { return m_view->logModel(); }

    virtual void updateReadWrite( bool readwrite );

    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
signals:
    void requestPopupMenu( const QString&, const QPoint& );

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    void slotScheduleSelectionChanged( ScheduleManager *sm );

    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void updateActionsEnabled( const QModelIndex &index );
    void slotEnableActions( const ScheduleManager *sm );

    void slotOptions();

private:
    ScheduleLogTreeView *m_view;
    
    KAction *actionOptions;
};


//-----------------------------
class KPLATOUI_EXPORT ScheduleHandlerView : public SplitterView
{
    Q_OBJECT
public:
    ScheduleHandlerView( KoDocument *part, QWidget *parent );
    
    ScheduleEditor *scheduleEditor() const { return m_scheduleEditor; }
    /// Returns a list of actionlist names for all shown views
    QStringList actionListNames() const;
    /// Returns the list of actions associated with the action list @p name
    virtual QList<QAction*> actionList( const QString name ) const;
    /// Allways returns this (if we are called, we are hit)
    virtual ViewBase *hitView( const QPoint &glpos );

signals:
    void currentScheduleManagerChanged( ScheduleManager* );

public slots:
    /// Activate/deactivate the gui (also of subviews)
    virtual void setGuiActive( bool activate );

protected slots:
    /// Noop, we handle subviews ourselves
    virtual void slotGuiActivated( ViewBase *v, bool active );

private:
    ScheduleEditor *m_scheduleEditor;
};


}  //KPlato namespace

#endif
