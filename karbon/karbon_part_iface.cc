/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <dcopclient.h>

#include "karbon_part.h"
#include "karbon_part_iface.h"
#include "vselection.h"

KarbonPartIface::KarbonPartIface( KarbonPart *part_ )
	: KoDocumentIface( part_ )
{
    m_part = part_;
}

void KarbonPartIface::selectAllObjects()
{
    m_part->document().selection()->append();
}

void KarbonPartIface::deselectAllObjects()
{
    m_part->document().selection()->clear();
}

bool KarbonPartIface::showStatusBar () const
{
    return m_part->showStatusBar();
}

void KarbonPartIface::setShowStatusBar (bool b)
{
    m_part->setShowStatusBar( b );
    m_part->reorganizeGUI();
}

void KarbonPartIface::setUndoRedoLimit( int _undo )
{
    m_part->setUndoRedoLimit( _undo );
}

void KarbonPartIface::initConfig()
{
    m_part->initConfig();
}

int KarbonPartIface::maxRecentFiles() const
{
    return m_part->maxRecentFiles();
}


void KarbonPartIface::purgeHistory()
{
    m_part->purgeHistory();
}

