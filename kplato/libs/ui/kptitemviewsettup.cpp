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
    m_view( view )
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

//---------------------------
ItemViewSettupDialog::ItemViewSettupDialog( TreeViewBase *view, bool includeColumn0, QWidget *parent )
    : KDialog( parent )
{
    setCaption( i18n("View Settings") );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    showButtonSeparator( true );

    m_panel = new ItemViewSettup( view, includeColumn0, this );
    setMainWidget( m_panel );
    
    connect( m_panel, SIGNAL( changed( bool ) ), this, SLOT( enableButtonOk( bool ) ) );
    
    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
}

void ItemViewSettupDialog::slotOk()
{
    m_panel->slotOk();
}

} //namespace KPlato

#include "kptitemviewsettup.moc"
