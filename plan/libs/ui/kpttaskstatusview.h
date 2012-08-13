/* This file is part of the KDE project
  Copyright (C) 2007 - 2010 Dag Andersen <danders@get2net.dk>

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
#include <QTableWidget>

#include "KDChartBarDiagram"


class QTextBrowser;
class QItemSelection;

class KoDocument;
class KoPageLayoutWidget;
class PrintingHeaderFooter;

class KAction;

namespace KDChart
{
    class Chart;
    class CartesianCoordinatePlane;
    class CartesianAxis;
    class Legend;
};

namespace KPlato
{

class Project;
class Node;
class Task;
class ScheduleManager;
class TaskStatusItemModel;
class NodeItemModel;
class PerformanceStatusViewSettingsPanel;
class PerformanceStatusBase;

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
    TaskStatusView(KoPart *part, KoDocument *doc, QWidget *parent);
    
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
    explicit TaskStatusViewSettingsDialog( ViewBase *view, TaskStatusTreeView *treeview, QWidget *parent = 0 );

};

struct PerformanceChartInfo
{
    bool showBarChart;
    bool showLineChart;

    bool showCost;
    bool showBCWSCost;
    bool showBCWPCost;
    bool showACWPCost;

    bool showEffort;
    bool showBCWSEffort;
    bool showBCWPEffort;
    bool showACWPEffort;

    bool bcwsCost() const { return showCost && showBCWSCost; }
    bool bcwpCost() const { return showCost && showBCWPCost; }
    bool acwpCost() const { return showCost && showACWPCost; }
    bool bcwsEffort() const { return showEffort && showBCWSEffort; }
    bool bcwpEffort() const { return showEffort && showBCWPEffort; }
    bool acwpEffort() const { return showEffort && showACWPEffort; }

    PerformanceChartInfo() {
        showBarChart = false; showLineChart = true;
        showCost = showBCWSCost = showBCWPCost = showACWPCost = true;
        showEffort = showBCWSEffort = showBCWPEffort = showACWPEffort = true;
    }
    bool operator!=( const PerformanceChartInfo &o ) const { return ! operator==( o ); }
    bool operator==( const PerformanceChartInfo &o ) const {
        return showBarChart == o.showBarChart && showLineChart == o.showLineChart &&
                showCost == o.showCost && 
                showBCWSCost == o.showBCWSCost &&
                showBCWPCost == o.showBCWPCost &&
                showACWPCost == o.showACWPCost &&
                showEffort == o.showEffort &&
                showBCWSEffort == o.showBCWSEffort &&
                showBCWPEffort == o.showBCWPEffort &&
                showACWPEffort == o.showACWPEffort;
    }
};

//----------------------------------
class KPLATOUI_EXPORT PerformanceStatusPrintingDialog : public PrintingDialog
{
    Q_OBJECT
public:
    PerformanceStatusPrintingDialog( ViewBase *view, PerformanceStatusBase *chart, Project *project = 0 );
    ~PerformanceStatusPrintingDialog() {}

    virtual int documentLastPage() const;
    virtual QList<QWidget*> createOptionWidgets() const;

protected:
    virtual void printPage( int pageNumber, QPainter &painter );

private:
    PerformanceStatusBase *m_chart;
    Project *m_project;
};

class PerformanceStatusBase : public QWidget, public Ui::PerformanceStatus
{
    Q_OBJECT
public:
    explicit PerformanceStatusBase( QWidget *parent );
    
    void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );

    ChartItemModel *model() const { return const_cast<ChartItemModel*>( &m_chartmodel ); }
    
    void setupChart();
    void setChartInfo( const PerformanceChartInfo &info );
    PerformanceChartInfo chartInfo() const { return m_chartinfo; }
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &context ) const;

    /// Create a print job dialog
    KoPrintJob *createPrintJob( ViewBase *parent );

public slots:
    void refreshChart();

protected:
    void contextMenuEvent( QContextMenuEvent *event );
    
    void createBarChart();
    void createLineChart();
    void setEffortValuesVisible( bool visible );
    void setCostValuesVisible( bool visible );

    void drawValues();

protected slots:
    void slotUpdate();
    void slotLocaleChanged();

private:
    struct ChartContents {
        ~ChartContents() {
            delete effortplane;
            delete costplane;
            delete effortdiagram;
            delete costdiagram;
            delete dateaxis;
            delete effortaxis;
            delete costaxis;
        }
        ChartProxyModel costproxy;
        ChartProxyModel effortproxy;
    
        KDChart::CartesianCoordinatePlane *effortplane;
        KDChart::CartesianCoordinatePlane *costplane;
        KDChart::AbstractDiagram *effortdiagram;
        KDChart::AbstractDiagram *costdiagram;
        KDChart::CartesianAxis *effortaxis;
        KDChart::CartesianAxis *costaxis;
        KDChart::CartesianAxis *dateaxis;
    };
    void setupChart( ChartContents &cc );

private:
    Project *m_project;
    ScheduleManager *m_manager;
    PerformanceChartInfo m_chartinfo;

    ChartItemModel m_chartmodel;
    KDChart::Legend *m_legend;
    KDChart::BarDiagram m_legenddiagram;
    struct ChartContents m_barchart;
    struct ChartContents m_linechart;
};

//----------------------------------
class KPLATOUI_EXPORT ProjectStatusView : public ViewBase
{
    Q_OBJECT
public:
    ProjectStatusView(KoPart *part, KoDocument *doc, QWidget *parent );

    void setupGui();
    Project *project() const { return m_project; }
    virtual void setProject( Project *project );

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;

    KoPrintJob *createPrintJob();

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

    /// Loads context info into this view.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view.
    virtual void saveContext( QDomElement &context ) const;

    TreeViewBase *treeView() const { return m_tree; }
    PerformanceStatusBase *chartView() const { return m_chart; }
    
    KoPrintJob *createPrintJob( ViewBase *view );

protected slots:
    void slotSelectionChanged( const QItemSelection & selected, const QItemSelection & deselected );
    void resizeSplitters();

private:
    TreeViewBase *m_tree;
    PerformanceStatusBase *m_chart;
};


//----------------------------------
class KPLATOUI_EXPORT PerformanceStatusView : public ViewBase
{
    Q_OBJECT
public:
    PerformanceStatusView(KoPart *part, KoDocument *doc, QWidget *parent );

    void setupGui();
    Project *project() const { return m_view->project(); }
    virtual void setProject( Project *project );

    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &context );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &context ) const;

    Node *currentNode() const;

    KoPrintJob *createPrintJob();

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
    explicit PerformanceStatusViewSettingsDialog( PerformanceStatusView *view, PerformanceStatusTreeView *treeview, QWidget *parent = 0 );

};

class ProjectStatusViewSettingsDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit ProjectStatusViewSettingsDialog( ViewBase *base, PerformanceStatusBase *view, QWidget *parent = 0 );

protected slots:
    void slotOk();

private:
    ViewBase *m_base;
    KoPageLayoutWidget *m_pagelayout;
    PrintingHeaderFooter *m_headerfooter;
};



} //namespace KPlato


#endif
