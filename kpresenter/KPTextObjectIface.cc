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
#include "KPresenterObjectIface.h"
#include "kptextobject.h"
#include <KoTextViewIface.h>
#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include <kotextobject.h>
#include <kapplication.h>
#include <dcopclient.h>
#include "kprcanvas.h"

KPTextObjectIface::KPTextObjectIface( KPTextObject *_textobject )
    : KPresenterObjectIface(_textobject)
{
   m_textobject = _textobject;
}

DCOPRef KPTextObjectIface::startEditing()
{
    KPresenterDoc *doc=m_textobject->kPresenterDocument();
    KPresenterView *view=doc->getKPresenterView();
    view->getCanvas()->createEditing( m_textobject);
    return DCOPRef( kapp->dcopClient()->appId(),
		    view->getCanvas()->currentTextObjectView()->dcopObject()->objId() );
}

bool KPTextObjectIface::hasSelection() const
{
    return m_textobject->textObject()->hasSelection();
}

QString KPTextObjectIface::selectedText() const
{
    return m_textobject->textObject()->selectedText();
}

void KPTextObjectIface::selectAll( bool select )
{
    m_textobject->textObject()->selectAll(select);
}

void KPTextObjectIface::recalcPageNum( )
{
    //FIXME
    //m_textobject->recalcPageNum(m_textobject->kPresenterDocument());

}
