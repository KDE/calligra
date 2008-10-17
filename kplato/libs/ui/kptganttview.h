/* This file is part of the KDE project
  Copyright (C) 2005 Dag Andersen <danders@get2net.dk>
  Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

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

#ifndef KPTGANTTVIEW_H
#define KPTGANTTVIEW_H

#include "kplatoui_export.h"

#include "kptviewbase.h"
#include "kptitemviewsettup.h"

#include "ui_kptganttprintingoptions.h"
#include "ui_kptganttchartdisplayoptions.h"

#include <kdganttglobal.h>
#include <kdganttview.h>

#include "kdganttitemdelegate.h"

#include <QBrush>

namespace KDGantt
{
    class StyleOptionGanttItem;
    class Constraint;
}

class KoDocument;

class QPainter;
class QPoint;
class QSplitter;
class QModelIndex;
class QPointF;

class KoPrintJob;


namespace KPlato
{

class TaskAppointmentsView;

class Node;
class MilestoneItemModel;
class NodeItemModel;
class Task;
class Project;
class Relation;
class ScheduleManager;
class MyKDGanttView;
class GanttPrintingOptions;

class KPLATOMODELS_EXPORT GanttItemDelegate : public KDGantt::ItemDelegate
{
    Q_OBJECT
public:
    GanttItemDelegate( QObject *parent = 0 );

//    virtual QString toolTip( const QModelIndex& idx ) const;
    virtual KDGantt::Span itemBoundingSpan( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const;
    virtual void paintGanttItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx );
    
    virtual void paintConstraintItem( QPainter* p, const QStyleOptionGraphicsItem& opt, const QPointF& start, const QPointF& end, const KDGantt::Constraint &constraint );

    QString itemText( const QModelIndex& idx, int type ) const;

    bool showResources;
    bool showTaskName;
    bool showTaskLinks;
    bool showProgress;
    bool showPositiveFloat;
    bool showCriticalPath;
    bool showCriticalTasks;
    bool showAppointments;
    bool showNoInformation;

private:
    Q_DISABLE_COPY(GanttItemDelegate);
    QBrush m_criticalBrush;

};

//--------------------------------------
class GanttChartDisplayOptionsPanel : public QWidget, public Ui::GanttChartDisplayOptions
{
    Q_OBJECT
public:
    explicit GanttChartDisplayOptionsPanel( GanttItemDelegate *delegate, QWidget *parent = 0 );

    void setValues( const GanttItemDelegate &del );

public slots:
    void slotOk();
    void setDefault();

signals:
    void changed();

private:
    GanttItemDelegate *m_delegate;
};

class GanttViewSettingsDialog : public ItemViewSettupDialog
{
    Q_OBJECT
public:
    explicit GanttViewSettingsDialog( TreeViewBase *view, GanttItemDelegate *delegate, QWidget *parent = 0 );

};

//--------------------
class KPLATOUI_EXPORT GanttPrintingOptions : public QWidget, public Ui::GanttPrintingOptionsWidget
{
    Q_OBJECT
public:
    explicit GanttPrintingOptions( QWidget *parent = 0 );

    void setPrintRowLabels( bool value ) { ui_printRowLabels->setChecked( value ); }
    bool printRowLabels() const { return ui_printRowLabels->isChecked(); }
    
    void setSinglePage( bool value )  { value ? ui_singlePage->setChecked( false ) : ui_multiplePages->setChecked( true ); }
    bool singlePage() const { return ui_singlePage->isChecked(); }

};

class GanttPrintingDialog : public PrintingDialog
{
    Q_OBJECT
public:
    GanttPrintingDialog( ViewBase *view, KDGantt::View *gantt );
    
    void startPrinting( RemovePolicy removePolicy );
    QList<QWidget*> createOptionWidgets() const;
    void printPage( int page, QPainter &painter );
    
    int documentLastPage() const;
    
protected:
    KDGantt::View *m_gantt;
    QRectF m_sceneRect;
    int m_horPages;
    int m_vertPages;
    double m_headerHeight;
    GanttPrintingOptions *m_options;
    bool m_singlePage;
    bool m_printRowLabels;
    QRectF m_pageRect;
};

class GanttTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    GanttTreeView( QWidget *parent );

};

class MyKDGanttView : public KDGantt::View
{
    Q_OBJECT
public:
    MyKDGanttView( QWidget *parent );
    
    NodeItemModel *model() const { return m_model; }
    void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );
    void update();

    void clearDependencies();
    void createDependencies();

    GanttTreeView *treeView() const;
    
    GanttItemDelegate *delegate() const { return m_ganttdelegate; }
    
    bool loadContext( const KoXmlElement &settings );
    void saveContext( QDomElement &settings ) const;

public slots:
    void addDependency( Relation *rel );
    void removeDependency( Relation *rel );
    void slotProjectCalculated( ScheduleManager *sm );
    
    void slotNodeInserted( Node *node );
    
protected:
    NodeItemModel *m_model;
    Project *m_project;
    ScheduleManager *m_manager;
    GanttItemDelegate *m_ganttdelegate;
};

class KPLATOUI_EXPORT GanttView : public ViewBase
{
    Q_OBJECT
public:
    GanttView( KoDocument *part, QWidget *parent, bool readWrite = true );

    //~GanttView();

    virtual void setZoom( double zoom );
    void setupGui();
    virtual void setProject( Project *project );
    
    using ViewBase::draw;
    virtual void draw( Project &project );
    virtual void drawChanges( Project &project );

    Node *currentNode() const;

    void clear();

    virtual bool loadContext( const KoXmlElement &context );
    virtual void saveContext( QDomElement &context ) const;

    void updateReadWrite( bool on );

    KoPrintJob *createPrintJob();
    
signals:
    void modifyRelation( Relation *rel ) ;
    void addRelation( Node *par, Node *child );
    void modifyRelation( Relation *rel, int linkType ) ;
    void addRelation( Node *par, Node *child, int linkType );
    void itemDoubleClicked();

public slots:
    void setScheduleManager( ScheduleManager *sm );
    void setShowResources( bool on );
    void setShowTaskName( bool on );
    void setShowTaskLinks( bool on );
    void setShowProgress( bool on );
    void setShowPositiveFloat( bool on );
    void setShowCriticalTasks( bool on );
    void setShowCriticalPath( bool on );
    void setShowNoInformation( bool on );
    void setShowAppointments( bool on );
    void update();

protected slots:
    void slotContextMenuRequested( QModelIndex, const QPoint &pos );
    virtual void slotOptions();
    
private:
    bool m_readWrite;
    int m_defaultFontSize;
    QSplitter *m_splitter;
    MyKDGanttView *m_gantt;
    TaskAppointmentsView *m_taskView;
    Project *m_project;
};

class KPLATOUI_EXPORT MilestoneKDGanttView : public KDGantt::View
{
    Q_OBJECT
public:
    MilestoneKDGanttView( QWidget *parent );

    MilestoneItemModel *model() const { return m_model; }
    void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );
    void update();

    GanttTreeView *treeView() const;
    
public slots:
    void slotProjectCalculated( ScheduleManager *sm );

protected:
    Project *m_project;
    ScheduleManager *m_manager;
    MilestoneItemModel *m_model;
};

class KPLATOUI_EXPORT MilestoneGanttView : public ViewBase
{
    Q_OBJECT
public:
    MilestoneGanttView( KoDocument *part, QWidget *parent, bool readWrite = true );

    virtual void setZoom( double zoom );
    void show();
    virtual void setProject( Project *project );
    using ViewBase::draw;
    virtual void draw( Project &project );
    virtual void drawChanges( Project &project );

    void setupGui();
    
    Node *currentNode() const;

    void clear();

    virtual bool loadContext( const KoXmlElement &context );
    virtual void saveContext( QDomElement &context ) const;

    void updateReadWrite( bool on );

    bool showNoInformation() const { return m_showNoInformation; }

    KoPrintJob *createPrintJob();

signals:
    void itemDoubleClicked();

public slots:
    void setScheduleManager( ScheduleManager *sm );

    void setShowTaskName( bool on ) { m_showTaskName = on; }
    void setShowProgress( bool on ) { m_showProgress = on; }
    void setShowPositiveFloat( bool on ) { m_showPositiveFloat = on; }
    void setShowCriticalTasks( bool on ) { m_showCriticalTasks = on; }
    void setShowNoInformation( bool on ) { m_showNoInformation = on; }
    void update();

protected slots:
    void slotContextMenuRequested( QModelIndex, const QPoint &pos );
    virtual void slotOptions();

private:
    bool m_readWrite;
    int m_defaultFontSize;
    QSplitter *m_splitter;
    MilestoneKDGanttView *m_gantt;
    bool m_showTaskName;
    bool m_showProgress;
    bool m_showPositiveFloat;
    bool m_showCriticalTasks;
    bool m_showNoInformation;
    Project *m_project;

};

}  //KPlato namespace

#endif
