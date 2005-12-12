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

#ifndef searchdia_h
#define searchdia_h

#include <qstring.h>
#include <qstringlist.h>
#include <KoRichText.h>
#include <KoTextObject.h>
#include <KoTextView.h>
#include <KoSearchDia.h>

class KWCanvas;
class KWTextFrameSet;
class KMacroCommand;
class KWTextFrameSetEdit;
/**
 * This class implements the 'find' functionality ( the "search next, prompt" loop )
 * and the 'replace' functionality. Same class, to allow centralizing the code that
 * finds the framesets and paragraphs to look into.
 */
class KWFindReplace : public KoFindReplace
{
    Q_OBJECT
public:
    KWFindReplace( KWCanvas * canvas, KoSearchDia * dialog, const QValueList<KoTextObject *> & lstObjects, KWTextFrameSetEdit* textView );
    KWFindReplace( KWCanvas * parent, KoReplaceDia * dialog, const QValueList<KoTextObject *> & lstObjects, KWTextFrameSetEdit* textView );
    ~KWFindReplace();

    virtual void emitNewCommand(KCommand *);
    virtual void highlightPortion(KoTextParag * parag, int index, int length, KoTextDocument *textdocument, KDialogBase* dialog);

private:
    KWCanvas *m_canvas;
};

#endif
