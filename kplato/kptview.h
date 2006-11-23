/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2006 Dag Andersen <danders@get2net.dk>

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

#include <KoView.h>
#include <KoQueryTrader.h>

#include "kptcontext.h"

#include <QMenu>
#include <QDockWidget>
#include <QTreeWidget>


class QStackedWidget;
class QSplitter;

class KPrinter;
class KAction;
class KActionMenu;
class KSelectAction;
class KToggleAction;
class KStatusBarLabel;
class KSeparatorAction;

class KoView;

namespace KPlato
{

class View;
class ViewListItem;
class ViewListWidget;
class AccountsView;
class GanttView;
class ResourceView;
class TaskEditor;
class ResourceEditor;
//class ReportView;
class Part;
class DocumentChild;
class Node;
class Project;
class Resource;
class ResourceGroup;
class Relation;
class Context;
class ViewAdaptor;

class ViewListTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    ViewListTreeWidget( QWidget *parent );
    QTreeWidgetItem *findCategory( const QString cat );

protected:
    void drawRow( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    virtual void mousePressEvent ( QMouseEvent *event );

signals:
    void activated( QTreeWidgetItem* );

private slots:
    void handleMousePress( QTreeWidgetItem *item );
};

class ViewListWidget : public QWidget
{
    Q_OBJECT
public:
    enum ItemType { Category = QTreeWidgetItem::Type, View = QTreeWidgetItem::UserType, SubView, ChildDocument };
    
    ViewListWidget( QWidget *parent );//QString name, KMainWindow *parent );
    ~ViewListWidget();
    
    QTreeWidgetItem *addCategory( QString name );
    ViewListItem *addView(QTreeWidgetItem *category, const QString name, KoView *view, KoDocument *doc, QString icon = QString::null );
    ViewListItem *addView( QTreeWidgetItem *category, QString name, KoView *view, DocumentChild *ch, QString icon = QString::null );
    void setSelected( QTreeWidgetItem *item );
    ViewListItem *findItem( KoView *view, QTreeWidgetItem* parent = 0 );
    
signals:
    void activated( ViewListItem*, ViewListItem* );
    void createKofficeDocument( KoDocumentEntry &entry );
    
protected slots:
    void slotActionTriggered();
    void slotActivated( QTreeWidgetItem *item, QTreeWidgetItem *prev );
    void renameCategory();
    
protected:
    virtual void contextMenuEvent ( QContextMenuEvent *event );

private:
    void setupContextMenus();
    
private:
    ViewListTreeWidget *m_viewlist;
    Q3ValueList<KoDocumentEntry> m_lstEntries;
    KoDocumentEntry m_documentEntry;
    
    ViewListItem *m_contextitem;
    KSeparatorAction *m_separator;
    QList<QAction*> m_noitem;
    QList<QAction*> m_category;
    QList<QAction*> m_view;
    QList<QAction*> m_document;
    QList<QAction*> m_parts;
};


//-------------
class View : public KoView
{
    Q_OBJECT

public:
    View( Part* part, QWidget* parent = 0 );
    ~View();
    /**
     * Support zooming.
     */
    virtual void setZoom( double zoom );

    Part *getPart() const;

    Project& getProject() const;

    virtual void setupPrinter( KPrinter &printer );
    virtual void print( KPrinter &printer );

    QMenu *popupMenu( const QString& name );

    void projectCalculate();

    virtual ViewAdaptor* dbusObject();

    virtual bool setContext( Context &context );
    virtual void getContext( Context &context ) const;

    void setTaskActionsEnabled( QWidget *w, bool on );
    void setScheduleActionsEnabled();

    QWidget *canvas() const;

    //virtual QDockWidget *createToolBox();
    
    KoDocument *hitTest( const QPoint &viewPos );

public slots:
    void slotUpdate( bool calculate );
    void slotEditResource();
    void slotEditCut();
    void slotEditCopy();
    void slotEditPaste();
    void slotViewSelector( bool show );
    void slotViewGantt();
    void slotViewExpected();
    void slotViewOptimistic();
    void slotViewPessimistic();

    void slotViewGanttResources();
    void slotViewGanttTaskName();
    void slotViewGanttTaskLinks();
    void slotViewGanttProgress();
    void slotViewGanttFloat();
    void slotViewGanttCriticalTasks();
    void slotViewGanttCriticalPath();
    void slotViewGanttNoInformation();
    void slotViewTaskAppointments();
    void slotViewResources();
    void slotViewResourceAppointments();
    void slotViewAccounts();
    void slotViewTaskEditor();
    void slotAddTask();
    void slotAddSubTask();
    void slotAddMilestone();
    void slotProjectEdit();
    void slotDefineWBS();
    void slotGenerateWBS();
    
    void slotCreateKofficeDocument( KoDocumentEntry& );
    
    void slotConfigure();
    void slotAddRelation( Node *par, Node *child );
    void slotModifyRelation( Relation *rel );
    void slotAddRelation( Node *par, Node *child, int linkType );
    void slotModifyRelation( Relation *rel, int linkType );

    void slotExportGantt(); // testing
    void setTaskActionsEnabled( bool on );

    void slotRenameNode( Node *node, const QString& name );

    void slotPopupMenu( const QString& menuname, const QPoint &pos );
    void slotPopupMenu( const QString& menuname, const QPoint &pos, ViewListItem *item );

protected slots:
    void slotViewActivated( ViewListItem*, ViewListItem* );

    void slotProjectCalendar();
    void slotProjectWorktime();
    void slotProjectCalculate();
    void slotProjectCalculateExpected();
    void slotProjectCalculateOptimistic();
    void slotProjectCalculatePessimistic();
    void slotProjectAccounts();
    void slotProjectResources();
    void slotViewReportDesign();
    void slotViewReports();

    void slotOpenNode();
    void slotTaskProgress();
    void slotDeleteTask( QList<Node*> lst );
    void slotDeleteTask( Node *node );
    void slotDeleteTask();
    void slotIndentTask();
    void slotUnindentTask();
    void slotMoveTaskUp();
    void slotMoveTaskDown();

    void slotConnectNode();

    void slotCurrentChanged( int );

#ifndef NDEBUG
    void slotPrintDebug();
    void slotPrintSelectedDebug();
    void slotPrintCalendarDebug();
    void slotPrintTestDebug();
#else
    static void slotPrintDebug() { };
    static void slotPrintSelectedDebug() { };
    static void slotPrintCalendarDebug() { };
    static void slotPrintTestDebug() { };
#endif

protected:
    virtual void partActivateEvent( KParts::PartActivateEvent *event );
    virtual void guiActivateEvent( KParts::GUIActivateEvent *event );

    virtual void updateReadWrite( bool readwrite );
    Node *currentTask();
    Resource *currentResource();
    ResourceGroup *currentResourceGroup();
    void updateView( QWidget *widget );

private:
    QSplitter *m_sp;
    QStackedWidget *m_tab;
    GanttView *m_ganttview;
    ResourceView *m_resourceview;
    AccountsView *m_accountsview;
    TaskEditor *m_taskeditor;
    ResourceEditor *m_resourceeditor;
    //    ReportView *m_reportview;
    //    Q3PtrList<QString> m_reportTemplateFiles;

    ViewListWidget *m_viewlist;
    ViewListItem *m_viewlistItem; // requested popupmenu item
    
    //QDockWidget *m_toolbox;
    
    int m_viewGrp;
    int m_defaultFontSize;
    int m_currentEstimateType;

    bool m_updateGanttview;
    bool m_updateResourceview;
    bool m_updateAccountsview;

    KStatusBarLabel *m_estlabel;

    ViewAdaptor* m_dbus;

    // ------ Edit
    KAction *actionCut;
    KAction *actionCopy;
    KAction *actionPaste;

    KAction *actionIndentTask;
    KAction *actionUnindentTask;
    KAction *actionMoveTaskUp;
    KAction *actionMoveTaskDown;

    // ------ View
    KAction *actionViewGantt;

    KToggleAction *actionViewSelector;

    KToggleAction *actionViewExpected;
    KToggleAction *actionViewOptimistic;
    KToggleAction *actionViewPessimistic;

    KToggleAction *actionViewGanttResources;
    KToggleAction *actionViewGanttTaskName;
    KToggleAction *actionViewGanttTaskLinks;
    KToggleAction *actionViewGanttProgress;
    KToggleAction *actionViewGanttFloat;
    KToggleAction *actionViewGanttCriticalTasks;
    KToggleAction *actionViewGanttCriticalPath;
    KToggleAction *actionViewGanttNotScheduled;
    KToggleAction *actionViewTaskAppointments;

    KAction *actionViewResources;
    KToggleAction *actionViewResourceAppointments;
    KAction *actionViewAccounts;

    // ------ Insert
    KAction *actionAddTask;
    KAction *actionAddSubtask;
    KAction *actionAddMilestone;

    // ------ Project
    KAction *actionEditMainProject;
    KAction *actionEditStandardWorktime;
    KAction *actionEditCalendar;
    KAction *actionEditAccounts;
    KAction *actionEditResources;
    KActionMenu *actionCalculate;
    KAction *actionCalculateExpected;
    KAction *actionCalculateOptimistic;
    KAction *actionCalculatePessimistic;
    // ------ Reports
    KAction *actionFirstpage;
    KAction *actionPriorpage;
    KAction *actionNextpage;
    KAction *actionLastpage;

    // ------ Tools
    KAction *actionDefineWBS;
    KAction *actionGenerateWBS;
    
    // ------ Export (testing)
    KAction *actionExportGantt;

    // ------ Settings
    KAction *actionConfigure;

    // ------ Popup
    KAction *actionOpenNode;
    KAction *actionTaskProgress;
    KAction *actionDeleteTask;
    KAction *actionEditResource;

    //Test
    KAction *actNoInformation;
    
};

} //Kplato namespace

#endif
