/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KWordFootNoteFrameSetIface.h"
#include "KWTextFrameSet.h"
#include <kdebug.h>

KWFootNoteFrameSetIface::KWFootNoteFrameSetIface( KWFootNoteFrameSet *_note )
    : KWordTextFrameSetIface( _note )
{
   m_footNote = _note;
}


QString KWFootNoteFrameSetIface::footEndNoteText() const
{
    KWFootNoteVariable* var = m_footNote->footNoteVariable();
    if ( var )
        return var->text();
    else
        return QString();
}

bool KWFootNoteFrameSetIface::isFootNote() const
{
    return m_footNote->isFootNote();
}

bool KWFootNoteFrameSetIface::isEndNote() const
{
    return m_footNote->isEndNote();
}

void KWFootNoteFrameSetIface::setCounterText( const QString& text )
{
    m_footNote->setCounterText( text );
}

