/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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

#include "KWFrameSetEdit.h"
#include "KWView.h"
#include "KWCanvas.h"
#include "KWFrame.h"
#include "KWAnchor.h"
#include "KWFrameSet.h"
#include "KWTextFrameSet.h"

#include <KoTextParag.h>

KWFrameSetEdit::KWFrameSetEdit( KWFrameSet * fs, KWCanvas * canvas )
     : m_fs(fs), m_canvas(canvas), m_currentFrame( fs->frame(0) )
{
}

void KWFrameSetEdit::drawContents( QPainter *p, const QRect &crect,
                                   const QColorGroup &cg, bool onlyChanged, bool resetChanged,
                                   KWViewMode *viewMode, KWFrameViewManager *fvm )
{
    //kdDebug(32001) << "KWFrameSetEdit::drawContents " << frameSet()->name() << endl;
    frameSet()->drawContents( p, crect, cg, onlyChanged, resetChanged, this, viewMode, fvm );
}

bool KWFrameSetEdit::exitLeft()
{
    if ( m_fs->isFloating() ) {
        KWAnchor* anchor = m_fs->findAnchor( 0 );
        int index = anchor->index();
        KoTextParag *parag = anchor->paragraph();
        // This call deletes "this"!
        m_canvas->editTextFrameSet( m_fs->anchorFrameset(), parag, index );
        return true;
    }
    return false;
}

bool KWFrameSetEdit::exitRight()
{
    if ( m_fs->isFloating() ) {
        KWAnchor* anchor = m_fs->findAnchor( 0 );
        int index = anchor->index();
        KoTextParag *parag = anchor->paragraph();
        // This call deletes "this"!
        m_canvas->editTextFrameSet( m_fs->anchorFrameset(), parag, index+1 );
        return true;
    }
    return false;
}
