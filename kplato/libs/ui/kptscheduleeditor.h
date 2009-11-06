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

#include <QSortFilterProxyModel>

class KoDocument;

class QPoint;

class QKeyEvent;

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

    Project *project() const { return model()->project(); }
    void setProject( Project *project ) { model()->setProject( project ); }

    ScheduleManager *currentManager() const;

signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );

protected slots:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    
};

class KPLATOUI_EXPORT ScheduleEditor : public ViewBase
{
    Q_OBJECT
public:
    ScheduleEditor( KoDocument *part, QWidget *parent );
    
    void setupGui();
    Project *project() const { return m_view->project(); }
    virtual void draw( Project &project );
    virtual void draw();
    
    ScheduleItemModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool readwrite );

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
    KoPrintJob *createPrintJob();
    
signals:
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

protected slots:
    virtual void slotOptions();

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void updateActionsEnabled( const QModelIndex &index );
    void slotEnableActions( const ScheduleManager *sm );

    void slotCalculateSchedule();
    void slotBaselineSchedule();
    void slotAddSchedule();
    void slotAddSubSchedule();
    void slotDeleteSelection();
    
private:
    ScheduleTreeView *m_view;

    KAction *actionCalculateSchedule;
    KAction *actionBaselineSchedule;
    KAction *actionAddSchedule;
    KAction *actionAddSubSchedule;
    KAction *actionDeleteSelection;
    
};


//-----------------------------
class KPLATOUI_EXPORT ScheduleLogTreeView : public QTreeView
{
    Q_OBJECT
public:
    ScheduleLogTreeView( QWidget *parent );

    Project *project() const { return logModel()->project(); }
    void setProject( Project *project ) { logModel()->setProject( project ); }

    ScheduleLogItemModel *logModel() const { return static_cast<ScheduleLogItemModel*>( m_model->sourceModel() ); }
    
    ScheduleManager *scheduleManager() const { return logModel()->manager(); }
    void setScheduleManager( ScheduleManager *manager ) { logModel()->setManager( manager ); }

    void setFilterWildcard( const QString &filter );
    QRegExp filterRegExp() const;

signals:
    void currentChanged( const QModelIndex& );
    void currentColumnChanged( QModelIndex, QModelIndex );
    void selectionChanged( const QModelIndexList );

    void contextMenuRequested( QModelIndex, const QPoint& );

public slots:
    void slotEditCopy();

protected slots:
    void headerContextMenuRequested( const QPoint &pos );
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );

private:
    QSortFilterProxyModel *m_model;
};

//----------------------------------------------
class KPLATOUI_EXPORT ScheduleLogView : public ViewBase
{
    Q_OBJECT
public:
    ScheduleLogView( KoDocument *part, QWidget *parent );

    void setupGui();
    virtual void setProject( Project *project );
    Project *project() const { return m_view->project(); }
    using ViewBase::draw;
    virtual void draw( Project &project );

    ScheduleLogItemModel *baseModel() const { return m_view->logModel(); }

    virtual void updateReadWrite( bool readwrite );

    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );
    void slotEditCopy();

protected slots:
    virtual void slotOptions();

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    void slotScheduleSelectionChanged( ScheduleManager *sm );

    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void updateActionsEnabled( const QModelIndex &index );
    void slotEnableActions( const ScheduleManager *sm );

private:
    ScheduleLogTreeView *m_view;
};


//-----------------------------
class KPLATOUI_EXPORT ScheduleHandlerView : public SplitterView
{
    Q_OBJECT
public:
    ScheduleHandlerView( KoDocument *part, QWidget *parent );
    
    Project *project() const { return 0; }

    ScheduleEditor *scheduleEditor() const { return m_scheduleEditor; }
    /// Returns a list of actionlist names for all shown views
    QStringList actionListNames() const;
    /// Returns the list of actions associated with the action list @p name
    virtual QList<QAction*> actionList( const QString name ) const;
    /// Always returns this (if we are called, we are hit)
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
