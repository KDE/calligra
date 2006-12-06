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

#include <kprinter.h>
#include <kmessagebox.h>

#include "KoDocumentInfo.h"
#include <KoMainWindow.h>
#include <KoToolManager.h>
#include <KoToolBox.h>

#include <QApplication>
#include <QDockWidget>
#include <QIcon>
#include <QLayout>
#include <QColor>
#include <QLabel>
#include <QString>
#include <QStringList>
#include <qsize.h>
#include <QStackedWidget>
#include <QHeaderView>
#include <QRect>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QItemDelegate>
#include <QStyle>
#include <QVariant>

#include <kicon.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <ktoolbar.h>
#include <kstdaccel.h>
#include <kaccelgen.h>
#include <kdeversion.h>
#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <kcommand.h>
#include <kfiledialog.h>
#include <kparts/event.h>
#include <kparts/partmanager.h>
#include <kseparatoraction.h>
#include <KoQueryTrader.h>

#include "kptview.h"
#include "kptviewbase.h"
#include "kptaccountsview.h"
#include "kptfactory.h"
#include "kptmilestoneprogressdialog.h"
#include "kptnode.h"
#include "kptpart.h"
#include "kptproject.h"
#include "kptmainprojectdialog.h"
#include "kpttask.h"
#include "kptsummarytaskdialog.h"
#include "kpttaskdialog.h"
#include "kpttaskprogressdialog.h"
#include "kptganttview.h"
//#include "kptreportview.h"
#include "kpttaskeditor.h"
#include "kptdatetime.h"
#include "kptcommand.h"
#include "kptrelation.h"
#include "kptrelationdialog.h"
#include "kptresourceview.h"
#include "kptresourceeditor.h"
#include "kptscheduleeditor.h"
#include "kptresourcedialog.h"
#include "kptresource.h"
#include "kptresourcesdialog.h"
#include "kptcalendarlistdialog.h"
#include "kptstandardworktimedialog.h"
#include "kptcanvasitem.h"
#include "kptconfigdialog.h"
#include "kptwbsdefinitiondialog.h"
#include "kptaccountsdialog.h"

#include "KDGanttView.h"
#include "KDGanttViewTaskItem.h"
#include "KPtViewAdaptor.h"

#include <assert.h>

namespace KPlato
{

// <Code mostly nicked from qt designer ;)>
class ViewCategoryDelegate : public QItemDelegate
{
public:
    ViewCategoryDelegate( QObject *parent, QTreeView *view )
            : QItemDelegate( parent ),
            m_view( view )
    {}

    virtual void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

private:
    QTreeView *m_view;
};

void ViewCategoryDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const QAbstractItemModel * model = index.model();
    Q_ASSERT( model );

    if ( !model->parent( index ).isValid() ) {
        // this is a top-level item.
        QStyleOptionButton buttonOption;
        buttonOption.state = option.state;
        buttonOption.state &= ~QStyle::State_HasFocus;

        buttonOption.rect = option.rect;
        buttonOption.palette = option.palette;
        buttonOption.features = QStyleOptionButton::None;
        m_view->style() ->drawControl( QStyle::CE_PushButton, &buttonOption, painter, m_view );

        QStyleOption branchOption;
        static const int i = 9; // ### hardcoded in qcommonstyle.cpp
        QRect r = option.rect;
        branchOption.rect = QRect( r.left() + i / 2, r.top() + ( r.height() - i ) / 2, i, i );
        branchOption.palette = option.palette;
        branchOption.state = QStyle::State_Children;

        if ( m_view->isExpanded( index ) )
            branchOption.state |= QStyle::State_Open;

        m_view->style() ->drawPrimitive( QStyle::PE_IndicatorBranch, &branchOption, painter, m_view );

        // draw text
        QRect textrect = QRect( r.left() + i * 2, r.top(), r.width() - ( ( 5 * i ) / 2 ), r.height() );
        QString text = elidedText( option.fontMetrics, textrect.width(), Qt::ElideMiddle,
                                   model->data( index, Qt::DisplayRole ).toString() );
        m_view->style() ->drawItemText( painter, textrect, Qt::AlignLeft,
                                        option.palette, m_view->isEnabled(), text );

    } else {
        QItemDelegate::paint( painter, option, index );
    }

}

class ViewListItem : public QTreeWidgetItem
{
public:
    ViewListItem( QTreeWidget *parent, const QStringList &strings, int type = Type );
    ViewListItem( QTreeWidgetItem *parent, const QStringList &strings, int type = Type );
    void setView( KoView *view );
    KoView *view() const;
    void setDocumentChild( DocumentChild *child );
    DocumentChild *documentChild() const;
    void setDocument( KoDocument *doc );
    KoDocument *document() const;
};

ViewListItem::ViewListItem( QTreeWidget *parent, const QStringList &strings, int type )
    : QTreeWidgetItem( parent, strings, type )
{
}

ViewListItem::ViewListItem( QTreeWidgetItem *parent, const QStringList &strings, int type )
    : QTreeWidgetItem( parent, strings, type )
{
}

void ViewListItem::setView( KoView *view )
{
    setData( 0, Qt::UserRole,  qVariantFromValue(static_cast<QWidget*>( view ) ) );
}

KoView *ViewListItem::view() const
{
    if ( data(0, Qt::UserRole ).isValid() ) {
        return static_cast<KoView*>( data(0, Qt::UserRole ).value<QWidget*>() );
    }
    return 0;
}

void ViewListItem::setDocument( KoDocument *doc )
{
    setData( 0, Qt::UserRole+1,  qVariantFromValue(static_cast<QObject*>( doc ) ) );
}

KoDocument *ViewListItem::document() const
{
    if ( data(0, Qt::UserRole+1 ).isValid() ) {
        return static_cast<KoDocument*>( data(0, Qt::UserRole+1 ).value<QObject*>() );
    }
    return 0;
}

void ViewListItem::setDocumentChild( DocumentChild *child )
{
    setData( 0, Qt::UserRole+2,  qVariantFromValue(static_cast<QObject*>( child ) ) );
}

DocumentChild *ViewListItem::documentChild() const
{
    if ( data(0, Qt::UserRole+2 ).isValid() ) {
        return static_cast<DocumentChild*>( data(0, Qt::UserRole+2 ).value<QObject*>() );
    }
    return 0;
}


ViewListTreeWidget::ViewListTreeWidget( QWidget *parent )
    : QTreeWidget( parent )
{
    header() ->hide();
    setRootIsDecorated( false );
    setItemDelegate( new ViewCategoryDelegate( this, this ) );
    setItemsExpandable( true );
    setSelectionMode( QAbstractItemView::SingleSelection );

    //setContextMenuPolicy( Qt::ActionsContextMenu );
    
    connect( this, SIGNAL( itemPressed( QTreeWidgetItem*, int ) ), SLOT( handleMousePress( QTreeWidgetItem* ) ) );
}

void ViewListTreeWidget::drawRow( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QTreeWidget::drawRow( painter, option, index );
}

void ViewListTreeWidget::handleMousePress( QTreeWidgetItem *item )
{
    kDebug()<<k_funcinfo<<endl;
    if ( item == 0 )
        return ;

    if ( item->parent() == 0 ) {
        setItemExpanded( item, !isItemExpanded( item ) );
        return ;
    }
}
void ViewListTreeWidget::mousePressEvent ( QMouseEvent *event )
{
    if ( event->button() == Qt::RightButton ) {
        event->accept();
        return;
    }
    QTreeWidget::mousePressEvent( event );
}

// </Code mostly nicked from qt designer ;)>

QTreeWidgetItem *ViewListTreeWidget::findCategory( const QString cat )
{
    QTreeWidgetItem * item;
    int cnt = topLevelItemCount();
    for ( int i = 0; i < cnt; ++i ) {
        item = topLevelItem( i );
        if ( item->text( 0 ) == cat )
            return item;
    }
    return 0;
}

ViewListWidget::ViewListWidget( QWidget *parent )//QString name, KMainWindow *parent )
        : QWidget( parent )
{
    setObjectName("ViewListWidget");
    m_viewlist = new ViewListTreeWidget( this );
    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    l->addWidget( m_viewlist );
    m_viewlist->setEditTriggers( QAbstractItemView::DoubleClicked );

    connect( m_viewlist, SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ), SLOT( slotActivated( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
    
    setupContextMenus();
}

ViewListWidget::~ViewListWidget()
{
    delete m_separator;
}

void ViewListWidget::slotActionTriggered()
{
    kDebug()<<k_funcinfo<<endl;
    QString servName = sender()->objectName();
    kDebug()<<k_funcinfo<<servName<<endl;
    KService::Ptr serv = KService::serviceByName( servName );
    KoDocumentEntry entry = KoDocumentEntry( serv );
    emit createKofficeDocument( entry );
}

void ViewListWidget::slotActivated( QTreeWidgetItem *item, QTreeWidgetItem *prev )
{
    kDebug()<<k_funcinfo<<endl;
    if ( item == 0 || item->type() == ViewListWidget::Category ) {
        return ;
    }
    emit activated( static_cast<ViewListItem*>( item ), static_cast<ViewListItem*>( prev ) );
}

QTreeWidgetItem *ViewListWidget::addCategory( const QString& name )
{
    //kDebug() << k_funcinfo << endl;
    QTreeWidgetItem *item = m_viewlist->findCategory( name );
    if ( item == 0 ) {
        item = new ViewListItem( m_viewlist, QStringList( name ), ViewListWidget::Category );
        item->setExpanded( true );
        item->setFlags( item->flags() | Qt::ItemIsEditable );
    }
    return item;
}

ViewListItem *ViewListWidget::addView( QTreeWidgetItem *category, const QString& name, KoView *view, KoDocument *doc, QString icon )
{
    ViewListItem * item = new ViewListItem( category, QStringList( name ), ViewListWidget::SubView );
    item->setView( view );
    item->setDocument( doc );
    if ( !icon.isEmpty() )
        item->setData( 0, Qt::DecorationRole, KIcon( icon ) );
    //kDebug() << k_funcinfo << "added: " << item << endl;
    return item;
}

ViewListItem *ViewListWidget::addView( QTreeWidgetItem *category, const QString& name, KoView *view, DocumentChild *ch, QString icon )
{
    ViewListItem * item = new ViewListItem( category, QStringList( name ), ViewListWidget::ChildDocument );
    item->setView( view );
    item->setDocument( ch->document() );
    item->setDocumentChild( ch );
    if ( !icon.isEmpty() )
        item->setData( 0, Qt::DecorationRole, KIcon( icon ) );
    //kDebug() << k_funcinfo << "added: " << item << endl;
    return item;
}

void ViewListWidget::setSelected( QTreeWidgetItem *item )
{
    kDebug()<<k_funcinfo<<item<<", "<<m_viewlist->currentItem()<<endl;
    if ( item == 0 ) {
        return;
    }
    m_viewlist->setCurrentItem( item );
    kDebug()<<k_funcinfo<<item<<", "<<m_viewlist->currentItem()<<endl;
}

ViewListItem *ViewListWidget::findItem(  KoView *view, QTreeWidgetItem *parent )
{
    if ( parent == 0 ) {
        return findItem( view, m_viewlist->invisibleRootItem() );
    }
    for (int i = 0; i < parent->childCount(); ++i ) {
        ViewListItem * ch = static_cast<ViewListItem*>( parent->child( i ) );
        if ( ch->view() == view ) {
            kDebug()<<k_funcinfo<<ch<<", "<<view<<endl;
            return ch;
        }
        ch = findItem( view, ch );
        if ( ch ) {
            return ch;
        }
    }
    return 0;
}

void ViewListWidget::setupContextMenus()
{
    // NOTE: can't use xml file as there may not be a factory()
    QAction *action;
    m_separator = new KSeparatorAction();
    // Query for documents
    m_lstEntries = KoDocumentEntry::query();
    Q3ValueList<KoDocumentEntry>::Iterator it = m_lstEntries.begin();
    for( ; it != m_lstEntries.end(); ++it ) {
        KService::Ptr serv = (*it).service();
        if ( serv->genericName().isEmpty() ||
             serv->serviceTypes().contains( "application/vnd.oasis.opendocument.formula" ) ||
             serv->serviceTypes().contains( "application/x-vnd.kde.kplato" ) ) {
            continue;
        }
        action = new QAction( KIcon(serv->icon()), serv->genericName().replace('&',"&&"), this );
        action->setObjectName( serv->name().toLatin1() );
        connect(action, SIGNAL( triggered( bool ) ), this, SLOT( slotActionTriggered() ) );
        m_parts.append( action );
    }
    // no item actions
    //action = new QAction( KIcon( "filenew" ), i18n( "New Category..." ), this );
    //m_noitem.append( action );
    
    // Category actions
/*    action = new QAction( KIcon( "rename" ), i18n( "Rename Category" ), this );
    m_category.append( action );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( renameCategory() ) );*/
    //action = new QAction( KIcon( "remove" ), i18n( "Remove Category" ), this );
    //m_category.append( action );
    
    // view actions
    //action = new QAction( KIcon( "show" ), i18n( "Show" ), this );
    //m_view.append( action );
    // document actions
    //action = new QAction( KIcon( "show" ), i18n( "Show" ), this );
    //m_document.append( action );
/*    action = new QAction( KIcon( "info" ), i18n( "Document information" ), this );
    m_document.append( action );*/
}

void ViewListWidget::renameCategory()
{
    if ( m_contextitem ) {
        m_viewlist->editItem( m_contextitem, 0 );
    }
}

void ViewListWidget::contextMenuEvent ( QContextMenuEvent *event )
{
    m_contextitem = static_cast<ViewListItem*>(m_viewlist->itemAt( event->pos() ) );
    QList<QAction*> lst;
    if ( m_contextitem == 0 ) {
        lst = m_noitem;
        lst.append( m_separator );
        lst += m_parts;
    } else if ( m_contextitem->type() == Category ) {
        lst = m_category;
        lst.append( m_separator );
        lst += m_parts;
    } else if ( m_contextitem->type() == SubView ) {
        lst = m_view;
        lst.append( m_separator );
        lst += m_parts;
    } else if ( m_contextitem->type() == ChildDocument ) {
        lst = m_document;
        lst.append( m_separator );
        lst += m_parts;
    }
    if ( ! lst.isEmpty() ) {
        QMenu::exec(lst, event->globalPos());
    }
}


//-------------------------------
View::View( Part* part, QWidget* parent )
        : KoView( part, parent ),
        m_ganttview( 0 ),
        m_currentEstimateType( Effort::Use_Expected )
{
    //kDebug()<<k_funcinfo<<endl;
    getProject().setCurrentSchedule( Schedule::Expected );

    setInstance( Factory::global() );
    if ( !part->isReadWrite() )
        setXMLFile( "kplato_readonly.rc" );
    else
        setXMLFile( "kplato.rc" );

    m_dbus = new ViewAdaptor( this );
    QDBusConnection::sessionBus().registerObject( '/' + objectName(), this );

    m_sp = new QSplitter( this );
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin(0);
    layout->addWidget( m_sp );
    
    // to the left
    m_viewlist = new ViewListWidget( m_sp );
    // to the right
    m_tab = new QStackedWidget( m_sp );
    
    m_taskeditor = new TaskEditor( getPart(), m_tab );
    m_tab->addWidget( m_taskeditor );
    m_taskeditor->draw( getProject() );
    connect( m_taskeditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );
    
    m_resourceeditor = new ResourceEditor( getPart(), m_tab );
    m_tab->addWidget( m_resourceeditor );
    m_resourceeditor->draw( getProject() );
    connect( m_resourceeditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    m_scheduleeditor = new ScheduleEditor( getPart(), m_tab );
    m_tab->addWidget( m_scheduleeditor );
    m_scheduleeditor->draw( getProject() );
    connect( m_scheduleeditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    m_ganttview = new GanttView( getPart(), m_tab, part->isReadWrite() );
    m_tab->addWidget( m_ganttview );
    m_updateGanttview = false;
    m_ganttview->draw( getProject() );
    connect( m_ganttview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    m_resourceview = new ResourceView( getPart(), m_tab );
    m_updateResourceview = true;
    m_tab->addWidget( m_resourceview );
    connect( m_resourceview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    m_accountsview = new AccountsView( getProject(), getPart(), m_tab );
    m_updateAccountsview = true;
    m_tab->addWidget( m_accountsview );
    connect( m_accountsview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( m_taskeditor, SIGNAL( addTask() ), SLOT( slotAddTask() ) );
    connect( m_taskeditor, SIGNAL( addMilestone() ), SLOT( slotAddMilestone() ) );
    connect( m_taskeditor, SIGNAL( addSubtask() ), SLOT( slotAddSubTask() ) );
    connect( m_taskeditor, SIGNAL( deleteTaskList( QList<Node*> ) ), SLOT( slotDeleteTask( QList<Node*> ) ) );
    connect( m_taskeditor, SIGNAL( moveTaskUp() ), SLOT( slotMoveTaskUp() ) );
    connect( m_taskeditor, SIGNAL( moveTaskDown() ), SLOT( slotMoveTaskDown() ) );
    connect( m_taskeditor, SIGNAL( indentTask() ), SLOT( slotIndentTask() ) );
    connect( m_taskeditor, SIGNAL( unindentTask() ), SLOT( slotUnindentTask() ) );

    connect( m_scheduleeditor, SIGNAL( addScheduleManager( Project* ) ), SLOT( slotAddScheduleManager( Project* ) ) );
    connect( m_scheduleeditor, SIGNAL( deleteScheduleManager( Project*, ScheduleManager* ) ), SLOT( slotDeleteScheduleManager( Project*, ScheduleManager* ) ) );
    connect( m_scheduleeditor, SIGNAL( calculateSchedule( Project*, ScheduleManager* ) ), SLOT( slotCalculateSchedule( Project*, ScheduleManager* ) ) );
    //m_reportview = new ReportView(this, m_tab);
    //m_tab->addWidget(m_reportview);

    QTreeWidgetItem *cat;
    cat = m_viewlist->addCategory( i18n( "Editors" ) );
    m_viewlist->addView( cat, i18n( "Tasks" ), m_taskeditor, getPart(), "task_editor" );
    m_viewlist->addView( cat, i18n( "Resources" ), m_resourceeditor, getPart(), "resource_editor" );
    m_viewlist->addView( cat, i18n( "Schedules" ), m_scheduleeditor, getPart(), "schedule_editor" );

    cat = m_viewlist->addCategory( i18n( "Views" ) );
    m_viewlist->addView( cat, i18n( "Gantt" ), m_ganttview, getPart(), "gantt_chart" );
    m_viewlist->addView( cat, i18n( "Resources" ), m_resourceview, getPart(), "resources" );
    m_viewlist->addView( cat, i18n( "Accounts" ), m_accountsview, getPart(), "accounts" );
    
    connect( m_viewlist, SIGNAL( activated( ViewListItem*, ViewListItem* ) ), SLOT( slotViewActivated( ViewListItem*, ViewListItem* ) ) );
    connect( m_tab, SIGNAL( currentChanged( int ) ), this, SLOT( slotCurrentChanged( int ) ) );

    connect( m_ganttview, SIGNAL( enableActions( bool ) ), SLOT( setTaskActionsEnabled( bool ) ) );
    connect( m_ganttview, SIGNAL( addRelation( Node*, Node*, int ) ), SLOT( slotAddRelation( Node*, Node*, int ) ) );
    connect( m_ganttview, SIGNAL( modifyRelation( Relation*, int ) ), SLOT( slotModifyRelation( Relation*, int ) ) );
    connect( m_ganttview, SIGNAL( modifyRelation( Relation* ) ), SLOT( slotModifyRelation( Relation* ) ) );
    connect( m_ganttview, SIGNAL( itemDoubleClicked() ), SLOT( slotOpenNode() ) );
    connect( m_ganttview, SIGNAL( itemRenamed( Node*, const QString& ) ), this, SLOT( slotRenameNode( Node*, const QString& ) ) );
    connect( m_ganttview, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    
    connect( m_resourceview, SIGNAL( itemDoubleClicked() ), SLOT( slotEditResource() ) );
    connect( m_resourceview, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

    connect( m_resourceeditor, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );


    // The menu items
    // ------ Edit
    actionCut = KStdAction::cut( this, SLOT( slotEditCut() ), actionCollection(), "edit_cut" );
    actionCopy = KStdAction::copy( this, SLOT( slotEditCopy() ), actionCollection(), "edit_copy" );
    actionPaste = KStdAction::paste( this, SLOT( slotEditPaste() ), actionCollection(), "edit_paste" );

    // ------ View
    actionViewGantt = new KAction( KIcon( "gantt_chart" ), i18n( "Gantt" ), actionCollection(), "view_gantt" );
    connect( actionViewGantt, SIGNAL( triggered( bool ) ), SLOT( slotViewGantt() ) );

    actionViewSelector = new KToggleAction( i18n( "Show Selector" ), actionCollection(), "view_show_selector" );
    connect( actionViewSelector, SIGNAL( triggered( bool ) ), SLOT( slotViewSelector( bool ) ) );

    m_scheduleActionGroup = new QActionGroup( this );
    m_scheduleActionGroup->setExclusive( true );
    connect( m_scheduleActionGroup, SIGNAL( triggered( QAction* ) ), SLOT( slotViewSchedule( QAction* ) ) );

    actionViewGanttResources = new KToggleAction( i18n( "Resources" ), actionCollection(), "view_gantt_showResources" );
    connect( actionViewGanttResources, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttResources() ) );
    actionViewGanttTaskName = new KToggleAction( i18n( "Task Name" ), actionCollection(), "view_gantt_showTaskName" );
    connect( actionViewGanttTaskName, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttTaskName() ) );
    actionViewGanttTaskLinks = new KToggleAction( i18n( "Task Links" ), actionCollection(), "view_gantt_showTaskLinks" );
    connect( actionViewGanttTaskLinks, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttTaskLinks() ) );
    actionViewGanttProgress = new KToggleAction( i18n( "Progress" ), actionCollection(), "view_gantt_showProgress" );
    connect( actionViewGanttProgress, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttProgress() ) );
    actionViewGanttFloat = new KToggleAction( i18n( "Float" ), actionCollection(), "view_gantt_showFloat" );
    connect( actionViewGanttFloat, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttFloat() ) );
    actionViewGanttCriticalTasks = new KToggleAction( i18n( "Critical Tasks" ), actionCollection(), "view_gantt_showCriticalTasks" );
    connect( actionViewGanttCriticalTasks, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttCriticalTasks() ) );
    actionViewGanttCriticalPath = new KToggleAction( i18n( "Critical Path" ), actionCollection(), "view_gantt_showCriticalPath" );
    connect( actionViewGanttCriticalPath, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttCriticalPath() ) );

    //    actionViewGanttNotScheduled = new KToggleAction(i18n("Not Scheduled"), 0, 0, this, SLOT(slotViewGanttNotScheduled()), actionCollection(), "view_gantt_showNotScheduled");

    actionViewTaskAppointments = new KToggleAction( i18n( "Show allocations" ), actionCollection(), "view_task_appointments" );
    connect( actionViewTaskAppointments, SIGNAL( triggered( bool ) ), SLOT( slotViewTaskAppointments() ) );

    actionViewResources = new KAction( KIcon( "resources" ), i18n( "Resources" ), actionCollection(), "view_resources" );
    connect( actionViewResources, SIGNAL( triggered( bool ) ), SLOT( slotViewResources() ) );

    actionViewResourceAppointments = new KToggleAction( i18n( "Show allocations" ), actionCollection(), "view_resource_appointments" );
    connect( actionViewResourceAppointments, SIGNAL( triggered( bool ) ), SLOT( slotViewResourceAppointments() ) );

    actionViewAccounts = new KAction( KIcon( "accounts" ), i18n( "Accounts" ), actionCollection(), "view_accounts" );
    connect( actionViewAccounts, SIGNAL( triggered( bool ) ), SLOT( slotViewAccounts() ) );

    //actionViewReports = new KAction(i18n("Reports"), "reports", 0, this, SLOT(slotViewReports()), actionCollection(), "view_reports");

    // ------ Insert

    // ------ Project
    actionEditMainProject = new KAction( KIcon( "edit" ), i18n( "Edit Main Project..." ), actionCollection(), "project_edit" );
    connect( actionEditMainProject, SIGNAL( triggered( bool ) ), SLOT( slotProjectEdit() ) );
    actionEditStandardWorktime = new KAction( KIcon( "edit" ), i18n( "Edit Standard Worktime..." ), actionCollection(), "project_worktime" );
    connect( actionEditStandardWorktime, SIGNAL( triggered( bool ) ), SLOT( slotProjectWorktime() ) );
    actionEditCalendar = new KAction( KIcon( "edit" ), i18n( "Edit Calendar..." ), actionCollection(), "project_calendar" );
    connect( actionEditCalendar, SIGNAL( triggered( bool ) ), SLOT( slotProjectCalendar() ) );
    actionEditAccounts = new KAction( KIcon( "edit" ), i18n( "Edit Accounts..." ), actionCollection(), "project_accounts" );
    connect( actionEditAccounts, SIGNAL( triggered( bool ) ), SLOT( slotProjectAccounts() ) );
    actionEditResources = new KAction( KIcon( "edit" ), i18n( "Edit Resources..." ), actionCollection(), "project_resources" );
    connect( actionEditResources, SIGNAL( triggered( bool ) ), SLOT( slotProjectResources() ) );


    /*    // ------ Reports
        actionFirstpage = KStdAction::firstPage(m_reportview,SLOT(slotPrevPage()),actionCollection(),"go_firstpage");
        connect(m_reportview, SIGNAL(setFirstPageActionEnabled(bool)), actionFirstpage, SLOT(setEnabled(bool)));
        actionPriorpage = KStdAction::prior(m_reportview,SLOT(slotPrevPage()),actionCollection(),"go_prevpage");
        connect(m_reportview, SIGNAL(setPriorPageActionEnabled(bool)), actionPriorpage, SLOT(setEnabled(bool)));
        actionNextpage = KStdAction::next(m_reportview,SLOT(slotNextPage()),actionCollection(), "go_nextpage");
        connect(m_reportview, SIGNAL(setNextPageActionEnabled(bool)), actionNextpage, SLOT(setEnabled(bool)));
        actionLastpage = KStdAction::lastPage(m_reportview,SLOT(slotLastPage()),actionCollection(), "go_lastpage");
        connect(m_reportview, SIGNAL(setLastPageActionEnabled(bool)), actionLastpage, SLOT(setEnabled(bool)));
        m_reportview->enableNavigationBtn();*/
    mainWindow() ->toolBar( "report" ) ->hide();

    //     new KAction(i18n("Design..."), "report_design", 0, this,
    //         SLOT(slotReportDesign()), actionCollection(), "report_design");


    // ------ Tools
    actionDefineWBS = new KAction( KIcon( "tools_define_wbs" ), i18n( "Define WBS Pattern..." ), actionCollection(), "tools_generate_wbs" );
    connect( actionDefineWBS, SIGNAL( triggered( bool ) ), SLOT( slotDefineWBS() ) );

    actionGenerateWBS = new KAction( KIcon( "tools_generate_wbs" ), i18n( "Generate WBS Code" ), actionCollection(), "tools_define_wbs" );
    connect( actionGenerateWBS, SIGNAL( triggered( bool ) ), SLOT( slotGenerateWBS() ) );
    
    // ------ Export (testing)
    //actionExportGantt = new KAction(i18n("Export Ganttview"), "export_gantt", 0, this,
    //    SLOT(slotExportGantt()), actionCollection(), "export_gantt");

    // ------ Settings
    actionConfigure = new KAction( KIcon( "configure" ), i18n( "Configure KPlato..." ), actionCollection(), "configure" );
    connect( actionConfigure, SIGNAL( triggered( bool ) ), SLOT( slotConfigure() ) );

    // ------ Popup
    actionOpenNode = new KAction( KIcon( "edit" ), i18n( "Edit..." ), actionCollection(), "node_properties" );
    connect( actionOpenNode, SIGNAL( triggered( bool ) ), SLOT( slotOpenNode() ) );
    actionTaskProgress = new KAction( KIcon( "edit" ), i18n( "Progress..." ), actionCollection(), "task_progress" );
    connect( actionTaskProgress, SIGNAL( triggered( bool ) ), SLOT( slotTaskProgress() ) );
    actionDeleteTask = new KAction( KIcon( "editdelete" ), i18n( "Delete Task" ), actionCollection(), "delete_task" );
    connect( actionDeleteTask, SIGNAL( triggered( bool ) ), SLOT( slotDeleteTask() ) );

    actionEditResource = new KAction( KIcon( "edit" ), i18n( "Edit Resource..." ), actionCollection(), "edit_resource" );
    connect( actionEditResource, SIGNAL( triggered( bool ) ), SLOT( slotEditResource() ) );

    // Viewlist popup
    connect( m_viewlist, SIGNAL( createKofficeDocument( KoDocumentEntry& ) ), SLOT( slotCreateKofficeDocument( KoDocumentEntry& ) ) );
    
    // ------------------- Actions with a key binding and no GUI item
    // Temporary, till we get a menu entry
    actNoInformation = new KAction( "Toggle no information", actionCollection(), "show_noinformation" );
    connect( actNoInformation, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttNoInformation() ) );
    actNoInformation->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_T ) );

#ifndef NDEBUG
    //new KAction("Print Debug", CTRL+Qt::SHIFT+Qt::Key_P, this, SLOT( slotPrintDebug()), actionCollection(), "print_debug");
    KAction *action = new KAction( "Print Debug", actionCollection(), "print_debug" );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( slotPrintSelectedDebug() ) );
    action->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_P ) );
    action = new KAction( "Print Calendar Debug", actionCollection(), "print_calendar_debug" );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( slotPrintCalendarDebug() ) );
    action->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_C ) );
    //     new KAction("Print Test Debug", CTRL+Qt::SHIFT+Qt::Key_T, this, SLOT(slotPrintTestDebug()), actionCollection(), "print_test_debug");

    KAction *actExportGantt = new KAction( i18n( "Export Gantt" ), actionCollection(), "export_gantt" );
    connect( actExportGantt, SIGNAL( triggered( bool ) ), SLOT( slotExportGantt() ) );
    actExportGantt->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_G ) );

    
#endif
    // Stupid compilers ;)
#ifndef NDEBUG 
    /*  Q_UNUSED( actPrintSelectedDebug );
        Q_UNUSED( actPrintCalendarDebug );*/
    Q_UNUSED( actExportGantt );
#endif

    m_progress = 0;
    m_estlabel = new KStatusBarLabel( "", 0 );
    if ( statusBar() ) {
        addStatusBarItem( m_estlabel, 0, true );
        //m_progress = new QProgressBar();
        //addStatusBarItem( m_progress, 0, true );
        //m_progress->hide();
    }
    slotPlugScheduleActions();
    connect( &getProject(), SIGNAL( scheduleAdded( const MainSchedule* ) ), SLOT( slotPlugScheduleActions() ) );
    connect( &getProject(), SIGNAL( currentScheduleChanged() ), SLOT( slotCurrentScheduleChanged() ) );
    
    setScheduleActionsEnabled();

    m_viewlist->setSelected( m_viewlist->findItem( m_taskeditor ) );
    //kDebug()<<k_funcinfo<<" end "<<endl;
}

View::~View()
{
    removeStatusBarItem( m_estlabel );
    delete m_estlabel;
    delete m_scheduleActionGroup;
}

ViewAdaptor* View::dbusObject()
{
    return m_dbus;
}

// TODO
// QDockWidget *View::createToolBox() {
//     m_toolbox = KoToolManager::instance()->toolBox("KPlato");
//     m_toolbox->hide();
//     return m_toolbox;
// }

Project& View::getProject() const
{
    return getPart() ->getProject();
}

void View::setZoom( double zoom )
{
    m_ganttview->setZoom( zoom );
}

void View::setupPrinter( KPrinter & /*printer*/ )
{
    //kDebug()<<k_funcinfo<<endl;
}

void View::print( KPrinter &printer )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( printer.previewOnly() ) {
        //HACK: KoMainWindow shows setup on print, but not on print preview!
        if ( !printer.setup() ) {
            return ;
        }
    }
    if ( m_tab->currentWidget() == m_ganttview ) {
        m_ganttview->print( printer );
    } else if ( m_tab->currentWidget() == m_resourceview ) {
        m_resourceview->print( printer );
    } else if ( m_tab->currentWidget() == m_accountsview ) {
        m_accountsview->print( printer );
    }
    // 	else if (m_tab->currentWidget() == m_reportview)
    // 	{
    //         m_reportview->print(printer);
    // 	}

}

void View::slotEditCut()
{
    //kDebug()<<k_funcinfo<<endl;
}

void View::slotEditCopy()
{
    //kDebug()<<k_funcinfo<<endl;
}

void View::slotEditPaste()
{
    //kDebug()<<k_funcinfo<<endl;
}

void View::slotViewGanttResources()
{
    //kDebug()<<k_funcinfo<<endl;
    m_ganttview->setShowResources( actionViewGanttResources->isChecked() );
    if ( m_tab->currentWidget() == m_ganttview )
        slotUpdate();
}

void View::slotViewGanttTaskName()
{
    //kDebug()<<k_funcinfo<<endl;
    m_ganttview->setShowTaskName( actionViewGanttTaskName->isChecked() );
    if ( m_tab->currentWidget() == m_ganttview )
        slotUpdate();
}

void View::slotViewGanttTaskLinks()
{
    //kDebug()<<k_funcinfo<<endl;
    m_ganttview->setShowTaskLinks( actionViewGanttTaskLinks->isChecked() );
    if ( m_tab->currentWidget() == m_ganttview )
        slotUpdate();
}

void View::slotViewGanttProgress()
{
    //kDebug()<<k_funcinfo<<endl;
    m_ganttview->setShowProgress( actionViewGanttProgress->isChecked() );
    if ( m_tab->currentWidget() == m_ganttview )
        slotUpdate();
}

void View::slotViewGanttFloat()
{
    //kDebug()<<k_funcinfo<<endl;
    m_ganttview->setShowPositiveFloat( actionViewGanttFloat->isChecked() );
    if ( m_tab->currentWidget() == m_ganttview )
        slotUpdate();
}

void View::slotViewGanttCriticalTasks()
{
    //kDebug()<<k_funcinfo<<endl;
    m_ganttview->setShowCriticalTasks( actionViewGanttCriticalTasks->isChecked() );
    if ( m_tab->currentWidget() == m_ganttview )
        slotUpdate();
}

void View::slotViewGanttCriticalPath()
{
    //kDebug()<<k_funcinfo<<endl;
    m_ganttview->setShowCriticalPath( actionViewGanttCriticalPath->isChecked() );
    if ( m_tab->currentWidget() == m_ganttview )
        slotUpdate();
}

void View::slotViewGanttNoInformation()
{
    kDebug() << k_funcinfo << m_ganttview->showNoInformation() << endl;
    m_ganttview->setShowNoInformation( !m_ganttview->showNoInformation() ); //Toggle
    if ( m_tab->currentWidget() == m_ganttview )
        slotUpdate();
}

void View::slotViewTaskAppointments()
{
    //kDebug()<<k_funcinfo<<endl;
    m_ganttview->setShowAppointments( actionViewTaskAppointments->isChecked() );
    m_updateGanttview = true;
    if ( m_tab->currentWidget() == m_ganttview )
        slotUpdate();
}

void View::slotViewSelector( bool show )
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setVisible( show );
}

void View::slotViewGantt()
{
    kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( m_ganttview ) );
    //m_tab->setCurrentWidget( m_ganttview );
}

void View::slotViewResources()
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( m_resourceview ) );
}

void View::slotViewResourceAppointments()
{
    //kDebug()<<k_funcinfo<<endl;

}

void View::slotViewAccounts()
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( m_accountsview ) );
}

void View::slotViewTaskEditor()
{
    kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( m_taskeditor ) );
}

void View::slotProjectEdit()
{
    MainProjectDialog * dia = new MainProjectDialog( getProject() );
    if ( dia->exec()  == QDialog::Accepted) {
        KCommand * cmd = dia->buildCommand( getPart() );
        if ( cmd ) {
            getPart() ->addCommand( cmd );
        }
    }
    delete dia;
}

void View::slotProjectCalendar()
{
    CalendarListDialog * dia = new CalendarListDialog( getProject() );
    if ( dia->exec()  == QDialog::Accepted) {
        KCommand * cmd = dia->buildCommand( getPart() );
        if ( cmd ) {
            //kDebug()<<k_funcinfo<<"Modifying calendar(s)"<<endl;
            getPart() ->addCommand( cmd ); //also executes
        }
    }
    delete dia;
}

void View::slotProjectAccounts()
{
    AccountsDialog * dia = new AccountsDialog( getProject().accounts() );
    if ( dia->exec()  == QDialog::Accepted) {
        KCommand * cmd = dia->buildCommand( getPart() );
        if ( cmd ) {
            //kDebug()<<k_funcinfo<<"Modifying account(s)"<<endl;
            getPart() ->addCommand( cmd ); //also executes
        }
    }
    delete dia;
}

void View::slotProjectWorktime()
{
    StandardWorktimeDialog * dia = new StandardWorktimeDialog( getProject() );
    if ( dia->exec()  == QDialog::Accepted) {
        KCommand * cmd = dia->buildCommand( getPart() );
        if ( cmd ) {
            //kDebug()<<k_funcinfo<<"Modifying calendar(s)"<<endl;
            getPart() ->addCommand( cmd ); //also executes
        }
    }
    delete dia;
}

void View::slotProjectResources()
{
    ResourcesDialog * dia = new ResourcesDialog( getProject() );
    if ( dia->exec()  == QDialog::Accepted) {
        KCommand * cmd = dia->buildCommand( getPart() );
        if ( cmd ) {
            //kDebug()<<k_funcinfo<<"Modifying resources"<<endl;
            getPart() ->addCommand( cmd ); //also executes
        }
    }
    delete dia;
}

void View::slotCurrentScheduleChanged()
{
    kDebug()<<k_funcinfo<<endl;
    //TODO
    setLabel();
}

void View::slotViewSchedule( QAction *act )
{
    kDebug()<<k_funcinfo<<endl;
    Schedule *sch = m_scheduleActions.value( static_cast<KAction*>( act ), 0 );
    getProject().setCurrentSchedule( sch->id() );
}

void View::slotActionDestroyed( QObject *o )
{
    kDebug()<<k_funcinfo<<o->name()<<endl;
    m_scheduleActions.remove( static_cast<KAction*>( o ) );
}

void View::slotPlugScheduleActions()
{
    kDebug()<<k_funcinfo<<endl;
    unplugActionList( "view_schedule_list" );
    foreach( KAction *act, m_scheduleActions.keys() ) {
        m_scheduleActionGroup->removeAction( act );
        delete act;
    }
    m_scheduleActions.clear();
    Schedule *cs = getProject().currentSchedule();
    QAction *ca = 0;
    foreach( Schedule *sch, getProject().schedules().values() ) {
        if ( ! sch->isDeleted() ) {
            QString n = sch->name() + " (" + sch->typeToString( true ) + ")";
            KAction *act = new KToggleAction( n, actionCollection(), n );
            m_scheduleActions.insert( act, sch );
            m_scheduleActionGroup->addAction( act );
            if ( ca == 0 && cs == sch ) {
                ca = act;
            }
            kDebug()<<k_funcinfo<<"Add: "<<act->name()<<endl;
            connect( act, SIGNAL(destroyed( QObject* ) ), SLOT( slotActionDestroyed( QObject* ) ) );
        }
    }
    plugActionList( "view_schedule_list", m_scheduleActions.keys() );
    if ( ca == 0 && m_scheduleActionGroup->actions().count() > 0 ) {
        ca = m_scheduleActionGroup->actions().first();
    }
    if ( ca ) {
        ca->setChecked( true );
    }
    setLabel();
}

void View::slotProgressChanged( int value )
{
/*    if ( m_progress == 0 ) {
        return;
    }
    if ( value < 0 ) {
        disconnect( sender(), SIGNAL( sigProgress( int ) ), this, SLOT(slotProgressChanged( int ) ) );
        m_progress->hide();
        return;
    }
    m_progress->show();
    m_progress->setValue( value );*/
}

void View::slotCalculateSchedule( Project *project, ScheduleManager *sm )
{
    if ( project == 0 || sm == 0 ) {
        return;
    }
    statusBar()->showMessage( i18n( "%1: Calculating...", sm->name() ) );
//    connect( project, SIGNAL( sigProgress( int ) ), SLOT(slotProgressChanged( int ) ) );
    QApplication::setOverrideCursor( Qt::WaitCursor );
    CalculateScheduleCmd *cmd =  new CalculateScheduleCmd( getPart(), *project, *sm, i18n( "Calculate %1", sm->name() ) );
    getPart() ->addCommand( cmd );
    QApplication::restoreOverrideCursor();
    statusBar()->clearMessage();
    statusBar()->showMessage( i18n( "%1: Calculating done", sm->name() ), 2000 );

}

void View::slotAddScheduleManager( Project *project )
{
    if ( project == 0 ) {
        return;
    }
    ScheduleManager *sm = project->createScheduleManager();
    AddScheduleManagerCmd *cmd =  new AddScheduleManagerCmd( getPart(), *project, sm, i18n( "Add Schedule %1", sm->name() ) );
    getPart() ->addCommand( cmd );
}

void View::slotDeleteScheduleManager( Project *project, ScheduleManager *sm )
{
    if ( project == 0 || sm == 0) {
        return;
    }
    DeleteScheduleManagerCmd *cmd =  new DeleteScheduleManagerCmd( getPart(), *project, sm, i18n( "Delete Schedule %1", sm->name() ) );
    getPart() ->addCommand( cmd );
}

void View::slotViewReportDesign()
{
    //kDebug()<<k_funcinfo<<endl;
}

void View::slotViewReports()
{
    //kDebug()<<k_funcinfo<<endl;
    //m_tab->setCurrentWidget(m_reportview);
}

void View::slotAddSubTask()
{
    // If we are positionend on the root project, then what we really want to
    // do is to add a first project. We will silently accept the challenge
    // and will not complain.
    Task * node = getProject().createTask( getPart() ->config().taskDefaults(), currentTask() );
    TaskDialog *dia = new TaskDialog( *node, getProject().accounts(), getProject().standardWorktime() );
    if ( dia->exec()  == QDialog::Accepted) {
        Node * currNode = currentTask();
        if ( currNode ) {
            KCommand * m = dia->buildCommand( getPart() );
            m->execute(); // do changes to task
            delete m;
            SubtaskAddCmd *cmd = new SubtaskAddCmd( getPart(), &( getProject() ), node, currNode, i18n( "Add Subtask" ) );
            getPart() ->addCommand( cmd ); // add task to project
            delete dia;
            return ;
        } else
            kDebug() << k_funcinfo << "Cannot insert new project. Hmm, no current node!?" << endl;
    }
    delete node;
    delete dia;
}


void View::slotAddTask()
{
    Task * node = getProject().createTask( getPart() ->config().taskDefaults(), currentTask() );
    TaskDialog *dia = new TaskDialog( *node, getProject().accounts(), getProject().standardWorktime() );
    if ( dia->exec()  == QDialog::Accepted) {
        Node * currNode = currentTask();
        if ( currNode ) {
            KCommand * m = dia->buildCommand( getPart() );
            m->execute(); // do changes to task
            delete m;
            TaskAddCmd *cmd = new TaskAddCmd( getPart(), &( getProject() ), node, currNode, i18n( "Add Task" ) );
            getPart() ->addCommand( cmd ); // add task to project
            delete dia;
            return ;
        } else
            kDebug() << k_funcinfo << "Cannot insert new task. Hmm, no current node!?" << endl;
    }
    delete node;
    delete dia;
}

void View::slotAddMilestone()
{
    Task * node = getProject().createTask( currentTask() );
    node->effort() ->set
    ( Duration::zeroDuration );

    TaskDialog *dia = new TaskDialog( *node, getProject().accounts(), getProject().standardWorktime() );
    if ( dia->exec() == QDialog::Accepted ) {
        Node * currNode = currentTask();
        if ( currNode ) {
            KCommand * m = dia->buildCommand( getPart() );
            m->execute(); // do changes to task
            delete m;
            TaskAddCmd *cmd = new TaskAddCmd( getPart(), &( getProject() ), node, currNode, i18n( "Add Milestone" ) );
            getPart() ->addCommand( cmd ); // add task to project
            delete dia;
            return ;
        } else
            kDebug() << k_funcinfo << "Cannot insert new milestone. Hmm, no current node!?" << endl;
    }
    delete node;
    delete dia;
}

void View::slotDefineWBS()
{
    //kDebug()<<k_funcinfo<<endl;
    WBSDefinitionDialog * dia = new WBSDefinitionDialog( getPart() ->wbsDefinition() );
    dia->exec();

    delete dia;
}

void View::slotGenerateWBS()
{
    //kDebug()<<k_funcinfo<<endl;
    getPart() ->generateWBS();
    slotUpdate();
}

void View::slotConfigure()
{
    //kDebug()<<k_funcinfo<<endl;
    ConfigDialog * dia = new ConfigDialog( getPart() ->config(), getProject() );
    dia->exec();
    delete dia;
}

Node *View::currentTask()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return 0;
    }
    Node * task = v->currentNode();
    if ( 0 != task ) {
        return task;
    }
    return &( getProject() );
}

Resource *View::currentResource()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return 0;
    }
    return v->currentResource();
}

ResourceGroup *View::currentResourceGroup()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return 0;
    }
    return v->currentResourceGroup();
}


void View::slotOpenNode()
{
    //kDebug()<<k_funcinfo<<endl;
    Node * node = currentTask();
    if ( !node )
        return ;

    switch ( node->type() ) {
        case Node::Type_Project: {
                Project * project = dynamic_cast<Project *>( node );
                MainProjectDialog *dia = new MainProjectDialog( *project );
                if ( dia->exec()  == QDialog::Accepted) {
                    KCommand * m = dia->buildCommand( getPart() );
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Subproject:
            //TODO
            break;
        case Node::Type_Task: {
                Task *task = dynamic_cast<Task *>( node );
                Q_ASSERT( task );
                TaskDialog *dia = new TaskDialog( *task, getProject().accounts(), getProject().standardWorktime() );
                if ( dia->exec()  == QDialog::Accepted) {
                    KCommand * m = dia->buildCommand( getPart() );
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Milestone: {
                // Use the normal task dialog for now.
                // Maybe milestone should have it's own dialog, but we need to be able to
                // enter a duration in case we accidentally set a tasks duration to zero
                // and hence, create a milestone
                Task *task = dynamic_cast<Task *>( node );
                Q_ASSERT( task );
                TaskDialog *dia = new TaskDialog( *task, getProject().accounts(), getProject().standardWorktime() );
                if ( dia->exec()  == QDialog::Accepted) {
                    KCommand * m = dia->buildCommand( getPart() );
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Summarytask: {
                Task *task = dynamic_cast<Task *>( node );
                Q_ASSERT( task );
                SummaryTaskDialog *dia = new SummaryTaskDialog( *task );
                if ( dia->exec()  == QDialog::Accepted) {
                    KCommand * m = dia->buildCommand( getPart() );
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        default:
            break; // avoid warnings
    }
}

void View::slotTaskProgress()
{
    //kDebug()<<k_funcinfo<<endl;
    Node * node = currentTask();
    if ( !node )
        return ;

    switch ( node->type() ) {
        case Node::Type_Project: {
                break;
            }
        case Node::Type_Subproject:
            //TODO
            break;
        case Node::Type_Task: {
                Task *task = dynamic_cast<Task *>( node );
                Q_ASSERT( task );
                TaskProgressDialog *dia = new TaskProgressDialog( *task, getProject().standardWorktime() );
                if ( dia->exec()  == QDialog::Accepted) {
                    KCommand * m = dia->buildCommand( getPart() );
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Milestone: {
                Task *task = dynamic_cast<Task *>( node );
                MilestoneProgressDialog *dia = new MilestoneProgressDialog( *task );
                if ( dia->exec()  == QDialog::Accepted) {
                    KCommand * m = dia->buildCommand( getPart() );
                    if ( m ) {
                        getPart() ->addCommand( m );
                    }
                }
                delete dia;
                break;
            }
        case Node::Type_Summarytask: {
                // TODO
                break;
            }
        default:
            break; // avoid warnings
    }
}

void View::slotDeleteTask( QList<Node*> lst )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( lst.count() == 1 ) {
        slotDeleteTask( lst.takeFirst() );
        return;
    }
    int num = 0;
    KMacroCommand *cmd = new KMacroCommand( i18n( "Delete Tasks" ) );
    while ( !lst.isEmpty() ) {
        Node *node = lst.takeFirst();
        if ( node == 0 || node->getParent() == 0 ) {
            kDebug() << k_funcinfo << ( node ? "Task is main project" : "No current task" ) << endl;
            continue;
        }
        bool del = true;
        foreach ( Node *n, lst ) {
            if ( node->isChildOf( n ) ) {
                del = false; // node is going to be deleted when we delete n
                break;
            }
        }
        if ( del ) {
            //kDebug()<<k_funcinfo<<num<<": delete: "<<node->name()<<endl;
            cmd->addCommand( new NodeDeleteCmd( getPart(), node, i18n( "Delete Task" ) ) );
            num++;
        }
    }
    if ( num > 0 ) {
        getPart()->addCommand( cmd );
    } else {
        delete cmd;
    }
}

void View::slotDeleteTask( Node *node )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( node == 0 || node->getParent() == 0 ) {
        kDebug() << k_funcinfo << ( node ? "Task is main project" : "No current task" ) << endl;
        return ;
    }
    NodeDeleteCmd *cmd = new NodeDeleteCmd( getPart(), node, i18n( "Delete Task" ) );
    getPart() ->addCommand( cmd );
}

void View::slotDeleteTask()
{
    //kDebug()<<k_funcinfo<<endl;
    return slotDeleteTask( currentTask() );
}

void View::slotIndentTask()
{
    //kDebug()<<k_funcinfo<<endl;
    Node * node = currentTask();
    if ( node == 0 || node->getParent() == 0 ) {
        kDebug() << k_funcinfo << ( node ? "Task is main project" : "No current task" ) << endl;
        return ;
    }
    if ( getProject().canIndentTask( node ) ) {
        NodeIndentCmd * cmd = new NodeIndentCmd( getPart(), *node, i18n( "Indent Task" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotUnindentTask()
{
    //kDebug()<<k_funcinfo<<endl;
    Node * node = currentTask();
    if ( node == 0 || node->getParent() == 0 ) {
        kDebug() << k_funcinfo << ( node ? "Task is main project" : "No current task" ) << endl;
        return ;
    }
    if ( getProject().canUnindentTask( node ) ) {
        NodeUnindentCmd * cmd = new NodeUnindentCmd( getPart(), *node, i18n( "Unindent Task" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotMoveTaskUp()
{
    //kDebug()<<k_funcinfo<<endl;

    Node * task = currentTask();
    if ( 0 == task ) {
        // is always != 0. At least we would get the Project, but you never know who might change that
        // so better be careful
        kError() << k_funcinfo << "No current task" << endl;
        return ;
    }

    if ( Node::Type_Project == task->type() ) {
        kDebug() << k_funcinfo << "The root node cannot be moved up" << endl;
        return ;
    }
    if ( getProject().canMoveTaskUp( task ) ) {
        NodeMoveUpCmd * cmd = new NodeMoveUpCmd( getPart(), *task, i18n( "Move Task Up" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotMoveTaskDown()
{
    //kDebug()<<k_funcinfo<<endl;

    Node * task = currentTask();
    if ( 0 == task ) {
        // is always != 0. At least we would get the Project, but you never know who might change that
        // so better be careful
        return ;
    }

    if ( Node::Type_Project == task->type() ) {
        kDebug() << k_funcinfo << "The root node cannot be moved down" << endl;
        return ;
    }
    if ( getProject().canMoveTaskDown( task ) ) {
        NodeMoveDownCmd * cmd = new NodeMoveDownCmd( getPart(), *task, i18n( "Move Task Down" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotAddRelation( Node *par, Node *child )
{
    //kDebug()<<k_funcinfo<<endl;
    Relation * rel = new Relation( par, child );
    AddRelationDialog *dia = new AddRelationDialog( rel, this );
    if ( dia->exec()  == QDialog::Accepted) {
        KCommand * cmd = dia->buildCommand( getPart() );
        if ( cmd )
            getPart() ->addCommand( cmd );
    } else {
        delete rel;
    }
    delete dia;
}

void View::slotAddRelation( Node *par, Node *child, int linkType )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( linkType == Relation::FinishStart ||
            linkType == Relation::StartStart ||
            linkType == Relation::FinishFinish ) {
        Relation * rel = new Relation( par, child, static_cast<Relation::Type>( linkType ) );
        getPart() ->addCommand( new AddRelationCmd( getPart(), rel, i18n( "Add Relation" ) ) );
    } else {
        slotAddRelation( par, child );
    }
}

void View::slotModifyRelation( Relation *rel )
{
    //kDebug()<<k_funcinfo<<endl;
    ModifyRelationDialog * dia = new ModifyRelationDialog( rel, this );
    if ( dia->exec()  == QDialog::Accepted) {
        if ( dia->relationIsDeleted() ) {
            getPart() ->addCommand( new DeleteRelationCmd( getPart(), rel, i18n( "Delete Relation" ) ) );
        } else {
            KCommand *cmd = dia->buildCommand( getPart() );
            if ( cmd ) {
                getPart() ->addCommand( cmd );
            }
        }
    }
    delete dia;
}

void View::slotModifyRelation( Relation *rel, int linkType )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( linkType == Relation::FinishStart ||
            linkType == Relation::StartStart ||
            linkType == Relation::FinishFinish ) {
        getPart() ->addCommand( new ModifyRelationTypeCmd( getPart(), rel, static_cast<Relation::Type>( linkType ) ) );
    } else {
        slotModifyRelation( rel );
    }
}

// testing
void View::slotExportGantt()
{
    //kDebug()<<k_funcinfo<<endl;
    if ( !m_ganttview ) {
        return ;
    }
    QString fn = KFileDialog::getSaveFileName( KUrl(),
                 QString::null, this );
    if ( !fn.isEmpty() ) {
        QFile f( fn );
        m_ganttview->exportGantt( &f );
    }
}

void View::slotEditResource()
{
    //kDebug()<<k_funcinfo<<endl;
    Resource * r = currentResource();
    if ( !r )
        return ;
    ResourceDialog *dia = new ResourceDialog( getProject(), r );
    if ( dia->exec()  == QDialog::Accepted) {
        KCommand * cmd = dia->buildCommand( getPart() );
        if ( cmd )
            getPart() ->addCommand( cmd );
    }
    delete dia;
}

void View::updateReadWrite( bool /*readwrite*/ )
{}

Part *View::getPart() const
{
    return ( Part * ) koDocument();
}

void View::slotConnectNode()
{
    //kDebug()<<k_funcinfo<<endl;
    /*    NodeItem *curr = m_ganttview->currentItem();
        if (curr) {
            kDebug()<<k_funcinfo<<"node="<<curr->getNode().name()<<endl;
        }*/
}

QMenu * View::popupMenu( const QString& name )
{
    //kDebug()<<k_funcinfo<<endl;
    Q_ASSERT( factory() );
    if ( factory() )
        return ( ( QMenu* ) factory() ->container( name, this ) );
    return 0L;
}

void View::slotUpdate()
{
    //kDebug()<<k_funcinfo<<"calculate="<<calculate<<endl;

    m_updateGanttview = true;
    m_updateResourceview = true;
    m_updateAccountsview = true;

    updateView( m_tab->currentWidget() );
}

void View::slotGuiActivated( ViewBase *view, bool activate )
{
    if ( activate ) {
        foreach( QString name, view->actionListNames() ) {
            kDebug()<<k_funcinfo<<"activate "<<name<<", "<<view->actionList( name ).count()<<endl;
            plugActionList( name, view->actionList( name ) );
        }
    } else {
        foreach( QString name, view->actionListNames() ) {
            kDebug()<<k_funcinfo<<"deactivate "<<name<<endl;
            unplugActionList( name );
        }
    }
}

void View::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    kDebug()<<k_funcinfo<<ev->activated()<<endl;
    KoView::guiActivateEvent( ev );
    if ( ev->activated() ) {
        // plug my own actionlists, they may be gone
        slotPlugScheduleActions();
    }
    // propagate to sub-view
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v ) {
        v->setGuiActive( ev->activated() );
    }
}

KoDocument *View::hitTest( const QPoint &pos )
{
    // pos is in m_tab->currentWidget() coordinates
    QPoint gl = m_tab->currentWidget()->mapToGlobal(pos);
    if ( m_tab->currentWidget()->frameGeometry().contains( m_tab->currentWidget()->mapFromGlobal( gl ) ) ) {
        return koDocument()->hitTest( pos, this );
    }
    // get a 0 based geometry
    QRect r = m_viewlist->frameGeometry();
    r.translate( -r.topLeft() );
    if ( r.contains( m_viewlist->mapFromGlobal( gl ) ) ) {
        if ( getPart()->isEmbedded() ) {
            // TODO: Needs testing
            return dynamic_cast<KoDocument*>(partManager()->activePart()); // NOTE: We only handle koffice parts!
        }
        return 0;
    }
    for (int i = 0; i < m_sp->count(); ++i ) {
        QWidget *w = m_sp->handle( i );
        r = w->frameGeometry();
        r.translate( -r.topLeft() );
        if ( r.contains( w->mapFromGlobal( gl ) ) ) {
            if ( getPart()->isEmbedded() ) {
            // TODO: Needs testing
                return dynamic_cast<KoDocument*>(partManager()->activePart()); // NOTE: We only handle koffice parts!
            }
            return 0;
        }
    }
    kDebug()<<k_funcinfo<<"No hit: "<<pos<<endl;
    return 0;

}

void View::slotCreateKofficeDocument( KoDocumentEntry &entry)
{
    QString e;
    KoDocument *doc = entry.createDoc( &e, getPart() );
    if ( doc == 0 ) {
        return;
    }
    if ( ! doc->showEmbedInitDialog( this ) ) {
        delete doc;
        return;
    }
    DocumentChild *ch = getPart()->createChild( doc, geometry() );
    
    QTreeWidgetItem *cat = m_viewlist->addCategory( i18n( "Documents" ) );
    cat->setIcon( 0, KIcon( "koshell" ) );
    QString title;
    if ( doc->documentInfo() ) {
        title = doc->documentInfo()->aboutInfo( "title" );
    }
    if ( title.isEmpty() ) {
        title = doc->url().pathOrUrl();
    }
    if ( title.isEmpty() ) {
        title = "Untitled";
    }
    KoView *v = doc->createView( this );
    ViewListItem *i = m_viewlist->addView( cat, title, v, ch );
    i->setIcon( 0, KIcon( entry.service()->icon() ) );
    m_tab->addWidget( v );
    kDebug()<<k_funcinfo<<"Added: "<<v<<endl;
    m_viewlist->setSelected( i );
}

void View::slotViewActivated( ViewListItem *item, ViewListItem *prev )
{
    kDebug() << k_funcinfo << "item=" << item << ", "<<prev<<endl;
    if ( prev && prev->type() != ViewListWidget::ChildDocument ) {
        // Remove sub-view specific gui
        kDebug()<<k_funcinfo<<"Deactivate: "<<prev<<endl;
        ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
        if ( v ) {
            v->setGuiActive( false );
        }
    }
    if ( item->type() == ViewListWidget::SubView ) {
        kDebug()<<k_funcinfo<<"Activate: "<<item<<endl;
        m_tab->setCurrentWidget( item->view() );
        if (  prev && prev->type() != ViewListWidget::SubView ) {
            // Put back my own gui (removed when (if) viewing different doc)
            getPart()->activate( this );
        }
        // Add sub-view specific gui
        ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
        if ( v ) {
            v->setGuiActive( true );
        }
        return;
    } 
    if ( item->type() == ViewListWidget::ChildDocument ) {
        kDebug()<<k_funcinfo<<"Activated: "<<item->view()<<endl;
        // changing doc also takes care of all gui
        m_tab->setCurrentWidget( item->view() );
        item->documentChild()->activate( item->view() );
        return;
    }
}

QWidget *View::canvas() const
{
    return m_tab->currentWidget();//KoView::canvas();
}

void View::slotCurrentChanged( int index )
{
    //kDebug()<<k_funcinfo<<m_tab->currentIndex()<<endl;
    
    KoView *currentview = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( currentview ) {
        kDebug()<<k_funcinfo<<"ViewBase: "<<currentview<<endl;
        updateView( currentview );
        return;
    }
}

void View::updateView( QWidget *widget )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    //setScheduleActionsEnabled();
    mainWindow() ->toolBar( "report" ) ->hide();
    if ( widget == m_ganttview ) {
        kDebug()<<k_funcinfo<<"draw gantt"<<endl;
        if ( m_updateGanttview )
            m_ganttview->drawChanges( getProject() );
        //setTaskActionsEnabled( widget, true );
        m_updateGanttview = false;
    } else if ( widget == m_resourceview ) {
        //kDebug()<<k_funcinfo<<"draw resourceview"<<endl;
        if ( m_updateResourceview )
            m_resourceview->draw( getPart() ->getProject() );
        m_updateResourceview = false;
    } else if ( widget == m_accountsview ) {
        //kDebug()<<k_funcinfo<<"draw accountsview"<<endl;
        if ( m_updateAccountsview )
            m_accountsview->draw();
        m_updateAccountsview = false;
    } else if ( widget == m_taskeditor ) {
        //kDebug()<<k_funcinfo<<"draw taskeditor"<<endl;
    } else if ( widget == m_resourceview ) {
        //kDebug()<<k_funcinfo<<"draw resourceeditor"<<endl;
    }
    /*    else if (widget == m_reportview)
        {
            mainWindow()->toolBar("report")->show();
            m_reportview->enableNavigationBtn();
        }*/
    QApplication::restoreOverrideCursor();
}

void View::slotRenameNode( Node *node, const QString& name )
{
    //kDebug()<<k_funcinfo<<name<<endl;
    if ( node ) {
        NodeModifyNameCmd * cmd = new NodeModifyNameCmd( getPart(), *node, name, i18n( "Modify Name" ) );
        getPart() ->addCommand( cmd );
    }
}

void View::slotPopupMenu( const QString& menuname, const QPoint & pos )
{
    QMenu * menu = this->popupMenu( menuname );
    if ( menu )
        menu->exec( pos );
}

void View::slotPopupMenu( const QString& menuname, const QPoint &pos, ViewListItem *item )
{
    kDebug()<<k_funcinfo<<menuname<<endl;
    m_viewlistItem = item;
    slotPopupMenu( menuname, pos );
}

bool View::setContext( Context &context )
{
    //kDebug()<<k_funcinfo<<endl;
    m_currentEstimateType = context.currentEstimateType;
    getProject().setCurrentSchedule( context.currentSchedule );
//     actionViewExpected->setChecked( context.actionViewExpected );
//     actionViewOptimistic->setChecked( context.actionViewOptimistic );
//     actionViewPessimistic->setChecked( context.actionViewPessimistic );

    m_ganttview->setContext( context.ganttview, getProject() );
    // hmmm, can't decide if these should be here or actions moved to ganttview
    actionViewGanttResources->setChecked( context.ganttview.showResources );
    actionViewGanttTaskName->setChecked( context.ganttview.showTaskName );
    actionViewGanttTaskLinks->setChecked( context.ganttview.showTaskLinks );
    actionViewGanttProgress->setChecked( context.ganttview.showProgress );
    actionViewGanttFloat->setChecked( context.ganttview.showPositiveFloat );
    actionViewGanttCriticalTasks->setChecked( context.ganttview.showCriticalTasks );
    actionViewGanttCriticalPath->setChecked( context.ganttview.showCriticalPath );

    m_resourceview->setContext( context.resourceview );
    m_accountsview->setContext( context.accountsview );
    //    m_reportview->setContext(context.reportview);

    if ( context.currentView == "ganttview" ) {
/*        m_ganttview->setShowExpected( actionViewExpected->isChecked() );
        m_ganttview->setShowOptimistic( actionViewOptimistic->isChecked() );
        m_ganttview->setShowPessimistic( actionViewPessimistic->isChecked() );*/
        slotViewGantt();
    } else if ( context.currentView == "resourceview" ) {
        slotViewResources();
    } else if ( context.currentView == "accountsview" ) {
        slotViewAccounts();
    } else if ( context.currentView == "taskeditor" ) {
        slotViewTaskEditor();
    } else {
        slotViewGantt();
    }
    slotUpdate();
    return true;
}

void View::getContext( Context &context ) const
{
    //kDebug()<<k_funcinfo<<endl;
    context.currentEstimateType = m_currentEstimateType;
    if ( getProject().currentSchedule() )
        context.currentSchedule = getProject().currentSchedule() ->id();
//     context.actionViewExpected = actionViewExpected->isChecked();
//     context.actionViewOptimistic = actionViewOptimistic->isChecked();
//     context.actionViewPessimistic = actionViewPessimistic->isChecked();

    if ( m_tab->currentWidget() == m_ganttview ) {
        context.currentView = "ganttview";
    } else if ( m_tab->currentWidget() == m_resourceview ) {
        context.currentView = "resourceview";
    } else if ( m_tab->currentWidget() == m_accountsview ) {
        context.currentView = "accountsview";
    } else if (m_tab->currentWidget() == m_taskeditor) {
        context.currentView = "taskeditor";
    } else if (m_tab->currentWidget() == m_resourceeditor) {
        context.currentView = "resourceeditor";
    }
m_ganttview->getContext( context.ganttview );
//    m_resourceview->getContext( context.resourceview );
    m_accountsview->getContext( context.accountsview );
    //    m_reportview->getContext(context.reportview);
}

void View::setLabel()
{
    kDebug()<<k_funcinfo<<endl;
    Schedule *s = getProject().currentSchedule();
    if ( s == 0 || getProject().notScheduled() ) {
        m_estlabel->setText( i18n( "Not scheduled" ) );
    } else {
        m_estlabel->setText( s->name() + " ("+ s->typeToString( true ) + ")"  );
    }
}

void View::setScheduleActionsEnabled()
{
/*    actionViewExpected->setEnabled( getProject().findSchedule( Schedule::Expected ) );
    actionViewOptimistic->setEnabled( getProject().findSchedule( Schedule::Optimistic ) );
    actionViewPessimistic->setEnabled( getProject().findSchedule( Schedule::Pessimistic ) );*/
    if ( getProject().notScheduled() ) {
        setLabel();
        return ;
    }
    Schedule *ns = getProject().currentSchedule();
    if ( ns->type() == Schedule::Expected ) {
//        actionViewExpected->setChecked( true );
    } else if ( ns->type() == Schedule::Optimistic ) {
//        actionViewOptimistic->setChecked( true );
    } else if ( ns->type() == Schedule::Pessimistic ) {
//        actionViewPessimistic->setChecked( true );
    }
    setLabel();
}


#ifndef NDEBUG
void View::slotPrintDebug()
{
    kDebug() << "-------- Debug printout: Node list" << endl;
    /*    Node *curr = m_ganttview->currentNode();
        if (curr) {
            curr->printDebug(true,"");
        } else*/
    getPart() ->getProject().printDebug( true, "" );
}
void View::slotPrintSelectedDebug()
{
    Node * curr = m_ganttview->currentNode();
    if ( curr ) {
        kDebug() << "-------- Debug printout: Selected node" << endl;
        curr->printDebug( true, "" );
    } else
        slotPrintDebug();
}
void View::slotPrintCalendarDebug()
{
    kDebug() << "-------- Debug printout: Node list" << endl;
    /*    Node *curr = m_ganttview->currentNode();
        if (curr) {
            curr->printDebug(true,"");
        } else*/
    getPart() ->getProject().printCalendarDebug( "" );
}
void View::slotPrintTestDebug()
{
    const QStringList & lst = getPart() ->xmlLoader().log();

    for ( QStringList::ConstIterator it = lst.constBegin(); it != lst.constEnd(); ++it ) {
        kDebug() << *it << endl;
    }
    //     kDebug()<<"------------Test 1---------------------"<<endl;
    //     {
    //     DateTime d1(QDate(2006,1,2), QTime(8,0,0));
    //     DateTime d2 = d1.addSecs(3600);
    //     Duration d = d2 - d1;
    //     bool b = d==Duration(0,0,0,3600);
    //     kDebug()<<"1: Success="<<b<<"    "<<d2.toString()<<"-"<<d1.toString()<<"="<<d.toString()<<endl;
    //     d = d1 - d2;
    //     b = d==Duration(0,0,0,3600);
    //     kDebug()<<"2: Success="<<b<<"    "<<d1.toString()<<"-"<<d2.toString()<<"="<<d.toString()<<endl;
    //     d2 = d2.addDays(-2);
    //     d = d1 - d2;
    //     b = d==Duration(2,0,0)-Duration(0,0,0,3600);
    //     kDebug()<<"3: Success="<<b<<"    "<<d1.toString()<<"-"<<d2.toString()<<"="<<d.toString()<<endl;
    //     d = d2 - d1;
    //     b = d==Duration(2,0,0)-Duration(0,0,0,3600);
    //     kDebug()<<"4: Success="<<b<<"     "<<d2.toString()<<"-"<<d1.toString()<<"="<<d.toString()<<endl;
    //     kDebug()<<endl;
    //     b = (d2 + d)==d1;
    //     kDebug()<<"5: Success="<<b<<"   "<<d2<<"+"<<d.toString()<<"="<<d1<<endl;
    //     b = (d1 - d)==d2;
    //     kDebug()<<"6: Success="<<b<<"   "<<d1<<"-"<<d.toString()<<"="<<d2<<endl;
    //     } // end test 1
    //     kDebug()<<endl;
    //     kDebug()<<"------------Test 2 Single calendar-----------------"<<endl;
    //     {
    //     Calendar *t = new Calendar("Test 2");
    //     QDate wdate(2006,1,2);
    //     DateTime before = DateTime(wdate.addDays(-1));
    //     DateTime after = DateTime(wdate.addDays(1));
    //     QTime t1(8,0,0);
    //     QTime t2(10,0,0);
    //     DateTime wdt1(wdate, t1);
    //     DateTime wdt2(wdate, t2);
    //     CalendarDay *day = new CalendarDay(QDate(2006,1,2), Map::Working);
    //     day->addInterval(TimeInterval(t1, t2));
    //     if (!t->addDay(day)) {
    //         kDebug()<<"Failed to add day"<<endl;
    //         delete day;
    //         delete t;
    //         return;
    //     }
    //     kDebug()<<"Added     date="<<day->date().toString()<<" "<<day->startOfDay().toString()<<" - "<<day->endOfDay()<<endl;
    //     kDebug()<<"Found     date="<<day->date().toString()<<" "<<day->startOfDay().toString()<<" - "<<day->endOfDay()<<endl;
    //
    //     CalendarDay *d = t->findDay(wdate);
    //     bool b = (day == d);
    //     kDebug()<<"1: Success="<<b<<"      Find same day"<<endl;
    //
    //     DateTime dt = t->firstAvailableAfter(after, after.addDays(10));
    //     b = !dt.isValid();
    //     kDebug()<<"2: Success="<<b<<"      firstAvailableAfter("<<after<<"): ="<<dt<<endl;
    //
    //     dt = t->firstAvailableBefore(before, before.addDays(-10));
    //     b = !dt.isValid();
    //     kDebug()<<"3: Success="<<b<<"       firstAvailableBefore("<<before.toString()<<"): ="<<dt<<endl;
    //
    //     dt = t->firstAvailableAfter(before, after);
    //     b = dt == wdt1;
    //     kDebug()<<"4: Success="<<b<<"      firstAvailableAfter("<<before<<"): ="<<dt<<endl;
    //
    //     dt = t->firstAvailableBefore(after, before);
    //     b = dt == wdt2;
    //     kDebug()<<"5: Success="<<b<<"      firstAvailableBefore("<<after<<"): ="<<dt<<endl;
    //
    //     b = t->hasInterval(before, after);
    //     kDebug()<<"6: Success="<<b<<"      hasInterval("<<before<<", "<<after<<")"<<endl;
    //
    //     b = !t->hasInterval(after, after.addDays(1));
    //     kDebug()<<"7: Success="<<b<<"      !hasInterval("<<after<<", "<<after.addDays(1)<<")"<<endl;
    //
    //     b = !t->hasInterval(before, before.addDays(-1));
    //     kDebug()<<"8: Success="<<b<<"      !hasInterval("<<before<<", "<<before.addDays(-1)<<")"<<endl;
    //
    //     Duration e1(0, 2, 0); // 2 hours
    //     Duration e2 = t->effort(before, after);
    //     b = e1==e2;
    //     kDebug()<<"9: Success="<<b<<"      effort"<<e1.toString()<<" = "<<e2.toString()<<endl;
    //
    //     delete t;
    //     }// end test 2
    //
    //     kDebug()<<endl;
    //     kDebug()<<"------------Test 3 Parent calendar-----------------"<<endl;
    //     {
    //     Calendar *t = new Calendar("Test 3");
    //     Calendar *p = new Calendar("Test 3 parent");
    //     t->setParent(p);
    //     QDate wdate(2006,1,2);
    //     DateTime before = DateTime(wdate.addDays(-1));
    //     DateTime after = DateTime(wdate.addDays(1));
    //     QTime t1(8,0,0);
    //     QTime t2(10,0,0);
    //     DateTime wdt1(wdate, t1);
    //     DateTime wdt2(wdate, t2);
    //     CalendarDay *day = new CalendarDay(QDate(2006,1,2), Map::Working);
    //     day->addInterval(TimeInterval(t1, t2));
    //     if (!p->addDay(day)) {
    //         kDebug()<<"Failed to add day"<<endl;
    //         delete day;
    //         delete t;
    //         return;
    //     }
    //     kDebug()<<"Added     date="<<day->date().toString()<<" "<<day->startOfDay().toString()<<" - "<<day->endOfDay().toString()<<endl;
    //     kDebug()<<"Found     date="<<day->date().toString()<<" "<<day->startOfDay().toString()<<" - "<<day->endOfDay().toString()<<endl;
    //
    //     CalendarDay *d = p->findDay(wdate);
    //     bool b = (day == d);
    //     kDebug()<<"1: Success="<<b<<"      Find same day"<<endl;
    //
    //     DateTime dt = t->firstAvailableAfter(after, after.addDays(10));
    //     b = !dt.isValid();
    //     kDebug()<<"2: Success="<<b<<"      firstAvailableAfter("<<after.toString()<<"): ="<<!b<<endl;
    //
    //     dt = t->firstAvailableBefore(before, before.addDays(-10));
    //     b = !dt.isValid();
    //     kDebug()<<"3: Success="<<b<<"       firstAvailableBefore("<<before.toString()<<"): ="<<!b<<endl;
    //
    //     dt = t->firstAvailableAfter(before, after);
    //     b = dt == wdt1;
    //     kDebug()<<"4: Success="<<b<<"      firstAvailableAfter("<<before.toString()<<"): ="<<dt.toString()<<endl;
    //
    //     dt = t->firstAvailableBefore(after, before);
    //     b = dt == wdt2;
    //     kDebug()<<"5: Success="<<b<<"      firstAvailableBefore("<<after.toString()<<"): ="<<dt.toString()<<endl;
    //
    //     b = t->hasInterval(before, after);
    //     kDebug()<<"6: Success="<<b<<"      hasInterval("<<before.toString()<<", "<<after<<")"<<endl;
    //
    //     b = !t->hasInterval(after, after.addDays(1));
    //     kDebug()<<"7: Success="<<b<<"      !hasInterval("<<after.toString()<<", "<<after.addDays(1)<<")"<<endl;
    //
    //     b = !t->hasInterval(before, before.addDays(-1));
    //     kDebug()<<"8: Success="<<b<<"      !hasInterval("<<before.toString()<<", "<<before.addDays(-1)<<")"<<endl;
    //     Duration e1(0, 2, 0); // 2 hours
    //     Duration e2 = t->effort(before, after);
    //     b = e1==e2;
    //     kDebug()<<"9: Success="<<b<<"      effort "<<e1.toString()<<"=="<<e2.toString()<<endl;
    //
    //     delete t;
    //     delete p;
    //     }// end test 3
    //     kDebug()<<endl;
    //     kDebug()<<"------------Test 4 Parent calendar/weekdays-------------"<<endl;
    //     {
    //     QTime t1(8,0,0);
    //     QTime t2(10,0,0);
    //     Calendar *p = new Calendar("Test 4 parent");
    //     CalendarDay *wd1 = p->weekday(0); // monday
    //     if (wd1 == 0) {
    //         kDebug()<<"Failed to get weekday"<<endl;
    //     }
    //     wd1->setState(Map::NonWorking);
    //
    //     CalendarDay *wd2 = p->weekday(2); // wednesday
    //     if (wd2 == 0) {
    //         kDebug()<<"Failed to get weekday"<<endl;
    //     }
    //     wd2->addInterval(TimeInterval(t1, t2));
    //     wd2->setState(Map::Working);
    //
    //     Calendar *t = new Calendar("Test 4");
    //     t->setParent(p);
    //     QDate wdate(2006,1,2); // monday jan 2
    //     DateTime before = DateTime(wdate.addDays(-4)); //Thursday dec 29
    //     DateTime after = DateTime(wdate.addDays(4)); // Friday jan 6
    //     DateTime wdt1(wdate, t1);
    //     DateTime wdt2(QDate(2006, 1, 4), t2); // Wednesday
    //     CalendarDay *day = new CalendarDay(QDate(2006,1,2), Map::Working);
    //     day->addInterval(TimeInterval(t1, t2));
    //     if (!p->addDay(day)) {
    //         kDebug()<<"Failed to add day"<<endl;
    //         delete day;
    //         delete t;
    //         return;
    //     }
    //     kDebug()<<"Added     date="<<day->date().toString()<<" "<<day->startOfDay().toString()<<" - "<<day->endOfDay().toString()<<endl;
    //     kDebug()<<"Found     date="<<day->date().toString()<<" "<<day->startOfDay().toString()<<" - "<<day->endOfDay().toString()<<endl;
    //
    //     CalendarDay *d = p->findDay(wdate);
    //     bool b = (day == d);
    //     kDebug()<<"1: Success="<<b<<"      Find same day"<<endl;
    //
    //     DateTime dt = t->firstAvailableAfter(after, after.addDays(10));
    //     b = (dt.isValid() && dt == DateTime(QDate(2006,1,11), t1));
    //     kDebug()<<"2: Success="<<b<<"      firstAvailableAfter("<<after<<"): ="<<dt<<endl;
    //
    //     dt = t->firstAvailableBefore(before, before.addDays(-10));
    //     b = (dt.isValid() && dt == DateTime(QDate(2005, 12, 28), t2));
    //     kDebug()<<"3: Success="<<b<<"       firstAvailableBefore("<<before.toString()<<"): ="<<dt<<endl;
    //
    //     dt = t->firstAvailableAfter(before, after);
    //     b = dt == wdt1; // We find the day jan 2
    //     kDebug()<<"4: Success="<<b<<"      firstAvailableAfter("<<before.toString()<<"): ="<<dt.toString()<<endl;
    //
    //     dt = t->firstAvailableBefore(after, before);
    //     b = dt == wdt2; // We find the weekday (wednesday)
    //     kDebug()<<"5: Success="<<b<<"      firstAvailableBefore("<<after.toString()<<"): ="<<dt.toString()<<endl;
    //
    //     b = t->hasInterval(before, after);
    //     kDebug()<<"6: Success="<<b<<"      hasInterval("<<before.toString()<<", "<<after<<")"<<endl;
    //
    //     b = !t->hasInterval(after, after.addDays(1));
    //     kDebug()<<"7: Success="<<b<<"      !hasInterval("<<after.toString()<<", "<<after.addDays(1)<<")"<<endl;
    //
    //     b = !t->hasInterval(before, before.addDays(-1));
    //     kDebug()<<"8: Success="<<b<<"      !hasInterval("<<before.toString()<<", "<<before.addDays(-1)<<")"<<endl;
    //     Duration e1(0, 4, 0); // 2 hours
    //     Duration e2 = t->effort(before, after);
    //     b = e1==e2;
    //     kDebug()<<"9: Success="<<b<<"      effort "<<e1.toString()<<"="<<e2.toString()<<endl;
    //
    //     DateTimeInterval r = t->firstInterval(before, after);
    //     b = r.first == wdt1; // We find the monday jan 2
    //     kDebug()<<"10: Success="<<b<<"      firstInterval("<<before<<"): ="<<r.first<<", "<<r.second<<endl;
    //     r = t->firstInterval(r.second, after);
    //     b = r.first == DateTime(QDate(2006, 1, 4),t1); // We find the wednesday jan 4
    //     kDebug()<<"11: Success="<<b<<"      firstInterval("<<r.second<<"): ="<<r.first<<", "<<r.second<<endl;
    //
    //     delete t;
    //     delete p;
    //     }// end test 4
}
#endif

}  //KPlato namespace

#include "kptview.moc"
