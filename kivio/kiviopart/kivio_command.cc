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
