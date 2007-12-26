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
#include <kmenu.h>

#include "KoDocument.h"

#include "kptviewlistcommand.h"
#include "kptviewbase.h"
#include "kptpart.h"
#include "kptviewlistdialog.h"
#include "kptviewlistdocker.h"

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

void ViewListItem::setReadWrite( bool rw )
{
    if ( type() == ItemType_SubView ) {
        static_cast<ViewBase*>( view() )->updateReadWrite( rw );
    }
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

void ViewListItem::save( QDomElement &element ) const
{
    element.setAttribute( "itemtype", type() );
    element.setAttribute( "tag", tag() );
    element.setAttribute( "name", text( 0 ) );
    element.setAttribute( "tooltip", toolTip( 0 ) );
    if ( type() == ItemType_SubView ) {
        QString name = view()->metaObject()->className();
        if ( name.contains( ':' ) ) {
            name = name.remove( 0, name.lastIndexOf( ':' ) + 1 );
        }
        kDebug()<<view()->metaObject()->className()<<" -> "<<name;
        element.setAttribute( "viewtype", name );
    }
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
        itm->save( c );
        for ( int j = 0; j < itm->childCount(); ++j ) {
            ViewListItem *vi = static_cast<ViewListItem*>( itm->child( j ) );
            if ( vi->type() != ViewListItem::ItemType_SubView ) {
                continue;
            }
            QDomElement el = c.ownerDocument().createElement( "view" );
            c.appendChild( el );
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
    if ( item && item->type() == ViewListItem::ItemType_ChildDocument && col == 0 ) {
        DocumentChild *ch = static_cast<ViewListItem*>(item)->documentChild();
        if ( ch ) {
            ch->setTitle( item->text( 0 ) );
            //kDebug()<<ch->title();
        }
    }
}

void ViewListWidget::slotActivated( QTreeWidgetItem *item, QTreeWidgetItem *prev )
{
    //kDebug();
    if ( item == 0 || item->type() == ViewListItem::ItemType_Category ) {
        return ;
    }
    emit activated( static_cast<ViewListItem*>( item ), static_cast<ViewListItem*>( prev ) );
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
    //kDebug() << endl;
    QString tag = seed;
    for ( int i = 1; findItem( tag ); ++i ) {
        tag = QString("%1-%2").arg( seed ).arg( i );
    }
    return tag;
}

ViewListItem *ViewListWidget::addView( QTreeWidgetItem *category, const QString &tag, const QString& name, KoView *view, KoDocument *doc, const QString& icon )
{
    ViewListItem * item = new ViewListItem( category, uniqueTag( tag ), QStringList( name ), ViewListItem::ItemType_SubView );
    item->setView( view );
    item->setDocument( doc );
    if ( !icon.isEmpty() ) {
        item->setData( 0, Qt::DecorationRole, KIcon( icon ) );
    }
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    //kDebug() << "added: " << item->tag() << endl;
    return item;
}

ViewListItem *ViewListWidget::createChildDocumentView( const QString& tag, const QString& name, KoView *view, DocumentChild *ch, const QString& icon )
{
    ViewListItem * item = new ViewListItem( uniqueTag( tag ), QStringList( name ), ViewListItem::ItemType_ChildDocument );
    item->setView( view );
    item->setDocument( ch->document() );
    item->setDocumentChild( ch );
    if ( !icon.isEmpty() ) {
        item->setData( 0, Qt::DecorationRole, KIcon( icon ) );
    }
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    //kDebug() <<"added:" << item;
    return item;
}

void ViewListWidget::setSelected( QTreeWidgetItem *item )
{
    //kDebug()<<item<<","<<m_viewlist->currentItem();
    if ( item == 0 ) {
        return;
    }
    m_viewlist->setCurrentItem( item );
    //kDebug()<<item<<","<<m_viewlist->currentItem();
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

ViewListItem *ViewListWidget::findItem(  const QWidget *view, QTreeWidgetItem *parent ) const
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

void ViewListWidget::slotCreatePart()
{
    kDebug();
    QString servName = sender()->objectName();
    kDebug()<<servName;
    KService::Ptr serv = KService::serviceByName( servName );
    KoDocumentEntry entry = KoDocumentEntry( serv );
    emit createKofficeDocument( entry );
}

void ViewListWidget::slotAddView()
{
    emit createView();
}

void ViewListWidget::slotRemoveCategory()
{
    if ( m_contextitem && m_contextitem->type() == ViewListItem::ItemType_Category ) {
        kDebug()<<m_contextitem<<":"<<m_contextitem->type();
        takeViewListItem( m_contextitem );
        delete m_contextitem;
        m_contextitem = 0;
    }
}

void ViewListWidget::slotRemoveView()
{
    if ( m_contextitem ) {
        kDebug()<<m_contextitem<<":"<<m_contextitem->type();
        takeViewListItem( m_contextitem );
    }
}

void ViewListWidget::slotEditViewTitle()
{
    //QTreeWidgetItem *item = m_viewlist->currentItem();
    if ( m_contextitem ) {
        kDebug()<<m_contextitem<<":"<<m_contextitem->type();
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
        ViewListEditCategoryDialog dlg( *this, m_contextitem, this );
        dlg.exec();
    } else if ( m_contextitem->type() == ViewListItem::ItemType_SubView ) {
        ViewListEditViewDialog dlg( *this, m_contextitem, this );
        dlg.exec();
    } else if ( m_contextitem->type() == ViewListItem::ItemType_ChildDocument ) {
        ViewListEditViewDialog dlg( *this, m_contextitem, this );
        dlg.exec();
    }
}

void ViewListWidget::slotEditDocumentTitle()
{
    //QTreeWidgetItem *item = m_viewlist->currentItem();
    if ( m_contextitem ) {
        kDebug()<<m_contextitem<<":"<<m_contextitem->type();
        m_viewlist->editItem( m_contextitem );
    }
}

void ViewListWidget::slotRemoveDocument()
{
    if ( m_contextitem ) {
        kDebug()<<m_contextitem<<":"<<m_contextitem->type();
        m_part->addCommand( new DeleteEmbeddedDocumentCmd( this, m_contextitem, i18n( "Remove Document" ) ) );
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
    int pos = removeViewListItem( item );
    if ( pos != -1 ) {
        emit viewListItemRemoved( item );
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
    // DF: Why not use KoPartSelectAction though?
    QAction *action;
    // document insert actions
    // Query for document types
    m_lstEntries = KoDocumentEntry::query(KoDocumentEntry::OnlyEmbeddableDocuments);
    QList<KoDocumentEntry>::const_iterator it = m_lstEntries.begin();
    for( ; it != m_lstEntries.end(); ++it ) {
        KService::Ptr serv = (*it).service();
        // TODO: Make this test safer (possibly add desktopfile to define embedable parts)
        if ( serv->genericName().isEmpty() ||
            serv->serviceTypes().contains( "application/vnd.oasis.opendocument.formula" ) ||
            serv->serviceTypes().contains( "application/x-vnd.kde.kplato" ) ) {
            continue;
            }
            action = new QAction( KIcon(serv->icon()), serv->genericName().replace('&',"&&"), this );
            action->setObjectName( serv->name().toLatin1() );
            connect(action, SIGNAL( triggered( bool ) ), this, SLOT( slotCreatePart() ) );
            m_adddocument.append( action );
    }
    // no item actions
    //action = new QAction( KIcon( "document-new" ), i18n( "New Category..." ), this );
    //m_noitem.append( action );
    
    // view insert actions
    action = new QAction( KIcon( "list-add" ), i18n( "View..." ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotAddView() ) );
    m_addview.append( action );

    // Category edit actions
    action = new QAction( KIcon( "edit-rename" ), i18n( "Rename" ), this );
    connect( action, SIGNAL( triggered( bool ) ), SLOT( renameCategory() ) );
    m_editcategory.append( action );
    action = new QAction( KIcon( "list-remove" ), i18n( "Remove" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotRemoveCategory() ) );
    m_editcategory.append( action );
    action = new QAction( KIcon( "configure" ), i18n( "Configure..." ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotConfigureItem() ) );
    m_editcategory.append( action );

    // view edit actions
    action = new QAction( KIcon( "edit-rename" ), i18n( "Rename" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotEditViewTitle() ) );
    m_editview.append( action );
    action = new QAction( KIcon( "list-remove" ), i18n( "Remove" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotRemoveView() ) );
    m_editview.append( action );
    action = new QAction( KIcon( "configure" ), i18n( "Configure..." ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotConfigureItem() ) );
    m_editview.append( action );

    // document edit actions
    action = new QAction( KIcon( "edit-rename" ), i18n( "Rename" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotEditDocumentTitle() ) );
    m_editdocument.append( action );
    action = new QAction( KIcon( "edit-delete" ), i18n( "Remove" ), this );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( slotRemoveDocument() ) );
    m_editdocument.append( action );
}

void ViewListWidget::renameCategory()
{
    if ( m_contextitem ) {
        m_viewlist->editItem( m_contextitem, 0 );
    }
}

void ViewListWidget::contextMenuEvent ( QContextMenuEvent *event )
{
    KMenu menu;
    QList<QAction*> lst;
    m_contextitem = static_cast<ViewListItem*>(m_viewlist->itemAt( event->pos() ) );
    // first edit stuff
    if ( m_contextitem != 0 ) {
        if ( m_contextitem->type() == ViewListItem::ItemType_Category ) {
            lst += m_editcategory;
        } else if ( m_contextitem->type() == ViewListItem::ItemType_SubView ) {
            lst += m_editview;
            ViewBase *v = dynamic_cast<ViewBase*>( m_contextitem->view() );
            if ( v ) {
                lst += v->viewlistActionList();
            }
        } else if ( m_contextitem->type() == ViewListItem::ItemType_ChildDocument ) {
            lst += m_editdocument;
        }
        if ( ! lst.isEmpty() ) {
            menu.addTitle( i18n( "Edit" ) );
            foreach ( QAction *a, lst ) {
                menu.addAction( a );
            }
        }
    }
    // then the "add" stuff
    lst = m_addview;
    lst += m_adddocument;
    if ( ! lst.isEmpty() ) {
        menu.addTitle( i18n( "Insert" ) );
        foreach ( QAction *a, lst ) {
            menu.addAction( a );
        }
    }
    if ( ! menu.actions().isEmpty() ) {
        menu.exec( event->globalPos());
    }
}

void ViewListWidget::save( QDomElement &element ) const
{
    m_viewlist->save( element );
}

}  //KPlato namespace

#include "kptviewlist.moc"
