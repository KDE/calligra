/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001, S.R.Haque <srhaque@iee.org>
   Copyright (C) 2001, David Faure <faure@kde.org>

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
#include "kwcanvas.h"
#include "kwdoc.h"
#include "kwtextframeset.h"
#include <kotextobject.h>
#include <kcommand.h>
#include <koSearchDia.h>
#include "kwtextdocument.h"
//#include <kotextdocument.h>

KWFindReplace::KWFindReplace( KWCanvas * canvas, KoSearchDia * dialog, const QValueList<KoTextObject *> & lstObjects, KWTextFrameSetEdit *textView)
    : KoFindReplace( canvas, dialog, lstObjects, textView )
{
    m_canvas = canvas;
}

KWFindReplace::KWFindReplace( KWCanvas * canvas, KoReplaceDia * dialog, const QValueList<KoTextObject *> & lstObjects, KWTextFrameSetEdit *textView )
    : KoFindReplace( canvas, dialog, lstObjects, textView )
{
    m_canvas = canvas;
}

KWFindReplace::~KWFindReplace()
{
    //kdDebug() << "KWFindReplace::~KWFindReplace m_destroying=" << m_destroying << endl;
}

void KWFindReplace::emitNewCommand(KCommand *cmd)
{
    m_canvas->kWordDocument()->addCommand(cmd);
}

void KWFindReplace::highlightPortion(KoTextParag * parag, int index, int length, KoTextDocument *_textdoc, KDialogBase* dialog)
{
    KWTextDocument *textdoc=static_cast<KWTextDocument *>(_textdoc);
    bool repaint = isReplace() ? options() & KReplaceDialog::PromptOnReplace : true;
    textdoc->textFrameSet()->highlightPortion( parag, index, length, m_canvas, repaint, dialog );
}

#include "searchdia.moc"
