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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kdebug.h>
#include "KWFindReplace.h"
#include "KWCanvas.h"
#include "KWDocument.h"
#include "KWTextFrameSet.h"
#include <KoTextObject.h>
#include <kcommand.h>
#include <KoSearchDia.h>
#include "KWTextDocument.h"
//Added by qt3to4:
#include <QList>
//#include <KoTextDocument.h>

KWFindReplace::KWFindReplace( KWCanvas * canvas, KoSearchDia * dialog, const QList<KoTextObject *> & lstObjects, KWTextFrameSetEdit *textView)
    : KoFindReplace( canvas, dialog, lstObjects, textView )
{
    m_canvas = canvas;
}

KWFindReplace::KWFindReplace( KWCanvas * canvas, KoReplaceDia * dialog, const QList<KoTextObject *> & lstObjects, KWTextFrameSetEdit *textView )
    : KoFindReplace( canvas, dialog, lstObjects, textView )
{
    m_canvas = canvas;
}

KWFindReplace::~KWFindReplace()
{
    //kDebug() << "KWFindReplace::~KWFindReplace m_destroying=" << m_destroying << endl;
}

void KWFindReplace::emitNewCommand(KCommand *cmd)
{
    m_canvas->kWordDocument()->addCommand(cmd);
}

void KWFindReplace::highlightPortion(KoTextParag * parag, int index, int length, KoTextDocument *textdocument, KDialog* dialog)
{
#if 0
    KWTextDocument *textdoc=static_cast<KWTextDocument *>(textdocument);
    bool repaint = isReplace() ? options() & KReplaceDialog::PromptOnReplace : true;
    textdoc->textFrameSet()->highlightPortion( parag, index, length, m_canvas, repaint, dialog );
#endif
}

#include "KWFindReplace.moc"
