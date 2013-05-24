/* This file is part of the KDE project
  Copyright (C) 2007, 2012 Dag Andersen <danders@get2net.dk>

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


#include "kptitemviewsettup.h"
#include "kptitemmodelbase.h"
#include "kptviewbase.h"
#include "kptdebug.h"

#include "KoPageLayoutWidget.h"

#include <QCheckBox>
#include <QHeaderView>

#include <kactionselector.h>
#include <kpushbutton.h>

namespace KPlato
{

ItemViewSettup::Item::Item( int column, const QString &text )
    : QListWidgetItem( text ),
    m_column( column )
{
}

int ItemViewSettup::Item::column() const
{
    return m_column;
}

bool ItemViewSettup::Item::operator<( const QListWidgetItem & other ) const
{
    return m_column < static_cast<const Item&>( other ).column();
}

//--------------------------
ItemViewSettup::ItemViewSettup( TreeViewBase *view, bool includeColumn0, QWidget *parent )
    : QWidget( parent ),
    m_view( view ),
    m_includeColumn0( includeColumn0 )
{
    setupUi( this );
    
    stretchLastSection->setChecked( view->header()->stretchLastSection() );
    
    QAbstractItemModel *model = view->model();

    QMap<int, Item*> map;
    int c = includeColumn0 ? 0 : 1;
    kDebug(planDbg())<<includeColumn0<<c;
    for ( ; c < model->columnCount(); ++c ) {
        Item *item = new Item( c, model->headerData( c, Qt::Horizontal ).toString() );
        item->setToolTip( model->headerData( c, Qt::Horizontal, Qt::ToolTipRole ).toString() );
        if ( view->isColumnHidden( c ) ) {
            selector->availableListWidget()->addItem( item );
        } else {
            map.insert( view->section( c ), item );
        }
    }
    foreach( Item *i, map ) {
        selector->selectedListWidget()->addItem( i );
    }

    connect( stretchLastSection, SIGNAL( stateChanged ( int ) ), this, SLOT( slotChanged() ) );
    
    connect( selector, SIGNAL( added (QListWidgetItem *) ), this, SLOT( slotChanged() ) );
    connect( selector, SIGNAL( removed (QListWidgetItem *) ), this, SLOT( slotChanged() ) );
    connect( selector, SIGNAL( movedUp (QListWidgetItem *) ), this, SLOT( slotChanged() ) );
    connect( selector, SIGNAL( movedDown (QListWidgetItem *) ), this, SLOT( slotChanged() ) );

}

void ItemViewSettup::slotChanged()
{
    emit enableButtonOk( true );
}

void ItemViewSettup::slotOk()
{
    kDebug(planDbg());
    QListWidget *lst = selector->availableListWidget();
    for ( int r = 0; r < lst->count(); ++r ) {
        int c = static_cast<Item*>( lst->item( r ) )->column();
        m_view->hideColumn( c );
    }
    lst = selector->selectedListWidget();
    for ( int r = 0; r < lst->count(); ++r ) {
        int c = static_cast<Item*>( lst->item( r ) )->column();
        m_view->mapToSection( c, r );
        m_view->showColumn( c );
    }
    m_view->setStretchLastSection( stretchLastSection->isChecked() );
}

void ItemViewSettup::setDefault()
{
    kDebug(planDbg());
    selector->availableListWidget()->clear();
    selector->selectedListWidget()->clear();
    QAbstractItemModel *model = m_view->model();
    int c = m_includeColumn0 ? 0 : 1;
    QList<int> def = m_view->defaultColumns();
    for ( ; c < model->columnCount(); ++c ) {
        if ( ! def.contains( c ) ) {
            Item *item = new Item( c, model->headerData( c, Qt::Horizontal ).toString() );
            item->setToolTip( model->headerData( c, Qt::Horizontal, Qt::ToolTipRole ).toString() );
            selector->availableListWidget()->addItem( item );
        }
    }
    foreach ( int i, def ) {
        Item *item = new Item( i, model->headerData( i, Qt::Horizontal ).toString() );
        item->setToolTip( model->headerData( i, Qt::Horizontal, Qt::ToolTipRole ).toString() );
        selector->selectedListWidget()->addItem( item );
    }
}


//---------------------------
ItemViewSettupDialog::ItemViewSettupDialog( ViewBase *view, TreeViewBase *treeview, bool includeColumn0, QWidget *parent )
    : KPageDialog( parent ),
    m_view( view ),
    m_treeview( treeview ),
    m_pagelayout( 0 ),
    m_headerfooter( 0 )
{
    setCaption( i18n("View Settings") );
    setButtons( Ok|Cancel|Default );
    setDefaultButton( Ok );
    showButtonSeparator( true );

    button( Default )->setEnabled( ! treeview->defaultColumns().isEmpty() );
    
    m_panel = new ItemViewSettup( treeview, includeColumn0 );
    KPageWidgetItem *page = new KPageWidgetItem( m_panel, i18n( "Tree View" ) );
    page->setHeader( i18n( "Tree View Column Configuration" ) );
    addPage( page );
    m_pageList.append( page );
    
    connect(this, SIGNAL(okClicked()), this, SLOT( slotOk()));
    connect(this, SIGNAL(okClicked()), m_panel, SLOT( slotOk()));
    connect(this, SIGNAL(defaultClicked()), m_panel, SLOT(setDefault()));
}

void ItemViewSettupDialog::slotOk()
{
    kDebug(planDbg())<<m_view<<m_pagelayout<<m_headerfooter;
    if ( ! m_view ) {
        return;
    }
    if ( m_pagelayout ) {
        m_view->setPageLayout( m_pagelayout->pageLayout() );
    }
    if ( m_headerfooter ) {
        m_view->setPrintingOptions( m_headerfooter->options() );
    }
}

KPageWidgetItem *ItemViewSettupDialog::insertWidget( int index, QWidget *widget, const QString &name, const QString &header )
{
    KPageWidgetItem *before = m_pageList.value( index );
    KPageWidgetItem *page = new KPageWidgetItem( widget, name );
    page->setHeader( header );
    if ( before ) {
        insertPage( before, page );
        m_pageList.insert( index, page );
    } else {
        addPage( page );
        m_pageList.append( page );
    }
    return page;
}

void ItemViewSettupDialog::addPrintingOptions()
{
    if ( ! m_view ) {
        return;
    }
    QTabWidget *tab = new QTabWidget();
    QWidget *w = ViewBase::createPageLayoutWidget( m_view );
    tab->addTab( w, w->windowTitle() );
    m_pagelayout = w->findChild<KoPageLayoutWidget*>();
    Q_ASSERT( m_pagelayout );

    m_headerfooter = ViewBase::createHeaderFooterWidget( m_view );
    tab->addTab( m_headerfooter, m_headerfooter->windowTitle() );

    insertWidget( -1, tab, i18n( "Printing" ), i18n( "Printing Options" ) );
}

//-------------------------------
SplitItemViewSettupDialog::SplitItemViewSettupDialog( ViewBase *view, DoubleTreeViewBase *treeview, QWidget *parent )
    : KPageDialog( parent ),
    m_view( view ),
    m_treeview( treeview ),
    m_pagelayout( 0 ),
    m_headerfooter( 0 )
{
    setCaption( i18n("View Settings") );
    setButtons( Ok|Cancel|Default );
    setDefaultButton( Ok );
    showButtonSeparator( true );

    bool nodef = treeview->masterView()->defaultColumns().isEmpty() || treeview->slaveView()->defaultColumns().isEmpty();
    button( Default )->setEnabled( ! nodef );

    m_page1 = new ItemViewSettup( treeview->masterView(), true );
    KPageWidgetItem *page = new KPageWidgetItem( m_page1, i18n( "Main View" ) );
    page->setHeader( i18n( "Main View Column Configuration" ) );
    addPage( page );
    m_pageList.append( page );

    m_page2 = new ItemViewSettup( treeview->slaveView(), true );
    page = new KPageWidgetItem( m_page2, i18n( "Auxiliary View" ) );
    page->setHeader( i18n( "Auxiliary View Column Configuration" ) );
    addPage( page );
    m_pageList.append( page );

    //connect( m_page1, SIGNAL( enableButtonOk( bool ) ), this, SLOT( enableButtonOk( bool ) ) );
    //connect( m_page2, SIGNAL( enableButtonOk( bool ) ), this, SLOT( enableButtonOk( bool ) ) );

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
    connect( this, SIGNAL( okClicked() ), m_page1, SLOT( slotOk() ) );
    connect( this, SIGNAL( okClicked() ), m_page2, SLOT( slotOk() ) );
    connect( this, SIGNAL( defaultClicked() ), m_page1, SLOT( setDefault() ) );
    connect( this, SIGNAL( defaultClicked() ), m_page2, SLOT( setDefault() ) );
}

void SplitItemViewSettupDialog::slotOk()
{
    kDebug(planDbg());
    if ( ! m_view ) {
        return;
    }
    m_view->setPageLayout( m_pagelayout->pageLayout() );
    m_view->setPrintingOptions( m_headerfooter->options() );
}

KPageWidgetItem *SplitItemViewSettupDialog::insertWidget( int index, QWidget *widget, const QString &name, const QString &header )
{
    KPageWidgetItem *before = m_pageList.value( index );
    KPageWidgetItem *page = new KPageWidgetItem( widget, name );
    page->setHeader( header );
    if ( before ) {
        insertPage( before, page );
        m_pageList.insert( index, page );
    } else {
        addPage( page );
        m_pageList.append( page );
    }
    return page;
}

void SplitItemViewSettupDialog::addPrintingOptions()
{
    if ( ! m_view ) {
        return;
    }
    QTabWidget *tab = new QTabWidget();
    QWidget *w = ViewBase::createPageLayoutWidget( m_view );
    tab->addTab( w, w->windowTitle() );
    m_pagelayout = w->findChild<KoPageLayoutWidget*>();
    Q_ASSERT( m_pagelayout );
    m_pagelayout->setPageLayout( m_view->pageLayout() );

    m_headerfooter = ViewBase::createHeaderFooterWidget( m_view );
    tab->addTab( m_headerfooter, m_headerfooter->windowTitle() );
    m_headerfooter->setOptions( m_view->printingOptions() );

    insertWidget( -1, tab, i18n( "Printing" ), i18n( "Printing Options" ) );
}

} //namespace KPlato

#include "kptitemviewsettup.moc"
