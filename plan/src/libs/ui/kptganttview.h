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
#include "kptnodeitemmodel.h"
#include "kptganttitemdelegate.h"

#include "ui_kptganttprintingoptions.h"
#include "ui_kptganttchartdisplayoptions.h"

#include <KGanttGlobal>
#include <KGanttView>
#include <KGanttDateTimeGrid>

class KoDocument;

class QPoint;
class QSplitter;

class KoPrintJob;

namespace KGantt
{
    class TreeViewRowController;
}

namespace KPlato
{

class Node;
class MilestoneItemModel;
class GanttItemModel;
class ResourceAppointmentsGanttModel;
class Task;
class Project;
class Relation;
class ScheduleManager;
class MyKGanttView;
class GanttPrintingOptions;
class GanttViewBase;
class NodeGanttViewBase;
class GanttPrintingOptionsWidget;

//---------------------------------------
class GanttChartDisplayOptionsPanel : public QWidget, public Ui::GanttChartDisplayOptions
{
    Q_OBJECT
public:
    explicit GanttChartDisplayOptionsPanel( GanttItemDelegate *delegate, QWidget *parent = 0 );

    void setValues( const GanttItemDelegate &del );

public Q_SLOTS:
    void slotOk();
    void setDefault();

Q_SIGNALS:
    void changed();

private:
    GanttItemDelegate *m_delegate;
};

class GanttViewSettingsDialog : public ItemViewSettupDialog
{
    Q_OBJECT
public:
    explicit GanttViewSettingsDialog( GanttViewBase *gantt, GanttItemDelegate *delegate, ViewBase *view, bool selectPrint = false );

protected Q_SLOTS:
    void slotOk();

private:
    GanttViewBase *m_gantt;
    GanttPrintingOptionsWidget *m_printingoptions;
};

//--------------------
class GanttPrintingOptions
{
public:
    GanttPrintingOptions();

    bool loadContext( const KoXmlElement &settings );
    void saveContext( QDomElement &settings ) const;

    bool printRowLabels;
    bool singlePage;
};

class KPLATOUI_EXPORT GanttPrintingOptionsWidget : public QWidget, public Ui::GanttPrintingOptionsWidget
{
    Q_OBJECT
public:
    explicit GanttPrintingOptionsWidget( QWidget *parent = 0 );

    GanttPrintingOptions options() const;

    void setPrintRowLabels( bool value ) { ui_printRowLabels->setChecked( value ); }
    bool printRowLabels() const { return ui_printRowLabels->isChecked(); }

    void setSinglePage( bool value )  { value ? ui_singlePage->setChecked( false ) : ui_multiplePages->setChecked( true ); }
    bool singlePage() const { return ui_singlePage->isChecked(); }

public Q_SLOTS:
    void setOptions(const GanttPrintingOptions &opt);
};

class GanttPrintingDialog : public PrintingDialog
{
    Q_OBJECT
public:
    GanttPrintingDialog( ViewBase *view, GanttViewBase *gantt );

    void startPrinting( RemovePolicy removePolicy );
    QList<QWidget*> createOptionWidgets() const;
    void printPage( int page, QPainter &painter );

    int documentLastPage() const;

protected Q_SLOTS:
    void slotPrintRowLabelsToogled( bool on );
    void slotSinglePageToogled( bool on );

protected:
    GanttViewBase *m_gantt;
    QRectF m_sceneRect;
    int m_horPages;
    int m_vertPages;
    double m_headerHeight;
    GanttPrintingOptionsWidget *m_options;
    QImage m_image;
};

class KPLATOUI_EXPORT GanttTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    explicit GanttTreeView(QWidget *parent);

};

class GanttZoomWidget : public QSlider {
    Q_OBJECT
public:
    explicit GanttZoomWidget( QWidget *parent );

    void setGrid( KGantt::DateTimeGrid *grid );

    void setEnableHideOnLeave( bool hide );

protected:
    void leaveEvent( QEvent *event );

private Q_SLOTS:
    void sliderValueChanged( int value );

private:
    bool m_hide;
    KGantt::DateTimeGrid *m_grid;
};

class KPLATOUI_EXPORT GanttViewBase : public KGantt::View
{
    Q_OBJECT
public:
    explicit GanttViewBase(QWidget *parent);

    GanttTreeView *treeView() const;
    GanttPrintingOptions printingOptions() const { return m_printOptions; }

    virtual bool loadContext( const KoXmlElement &settings );
    virtual void saveContext( QDomElement &settings ) const;

public Q_SLOTS:
    void setPrintingOptions( const GanttPrintingOptions &opt ) { m_printOptions = opt; }

protected:
    bool eventFilter(QObject *obj, QEvent *event);

    friend class GanttPrintingDialog;
    GanttPrintingOptions m_printOptions;

private:
    GanttZoomWidget *m_zoomwidget;
};

class NodeGanttViewBase : public GanttViewBase 
{
    Q_OBJECT
public:
    explicit NodeGanttViewBase(QWidget *parent);
    ~NodeGanttViewBase();

    NodeSortFilterProxyModel *sfModel() const;
    void setItemModel( ItemModelBase *model );
    ItemModelBase *model() const;
    void setProject( Project *project );
    Project *project() const { return m_project; }

    GanttItemDelegate *delegate() const { return m_ganttdelegate; }

    bool loadContext( const KoXmlElement &settings );
    void saveContext( QDomElement &settings ) const;

protected:
    Project *m_project;
    GanttItemDelegate *m_ganttdelegate;
    NodeItemModel m_defaultModel;
    KGantt::TreeViewRowController *m_rowController;
};

class KPLATOUI_EXPORT MyKGanttView : public NodeGanttViewBase
{
    Q_OBJECT
public:
    explicit MyKGanttView(QWidget *parent);

    GanttItemModel *model() const;
    void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );

public Q_SLOTS:
    void clearDependencies();
    void createDependencies();
    void addDependency( Relation *rel );
    void removeDependency( Relation *rel );
    void slotProjectCalculated( ScheduleManager *sm );

    void slotNodeInserted( Node *node );

protected:
    ScheduleManager *m_manager;
};

class KPLATOUI_EXPORT GanttView : public ViewBase
{
    Q_OBJECT
public:
    GanttView(KoPart *part, KoDocument *doc, QWidget *parent, bool readWrite = true);

    //~GanttView();

    virtual void setZoom( double zoom );
    void setupGui();
    Project *project() const { return m_gantt->project(); }
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

    void setShowSpecialInfo( bool on ) { m_gantt->model()->setShowSpecial( on ); }
    bool showSpecialInfo() const { return m_gantt->model()->showSpecial(); }

Q_SIGNALS:
    void modifyRelation( Relation *rel ) ;
    void addRelation( Node *par, Node *child );
    void modifyRelation( Relation *rel, int linkType ) ;
    void addRelation( Node *par, Node *child, int linkType );
    void itemDoubleClicked();

public Q_SLOTS:
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

protected Q_SLOTS:
    void slotContextMenuRequested( const QModelIndex&, const QPoint &pos );
    virtual void slotOptions();
    virtual void slotOptionsFinished( int result );

private:
    bool m_readWrite;
    int m_defaultFontSize;
    QSplitter *m_splitter;
    MyKGanttView *m_gantt;
    Project *m_project;

    QAction *actionShowProject;
    QDomDocument m_domdoc;
};

class MilestoneGanttViewSettingsDialog : public ItemViewSettupDialog
{
    Q_OBJECT
public:
    MilestoneGanttViewSettingsDialog( GanttViewBase *gantt, ViewBase *view, bool selectPrint = false );

protected Q_SLOTS:
    virtual void slotOk();

private:
    GanttViewBase *m_gantt;
    GanttPrintingOptionsWidget *m_printingoptions;
};


class KPLATOUI_EXPORT MilestoneKGanttView : public NodeGanttViewBase
{
    Q_OBJECT
public:
    explicit MilestoneKGanttView(QWidget *parent);

    MilestoneItemModel *model() const;
    void setProject( Project *project );
    void setScheduleManager( ScheduleManager *sm );

public Q_SLOTS:
    void slotProjectCalculated( ScheduleManager *sm );

protected:
    ScheduleManager *m_manager;
};

class KPLATOUI_EXPORT MilestoneGanttView : public ViewBase
{
    Q_OBJECT
public:
    MilestoneGanttView(KoPart *part, KoDocument *doc, QWidget *parent, bool readWrite = true);

    virtual void setZoom( double zoom );
    void show();
    virtual void setProject( Project *project );
    Project *project() const { return m_gantt->project(); }
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

Q_SIGNALS:
    void itemDoubleClicked();

public Q_SLOTS:
    void setScheduleManager( ScheduleManager *sm );

    void setShowTaskName( bool on ) { m_showTaskName = on; }
    void setShowProgress( bool on ) { m_showProgress = on; }
    void setShowPositiveFloat( bool on ) { m_showPositiveFloat = on; }
    void setShowCriticalTasks( bool on ) { m_showCriticalTasks = on; }
    void setShowNoInformation( bool on ) { m_showNoInformation = on; }

protected Q_SLOTS:
    void slotContextMenuRequested( const QModelIndex&, const QPoint &pos );
    virtual void slotOptions();

private:
    bool m_readWrite;
    int m_defaultFontSize;
    QSplitter *m_splitter;
    MilestoneKGanttView *m_gantt;
    bool m_showTaskName;
    bool m_showProgress;
    bool m_showPositiveFloat;
    bool m_showCriticalTasks;
    bool m_showNoInformation;
    Project *m_project;

};

class ResourceAppointmentsGanttViewSettingsDialog : public ItemViewSettupDialog
{
    Q_OBJECT
public:
    ResourceAppointmentsGanttViewSettingsDialog(GanttViewBase *gantt, ViewBase *view, bool selectPrint = false);

public Q_SLOTS:
    void slotOk();

private:
    GanttViewBase *m_gantt;
    GanttPrintingOptionsWidget *m_printingoptions;

};

class KPLATOUI_EXPORT ResourceAppointmentsGanttView : public ViewBase
{
    Q_OBJECT
public:
    ResourceAppointmentsGanttView(KoPart *part, KoDocument *doc, QWidget *parent, bool readWrite = true);
    ~ResourceAppointmentsGanttView();

    virtual void setZoom( double zoom );
    virtual void setProject( Project *project );
    Project *project() const;

    void setupGui();

    virtual bool loadContext( const KoXmlElement &context );
    virtual void saveContext( QDomElement &context ) const;

    void updateReadWrite( bool on );

    KoPrintJob *createPrintJob();

    GanttTreeView *treeView() const { return static_cast<GanttTreeView*>( m_gantt->leftView() ); }

    Node *currentNode() const;

Q_SIGNALS:
    void itemDoubleClicked();

public Q_SLOTS:
    void setScheduleManager( ScheduleManager *sm );

protected Q_SLOTS:
    void slotContextMenuRequested( const QModelIndex&, const QPoint &pos );
    virtual void slotOptions();

private:
    GanttViewBase *m_gantt;
    Project *m_project;
    ResourceAppointmentsGanttModel *m_model;
    KGantt::TreeViewRowController *m_rowController;
    QDomDocument m_domdoc;

};

}  //KPlato namespace

#endif
