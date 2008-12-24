/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or ( at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrSetCustomSlideShowsCommand.h"

#include "KPrCustomSlideShows.h"
#include "KPrDocument.h"

KPrSetCustomSlideShowsCommand::KPrSetCustomSlideShowsCommand( KPrDocument * doc, KPrCustomSlideShows * newSlideShows, QUndoCommand *parent )
: QUndoCommand( parent )
, m_doc( doc )
, m_oldSlideShows( doc->customSlideShows() )
, m_newSlideShows( newSlideShows )
, m_deleteNewSlideShows( true )
{
    // TODO 2.1 change text to "Edit custom slide shows"
    setText( i18n("Edit") );
}

KPrSetCustomSlideShowsCommand::~KPrSetCustomSlideShowsCommand()
{
    if ( m_deleteNewSlideShows ) {
        delete m_newSlideShows;
    }
    else {
        delete m_oldSlideShows;
    }
}

void KPrSetCustomSlideShowsCommand::redo()
{
    m_doc->setCustomSlideShows( m_newSlideShows );
    m_deleteNewSlideShows = false;
}

void KPrSetCustomSlideShowsCommand::undo()
{
    m_doc->setCustomSlideShows( m_oldSlideShows );
    m_deleteNewSlideShows = true;
}
