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

#include "KPTextObjectIface.h"
#include "kptextobject.h"
#include <KoTextViewIface.h>

#include <kotextobject.h>
#include <kapplication.h>
#include <dcopclient.h>


KPTextObjectIface::KPTextObjectIface( KPTextObject *_textobject )
    : DCOPObject()
{
   m_textobject = _textobject;
}
#if 0
DCOPRef KPTextObjectIface::startEditing()
{
    KWDocument *doc=m_frametext->kWordDocument();
    QPtrList <KWView> lst=doc->getAllViews();
    lst.at(0)->getGUI()->canvasWidget()->checkCurrentTextEdit(m_frametext);
    return DCOPRef( kapp->dcopClient()->appId(),
		    (static_cast<KWTextFrameSetEdit *>( lst.at(0)->getGUI()->canvasWidget()->currentFrameSetEdit()))->dcopObject()->objId() );
}
#endif

bool KPTextObjectIface::hasSelection() const
{
    return m_textobject->textObject()->hasSelection();
}

int KPTextObjectIface::numberOfParagraphs()
{
    return -1; // FIXME m_textobject->textObject()->paragraphs();
}


QString KPTextObjectIface::selectedText() const
{
    return m_textobject->textObject()->selectedText();
}

void KPTextObjectIface::selectAll( bool select )
{
    m_textobject->textObject()->selectAll(select);
}
