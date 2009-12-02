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

#include "kptviewlist.h"

#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QItemDelegate>
#include <QStyle>
#include <QHeaderView>
#include <QBrush>
#include <QContextMenuEvent>
#include <QTimer>

#include <kmenu.h>
#include <KMessageBox>
#include <KComboBox>

#include "KoDocument.h"

#include "kptviewbase.h"
#include "kptpart.h"
#include "kptviewlistdialog.h"
#include "kptviewlistdocker.h"
#include "kptschedulemodel.h"

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

        QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
        virtual void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    private:
        QTreeView *m_view;
};

QSize ViewCategoryDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const QAbstractItemModel * model = index.model();
    Q_ASSERT( model );
    if ( model->parent( index ).isValid() ) {
        return QItemDelegate::sizeHint( option, index );
    }
    return QItemDelegate::sizeHint( option, index ).expandedTo( QSize( 0, 16 ) );
}

void ViewCategoryDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const QAbstractItemModel * model = index.model();
    Q_ASSERT( model );

    if ( !model->parent( index ).isValid() ) {
        // this is a top-level item.
        QStyleOptionButton buttonOption;
        buttonOption.state = option.state;

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
        m_view->style() ->drawItemText( painter, textrect, Qt::AlignLeft|Qt::AlignVCenter,
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

void ViewListItem::setReadWrite( bool rw )
{
    if ( type() == ItemType_SubView ) {
        static_cast<ViewBase*>( view() )->updateReadWrite( rw );
    }
}

void ViewListItem::setView( ViewBase *view )
{
    setData( 0, ViewListItem::DataRole_View,  qVariantFromValue(static_cast<QObject*>( view ) ) );
}

ViewBase *ViewListItem::view() const
{
    if ( data(0, ViewListItem::DataRole_View ).isValid() ) {
        return static_cast<ViewBase*>( data(0, ViewListItem::DataRole_View ).value<QObject*>() );
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

QString ViewListItem::viewType() const
{
    if ( type() != ItemType_SubView ) {
        return QString();
    }
    QString name = view()->metaObject()->className();
    if ( name.contains( ':' ) ) {
        name = name.remove( 0, name.lastIndexOf( ':' ) + 1 );
    }
    return name;
}

void ViewListItem::save( QDomElement &element ) const
{
    element.setAttribute( "itemtype", type() );
    element.setAttribute( "tag", tag() );
    element.setAttribute( "name", m_viewinfo.name == text( 0 ) ? "" : text( 0 ) );
    element.setAttribute( "tooltip", m_viewinfo.tip == toolTip( 0 ) ? TIP_USE_DEFAULT_TEXT : toolTip( 0 ) );

    if ( type() == ItemType_SubView ) {
        element.setAttribute( "viewtype", viewType() );
    }
}

ViewListTreeWidget::ViewListTreeWidget( QWidget *parent )
    : QTreeWidget( parent )
{
    setWhatsThis( i18nc( "@info:whatsthis",
        "<para>This is the list of available views and editors.</para>"
        "<para>You can configure the list by using the context menu:"
        "<list>"
        "<item>Rename categories or views</item>"
        "<item>Configure. Move, remove, rename or edit tool tip for categories or views</item>"
        "<item>Insert categories and views</item>"
        "</list>"
    ) );

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
    //kDebug();
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
        QTreeWidgetItem *item = itemAt( event->pos() );
        if ( item && item->type() == ViewListItem::ItemType_Category ) {
            setCurrentItem( item );
            emit customContextMenuRequested( event->pos() );
            event->accept();
            return;
        }
    }
    QTreeWidget::mousePressEvent( event );
}

void ViewListTreeWidget::save( QDomElement &element ) const
{
    int cnt = topLevelItemCount();
    if ( cnt == 0 ) {
        return;
    }
    QDomElement cs = element.ownerDocument().createElement( "categories" );
    element.appendChild( cs );
    for ( int i = 0; i < cnt; ++i ) {
        ViewListItem *itm = static_cast<ViewListItem*>( topLevelItem( i ) );
        if ( itm->type() != ViewListItem::ItemType_Category ) {
            continue;
        }
        QDomElement c = cs.ownerDocument().createElement( "category" );
        cs.appendChild( c );
        emit const_cast<ViewListTreeWidget*>( this )->updateViewInfo( itm );
        itm->save( c );
        for ( int j = 0; j < itm->childCount(); ++j ) {
            ViewListItem *vi = static_cast<ViewListItem*>( itm->child( j ) );
            if ( vi->type() != ViewListItem::ItemType_SubView ) {
                continue;
            }
            QDomElement el = c.ownerDocument().createElement( "view" );
            c.appendChild( el );
            emit const_cast<ViewListTreeWidget*>( this )->updateViewInfo( vi );
            vi->save( el );
            QDomElement elm = el.ownerDocument().createElement( "settings" );
            el.appendChild( elm );
            static_cast<ViewBase*>( vi->view() )->saveContext( elm );
        }
    }
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

ViewListItem *ViewListTreeWidget::category( const KoView *view ) const
{
    QTreeWidgetItem * item;
    int cnt = topLevelItemCount();
    for ( int i = 0; i < cnt; ++i ) {
        item = topLevelItem( i );
        for ( int c = 0; c < item->childCount(); ++c ) {
            if ( view == static_cast<ViewListItem*>( item->child( c ) )->view() ) {
                return static_cast<ViewListItem*>( item );
            }
        }
    }
    return 0;
}

//-----------------------
ViewListWidget::ViewListWidget( Part *part, QWidget *parent )//QString name, KXmlGuiWindow *parent )
    : QWidget( parent ),
    m_part( part ),
    m_prev( 0 ),
    m_temp( 0 )
{
    setObjectName("ViewListWidget");
    m_viewlist = new ViewListTreeWidget( this );
    m_viewlist->setEditTriggers( QAbstractItemView::NoEditTriggers );

    m_currentSchedule = new KComboBox( this );
    m_model.setFlat( true );

    m_sfModel.setFilterKeyColumn ( ScheduleModel::ScheduleScheduled );
    m_sfModel.setFilterRole( Qt::EditRole );
    m_sfModel.setFilterFixedString( "true" );
    m_sfModel.setDynamicSortFilter ( true );
    m_sfModel.setSourceModel( &m_model );
    m_currentSchedule->setModel( &m_sfModel );
    
    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin( 0 );
    l->addWidget( m_viewlist );
    l->addWidget( m_currentSchedule );

    connect( m_viewlist, SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ), SLOT( slotActivated( QTreeWidgetItem*, QTreeWidgetItem* ) ) );

    connect( m_viewlist, SIGNAL( itemChanged( QTreeWidgetItem*, int ) ), SLOT( slotItemChanged( QTreeWidgetItem*, int ) ) );

    setupContextMenus();

    connect( m_currentSchedule, SIGNAL( activated( int ) ), SLOT( slotCurrentScheduleChanged( int ) ) );

    connect( &m_model, SIGNAL( scheduleManagerAdded( ScheduleManager* ) ), SLOT( slotScheduleManagerAdded( ScheduleManager* ) ) );

    connect( m_viewlist, SIGNAL( updateViewInfo( ViewListItem* ) ), SIGNAL( updateViewInfo( ViewListItem* ) ) );
}

ViewListWidget::~ViewListWidget()
{
}

void ViewListWidget::setReadWrite( bool rw )
{
    foreach ( ViewListItem *c, categories() ) {
        for ( int i = 0; i < c->childCount(); ++i ) {
            static_cast<ViewListItem*>( c->child( i ) )->setReadWrite( rw );
        }
    }
}

void ViewListWidget::slotItemChanged( QTreeWidgetItem *item, int col )
{
    //kDebug();
}

void ViewListWidget::slotActivated( QTreeWidgetItem *item, QTreeWidgetItem *prev )
{
    qDebug()<<"slotActivated:"<<item<<prev<<m_prev;
    if ( m_prev ) {
        m_prev->setData( 0, Qt::BackgroundRole, QVariant() );
    }
    if ( item && item->type() == ViewListItem::ItemType_Category ) {
        return ;
    }
    emit activated( static_cast<ViewListItem*>( item ), static_cast<ViewListItem*>( prev ) );
    if ( item ) {
        QVariant v = QBrush( QColor( Qt::yellow ) );
        item->setData( 0, Qt::BackgroundRole, v );
        m_prev = static_cast<ViewListItem*>( item );
    }
}

ViewListItem *ViewListWidget::addCategory( const QString &tag, const QString& name )
{
    //kDebug() ;
    ViewListItem *item = m_viewlist->findCategory( tag );
    if ( item == 0 ) {
        item = new ViewListItem( m_viewlist, tag, QStringList( name ), ViewListItem::ItemType_Category );
        item->setExpanded( true );
        item->setFlags( item->flags() | Qt::ItemIsEditable );
    }
    return item;
}

QList<ViewListItem*> ViewListWidget::categories() const
{
    QList<ViewListItem*> lst;
    QTreeWidgetItem *item;
    int cnt = m_viewlist->topLevelItemCount();
    for ( int i = 0; i < cnt; ++i ) {
        item = m_viewlist->topLevelItem( i );
        if ( item->type() == ViewListItem::ItemType_Category )
            lst << static_cast<ViewListItem*>( item );
    }
    return lst;
}

ViewListItem *ViewListWidget::findCategory( const QString &tag ) const
{
    return m_viewlist->findCategory( tag );
}

ViewListItem *ViewListWidget::category( const KoView *view ) const
{
    return m_viewlist->category( view );
}

QString ViewListWidget::uniqueTag( const QString &seed ) const
{
    QString tag = seed;
    for ( int i = 1; findItem( tag ); ++i ) {
        tag = QString("%1-%2").arg( seed ).arg( i );
    }
    return tag;
}

ViewListItem *ViewListWidget::addView( QTreeWidgetItem *category, const QString &tag, const QString& name, ViewBase *view, KoDocument *doc, const QString& icon, int index )
{
    ViewListItem * item = new ViewListItem( uniqueTag( tag ), QStringList( name ), ViewListItem::ItemType_SubView );
    item->setView( view );
    item->setDocument( doc );
    if ( !icon.isEmpty() ) {
        item->setData( 0, Qt::DecorationRole, KIcon( icon ) );
    }
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    category->insertChild( ( index == -1 ? category->childCount() : index ), item );
    //kDebug() << "added: " << item->tag();
    return item;
}

void ViewListWidget::setSelected( QTreeWidgetItem *item )
{
    //kDebug()<<item<<","<<m_viewlist->currentItem();
    if ( item == 0 && m_viewlist->currentItem() ) {
        m_viewlist->currentItem()->setSelected( false );
        if ( m_prev ) {
            m_prev->setData( 0, Qt::BackgroundRole, QVariant() );
        }
    }
    m_viewlist->setCurrentItem( item );
    //kDebug()<<item<<","<<m_viewlist->currentItem();
}

void ViewListWidget::setCurrentItem( QTreeWidgetItem *item )
{
    qDebug()<<"setCurrentItem:"<<item<<","<<m_viewlist->currentItem();
    m_viewlist->setCurrentItem( item );
    //kDebug()<<item<<","<<m_viewlist->currentItem();
}

ViewListItem *ViewListWidget::currentItem() const
{
    return static_cast<ViewListItem*>( m_viewlist->currentItem() );
}

ViewListItem *ViewListWidget::currentCategory() const
{
    ViewListItem *item = static_cast<ViewListItem*>( m_viewlist->currentItem() );
    if ( item == 0 ) {
        return 0;
    }
    if ( item->type() == ViewListItem::ItemType_Category ) {
        return item;
    }
    return static_cast<ViewListItem*>( item->parent() );
}

KoView *ViewListWidget::findView( const QString &tag ) const
{
    ViewListItem *i = findItem( tag );
    if ( i == 0 ) {
        return 0;
    }
    return i->view();
}

ViewListItem *ViewListWidget::findItem( const QString &tag ) const
{
    ViewListItem *item = findItem( tag, m_viewlist->invisibleRootItem() );
    if ( item == 0 ) {
        QTreeWidgetItem *parent = m_viewlist->invisibleRootItem();
        for (int i = 0; i < parent->childCount(); ++i ) {
            item = findItem( tag, parent->child( i ) );
            if ( item != 0 ) {
                break;
            }
        }
    }
    return item;
}

ViewListItem *ViewListWidget::findItem( const QString &tag, QTreeWidgetItem *parent ) const
{
    if ( parent == 0 ) {
        return findItem( tag, m_viewlist->invisibleRootItem() );
    }
    for (int i = 0; i < parent->childCount(); ++i ) {
        ViewListItem * ch = static_cast<ViewListItem*>( parent->child( i ) );
        if ( ch->tag() == tag ) {
            //kDebug()<<ch<<","<<view;
            return ch;
        }
        ch = findItem( tag, ch );
        if ( ch ) {
            return ch;
        }
    }
    return 0;
}

ViewListItem *ViewListWidget::findItem(  const ViewBase *view, QTreeWidgetItem *parent ) const
{
    if ( parent == 0 ) {
        return findItem( view, m_viewlist->invisibleRootItem() );
    }
    for (int i = 0; i < parent->childCount(); ++i ) {
        ViewListItem * ch = static_cast<ViewListItem*>( parent->child( i ) );
        if ( ch->view() == view ) {
            //kDebug()<<ch<<","<<view;
            return ch;
        }
        ch = findItem( view, ch );
        if ( ch ) {
            return ch;
        }
    }
    return 0;
}

void ViewListWidget::slotAddView()
{
    emit createView();
}

void ViewListWidget::slotRemoveCategory()
{
    if ( m_contextitem == 0 ) {
        return;
    }
    if ( m_contextitem->type() != ViewListItem::ItemType_Category ) {
        return;
    }
    kDebug()<<m_contextitem<<":"<<m_contextitem->type();
    if ( m_contextitem->childCount() > 0 ) {
        if ( KMessageBox::warningContinueCancel( this, i18n( "Removing this category will also remove all its views." ) ) == KMessageBox::Cancel ) {
            return;
        }
    }
    takeViewListItem( m_contextitem );
    delete m_contextitem;
    m_contextitem = 0;
}

void ViewListWidget::slotRemoveView()
{
    if ( m_contextitem ) {
        takeViewListItem( m_contextitem );
        delete m_contextitem;
    }
}

void ViewListWidget::slotEditViewTitle()
{
    //QTreeWidgetItem *item = m_viewlist->currentItem();
    if ( m_contextitem ) {
        kDebug()<<m_contextitem<<":"<<m_contextitem->type();
        QString title = m_contextitem->text( 0 );
        m_viewlist->editItem( m_contextitem );
    }
}

void ViewListWidget::slotConfigureItem()
{
    if ( m_contextitem == 0 ) {
        return;
    }
    if ( m_contextitem->type() == ViewListItem::ItemType_Category ) {
        kDebug()<<m_contextitem<<":"<<m_contextitem->type();
        ViewListEditCategoryDialog *dlg = new ViewListEditCategoryDialog( *this, m_contextitem, this );
        dlg->exec();
        delete dlg;
    } else if ( m_contextitem->type() == ViewListItem::ItemType_SubView ) {
        ViewListEditViewDialog *dlg = new ViewListEditViewDialog( *this, m_contextitem, this );
        dlg->exec();
        delete dlg;
    }
}

void ViewListWidget::slotEditDocumentTitle()
{
    //QTreeWidgetItem *item = m_viewlist->currentItem();
    if ( m_contextitem ) {
        kDebug()<<m_contextitem<<":"<<m_contextitem->type();
        QString title = m_contextitem->text( 0 );
        m_viewlist->editItem( m_contextitem );
    }
}

int ViewListWidget::removeViewListItem( ViewListItem *item )
{
    QTreeWidgetItem *p = item->parent();
    if ( p == 0 ) {
        p = m_viewlist->invisibleRootItem();
    }
    int i = p->indexOfChild( item );
    if ( i != -1 ) {
        p->takeChild( i );
    }
    return i;
}

void ViewListWidget::addViewListItem( ViewListItem *item, QTreeWidgetItem *parent, int index )
{
    QTreeWidgetItem *p = parent;
    if ( p == 0 ) {
        p = m_viewlist->invisibleRootItem();
    }
    if ( index == -1 ) {
        index = p->childCount();
    }
    p->insertChild( index, item );
}

int ViewListWidget::takeViewListItem( ViewListItem *item )
{
    while ( item->childCount() > 0 ) {
        takeViewListItem( static_cast<ViewListItem*>( item->child( 0 ) ) );
    }
    //qDebug()<<"takeViewListItem:"<<item<<m_prev;
    int pos = removeViewListItem( item );
    if ( pos != -1 ) {
        emit viewListItemRemoved( item );
        if ( item == m_prev ) {
            m_prev = 0;
        }
        if ( m_prev ) {
            setCurrentItem( m_prev );
        }
    }
    return pos;
}

void ViewListWidget::insertViewListItem( ViewListItem *item, QTreeWidgetItem *parent, int index )
{
    addViewListItem( item, parent, index );
    emit viewListItemInserted( item );
}

void ViewListWidget::setupContextMenus()
{
    // NOTE: can't use xml file as there may not be a factory()
    QAction *action;
    // view actions
    action = new QAction( KIcon( "edit-rename" ), i18n( "Rename" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotEditViewTitle() ) );
    m_viewactions.append( action );

    action = new QAction( KIcon( "configure" ), i18n( "Configure..." ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotConfigureItem() ) );
    m_viewactions.append( action );

    action = new QAction( KIcon( "list-remove" ), i18n( "Remove" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotRemoveView() ) );
    m_viewactions.append( action );

    action = new QAction( this );
    action->setSeparator( true );
    m_viewactions.append( action );

    // Category actions
    action = new QAction( KIcon( "edit-rename" ), i18n( "Rename" ), this );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( renameCategory() ) );
    m_categoryactions.append( action );

    action = new QAction( KIcon( "configure" ), i18n( "Configure..." ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotConfigureItem() ) );
    m_categoryactions.append( action );

    action = new QAction( KIcon( "list-remove" ), i18n( "Remove" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotRemoveCategory() ) );
    m_categoryactions.append( action );

    action = new QAction( this );
    action->setSeparator( true );
    m_categoryactions.append( action );

    // list actions
    action = new QAction( KIcon( "list-add" ), i18nc( "@action Insert View", "Insert..." ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotAddView() ) );
    m_listactions.append( action );
}

void ViewListWidget::renameCategory()
{
    if ( m_contextitem ) {
        QString title = m_contextitem->text( 0 );
        m_viewlist->editItem( m_contextitem, 0 );
    }
}

void ViewListWidget::contextMenuEvent ( QContextMenuEvent *event )
{
    KMenu menu;
    QList<QAction*> lst;
    m_contextitem = static_cast<ViewListItem*>(m_viewlist->itemAt( event->pos() ) );
    if ( m_contextitem == 0 ) {
        lst += m_listactions;
    } else {
        if ( m_contextitem->type() == ViewListItem::ItemType_Category ) {
            lst += m_categoryactions;
        } else if ( m_contextitem->type() == ViewListItem::ItemType_SubView ) {
            lst += m_viewactions;
            ViewBase *v = dynamic_cast<ViewBase*>( m_contextitem->view() );
            if ( v ) {
                lst += v->viewlistActionList();
            }
        }
        lst += m_listactions;
    }
    if ( ! lst.isEmpty() ) {
        //menu.addTitle( i18n( "Edit" ) );
        foreach ( QAction *a, lst ) {
            menu.addAction( a );
        }
    }
    if ( ! menu.actions().isEmpty() ) {
        menu.exec( event->globalPos() );
    }
}

void ViewListWidget::save( QDomElement &element ) const
{
    m_viewlist->save( element );
}

void ViewListWidget::setProject( Project *project )
{
    kDebug()<<project;
    m_model.setProject( project );
}

void ViewListWidget::slotCurrentScheduleChanged( int idx )
{
    kDebug()<<idx<<selectedSchedule();
    emit selectionChanged( selectedSchedule() );
}

ScheduleManager *ViewListWidget::selectedSchedule() const
{
    QModelIndex idx = m_sfModel.index( m_currentSchedule->currentIndex(), m_currentSchedule->modelColumn() );
    kDebug()<<idx;
    return m_sfModel.manager( idx );
}

void ViewListWidget::setSelectedSchedule( ScheduleManager *sm )
{
    kDebug()<<sm<<m_model.index( sm );
    QModelIndex idx = m_sfModel.mapFromSource( m_model.index( sm ) );
    if ( sm && ! idx.isValid()) {
        m_temp = sm;
        return;
    }
    m_currentSchedule->setCurrentIndex( idx.row() );
    kDebug()<<sm<<idx;
    m_temp = 0;
}

void ViewListWidget::slotScheduleManagerAdded( ScheduleManager *sm )
{
    if ( m_temp && m_temp == sm ) {
        setSelectedSchedule( sm );
        m_temp = 0;
    }
}

}  //KPlato namespace

#include "kptviewlist.moc"
