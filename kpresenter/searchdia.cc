/* This file is part of the KDE project
   Copyright (C) 2001, Laurent Montel <lmontel@mandrakesoft.com>

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

#include <kdebug.h>
#include "searchdia.h"
#include "kprcanvas.h"
#include "kpresenter_view.h"
#include "kpresenter_doc.h"
#include "kptextobject.h"
#include <kotextobject.h>
#include <kcommand.h>
#include <koSearchDia.h>
#include "kprtextdocument.h"

KPrFindReplace::KPrFindReplace( KPrCanvas * canvas, KoSearchDia * dialog ,KPTextView *textView ,const QPtrList<KoTextObject> & lstObject)
    :KoFindReplace( canvas, dialog,textView ,lstObject)
{
    m_canvas= canvas;
}

KPrFindReplace::KPrFindReplace( KPrCanvas * canvas, KoReplaceDia * dialog, KPTextView *textView,const QPtrList<KoTextObject> & lstObject)
    :KoFindReplace( canvas, dialog,textView ,lstObject)
{
    m_canvas= canvas;
}

KPrFindReplace::~KPrFindReplace()
{
    //kdDebug(33001)() << "KWFindReplace::~KWFindReplace m_destroying=" << m_destroying << endl;
}

void KPrFindReplace::emitNewCommand(KCommand *cmd)
{
    m_canvas->getView()->kPresenterDoc()->addCommand(cmd);
}

void KPrFindReplace::highlightPortion(KoTextParag * parag, int index, int length, KoTextDocument *_textdoc)
{
    KPrTextDocument *textdoc=static_cast<KPrTextDocument *>(_textdoc);
    textdoc->textObject()->highlightPortion( parag, index, length,m_canvas );
}
#include "searchdia.moc"
