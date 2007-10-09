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

#include "kptviewlistcommand.h"

#include "kptviewlist.h"
#include "kptpart.h"

#include "KoView.h"

#include <kdebug.h>
#include <klocale.h>

namespace KPlato
{

InsertEmbeddedDocumentCmd::InsertEmbeddedDocumentCmd( ViewListWidget *list, ViewListItem *item, QTreeWidgetItem *parent, const QString& name )
    : NamedCommand( name ),
        m_list( list ),
        m_parent( parent ),
        m_item( item ),
        m_index( -1 ),
        m_mine( false )
{
}
InsertEmbeddedDocumentCmd::~InsertEmbeddedDocumentCmd()
{
    if ( m_mine ) {
        delete m_item;
    }
}
void InsertEmbeddedDocumentCmd::execute()
{
    m_list->insertViewListItem( m_item, m_parent, m_index );
    m_item->documentChild()->setDeleted( false );

}
void InsertEmbeddedDocumentCmd::unexecute()
{
    m_item->documentChild()->setDeleted( true );
    m_index = m_list->takeViewListItem( m_item );

}

DeleteEmbeddedDocumentCmd::DeleteEmbeddedDocumentCmd( ViewListWidget *list, ViewListItem *item, const QString& name )
    : NamedCommand( name ),
        m_list( list ),
        m_parent( item->parent() ),
        m_item( item ),
        m_index( -1 ),
        m_mine( false )
{
}
DeleteEmbeddedDocumentCmd::~DeleteEmbeddedDocumentCmd()
{
    if ( m_mine ) {
        delete m_item->view();
        delete m_item;
    }
}
void DeleteEmbeddedDocumentCmd::execute()
{
    m_index = m_list->takeViewListItem( m_item );
    m_item->documentChild()->setDeleted( true );

}
void DeleteEmbeddedDocumentCmd::unexecute()
{
    m_item->documentChild()->setDeleted( true );
    m_list->insertViewListItem( m_item, m_parent, m_index );

}


}  //KPlato namespace
