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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KWordTextFrameSetIface.h"
#include "KWordViewIface.h"
#include "kwtextframeset.h"
#include <KoTextViewIface.h>
#include "kwview.h"
#include <kotextobject.h>
#include "kwcanvas.h"
#include "kwdoc.h"
#include <kapplication.h>
#include <dcopclient.h>


KWordTextFrameSetIface::KWordTextFrameSetIface( KWTextFrameSet *_frame )
    : DCOPObject()
{
   m_frametext = _frame;
}

DCOPRef KWordTextFrameSetIface::startEditing()
{
    KWDocument *doc=m_frametext->kWordDocument();
    QPtrList <KWView> lst=doc->getAllViews();
    lst.at(0)->getGUI()->canvasWidget()->checkCurrentTextEdit(m_frametext);
    return DCOPRef( kapp->dcopClient()->appId(),
		    (static_cast<KWTextFrameSetEdit *>( lst.at(0)->getGUI()->canvasWidget()->currentFrameSetEdit()))->dcopObject()->objId() );
}

bool KWordTextFrameSetIface::hasSelection() const
{
    return m_frametext->hasSelection();
}

int KWordTextFrameSetIface::numberOfParagraphs()
{
    return m_frametext->paragraphs();
}

QString KWordTextFrameSetIface::name() const
{
    return m_frametext->getName();
}

bool KWordTextFrameSetIface::isVisible() const
{
    return m_frametext->isVisible();
}

bool KWordTextFrameSetIface::isFloating() const
{
    return m_frametext->isFloating();
}

bool KWordTextFrameSetIface::isAHeader() const
{
    return m_frametext->isAHeader();
}

bool KWordTextFrameSetIface::isAFooter() const
{
    return m_frametext->isAFooter();
}

QString KWordTextFrameSetIface::selectedText() const
{
    return m_frametext->selectedText();
}

void KWordTextFrameSetIface::selectAll( bool select )
{
    m_frametext->textObject()->selectAll(select);
}
