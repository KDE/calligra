/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include <kcommand.h>
#include "kivio_command.h"
#include "kivio_page.h"
#include "kivio_map.h"
#include "kivio_doc.h"
#include "kivio_layer.h"

KivioChangePageNameCommand::KivioChangePageNameCommand(const QString &_name,  const QString & _oldPageName, const QString & _newPageName, KivioPage *_page)
    : KNamedCommand( _name ),
      oldPageName(_oldPageName ),
      newPageName(_newPageName ),
      m_page( _page )
{

}

KivioChangePageNameCommand::~KivioChangePageNameCommand()
{
}

void KivioChangePageNameCommand::execute()
{
    m_page->setPageName( newPageName );
}

void KivioChangePageNameCommand::unexecute()
{
    m_page->setPageName( oldPageName);
}


KivioHidePageCommand::KivioHidePageCommand(const QString &_name, KivioPage *_page)
    : KNamedCommand( _name ),
      m_page( _page )
{
}

KivioHidePageCommand::~KivioHidePageCommand()
{
}

void KivioHidePageCommand::execute()
{
    m_page->setHidePage( true );
}

void KivioHidePageCommand::unexecute()
{
    m_page->setHidePage( false );
}

KivioShowPageCommand::KivioShowPageCommand( const QString &_name, KivioPage *_page)
    : KivioHidePageCommand( _name, _page)
{
}


KivioAddPageCommand::KivioAddPageCommand(const QString &_name, KivioPage *_page)
    : KNamedCommand( _name ),
      m_page( _page )
{
}

KivioAddPageCommand::~KivioAddPageCommand()
{
    delete m_page;
    m_page=0L;
}

void KivioAddPageCommand::execute()
{
    m_page->map()->insertPage( m_page );
    m_page->doc()->insertPage( m_page );
}

void KivioAddPageCommand::unexecute()
{
    m_page->map()->takePage( m_page );
    m_page->doc()->takePage( m_page );
}

KivioRemovePageCommand::KivioRemovePageCommand(const QString &_name, KivioPage *_page)
    : KNamedCommand( _name ),
      m_page( _page )
{
}

KivioRemovePageCommand::~KivioRemovePageCommand()
{
    delete m_page;
    m_page=0L;
}

void KivioRemovePageCommand::execute()
{
    m_page->map()->takePage( m_page );
    m_page->doc()->takePage( m_page );
}

void KivioRemovePageCommand::unexecute()
{
    m_page->map()->insertPage( m_page );
    m_page->doc()->insertPage( m_page );

}

KivioAddStencilCommand::KivioAddStencilCommand(const QString &_name, KivioPage *_page,  KivioLayer * _layer, KivioStencil *_stencil )
    : KNamedCommand( _name ),
      m_page( _page ),
      m_layer( _layer),
      m_stencil( _stencil)
{
}

KivioAddStencilCommand::~KivioAddStencilCommand()
{
}

void KivioAddStencilCommand::execute()
{
    m_layer->insertStencil( m_stencil );
    m_page->doc()->updateView(m_page);
    m_stencil->unselect();
    m_page->doc()->slotSelectionChanged();
}

void KivioAddStencilCommand::unexecute()
{
    m_layer->takeStencilFromList( m_stencil );
    m_page->doc()->updateView(m_page);
    m_stencil->unselect();
    m_page->doc()->slotSelectionChanged();
}
