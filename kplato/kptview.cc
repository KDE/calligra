/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2007 Dag Andersen <danders@get2net.dk>

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
#include <KoDocumentChild.h>

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
#include <kstandardaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <ktoolbar.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kdeversion.h>
#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <k3command.h>
#include <kfiledialog.h>
#include <kparts/event.h>
#include <kparts/partmanager.h>
#include <KoQueryTrader.h>

#include "kptview.h"
#include "kptviewbase.h"
#include "kptaccountsview.h"
#include "kptaccountseditor.h"
#include "kptcalendareditor.h"
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
#include "kptperteditor.h"
#include "kptpertresult.h"
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
#include "kptchartdialog.h"
#include "kptresourceassignmentview.h"
#include "kpttaskstatusview.h"

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

ViewListItem::ViewListItem( const QString &tag, const QStringList &strings, int type )
    : QTreeWidgetItem( strings, type ),
    m_tag( tag )
{
}

ViewListItem::ViewListItem( QTreeWidget *parent, const QString &tag, const QStringList &strings, int type )
    : QTreeWidgetItem( parent, strings, type ),
    m_tag( tag )
{
}

ViewListItem::ViewListItem( QTreeWidgetItem *parent, const QString &tag, const QStringList &strings, int type )
    : QTreeWidgetItem( parent, strings, type ),
    m_tag( tag )
{
}

void ViewListItem::setView( KoView *view )
{
    setData( 0, ViewListItem::DataRole_View,  qVariantFromValue(static_cast<QWidget*>( view ) ) );
}

KoView *ViewListItem::view() const
{
    if ( data(0, ViewListItem::DataRole_View ).isValid() ) {
        return static_cast<KoView*>( data(0, ViewListItem::DataRole_View ).value<QWidget*>() );
    }
    return 0;
}

void ViewListItem::setDocument( KoDocument *doc )
{
    setData( 0, ViewListItem::DataRole_Document,  qVariantFromValue(static_cast<QObject*>( doc ) ) );
}

KoDocument *ViewListItem::document() const
{
    if ( data(0, ViewListItem::DataRole_Document ).isValid() ) {
        return static_cast<KoDocument*>( data(0, ViewListItem::DataRole_Document ).value<QObject*>() );
    }
    return 0;
}

void ViewListItem::setDocumentChild( DocumentChild *child )
{
    setData( 0, ViewListItem::DataRole_ChildDocument,  qVariantFromValue(static_cast<QObject*>( child ) ) );
}

DocumentChild *ViewListItem::documentChild() const
{
    if ( data(0, ViewListItem::DataRole_ChildDocument ).isValid() ) {
        return static_cast<DocumentChild*>( data(0, ViewListItem::DataRole_ChildDocument ).value<QObject*>() );
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
    //kDebug()<<k_funcinfo<<endl;
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

ViewListItem *ViewListTreeWidget::findCategory( const QString &cat )
{
    QTreeWidgetItem * item;
    int cnt = topLevelItemCount();
    for ( int i = 0; i < cnt; ++i ) {
        item = topLevelItem( i );
        if ( static_cast<ViewListItem*>(item)->tag() == cat )
            return static_cast<ViewListItem*>(item);
    }
    return 0;
}

ViewListWidget::ViewListWidget( Part *part, QWidget *parent )//QString name, KXmlGuiWindow *parent )
        : QWidget( parent ),
        m_part( part )
{
    setObjectName("ViewListWidget");
    m_viewlist = new ViewListTreeWidget( this );
    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    l->addWidget( m_viewlist );
    m_viewlist->setEditTriggers( QAbstractItemView::DoubleClicked );

    connect( m_viewlist, SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ), SLOT( slotActivated( QTreeWidgetItem*, QTreeWidgetItem* ) ) );

    connect( m_viewlist, SIGNAL( itemChanged( QTreeWidgetItem*, int ) ), SLOT( slotItemChanged( QTreeWidgetItem*, int ) ) );

    setupContextMenus();
}

ViewListWidget::~ViewListWidget()
{
}

void ViewListWidget::slotItemChanged( QTreeWidgetItem *item, int col )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( item && item->type() == ViewListItem::ItemType_ChildDocument && col == 0 ) {
        DocumentChild *ch = static_cast<ViewListItem*>(item)->documentChild();
        if ( ch ) {
            ch->setTitle( item->text( 0 ) );
            //kDebug()<<k_funcinfo<<ch->title()<<endl;
        }
    }
}

void ViewListWidget::slotActivated( QTreeWidgetItem *item, QTreeWidgetItem *prev )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( item == 0 || item->type() == ViewListItem::ItemType_Category ) {
        return ;
    }
    emit activated( static_cast<ViewListItem*>( item ), static_cast<ViewListItem*>( prev ) );
}

ViewListItem *ViewListWidget::addCategory( const QString &tag, const QString& name )
{
    //kDebug() << k_funcinfo << endl;
    ViewListItem *item = m_viewlist->findCategory( tag );
    if ( item == 0 ) {
        item = new ViewListItem( m_viewlist, tag, QStringList( name ), ViewListItem::ItemType_Category );
        item->setExpanded( true );
        item->setFlags( item->flags() | Qt::ItemIsEditable );
    }
    return item;
}

ViewListItem *ViewListWidget::addView( QTreeWidgetItem *category, const QString &tag, const QString& name, KoView *view, KoDocument *doc, const QString& icon )
{
    ViewListItem * item = new ViewListItem( category, tag, QStringList( name ), ViewListItem::ItemType_SubView );
    item->setView( view );
    item->setDocument( doc );
    if ( !icon.isEmpty() )
        item->setData( 0, Qt::DecorationRole, KIcon( icon ) );
    //kDebug() << k_funcinfo << "added: " << item << endl;
    return item;
}

ViewListItem *ViewListWidget::createView( const QString& tag, const QString& name, KoView *view, DocumentChild *ch, const QString& icon )
{
    ViewListItem * item = new ViewListItem( tag, QStringList( name ), ViewListItem::ItemType_ChildDocument );
    item->setView( view );
    item->setDocument( ch->document() );
    item->setDocumentChild( ch );
    if ( !icon.isEmpty() ) {
        item->setData( 0, Qt::DecorationRole, KIcon( icon ) );
    }
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    //kDebug() << k_funcinfo << "added: " << item << endl;
    return item;
}

void ViewListWidget::setSelected( QTreeWidgetItem *item )
{
    //kDebug()<<k_funcinfo<<item<<", "<<m_viewlist->currentItem()<<endl;
    if ( item == 0 ) {
        return;
    }
    m_viewlist->setCurrentItem( item );
    //kDebug()<<k_funcinfo<<item<<", "<<m_viewlist->currentItem()<<endl;
}

KoView *ViewListWidget::findView( const QString &tag )
{
    ViewListItem *i = findItem( tag );
    if ( i == 0 ) {
        return 0;
    }
    return i->view();
}

ViewListItem *ViewListWidget::findItem( const QString &tag, QTreeWidgetItem *parent )
{
    if ( parent == 0 ) {
        return findItem( tag, m_viewlist->invisibleRootItem() );
    }
    for (int i = 0; i < parent->childCount(); ++i ) {
        ViewListItem * ch = static_cast<ViewListItem*>( parent->child( i ) );
        if ( ch->tag() == tag ) {
            //kDebug()<<k_funcinfo<<ch<<", "<<view<<endl;
            return ch;
        }
        ch = findItem( tag, ch );
        if ( ch ) {
            return ch;
        }
    }
    return 0;
}

ViewListItem *ViewListWidget::findItem(  const QWidget *view, QTreeWidgetItem *parent )
{
    if ( parent == 0 ) {
        return findItem( view, m_viewlist->invisibleRootItem() );
    }
    for (int i = 0; i < parent->childCount(); ++i ) {
        ViewListItem * ch = static_cast<ViewListItem*>( parent->child( i ) );
        if ( ch->view() == view ) {
            //kDebug()<<k_funcinfo<<ch<<", "<<view<<endl;
            return ch;
        }
        ch = findItem( view, ch );
        if ( ch ) {
            return ch;
        }
    }
    return 0;
}

void ViewListWidget::slotCreatePart()
{
    kDebug()<<k_funcinfo<<endl;
    QString servName = sender()->objectName();
    kDebug()<<k_funcinfo<<servName<<endl;
    KService::Ptr serv = KService::serviceByName( servName );
    KoDocumentEntry entry = KoDocumentEntry( serv );
    emit createKofficeDocument( entry );
}

void ViewListWidget::slotEditDocumentTitle()
{
    //QTreeWidgetItem *item = m_viewlist->currentItem();
    if ( m_contextitem ) {
        kDebug()<<k_funcinfo<<m_contextitem<<": "<<m_contextitem->type()<<endl;
        m_viewlist->editItem( m_contextitem );
    }
}

void ViewListWidget::slotRemoveDocument()
{
    if ( m_contextitem ) {
        kDebug()<<k_funcinfo<<m_contextitem<<": "<<m_contextitem->type()<<endl;
        m_part->addCommand( new DeleteEmbeddedDocumentCmd( m_part, this, m_contextitem, i18n( "Remove Document" ) ) );
        m_contextitem = 0;
    }
}

int ViewListWidget::takeViewListItem( ViewListItem *item )
{
    QTreeWidgetItem *p = item->parent();
    if ( p == 0 ) {
        p = m_viewlist->invisibleRootItem();
    }
    int i = p->indexOfChild( item );
    if ( i != -1 ) {
        p->takeChild( i );
        emit viewListItemRemoved( item );
    }
    return i;
}

void ViewListWidget::insertViewListItem( ViewListItem *item, QTreeWidgetItem *parent, int index )
{
    QTreeWidgetItem *p = parent;
    if ( p == 0 ) {
        p = m_viewlist->invisibleRootItem();
    }
    if ( index == -1 ) {
        index = p->childCount();
    }
    p->insertChild( index, item );
    emit viewListItemInserted( item );
}

void ViewListWidget::setupContextMenus()
{
    // NOTE: can't use xml file as there may not be a factory()
    QAction *action;
    m_separator = new QAction(this);
    m_separator->setSeparator(true);
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
        connect(action, SIGNAL( triggered( bool ) ), this, SLOT( slotCreatePart() ) );
        m_parts.append( action );
    }
    // no item actions
    //action = new QAction( KIcon( "document-new" ), i18n( "New Category..." ), this );
    //m_noitem.append( action );

    // Category actions
/*    action = new QAction( KIcon( "rename" ), i18n( "Rename Category" ), this );
    m_category.append( action );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( renameCategory() ) );*/
    //action = new QAction( KIcon( "list-remove" ), i18n( "Remove Category" ), this );
    //m_category.append( action );

    // view actions
    //action = new QAction( KIcon( "show" ), i18n( "Show" ), this );
    //m_view.append( action );
    // document actions
    //action = new QAction( KIcon( "show" ), i18n( "Show" ), this );
    //m_document.append( action );
    //action = new QAction( KIcon( "document-properties" ), i18n( "Document information" ), this );
    action = new QAction( KIcon( "fileedit" ), i18n( "Edit Document Title" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotEditDocumentTitle() ) );
    m_document.append( action );
    action = new QAction( KIcon( "view-remove" ), i18n( "Remove Document" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotRemoveDocument() ) );
    m_document.append( action );
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
    } else if ( m_contextitem->type() == ViewListItem::ItemType_Category ) {
        lst = m_category;
        lst.append( m_separator );
        lst += m_parts;
    } else if ( m_contextitem->type() == ViewListItem::ItemType_SubView ) {
        lst = m_view;
        lst.append( m_separator );
        lst += m_parts;
    } else if ( m_contextitem->type() == ViewListItem::ItemType_ChildDocument ) {
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
        m_currentEstimateType( Effort::Use_Expected )
{
    //kDebug()<<k_funcinfo<<endl;
    getProject().setCurrentSchedule( Schedule::Expected );

    setComponentData( Factory::global() );
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
    m_viewlist = new ViewListWidget( getPart(), m_sp );
    // to the right
    m_tab = new QStackedWidget( m_sp );

////////////////////////////////////////////////////////////////////////////////////////////////////

    ViewListItem *cat;
    cat = m_viewlist->addCategory( "Editors", i18n( "Editors" ) );
    createCalendarEditor( cat );
    createAccountsEditor( cat );
    createResourceditor( cat );
    createTaskeditor( cat );
    createScheduleEditor( cat );
    createDependencyEditor( cat );

    cat = m_viewlist->addCategory( "Views", i18n( "Views" ) );
    createTaskStatusView( cat );
    createGanttView( cat );
    createResourceView( cat );
    createAccountsView( cat );
    createPertResultView( cat );
    createResourceAssignmentView( cat );

    // Add child documents
    createChildDocumentViews();

    connect( m_viewlist, SIGNAL( activated( ViewListItem*, ViewListItem* ) ), SLOT( slotViewActivated( ViewListItem*, ViewListItem* ) ) );
    connect( m_viewlist, SIGNAL( viewListItemRemoved( ViewListItem* ) ), SLOT( slotViewListItemRemoved( ViewListItem* ) ) );
    connect( m_viewlist, SIGNAL( viewListItemInserted( ViewListItem* ) ), SLOT( slotViewListItemInserted( ViewListItem* ) ) );

    connect( m_tab, SIGNAL( currentChanged( int ) ), this, SLOT( slotCurrentChanged( int ) ) );

    // The menu items
    // ------ Edit
    actionCut = actionCollection()->addAction(KStandardAction::Cut,  "edit_cut", this, SLOT( slotEditCut() ));
    actionCopy = actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", this, SLOT( slotEditCopy() ));
    actionPaste = actionCollection()->addAction(KStandardAction::Paste,  "edit_paste", this, SLOT( slotEditPaste() ));

    // ------ View

    actionViewSelector  = new KToggleAction(i18n("Show Selector"), this);
    actionCollection()->addAction("view_show_selector", actionViewSelector );
    connect( actionViewSelector, SIGNAL( triggered( bool ) ), SLOT( slotViewSelector( bool ) ) );

    m_scheduleActionGroup = new QActionGroup( this );
    m_scheduleActionGroup->setExclusive( true );
    connect( m_scheduleActionGroup, SIGNAL( triggered( QAction* ) ), SLOT( slotViewSchedule( QAction* ) ) );

    actionViewGanttResources  = new KToggleAction(i18n("Resources"), this);
    actionCollection()->addAction("view_gantt_showResources", actionViewGanttResources );
    connect( actionViewGanttResources, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttResources() ) );
    actionViewGanttTaskName  = new KToggleAction(i18n("Task Name"), this);
    actionCollection()->addAction("view_gantt_showTaskName", actionViewGanttTaskName );
    connect( actionViewGanttTaskName, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttTaskName() ) );
    actionViewGanttTaskLinks  = new KToggleAction(i18n("Task Links"), this);
    actionCollection()->addAction("view_gantt_showTaskLinks", actionViewGanttTaskLinks );
    connect( actionViewGanttTaskLinks, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttTaskLinks() ) );
    actionViewGanttProgress  = new KToggleAction(i18n("Progress"), this);
    actionCollection()->addAction("view_gantt_showProgress", actionViewGanttProgress );
    connect( actionViewGanttProgress, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttProgress() ) );
    actionViewGanttFloat  = new KToggleAction(i18n("Float"), this);
    actionCollection()->addAction("view_gantt_showFloat", actionViewGanttFloat );
    connect( actionViewGanttFloat, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttFloat() ) );
    actionViewGanttCriticalTasks  = new KToggleAction(i18n("Critical Tasks"), this);
    actionCollection()->addAction("view_gantt_showCriticalTasks", actionViewGanttCriticalTasks );
    connect( actionViewGanttCriticalTasks, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttCriticalTasks() ) );
    actionViewGanttCriticalPath  = new KToggleAction(i18n("Critical Path"), this);
    actionCollection()->addAction("view_gantt_showCriticalPath", actionViewGanttCriticalPath );
    connect( actionViewGanttCriticalPath, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttCriticalPath() ) );

    // actionViewGanttNotScheduled  = new KToggleAction(i18n("Not Scheduled"), this);
    // actionCollection()->addAction("view_gantt_showNotScheduled", actionViewGanttNotScheduled );
    // connect(actionViewGanttNotScheduled, SIGNAL(triggered(bool)), this, SLOT(slotViewGanttNotScheduled()));

    actionViewTaskAppointments  = new KToggleAction(i18n("Show allocations"), this);
    actionCollection()->addAction("view_task_appointments", actionViewTaskAppointments );
    connect( actionViewTaskAppointments, SIGNAL( triggered( bool ) ), SLOT( slotViewTaskAppointments() ) );

    actionViewResourceAppointments  = new KToggleAction(i18n("Show allocations"), this);
    actionCollection()->addAction("view_resource_appointments", actionViewResourceAppointments );
    connect( actionViewResourceAppointments, SIGNAL( triggered( bool ) ), SLOT( slotViewResourceAppointments() ) );

    // ------ Insert

    // ------ Project
    actionEditMainProject  = new KAction(KIcon( "edit" ), i18n("Edit Main Project..."), this);
    actionCollection()->addAction("project_edit", actionEditMainProject );
    connect( actionEditMainProject, SIGNAL( triggered( bool ) ), SLOT( slotProjectEdit() ) );

    actionChartIndicators  = new KAction(KIcon( "edit" ), i18n("Chart Indicators..."), this);
    actionCollection()->addAction("chart_indicators", actionChartIndicators );
    connect( actionChartIndicators, SIGNAL( triggered( bool ) ), SLOT( slotChartDisplay() ) );

    actionEditStandardWorktime  = new KAction(KIcon( "edit" ), i18n("Edit Standard Worktime..."), this);
    actionCollection()->addAction("project_worktime", actionEditStandardWorktime );
    connect( actionEditStandardWorktime, SIGNAL( triggered( bool ) ), SLOT( slotProjectWorktime() ) );
    actionEditCalendarList  = new KAction(KIcon( "edit" ), i18n("Edit Calendar..."), this);
    actionCollection()->addAction("project_calendar", actionEditCalendarList );
    connect( actionEditCalendarList, SIGNAL( triggered( bool ) ), SLOT( slotProjectCalendar() ) );
    actionEditAccounts  = new KAction(KIcon( "edit" ), i18n("Edit Accounts..."), this);
    actionCollection()->addAction("project_accounts", actionEditAccounts );
    connect( actionEditAccounts, SIGNAL( triggered( bool ) ), SLOT( slotProjectAccounts() ) );
    actionEditResources  = new KAction(KIcon( "edit" ), i18n("Edit Resources..."), this);
    actionCollection()->addAction("project_resources", actionEditResources );
    connect( actionEditResources, SIGNAL( triggered( bool ) ), SLOT( slotProjectResources() ) );


    /*    // ------ Reports
    actionFirstpage = actionCollection()->addAction(KStandardAction::FirstPage, "go_firstpage", m_reportview,SLOT(slotPrevPage()));
        connect(m_reportview, SIGNAL(setFirstPageActionEnabled(bool)), actionFirstpage, SLOT(setEnabled(bool)));
    actionPriorpage = actionCollection()->addAction(KStandardAction::Prior, "go_prevpage", m_reportview,SLOT(slotPrevPage()));
        connect(m_reportview, SIGNAL(setPriorPageActionEnabled(bool)), actionPriorpage, SLOT(setEnabled(bool)));
    actionNextpage = actionCollection()->addAction(KStandardAction::Next,  "go_nextpage", m_reportview,SLOT(slotNextPage()));
        connect(m_reportview, SIGNAL(setNextPageActionEnabled(bool)), actionNextpage, SLOT(setEnabled(bool)));
    actionLastpage = actionCollection()->addAction(KStandardAction::LastPage,  "go_lastpage", m_reportview,SLOT(slotLastPage()));
        connect(m_reportview, SIGNAL(setLastPageActionEnabled(bool)), actionLastpage, SLOT(setEnabled(bool)));
        m_reportview->enableNavigationBtn();*/
    mainWindow() ->toolBar( "report" ) ->hide();

    //     new KAction(i18n("Design..."), "report_design", 0, this,
    //         SLOT(slotReportDesign()), actionCollection(), "report_design");


    // ------ Tools
    actionDefineWBS  = new KAction(KIcon( "tools_define_wbs" ), i18n("Define WBS Pattern..."), this);
    actionCollection()->addAction("tools_generate_wbs", actionDefineWBS );
    connect( actionDefineWBS, SIGNAL( triggered( bool ) ), SLOT( slotDefineWBS() ) );

    actionGenerateWBS  = new KAction(KIcon( "tools_generate_wbs" ), i18n("Generate WBS Code"), this);
    actionCollection()->addAction("tools_define_wbs", actionGenerateWBS );
    connect( actionGenerateWBS, SIGNAL( triggered( bool ) ), SLOT( slotGenerateWBS() ) );

    // ------ Export (testing)
    //actionExportGantt = new KAction(i18n("Export Ganttview"), "export_gantt", 0, this,
    //    SLOT(slotExportGantt()), actionCollection(), "export_gantt");

    // ------ Settings
    actionConfigure  = new KAction(KIcon( "configure" ), i18n("Configure KPlato..."), this);
    actionCollection()->addAction("configure", actionConfigure );
    connect( actionConfigure, SIGNAL( triggered( bool ) ), SLOT( slotConfigure() ) );

    // ------ Popup
    actionOpenNode  = new KAction(KIcon( "edit" ), i18n("Edit..."), this);
    actionCollection()->addAction("node_properties", actionOpenNode );
    connect( actionOpenNode, SIGNAL( triggered( bool ) ), SLOT( slotOpenNode() ) );
    actionTaskProgress  = new KAction(KIcon( "edit" ), i18n("Progress..."), this);
    actionCollection()->addAction("task_progress", actionTaskProgress );
    connect( actionTaskProgress, SIGNAL( triggered( bool ) ), SLOT( slotTaskProgress() ) );
    actionDeleteTask  = new KAction(KIcon( "edit-delete" ), i18n("Delete Task"), this);
    actionCollection()->addAction("delete_task", actionDeleteTask );
    connect( actionDeleteTask, SIGNAL( triggered( bool ) ), SLOT( slotDeleteTask() ) );

    actionEditResource  = new KAction(KIcon( "edit" ), i18n("Edit Resource..."), this);
    actionCollection()->addAction("edit_resource", actionEditResource );
    connect( actionEditResource, SIGNAL( triggered( bool ) ), SLOT( slotEditResource() ) );

    actionEditCalendar  = new KAction(KIcon( "edit" ), i18n("Edit Calendar..."), this);
    actionCollection()->addAction("edit_calendar", actionEditCalendar );
    connect( actionEditCalendar, SIGNAL( triggered( bool ) ), SLOT( slotEditCalendar() ) );

    // Viewlist popup
    connect( m_viewlist, SIGNAL( createKofficeDocument( KoDocumentEntry& ) ), SLOT( slotCreateKofficeDocument( KoDocumentEntry& ) ) );

    // ------------------- Actions with a key binding and no GUI item
    // Temporary, till we get a menu entry
    actNoInformation  = new KAction(i18n("Toggle no information"), this);
    actionCollection()->addAction("show_noinformation", actNoInformation );
    connect( actNoInformation, SIGNAL( triggered( bool ) ), SLOT( slotViewGanttNoInformation() ) );
    actNoInformation->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_T ) );

#ifndef NDEBUG
    //new KAction("Print Debug", CTRL+Qt::SHIFT+Qt::Key_P, this, SLOT( slotPrintDebug()), actionCollection(), "print_debug");
    QAction *action  = new KAction("Print Debug", this);
    actionCollection()->addAction("print_debug", action );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( slotPrintSelectedDebug() ) );
    action->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_P ) );
    action  = new KAction("Print Calendar Debug", this);
    actionCollection()->addAction("print_calendar_debug", action );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( slotPrintCalendarDebug() ) );
    action->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_C ) );
    //     new KAction("Print Test Debug", CTRL+Qt::SHIFT+Qt::Key_T, this, SLOT(slotPrintTestDebug()), actionCollection(), "print_test_debug");

    QAction *actExportGantt  = new KAction(i18n("Export Gantt"), this);
    actionCollection()->addAction("export_gantt", actExportGantt );
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
    m_estlabel = new QLabel( "", 0 );
    if ( statusBar() ) {
        addStatusBarItem( m_estlabel, 0, true );
        //m_progress = new QProgressBar();
        //addStatusBarItem( m_progress, 0, true );
        //m_progress->hide();
    }
    connect( &getProject(), SIGNAL( scheduleChanged( MainSchedule* ) ), SLOT( slotScheduleChanged( MainSchedule* ) ) );

    connect( &getProject(), SIGNAL( scheduleAdded( const MainSchedule* ) ), SLOT( slotScheduleAdded( const MainSchedule* ) ) );
    connect( &getProject(), SIGNAL( scheduleRemoved( const MainSchedule* ) ), SLOT( slotScheduleRemoved( const MainSchedule* ) ) );
    slotPlugScheduleActions();

    m_viewlist->setSelected( m_viewlist->findItem( "TaskEditor" ) );
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

void View::createResourceditor( ViewListItem *cat )
{
    ResourceEditor *resourceeditor = new ResourceEditor( getPart(), m_tab );
    m_tab->addWidget( resourceeditor );
    resourceeditor->draw( getProject() );

    ViewListItem *i = m_viewlist->addView( cat, "ResourceEditor", i18n( "Resources" ), resourceeditor, getPart(), "resource_editor" );
    i->setToolTip( 0, i18n( "Edit resource breakdown structure." ) );

    connect( resourceeditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( resourceeditor, SIGNAL( addResource( ResourceGroup* ) ), SLOT( slotAddResource( ResourceGroup* ) ) );
    connect( resourceeditor, SIGNAL( deleteObjectList( QObjectList ) ), SLOT( slotDeleteResourceObjects( QObjectList ) ) );

    connect( resourceeditor, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

}

void View::createTaskeditor( ViewListItem *cat )
{
    TaskEditor *taskeditor = new TaskEditor( getPart(), m_tab );
    m_tab->addWidget( taskeditor );

    ViewListItem *i = m_viewlist->addView( cat, "taskeditor", i18n( "Tasks" ), taskeditor, getPart(), "task_editor" );
    i->setToolTip( 0, i18n( "Edit work breakdown structure" ) );

    taskeditor->draw( getProject() );

    connect( taskeditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( taskeditor, SIGNAL( addTask() ), SLOT( slotAddTask() ) );
    connect( taskeditor, SIGNAL( addMilestone() ), SLOT( slotAddMilestone() ) );
    connect( taskeditor, SIGNAL( addSubtask() ), SLOT( slotAddSubTask() ) );
    connect( taskeditor, SIGNAL( deleteTaskList( QList<Node*> ) ), SLOT( slotDeleteTask( QList<Node*> ) ) );
    connect( taskeditor, SIGNAL( moveTaskUp() ), SLOT( slotMoveTaskUp() ) );
    connect( taskeditor, SIGNAL( moveTaskDown() ), SLOT( slotMoveTaskDown() ) );
    connect( taskeditor, SIGNAL( indentTask() ), SLOT( slotIndentTask() ) );
    connect( taskeditor, SIGNAL( unindentTask() ), SLOT( slotUnindentTask() ) );


    connect( taskeditor, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

}

void View::createAccountsEditor( ViewListItem *cat )
{
    AccountsEditor *ae = new AccountsEditor( getPart(), m_tab );
    m_tab->addWidget( ae );

    ViewListItem *i = m_viewlist->addView( cat, "AccountEditor", i18n( "Accounts" ), ae, getPart(), "accounts_editor" );
    i->setToolTip( 0, i18n( "Edit cost breakdown structure." ) );

    ae->draw( getProject() );

    connect( ae, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

}

void View::createCalendarEditor( ViewListItem *cat )
{
    CalendarEditor *calendareditor = new CalendarEditor( getPart(), m_tab );
    m_tab->addWidget( calendareditor );

    ViewListItem *i = m_viewlist->addView( cat, "CalendarEditor", i18n( "Work & Vacation" ), calendareditor, getPart(), "calendar_editor" );
    i->setToolTip( 0, i18n( "Edit working- and vacation days for resources" ) );

    calendareditor->draw( getProject() );

    connect( calendareditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( calendareditor, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

}

void View::createScheduleEditor( ViewListItem *cat )
{
    ScheduleEditor *scheduleeditor = new ScheduleEditor( getPart(), m_tab );
    m_tab->addWidget( scheduleeditor );

    ViewListItem *i = m_viewlist->addView( cat, "ScheduleEditor", i18n( "Schedules" ), scheduleeditor, getPart(), "schedule_editor" );
    i->setToolTip( 0, i18n( "Calculate project schedules" ) );

    scheduleeditor->draw( getProject() );

    connect( scheduleeditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( scheduleeditor, SIGNAL( addScheduleManager( Project* ) ), SLOT( slotAddScheduleManager( Project* ) ) );
    connect( scheduleeditor, SIGNAL( deleteScheduleManager( Project*, ScheduleManager* ) ), SLOT( slotDeleteScheduleManager( Project*, ScheduleManager* ) ) );

    connect( scheduleeditor, SIGNAL( calculateSchedule( Project*, ScheduleManager* ) ), SLOT( slotCalculateSchedule( Project*, ScheduleManager* ) ) );

}

void View::createDependencyEditor( ViewListItem *cat )
{
    PertEditor *perteditor = new PertEditor( getPart(), m_tab );
    m_tab->addWidget( perteditor );

    ViewListItem *i = m_viewlist->addView( cat, "PertEditor", i18n( "Pert" ), perteditor, getPart(), "task_editor" );
    i->setToolTip( 0, i18n( "Edit task dependenies" ) );

    perteditor->draw( getProject() );

    connect( perteditor, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );
    m_updatePertEditor = true;

}
void View::createPertResultView( ViewListItem *cat )
{
    PertResult *pertresult = new PertResult( getPart(), m_tab );
    m_tab->addWidget( pertresult );

    ViewListItem *i = m_viewlist->addView( cat, "PertResultView", i18n( "Pert Result" ), pertresult , getPart(), "pert_result" );
    i->setToolTip( 0, i18n( "PERT/CPM analizes" ) );

    pertresult->draw( getProject() );

    connect( pertresult, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );
}

void View::createTaskStatusView( ViewListItem *cat )
{
    TaskStatusView *taskstatusview = new TaskStatusView( getPart(), m_tab );
    m_tab->addWidget( taskstatusview );

    ViewListItem *i = m_viewlist->addView( cat, "TaskStatusView", i18n( "Task Status" ), taskstatusview, getPart(), "status_view" );
    i->setToolTip( 0, i18n( "View task progress information" ) );

    connect( taskstatusview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( taskstatusview, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

}

void View::createGanttView( ViewListItem *cat )
{
    GanttView *ganttview = new GanttView( getPart(), m_tab, getPart()->isReadWrite() );
    m_tab->addWidget( ganttview );
    m_updateGanttview = false;

    ViewListItem *i = m_viewlist->addView( cat, "GanttView", i18n( "Gantt" ), ganttview, getPart(), "gantt_chart" );
    i->setToolTip( 0, i18n( "View gantt chart" ) );

    ganttview->draw( getProject() );

    connect( ganttview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( ganttview, SIGNAL( enableActions( bool ) ), SLOT( setTaskActionsEnabled( bool ) ) );
    connect( ganttview, SIGNAL( addRelation( Node*, Node*, int ) ), SLOT( slotAddRelation( Node*, Node*, int ) ) );
    connect( ganttview, SIGNAL( modifyRelation( Relation*, int ) ), SLOT( slotModifyRelation( Relation*, int ) ) );
    connect( ganttview, SIGNAL( modifyRelation( Relation* ) ), SLOT( slotModifyRelation( Relation* ) ) );
    connect( ganttview, SIGNAL( itemDoubleClicked() ), SLOT( slotOpenNode() ) );
    connect( ganttview, SIGNAL( itemRenamed( Node*, const QString& ) ), this, SLOT( slotRenameNode( Node*, const QString& ) ) );
    connect( ganttview, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );


}

void View::createResourceView( ViewListItem *cat )
{
    ResourceView *resourceview = new ResourceView( getPart(), m_tab );
    m_updateResourceview = true;
    m_tab->addWidget( resourceview );

    ViewListItem *i = m_viewlist->addView( cat, "ResourceView", i18n( "Resources" ), resourceview, getPart(), "resources" );
    i->setToolTip( 0, i18n( "View resource information" ) );

    connect( resourceview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( resourceview, SIGNAL( itemDoubleClicked() ), SLOT( slotEditResource() ) );
    connect( resourceview, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

}

void View::createAccountsView( ViewListItem *cat )
{
    AccountsView *accountsview = new AccountsView( &getProject(), getPart(), m_tab );
    m_updateAccountsview = true;
    m_tab->addWidget( accountsview );

    ViewListItem *i = m_viewlist->addView( cat, "AccountsView", i18n( "Accounts" ), accountsview, getPart(), "accounts" );
    i->setToolTip( 0, i18n( "View planned cost" ) );

    connect( accountsview, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

}

void View::createResourceAssignmentView( ViewListItem *cat )
{
    ResourceAssignmentView *resourceAssignmentView = new ResourceAssignmentView( getPart(), m_tab );
    m_tab->addWidget( resourceAssignmentView );
    m_updateResourceAssignmentView = true;

    ViewListItem *i = m_viewlist->addView( cat, "ResourceAssignmentView", i18n( "Tasks by resources" ), resourceAssignmentView, getPart(), "resource_assignment" );
    i->setToolTip( 0, i18n( "View task status per resource" ) );

    resourceAssignmentView->draw( getProject() );

    connect( resourceAssignmentView, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( resourceAssignmentView, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

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
    //TODO
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
/* TODO
    if ( m_tab->currentWidget() == ganttview ) {
        ganttview->print( printer );
    } else if ( m_tab->currentWidget() == m_resourceview ) {
        m_resourceview->print( printer );
    } else if ( m_tab->currentWidget() == m_accountsview ) {
        m_accountsview->print( printer );
    }*/
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
    GanttView *v = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( v == 0 ) {
        return;
    }
    v->setShowResources( actionViewGanttResources->isChecked() );
    if ( m_tab->currentWidget() == v )
        slotUpdate();
}

void View::slotViewGanttTaskName()
{
    //kDebug()<<k_funcinfo<<endl;
    GanttView *v = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( v == 0 ) {
        return;
    }
    v->setShowTaskName( actionViewGanttTaskName->isChecked() );
    if ( m_tab->currentWidget() == v )
        slotUpdate();
}

void View::slotViewGanttTaskLinks()
{
    //kDebug()<<k_funcinfo<<endl;
    GanttView *v = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( v == 0 ) {
        return;
    }
    v->setShowTaskLinks( actionViewGanttTaskLinks->isChecked() );
    if ( m_tab->currentWidget() == v )
        slotUpdate();
}

void View::slotViewGanttProgress()
{
    //kDebug()<<k_funcinfo<<endl;
    GanttView *v = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( v == 0 ) {
        return;
    }
    v->setShowProgress( actionViewGanttProgress->isChecked() );
    if ( m_tab->currentWidget() == v )
        slotUpdate();
}

void View::slotViewGanttFloat()
{
    //kDebug()<<k_funcinfo<<endl;
    GanttView *v = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( v == 0 ) {
        return;
    }
    v->setShowPositiveFloat( actionViewGanttFloat->isChecked() );
    if ( m_tab->currentWidget() == v )
        slotUpdate();
}

void View::slotViewGanttCriticalTasks()
{
    //kDebug()<<k_funcinfo<<endl;
    GanttView *v = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( v == 0 ) {
        return;
    }
    v->setShowCriticalTasks( actionViewGanttCriticalTasks->isChecked() );
    if ( m_tab->currentWidget() == v )
        slotUpdate();
}

void View::slotViewGanttCriticalPath()
{
    //kDebug()<<k_funcinfo<<endl;
    GanttView *v = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( v == 0 ) {
        return;
    }
    v->setShowCriticalPath( actionViewGanttCriticalPath->isChecked() );
    if ( m_tab->currentWidget() == v )
        slotUpdate();
}

void View::slotViewGanttNoInformation()
{
    //kDebug() << k_funcinfo << ganttview->showNoInformation() << endl;
    GanttView *v = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( v == 0 ) {
        return;
    }
    v->setShowNoInformation( !v->showNoInformation() ); //Toggle
    if ( m_tab->currentWidget() == v )
        slotUpdate();
}

void View::slotViewTaskAppointments()
{
    //kDebug()<<k_funcinfo<<endl;
    GanttView *v = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( v == 0 ) {
        return;
    }
    v->setShowAppointments( actionViewTaskAppointments->isChecked() );
    m_updateGanttview = true;
    if ( m_tab->currentWidget() == v )
        slotUpdate();
}

void View::slotViewSelector( bool show )
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setVisible( show );
}

void View::slotViewGantt()
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( "Ganttview" ) );
}

void View::slotViewResources()
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( "ResourceView" ) );
}

void View::slotViewResourceAppointments()
{
    //kDebug()<<k_funcinfo<<endl;

}

void View::slotViewAccounts()
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( "AccountsView" ) );
}

void View::slotViewTaskEditor()
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( "TaskEditor" ) );
}

void View::slotViewCalendarEditor()
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( "CalendarEditor" ) );
}

void View::slotViewTaskStatusView()
{
    //kDebug()<<k_funcinfo<<endl;
    m_viewlist->setSelected( m_viewlist->findItem( "TaskStatusView" ) );
}


void View::slotProjectEdit()
{
    MainProjectDialog * dia = new MainProjectDialog( getProject() );
    if ( dia->exec()  == QDialog::Accepted) {
        K3Command * cmd = dia->buildCommand( getPart() );
        if ( cmd ) {
            getPart() ->addCommand( cmd );
        }
    }
    delete dia;
}

void View::slotEditCalendar()
{
    slotEditCalendar( currentCalendar() );
}

void View::slotEditCalendar( Calendar *calendar )
{
    if ( calendar == 0 ) {
        return;
    }
    CalendarEditDialog * dia = new CalendarEditDialog( getProject(), calendar );
    if ( dia->exec()  == QDialog::Accepted) {
        K3Command * cmd = dia->buildCommand( getPart() );
        if ( cmd ) {
            //kDebug()<<k_funcinfo<<"Modifying calendar"<<endl;
            getPart() ->addCommand( cmd ); //also executes
        }
    }
    delete dia;
}

void View::slotProjectCalendar()
{
    CalendarListDialog * dia = new CalendarListDialog( getProject() );
    if ( dia->exec()  == QDialog::Accepted) {
        K3Command * cmd = dia->buildCommand( getPart() );
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
        K3Command * cmd = dia->buildCommand( getPart() );
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
        K3Command * cmd = dia->buildCommand( getPart() );
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
        K3Command * cmd = dia->buildCommand( getPart() );
        if ( cmd ) {
            //kDebug()<<k_funcinfo<<"Modifying resources"<<endl;
            getPart() ->addCommand( cmd ); //also executes
        }
    }
    delete dia;
}

void View::slotScheduleRemoved( const MainSchedule *sch )
{
    QAction *a = 0;
    QAction *checked = m_scheduleActionGroup->checkedAction();
    QMapIterator<QAction*, Schedule*> i( m_scheduleActions );
    while (i.hasNext()) {
        i.next();
        if ( i.value() == sch ) {
            a = i.key();
            break;
        }
    }
    if ( a ) {
        unplugActionList( "view_schedule_list" );
        delete a;
        plugActionList( "view_schedule_list", m_scheduleActions.keys() );
        if ( checked && checked != a ) {
            checked->setChecked( true );
        } else if ( ! m_scheduleActions.isEmpty() ) {
            m_scheduleActions.keys().first()->setChecked( true );
        }
    }
    slotViewSchedule( m_scheduleActionGroup->checkedAction() );
}

void View::slotScheduleAdded( const MainSchedule *sch )
{
    if ( sch->type() != Schedule::Expected ) {
        return; // Only view expected
    }
    MainSchedule *s = const_cast<MainSchedule*>( sch ); // FIXME
    //kDebug()<<k_funcinfo<<sch->name()<<" deleted="<<sch->isDeleted()<<endl;
    QAction *checked = m_scheduleActionGroup->checkedAction();
    if ( ! sch->isDeleted() && sch->isScheduled() ) {
        unplugActionList( "view_schedule_list" );
        QAction *act = addScheduleAction( s );
        plugActionList( "view_schedule_list", m_scheduleActions.keys() );
        if ( checked ) {
            checked->setChecked( true );
        } else if ( act ) {
            act->setChecked( true );
        } else if ( ! m_scheduleActions.isEmpty() ) {
            m_scheduleActions.keys().first()->setChecked( true );
        }
    }
    slotViewSchedule( m_scheduleActionGroup->checkedAction() );
}

void View::slotScheduleChanged( MainSchedule *sch )
{
    //kDebug()<<k_funcinfo<<sch->name()<<" deleted="<<sch->isDeleted()<<endl;
    if ( sch->isDeleted() || ! sch->isScheduled() ) {
        slotScheduleRemoved( sch );
        return;
    }
    if ( m_scheduleActions.values().contains( sch ) ) {
        slotScheduleRemoved( sch ); // hmmm, how to avoid this?
    }
    slotScheduleAdded( sch );
}

QAction *View::addScheduleAction( Schedule *sch )
{
    QAction *act = 0;
    if ( ! sch->isDeleted() ) {
        QString n = sch->name();
        QAction *act = new KToggleAction( n, this);
        actionCollection()->addAction(n, act );
        m_scheduleActions.insert( act, sch );
        m_scheduleActionGroup->addAction( act );
        //kDebug()<<k_funcinfo<<"Add: "<<n<<endl;
        connect( act, SIGNAL(destroyed( QObject* ) ), SLOT( slotActionDestroyed( QObject* ) ) );
    }
    return act;
}

void View::slotViewSchedule( QAction *act )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( act != 0 ) {
        Schedule *sch = m_scheduleActions.value( act, 0 );
        if ( sch->id() != getProject().currentViewScheduleId() ) {
            getProject().setCurrentViewScheduleId( sch->id() );
            //kDebug()<<k_funcinfo<<sch->id()<<endl;
        }
    }
    setLabel();
}

void View::slotActionDestroyed( QObject *o )
{
    //kDebug()<<k_funcinfo<<o->name()<<endl;
    m_scheduleActions.remove( static_cast<QAction*>( o ) );
    slotViewSchedule( m_scheduleActionGroup->checkedAction() );
}

void View::slotPlugScheduleActions()
{
    //kDebug()<<k_funcinfo<<endl;
    unplugActionList( "view_schedule_list" );
    foreach( QAction *act, m_scheduleActions.keys() ) {
        m_scheduleActionGroup->removeAction( act );
        delete act;
    }
    m_scheduleActions.clear();
    Schedule *cs = getProject().currentSchedule();
    QAction *ca = 0;
    foreach( ScheduleManager *sm, getProject().allScheduleManagers() ) {
        Schedule *sch = sm->expected();
        if ( sch == 0 ) {
            continue;
        }
        QAction *act = addScheduleAction( sch );
        if ( act ) {
            if ( ca == 0 && cs == sch ) {
                ca = act;
            }
        }
    }
    plugActionList( "view_schedule_list", m_scheduleActions.keys() );
    if ( ca == 0 && m_scheduleActionGroup->actions().count() > 0 ) {
        ca = m_scheduleActionGroup->actions().first();
    }
    if ( ca ) {
        ca->setChecked( true );
    }
    slotViewSchedule( ca );
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
    slotUpdate();
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
            K3Command * m = dia->buildCommand( getPart() );
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
            K3Command * m = dia->buildCommand( getPart() );
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
            K3Command * m = dia->buildCommand( getPart() );
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


void View::slotChartDisplay()
{
    ChartDialog * dia = new ChartDialog(getProject());
    if ( dia->exec()  == QDialog::Accepted) {
       // K3Command * cmd = dia->buildCommand( getPart() );
      //  if ( cmd ) {
        //    getPart() ->addCommand( cmd );
       // }
    }
    delete dia;
}

Calendar *View::currentCalendar()
{
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v == 0 ) {
        return 0;
    }
    return v->currentCalendar();
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
    slotOpenNode( node );
}

void View::slotOpenNode( Node *node )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( !node )
        return ;

    switch ( node->type() ) {
        case Node::Type_Project: {
                Project * project = dynamic_cast<Project *>( node );
                MainProjectDialog *dia = new MainProjectDialog( *project );
                if ( dia->exec()  == QDialog::Accepted) {
                    K3Command * m = dia->buildCommand( getPart() );
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
                    K3Command * m = dia->buildCommand( getPart() );
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
                    K3Command * m = dia->buildCommand( getPart() );
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
                    K3Command * m = dia->buildCommand( getPart() );
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
                    K3Command * m = dia->buildCommand( getPart() );
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
                    K3Command * m = dia->buildCommand( getPart() );
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
    K3MacroCommand *cmd = new K3MacroCommand( i18n( "Delete Tasks" ) );
    while ( !lst.isEmpty() ) {
        Node *node = lst.takeFirst();
        if ( node == 0 || node->parentNode() == 0 ) {
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
    if ( node == 0 || node->parentNode() == 0 ) {
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
    if ( node == 0 || node->parentNode() == 0 ) {
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
    if ( node == 0 || node->parentNode() == 0 ) {
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
        K3Command * cmd = dia->buildCommand( getPart() );
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
            K3Command *cmd = dia->buildCommand( getPart() );
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
    GanttView *gv = dynamic_cast<GanttView*>( m_viewlist->findView( "Ganttview" ) );
    if ( gv == 0 ) {
        return ;
    }
    QString fn = KFileDialog::getSaveFileName( KUrl(), QString(), this );
    if ( !fn.isEmpty() ) {
        QFile f( fn );
        gv->exportGantt( &f );
    }
}

void View::slotAddResource( ResourceGroup *group )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( group == 0 ) {
        return;
    }
    Resource *r = new Resource();
    ResourceDialog *dia = new ResourceDialog( getProject(), r );
    if ( dia->exec()  == QDialog::Accepted) {
        K3MacroCommand *m = new K3MacroCommand( i18n( "Add resource" ) );
        m->addCommand( new AddResourceCmd( getPart(), group, r ) );
        K3Command * cmd = dia->buildCommand( getPart() );
        if ( cmd ) {
            m->addCommand( cmd );
        }
        getPart()->addCommand( m );
        delete dia;
        return;
    }
    delete r;
    delete dia;
}

void View::slotEditResource()
{
    //kDebug()<<k_funcinfo<<endl;
    Resource * r = currentResource();
    if ( r == 0 ) {
        return ;
    }
    ResourceDialog *dia = new ResourceDialog( getProject(), r );
    if ( dia->exec()  == QDialog::Accepted) {
        K3Command * cmd = dia->buildCommand( getPart() );
        if ( cmd )
            getPart() ->addCommand( cmd );
    }
    delete dia;
}

void View::slotDeleteResource( Resource *resource )
{
    getPart()->addCommand( new RemoveResourceCmd( getPart(), resource->parentGroup(), resource, i18n( "Delete Resource" ) ) );
}

void View::slotDeleteResourceGroup( ResourceGroup *group )
{
    getPart()->addCommand( new RemoveResourceGroupCmd( getPart(), group->project(), group, i18n( "Delete Resourcegroup" ) ) );
}

void View::slotDeleteResourceObjects( QObjectList lst )
{
    //kDebug()<<k_funcinfo<<endl;
    if ( lst.count() == 1 ) {
        Resource *r = qobject_cast<Resource*>( lst.first() );
        if ( r ) {
            slotDeleteResource( r );
        } else {
            ResourceGroup *g = qobject_cast<ResourceGroup*>( lst.first() );
            if ( g ) {
                slotDeleteResourceGroup( g );
            }
        }
        return;
    }
    int num = 0;
    K3MacroCommand *cmd = 0, *rc = 0, *gc = 0;
    foreach ( QObject *o, lst ) {
        Resource *r = qobject_cast<Resource*>( o );
        if ( r ) {
            if ( rc == 0 )  rc = new K3MacroCommand( "" );
            rc->addCommand( new RemoveResourceCmd( getPart(), r->parentGroup(), r ) );
            continue;
        }
        ResourceGroup *g = qobject_cast<ResourceGroup*>( o );
        if ( g ) {
            if ( gc == 0 )  gc = new K3MacroCommand( "" );
            gc->addCommand( new RemoveResourceGroupCmd( getPart(), g->project(), g ) );
        }
    }
    if ( rc || gc ) {
        cmd = new K3MacroCommand( i18n( "Delete Resource Objects" ) );
    }
    if ( rc )
        cmd->addCommand( rc );
    if ( gc )
        cmd->addCommand( gc );
    if ( cmd )
        getPart()->addCommand( cmd );
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
    /*    NodeItem *curr = ganttview->currentItem();
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
    m_updateResourceAssignmentView = true;
    m_updatePertEditor = true;
    m_updatePertResult = true;
    updateView( m_tab->currentWidget() );
}

void View::slotGuiActivated( ViewBase *view, bool activate )
{
    if ( activate ) {
        foreach( QString name, view->actionListNames() ) {
            //kDebug()<<k_funcinfo<<"activate "<<name<<", "<<view->actionList( name ).count()<<endl;
            plugActionList( name, view->actionList( name ) );
        }
    } else {
        foreach( QString name, view->actionListNames() ) {
            //kDebug()<<k_funcinfo<<"deactivate "<<name<<endl;
            unplugActionList( name );
        }
    }
}

void View::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    //kDebug()<<k_funcinfo<<ev->activated()<<endl;
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

void View::createChildDocumentViews()
{
    QTreeWidgetItem *cat = m_viewlist->addCategory( "Documents", i18n( "Documents" ) );
    cat->setIcon( 0, KIcon( "koshell" ) );

    foreach ( KoDocumentChild *ch, getPart()->children() ) {
        if ( ! ch->isDeleted() ) {
            ViewListItem *i = createChildDocumentView( static_cast<DocumentChild*>( ch ) );
            cat->insertChild( cat->childCount(), i );
        }
    }
}

ViewListItem *View::createChildDocumentView( DocumentChild *ch )
{
    KoDocument *doc = ch->document();


    QString title = ch->title();
    if ( title.isEmpty() && doc->documentInfo() ) {
        title = doc->documentInfo()->aboutInfo( "title" );
    }
    if ( title.isEmpty() ) {
        title = doc->url().pathOrUrl();
    }
    if ( title.isEmpty() ) {
        title = "Untitled";
    }
    KoView *v = doc->createView( this );
    ch->setGeometry( geometry(), true );
    m_tab->addWidget( v );
    ViewListItem *i = m_viewlist->createView( doc->objectName(), title, v, ch );
    if ( ! ch->icon().isEmpty() ) {
        i->setIcon( 0, KIcon( ch->icon() ) );
    }
    return i;
}

void View::slotViewListItemRemoved( ViewListItem *item )
{
    // atm. it's only child docs that can be removed, so this restores basic ui
    if ( item->documentChild() ) {
        item->documentChild()->setActivated( false, this );
    }
    m_tab->removeWidget( item->view() );
}

void View::slotViewListItemInserted( ViewListItem *item )
{
    m_tab->addWidget( item->view() );
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
    QTreeWidgetItem *cat = m_viewlist->addCategory( "Documents", i18n( "Documents" ) );
    cat->setIcon( 0, KIcon( "koshell" ) );

    DocumentChild *ch = getPart()->createChild( doc );
    ch->setIcon( entry.service()->icon() );
    ViewListItem *i = createChildDocumentView( ch );
    getPart()->addCommand( new InsertEmbeddedDocumentCmd( getPart(), m_viewlist, i, cat, i18n( "Insert Document" ) ) );
    m_viewlist->setSelected( i );
}

void View::slotViewActivated( ViewListItem *item, ViewListItem *prev )
{
    //kDebug() << k_funcinfo << "item=" << item << ", "<<prev<<endl;
    if ( prev && prev->type() != ViewListItem::ItemType_ChildDocument ) {
        // Remove sub-view specific gui
        //kDebug()<<k_funcinfo<<"Deactivate: "<<prev<<endl;
        ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
        if ( v ) {
            v->setGuiActive( false );
        }
    }
    if ( item->type() == ViewListItem::ItemType_SubView ) {
        //kDebug()<<k_funcinfo<<"Activate: "<<item<<endl;
        m_tab->setCurrentWidget( item->view() );
        if (  prev && prev->type() != ViewListItem::ItemType_SubView ) {
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
    if ( item->type() == ViewListItem::ItemType_ChildDocument ) {
        //kDebug()<<k_funcinfo<<"Activated: "<<item->view()<<endl;
        // changing doc also takes care of all gui
        m_tab->setCurrentWidget( item->view() );
        item->documentChild()->setActivated( true, item->view() );
        return;
    }
}

QWidget *View::canvas() const
{
    return m_tab->currentWidget();//KoView::canvas();
}

void View::slotCurrentChanged( int index )
{
    kDebug()<<k_funcinfo<<m_tab->currentIndex()<<endl;
    ViewListItem *item = m_viewlist->findItem( m_tab->currentWidget() );
    if ( item == 0 ) {
        return;
    }
    kDebug()<<k_funcinfo<<item->text(0)<<endl;
    item->setSelected( true );
}

void View::updateView( QWidget *widget )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    //setScheduleActionsEnabled();

    mainWindow() ->toolBar( "report" ) ->hide();
    QWidget *widget2;

    widget2 = m_viewlist->findView( "GanttView" ) ;
    if ( m_updateGanttview )
	static_cast<ViewBase*>( widget2 ) ->drawChanges( getProject() );
        //setTaskActionsEnabled( widget2, true );
    m_updateGanttview = false;

    widget2 = m_viewlist->findView( "ResourceView" ) ;
    if ( m_updateResourceview )
        static_cast<ViewBase*>( widget2 ) ->draw( getPart() ->getProject() );
    m_updateResourceview = false;

    widget2 = m_viewlist->findView( "AccountsView" );
    if ( m_updateAccountsview )
        static_cast<ViewBase*>( widget2 ) ->draw();
    m_updateAccountsview = false;

    widget2 = m_viewlist->findView( "ResourceAssignmentView" );
    if ( m_updateResourceAssignmentView )
        static_cast<ViewBase*>( widget2 ) ->draw( getProject() );
    m_updateResourceAssignmentView = false;

    widget2 = m_viewlist->findView( "PertEditor" );
        static_cast<ViewBase*>( widget2 ) -> draw( getProject() );


    widget2 = m_viewlist->findView( "PertResultView" ) ;
        static_cast<ViewBase*>( widget2 ) -> draw( getProject() );

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
    if ( menu ) {
        //kDebug()<<k_funcinfo<<menu<<": "<<menu->actions().count()<<endl;
        menu->exec( pos );
    }
}

void View::slotPopupMenu( const QString& menuname, const QPoint &pos, ViewListItem *item )
{
    //kDebug()<<k_funcinfo<<menuname<<endl;
    m_viewlistItem = item;
    slotPopupMenu( menuname, pos );
}

bool View::setContext( const Context &context )
{
    //kDebug()<<k_funcinfo<<endl;
    m_currentEstimateType = context.currentEstimateType;

    getProject().setCurrentViewScheduleId( context.currentSchedule );

    // set context for each subview
    for ( int i = 0; i < m_tab->count(); ++i ) {
        ViewBase *v = dynamic_cast<ViewBase*>( m_tab->widget( i ) );
        if ( v ) {
            v->setContext( context );
        }
    }
    // hmmm, can't decide if these should be here or actions moved to ganttview
    actionViewGanttResources->setChecked( context.ganttview.showResources );
    actionViewGanttTaskName->setChecked( context.ganttview.showTaskName );
    actionViewGanttTaskLinks->setChecked( context.ganttview.showTaskLinks );
    actionViewGanttProgress->setChecked( context.ganttview.showProgress );
    actionViewGanttFloat->setChecked( context.ganttview.showPositiveFloat );
    actionViewGanttCriticalTasks->setChecked( context.ganttview.showCriticalTasks );
    actionViewGanttCriticalPath->setChecked( context.ganttview.showCriticalPath );

    /* TODO
    m_resourceview->setContext( context.resourceview );
    m_accountsview->setContext( context.accountsview );*/

    m_viewlist->setSelected( m_viewlist->findItem( context.currentView ) );
    slotUpdate();
    return true;
}

void View::getContext( Context &context ) const
{
    //kDebug()<<k_funcinfo<<endl;
    context.currentEstimateType = m_currentEstimateType;

    context.currentSchedule = getProject().currentViewScheduleId();
    ViewListItem *item = m_viewlist->findItem( m_tab->currentWidget() );
    if ( item ) {
        context.currentView = item->tag();
        kDebug()<<k_funcinfo<<"Context currentview: "<<context.currentView<<", "<<item->text( 0 )<<endl;
    }
    // get context for each subview
    for ( int i = 0; i < m_tab->count(); ++i ) {
        ViewBase *v = dynamic_cast<ViewBase*>( m_tab->widget( i ) );
        if ( v ) {
            v->getContext( context );
        }
    }
}

void View::setLabel()
{
    //kDebug()<<k_funcinfo<<endl;
    long id = getProject().currentViewScheduleId();
    Schedule *s = id == -1 ? getProject().currentSchedule() : getProject().findSchedule( id );
    if ( s && !s->isDeleted() && s->isScheduled() ) {
        m_estlabel->setText( s->name() );
        return;
    }
    m_estlabel->setText( i18n( "Not scheduled" ) );
}

#ifndef NDEBUG
void View::slotPrintDebug()
{
    kDebug() << "-------- Debug printout: Node list" << endl;
    /*    Node *curr = ganttview->currentNode();
        if (curr) {
            curr->printDebug(true,"");
        } else*/
    getPart() ->getProject().printDebug( true, "" );
}
void View::slotPrintSelectedDebug()
{
/*TODO
    if ( m_tab->currentWidget() == ganttview ) {
        Node * curr = ganttview->currentNode();
        if ( curr ) {
            kDebug() << "-------- Debug printout: Selected node" << endl;
            curr->printDebug( true, "" );
        } else
            slotPrintDebug();
        return;
    } else if ( m_tab->currentWidget() == m_viewlist->findView( "ResourceEditor" ) ) {
        Resource *r = static_cast<ViewBase*>( m_tab->currentWidget() )->currentResource();
        if ( r ) {
            kDebug() <<"-------- Debug printout: Selected resource"<<endl;
            r->printDebug("  !");
            return;
        }
        ResourceGroup *g = static_cast<ViewBase*>( m_tab->currentWidget() )->currentResourceGroup();
        if ( g ) {
            kDebug() <<"-------- Debug printout: Selected group"<<endl;
            g->printDebug("  !");
            return;
        }
    }
    slotPrintDebug();*/
}
void View::slotPrintCalendarDebug()
{
    //kDebug() << "-------- Debug printout: Calendars" << endl;
    /*    Node *curr = ganttview->currentNode();
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
    //     CalendarDay *day = new CalendarDay(QDate(2006,1,2), CalendarDay::Working);
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
    //     CalendarDay *day = new CalendarDay(QDate(2006,1,2), CalendarDay::Working);
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
    //     wd1->setState(CalendarDay::NonWorking);
    //
    //     CalendarDay *wd2 = p->weekday(2); // wednesday
    //     if (wd2 == 0) {
    //         kDebug()<<"Failed to get weekday"<<endl;
    //     }
    //     wd2->addInterval(TimeInterval(t1, t2));
    //     wd2->setState(CalendarDay::Working);
    //
    //     Calendar *t = new Calendar("Test 4");
    //     t->setParent(p);
    //     QDate wdate(2006,1,2); // monday jan 2
    //     DateTime before = DateTime(wdate.addDays(-4)); //Thursday dec 29
    //     DateTime after = DateTime(wdate.addDays(4)); // Friday jan 6
    //     DateTime wdt1(wdate, t1);
    //     DateTime wdt2(QDate(2006, 1, 4), t2); // Wednesday
    //     CalendarDay *day = new CalendarDay(QDate(2006,1,2), CalendarDay::Working);
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
