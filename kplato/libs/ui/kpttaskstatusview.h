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

#ifndef TASKSTATUSVIEW_H
#define TASKSTATUSVIEW_H

#include "kplatoui_export.h"

#include "kptitemmodelbase.h"

#include "kptviewbase.h"
#include "ui_kpttaskstatusviewsettingspanel.h"
#include "kptitemviewsettup.h"
#include "ui_kptperformancestatus.h"
#include "ui_kptperformancestatusviewsettingspanel.h"
#include "kptnodechartmodel.h"

#include <QSplitter>

class QTextBrowser;
class QItemSelection;

class KoDocument;

class KAction;

namespace KPlato
{

class Project;
class Node;
class Task;
class ScheduleManager;
class TaskStatusItemModel;
class NodeItemModel;
class PerformanceStatusViewSettingsPanel;

typedef QList<Node*> NodeList;


class KPLATOUI_EXPORT TaskStatusTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    TaskStatusTreeView( QWidget *parent );
    
    
    //void setSelectionModel( QItemSelectionModel *selectionModel );

    TaskStatusItemModel *model() const;
    
    Project *project() const;
    void setProject( Project *project );
    
    
    int defaultWeekday() const { return Qt::Friday; }
    int weekday() const;
    void setWeekday( int day );
    
    int defaultPeriod() const { return 7; }
    int period() const;
    void setPeriod( int days );

    int defaultPeriodType() const;
    int periodType() const;
    void setPeriodType( int type );

protected:
    void dragMoveEvent(QDragMoveEvent *event);
};


class KPLATOUI_EXPORT TaskStatusView : public ViewBase
{
    Q_OBJECT
public:
    TaskStatusView( KoDocument *part, QWidget *parent );
    
    void setupGui();
    virtual void setProject( Project *project );
    Project *project() const { return m_view->project(); }
    using ViewBase::draw;
    virtual void draw( Project &project );

    TaskStatusItemModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool readwrite );
    virtual Node *currentNode() const;
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;

    KoPrintJob *createPrintJob();
    
signals:
    void openNode();

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

    void setScheduleManager( ScheduleManager *sm );

    virtual void slotRefreshView();

protected slots:
    virtual void slotOptions();

protected:
    void updateActionsEnabled( bool on );

private slots:
    void slotContextMenuRequested( const QModelIndex &index, const QPoint& pos );
    void slotContextMenuRequested( Node *node, const QPoint& pos );
    void slotSplitView();
    
private:
    Project *m_project;
    int m_id;
    TaskStatusTreeView *m_view;

};

//--------------------------------------
class TaskStatusViewSettingsPanel : public QWidget, public Ui::TaskStatusViewSettingsPanel
{
    Q_OBJECT
public:
    explicit TaskStatusViewSettingsPanel( TaskStatusTreeView *view, QWidget *parent = 0 );

public slots:
    void slotOk();
    void setDefault();

signals:
    void changed();

private:
    TaskStatusTreeView *m_view;
};

class TaskStatusViewSettingsDialog : public SplitItemViewSettupDialog
{
    Q_OBJECT
public:
    explicit TaskStatusViewSettingsDialog( TaskStatusTreeView *view, QWidget *parent = 0 );

};

//----------------------------------
class PerformanceStatusBase : public QWidget, public Ui::PerformanceStatus
{
    Q_OBJECT
public:
    explicit PerformanceStatusBase( QWidget *parent );
    
    void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );

    void draw();
    NodeChartModel *model() const { return const_cast<NodeChartModel*>( &m_model ); }
    
    void setDataShown( const NodeChartModel::DataShown &show ) {
        model()->setDataShown( show );
    }
    NodeChartModel::DataShown dataShown() const {
        return model()->dataShown();
    }
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &context ) const;
    
protected:
    void contextMenuEvent( QContextMenuEvent *event );
    
    void drawValues();
    void drawPlot( Project &p, ScheduleManager &sm );
    QList<QPointF> drawAxis( const ChartAxisIndex &idx );
    void drawData( const ChartAxisIndex &idx );
    void drawData( const ChartDataIndex &index, const ChartAxisIndex &axisSet );

protected slots:
    void slotReset();
    void slotUpdate();

private:
    Project *m_project;
    ScheduleManager *m_manager;
    NodeChartModel m_model;
    
};

//----------------------------------
class KPLATOUI_EXPORT ProjectStatusView : public ViewBase
{
    Q_OBJECT
public:
    ProjectStatusView( KoDocument *part, QWidget *parent );

    void setupGui();
    Project *project() const { return m_project; }
    virtual void setProject( Project *project );

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;

    using ViewBase::draw;
    virtual void draw();

public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

    void setScheduleManager( ScheduleManager *sm );

protected:
    void updateActionsEnabled( bool on );

protected slots:
    virtual void slotOptions();

private:
    Project *m_project;
    PerformanceStatusBase *m_view;
};

//----------------------------------
class PerformanceStatusTreeView : public QSplitter
{
    Q_OBJECT
public:
    explicit PerformanceStatusTreeView( QWidget *parent );

    NodeItemModel *nodeModel() const;
    Project *project() const;
    void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );
    void draw();

    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

    TreeViewBase *treeView() const { return m_tree; }
    PerformanceStatusBase *chartView() const { return m_chart; }
    
protected slots:
    void slotSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );
    
private:
    TreeViewBase *m_tree;
    PerformanceStatusBase *m_chart;
};


//----------------------------------
class KPLATOUI_EXPORT PerformanceStatusView : public ViewBase
{
    Q_OBJECT
public:
    PerformanceStatusView( KoDocument *part, QWidget *parent );

    void setupGui();
    Project *project() const { return m_view->project(); }
    virtual void setProject( Project *project );

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &context ) const;

    using ViewBase::draw;
    virtual void draw();
    
    Node *currentNode() const;
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

    void setScheduleManager( ScheduleManager *sm );

protected slots:
    virtual void slotOptions();

protected:
    void updateActionsEnabled( bool on );

private slots:
    void slotContextMenuRequested( Node *node, const QPoint& pos );
    void slotContextMenuRequested( const QModelIndex &index, const QPoint& pos );

private:
    PerformanceStatusTreeView *m_view;

};

//--------------------------------------
class PerformanceStatusViewSettingsPanel : public QWidget, public Ui::PerformanceStatusViewSettingsPanel
{
    Q_OBJECT
public:
    explicit PerformanceStatusViewSettingsPanel( PerformanceStatusBase *view, QWidget *parent = 0 );

public slots:
    void slotOk();
    void setDefault();

signals:
    void changed();

private:
    PerformanceStatusBase *m_view;
};

class PerformanceStatusViewSettingsDialog : public ItemViewSettupDialog
{
    Q_OBJECT
public:
    explicit PerformanceStatusViewSettingsDialog( PerformanceStatusTreeView *view, QWidget *parent = 0 );

};

class ProjectStatusViewSettingsDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit ProjectStatusViewSettingsDialog( PerformanceStatusBase *view, QWidget *parent = 0 );

};



} //namespace KPlato


#endif
