/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2010 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATO_VIEW
#define KPLATO_VIEW

#include "kplato_export.h"

#include <KoView.h>

#include "kptcontext.h"
#include "kptviewbase.h"

#include <QMenu>
#include <QDockWidget>
#include <QTimer>
#include <QMap>
#include <QPointer>
#include <QPrintDialog>

#include <KConfigDialog>

class QStackedWidget;
class QSplitter;
class KUndo2Command;

class KAction;
class KToggleAction;
class QLabel;
class KConfigSkeleton;
class KConfigSkeletonItem;

class KoView;

namespace KPlato
{

class View;
class ViewBase;
class ViewListItem;
class ViewListWidget;
struct ViewInfo;
class AccountsView;
class GanttView;
class PertEditor;
class ResourceView;
class AccountsEditor;
class TaskEditor;
class CalendarEditor;
class ScheduleEditor;
class ScheduleManager;
class CalculateScheduleCmd;
class ResourceAssignmentView;
class TaskStatusView;
class TaskModuleModel;
class Calendar;
class Part;
class PartPart;
class Node;
class Project;
class Task;
class MainSchedule;
class Schedule;
class Resource;
class ResourceGroup;
class Relation;
class Context;
class ViewAdaptor;
class HtmlView;
class ReportView;
class ReportWidget;

class ReportDesignDialog;

class DockWidget;

class ConfigDialog : public KConfigDialog
{
    Q_OBJECT
public:
    ConfigDialog( QWidget *parent, const QString &name, KConfigSkeleton *config );

protected slots:
    /// Return true if any widget has changed
    virtual bool hasChanged();
    /**
    * Update the settings from the dialog.
    * Virtual function for custom additions.
    *
    * Example use: User clicks Ok or Apply button in a configure dialog.
    */
    virtual void updateSettings();

    /**
    * Update the dialog based on the settings.
    * Virtual function for custom additions.
    *
    * Example use: Initialisation of dialog.
    * Example use: User clicks Reset button in a configure dialog.
    */
    virtual void updateWidgets();

    /**
    * Update the dialog based on the default settings.
    * Virtual function for custom additions.
    *
    * Example use: User clicks Defaults button in a configure dialog.
    */
    virtual void updateWidgetsDefault();

  /**
   * Returns whether the current state of the dialog is
   * the same as the default configuration.
   */
  virtual bool isDefault();

private:
    KConfigSkeleton *m_config;
    QMap<QString, QByteArray> m_signalsmap;
    QMap<QWidget*, KConfigSkeletonItem*> m_itemmap;
    QMap<QString, QByteArray> m_propertymap;
};

//-------------
class KPLATO_EXPORT View : public KoView
{
    Q_OBJECT

public:
    explicit View(KoPart *part, Part *doc, QWidget *parent = 0);
    ~View();

    Part *getPart() const;

    KoPart *getKoPart() const;

    Project& getProject() const;

    QMenu *popupMenu( const QString& name );

    virtual bool loadContext();
    virtual void saveContext( QDomElement &context ) const;

    /// Load the workpackage from @p url into @p project. Return true if successful, else false.
    bool loadWorkPackage( Project &project, const KUrl &url );

    QWidget *canvas() const;

    KoPageLayout pageLayout() const;

    ScheduleManager *currentScheduleManager() const;
    long activeScheduleId() const;
    void setActiveSchedule( long id );

    /// Returns the default view information like standard name and tooltip for view type @p type
    ViewInfo defaultViewInfo( const QString type ) const;
    /// Returns the default category information like standard name and tooltip for category type @p type
    ViewInfo defaultCategoryInfo( const QString type ) const;

    ViewBase *createTaskEditor( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createResourceEditor( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createAccountsEditor( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createCalendarEditor( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createScheduleHandler( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ScheduleEditor *createScheduleEditor( QWidget *parent );
    ViewBase *createScheduleEditor( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createDependencyEditor( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createPertEditor( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createProjectStatusView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createPerformanceStatusView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createTaskStatusView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createTaskView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createTaskWorkPackageView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createGanttView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createMilestoneGanttView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createResourceAppointmentsView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createResourceAppointmentsGanttView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createAccountsView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createResourceAssignmentView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createChartView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );
    ViewBase *createReportView( ViewListItem *cat, const QString tag, const QString &name = QString(), const QString &tip = QString(), int index = -1 );

    KoPrintJob * createPrintJob();
    QPrintDialog* createPrintDialog(KoPrintJob*, QWidget*);

    virtual KoZoomController *zoomController() const {
        return 0;
    }

signals:
    void currentScheduleManagerChanged( ScheduleManager *sm );

public slots:
    void slotUpdate();
    void slotCreateTemplate();
    void slotEditResource();
    void slotEditResource( Resource *resource );
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotRefreshView();
    void slotViewSelector( bool show );

    void slotAddTask();
    void slotAddSubTask();
    void slotAddMilestone();
    void slotAddSubMilestone();
    void slotProjectEdit();
    void slotDefineWBS();
    void slotCurrencyConfig();

    void slotCreateView();

    void slotConfigure();
    void slotIntroduction();

    void slotAddRelation( Node *par, Node *child );
    void slotModifyRelation( Relation *rel );
    void slotAddRelation( Node *par, Node *child, int linkType );
    void slotModifyRelation( Relation *rel, int linkType );
    void slotModifyRelation();
    void slotDeleteRelation();

    void slotRenameNode( Node *node, const QString& name );

    void slotPopupMenu( const QString& menuname, const QPoint &pos );
    void slotPopupMenu( const QString& menuname, const QPoint &pos, ViewListItem *item );

    void addViewListItem( const ViewListItem *item, const ViewListItem *parent, int index );
    void removeViewListItem( const ViewListItem *item );

protected slots:
    void slotGuiActivated( ViewBase *view, bool );
    void slotViewActivated( ViewListItem*, ViewListItem* );
    void slotPlugScheduleActions();
    void slotViewSchedule( QAction *act );
    void slotScheduleChanged( MainSchedule* );
    void slotScheduleAdded( const MainSchedule * );
    void slotScheduleRemoved( const MainSchedule * );
    void slotSelectionChanged( ScheduleManager *sm );

    void slotAddScheduleManager( Project *project );
    void slotDeleteScheduleManager( Project *project, ScheduleManager *sm );
    void slotMoveScheduleManager( ScheduleManager *sm, ScheduleManager *parent, int index );
    void slotCalculateSchedule( Project*, ScheduleManager* );
    void slotBaselineSchedule( Project *project, ScheduleManager *sm );

    void slotProjectWorktime();

    void slotOpenNode();
    void slotOpenNode( Node *node );
    void slotTaskProgress();
    void slotTaskDescription();
    void slotDeleteTask( QList<Node*> lst );
    void slotDeleteTask( Node *node );
    void slotDeleteTask();
    void slotIndentTask();
    void slotUnindentTask();
    void slotMoveTaskUp();
    void slotMoveTaskDown();

    void slotConnectNode();

    void slotDeleteResource( Resource *resource );
    void slotDeleteResourceGroup( ResourceGroup *group );
    void slotDeleteResourceObjects( QObjectList );

    void slotCurrentChanged( int );

    void slotInsertFile();

    void slotWorkPackageLoaded();
    void slotMailWorkpackage( Node *node, Resource *resource = 0 );
    void slotMailWorkpackages( QList<Node*> &nodes, Resource *resource = 0 );

    void slotOpenUrlRequest( HtmlView *v, const KUrl &url );

    void slotProjectCalculated( ScheduleManager *sm );

    void slotUpdateViewInfo( ViewListItem *itm );

    void slotOpenReportFile();
    void slotModifyReportDefinition( KUndo2Command *cmd );

    void saveTaskModule( const KUrl &url, Project *project );
    void removeTaskModule( const KUrl &url );

protected:
    virtual void guiActivateEvent( KParts::GUIActivateEvent *event );
    virtual void updateReadWrite( bool readwrite );

    QList<QAction*> sortedActionList();
    QAction *addScheduleAction( Schedule *sch );
    void setLabel( ScheduleManager *sm = 0 );
    Task *currentTask() const;
    Node *currentNode() const;
    Resource *currentResource();
    ResourceGroup *currentResourceGroup();
    Calendar *currentCalendar();
    void updateView( QWidget *widget );

    ViewBase *currentView() const;

    ViewBase *createWelcomeView();

private slots:
    void slotActionDestroyed( QObject *o );
    void slotViewListItemRemoved( ViewListItem *item );
    void slotViewListItemInserted( ViewListItem *item, ViewListItem *parent, int index );

    void slotCreateReportView( ReportDesignDialog *dlg );

    void slotProjectEditFinished( int result );
    void slotTaskEditFinished( int result );
    void slotSummaryTaskEditFinished( int result );
    void slotEditResourceFinished( int result );
    void slotProjectWorktimeFinished( int result );
    void slotDefineWBSFinished( int result );
    void slotCurrencyConfigFinished( int result );
    void slotInsertFileFinished( int result );
    void slotAddSubTaskFinished( int result );
    void slotAddTaskFinished( int result );
    void slotAddSubMilestoneFinished( int result );
    void slotAddMilestoneFinished( int result );
    void slotTaskProgressFinished( int result );
    void slotMilestoneProgressFinished( int result );
    void slotTaskDescriptionFinished( int result );
    void slotAddRelationFinished( int result );
    void slotModifyRelationFinished( int result );
    void slotReportDesignFinished( int result );
    void slotOpenReportFileFinished( int result );
    void slotCreateViewFinished( int result );

    void slotRemoveCommands();

    void hideToolDocker();
    void initiateViews();
    void slotViewScheduleManager();

private:
    void createViews();
    ViewBase *createView( ViewListItem *cat, const QString &type, const QString &tag, const QString &name, const QString &tip, int index = -1 );

    QString standardTaskStatusReport() const;

private:
    QSplitter *m_sp;
    QStackedWidget *m_tab;

    ViewListWidget *m_viewlist;
    ViewListItem *m_viewlistItem; // requested popupmenu item

    //QDockWidget *m_toolbox;

    int m_viewGrp;
    int m_defaultFontSize;
    int m_currentEstimateType;

    bool m_updateAccountsview;
    bool m_updateResourceAssignmentView;
    bool m_updatePertEditor;

    QLabel *m_estlabel;

    ViewAdaptor* m_dbus;

    QActionGroup *m_scheduleActionGroup;
    QMap<QAction*, Schedule*> m_scheduleActions;
    // if multiple changes occur, only issue the last change
    bool m_trigged;
    ScheduleManager *m_nextScheduleManager;

    QMultiMap<ScheduleManager*, CalculateScheduleCmd*> m_calculationcommands;
    QList<KUndo2Command*> m_undocommands;

    bool m_readWrite;

    QList<DockWidget*> m_dockers;

    // ------ File
    QAction *actionCreateTemplate;

    // ------ Edit
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;

    // ------ View
    KToggleAction *actionViewSelector;

    // ------ Insert
    // ------ Project
    KAction *actionEditMainProject;

    // ------ Tools
    KAction *actionEditStandardWorktime;
    KAction *actionDefineWBS;
    KAction *actionInsertFile;
    KAction *actionCurrencyConfig;

    KAction *actionOpenReportFile;

    // ------ Settings
    KAction *actionConfigure;

    // ------ Help
    KAction *actionIntroduction;

    // ------ Popup
    KAction *actionOpenNode;
    KAction *actionTaskProgress;
    KAction *actionTaskDescription;
    KAction *actionDeleteTask;
    KAction *actionIndentTask;
    KAction *actionUnindentTask;
    KAction *actionMoveTaskUp;
    KAction *actionMoveTaskDown;

    KAction *actionEditResource;
    KAction *actionEditRelation;
    KAction *actionDeleteRelation;

    //Test
    KAction *actNoInformation;

    QMap<ViewListItem*, QAction*> m_reportActionMap;

    KoPart *m_partpart;
};


} //Kplato namespace

#endif
