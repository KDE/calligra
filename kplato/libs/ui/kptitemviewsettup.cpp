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


#include "kptitemviewsettup.h"
#include "kptitemmodelbase.h"
#include "kptviewbase.h"

#include <QCheckBox>
#include <QHeaderView>

#include <kactionselector.h>
#include <kpushbutton.h>
#include <kdebug.h>

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
    
    ItemModelBase *model = view->model();

    QMap<int, Item*> map;
    int c = includeColumn0 ? 0 : 1;
    kDebug()<<includeColumn0<<c;
    for ( ; c < model->columnCount(); ++c ) {
        Item *item = new Item( c, model->headerData( c, Qt::Horizontal ).toString() );
        item->setToolTip( model->headerData( c, Qt::Horizontal, Qt::ToolTipRole ).toString() );
        if ( view->isColumnHidden( c ) ) {
            selector->availableListWidget()->addItem( item );
        } else {
            map.insert( view->section( c ), item );
        }
    }
    foreach( Item *i, map.values() ) {
        selector->selectedListWidget()->addItem( i );
    }

    connect( stretchLastSection, SIGNAL( stateChanged ( int ) ), this, SLOT( changed() ) );
    
    connect( selector, SIGNAL( added (QListWidgetItem *) ), this, SLOT( changed() ) );
    connect( selector, SIGNAL( removed (QListWidgetItem *) ), this, SLOT( changed() ) );
    connect( selector, SIGNAL( movedUp (QListWidgetItem *) ), this, SLOT( changed() ) );
    connect( selector, SIGNAL( movedDown (QListWidgetItem *) ), this, SLOT( changed() ) );

}

void ItemViewSettup::changed()
{
    emit enableButtonOk( true );
}

void ItemViewSettup::slotOk()
{
    kDebug();
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
    kDebug();
    selector->availableListWidget()->clear();
    selector->selectedListWidget()->clear();
    ItemModelBase *model = m_view->model();
    int c = m_includeColumn0 ? 0 : 1;
    QList<int> def = m_view->defaultColumns();
    for ( ; c < m_view->model()->columnCount(); ++c ) {
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
ItemViewSettupDialog::ItemViewSettupDialog( TreeViewBase *view, bool includeColumn0, QWidget *parent )
    : KDialog( parent )
{
    setCaption( i18n("View Settings") );
    setButtons( Ok|Cancel|Default );
    setDefaultButton( Ok );
    showButtonSeparator( true );

    button( Default )->setEnabled( ! view->defaultColumns().isEmpty() );
    
    m_panel = new ItemViewSettup( view, includeColumn0, this );
    setMainWidget( m_panel );
    
    connect( m_panel, SIGNAL( changed( bool ) ), this, SLOT( enableButtonOk( bool ) ) );
    
    connect( this, SIGNAL( okClicked() ), m_panel, SLOT( slotOk() ) );
    connect( this, SIGNAL( defaultClicked() ), m_panel, SLOT( setDefault() ) );
}

//-------------------------------
SplitItemViewSettupDialog::SplitItemViewSettupDialog( DoubleTreeViewBase *view, QWidget *parent )
    : KPageDialog( parent )
{
    setCaption( i18n("View Settings") );
    setButtons( Ok|Cancel|Default );
    setDefaultButton( Ok );
    showButtonSeparator( true );

    bool nodef = view->masterView()->defaultColumns().isEmpty() || view->slaveView()->defaultColumns().isEmpty();
    button( Default )->setEnabled( ! nodef );
    
    m_page1 = new ItemViewSettup( view->masterView(), true, this );
    m_page2 = new ItemViewSettup( view->slaveView(), true, this );
    
    addPage( m_page1, i18n( "Master" ) );
    addPage( m_page2, i18n( "Slave" ) );
    
    connect( m_page1, SIGNAL( changed( bool ) ), this, SLOT( enableButtonOk( bool ) ) );
    connect( m_page2, SIGNAL( changed( bool ) ), this, SLOT( enableButtonOk( bool ) ) );
    
    connect( this, SIGNAL( okClicked() ), m_page1, SLOT( slotOk() ) );
    connect( this, SIGNAL( okClicked() ), m_page2, SLOT( slotOk() ) );
    connect( this, SIGNAL( defaultClicked() ), m_page1, SLOT( setDefault() ) );
    connect( this, SIGNAL( defaultClicked() ), m_page2, SLOT( setDefault() ) );
}

} //namespace KPlato

#include "kptitemviewsettup.moc"
