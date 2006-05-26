// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef searchdia_h
#define searchdia_h

#include "KPrTextObject.h"
#include <KoSearchDia.h>
//Added by qt3to4:
#include <Q3ValueList>

class KPrCanvas;
class KMacroCommand;
class KPrTextView;
class KoTextObject;
class KoFindReplace;
class KoReplaceDia;
class KoSearchDia;
class KPrTextView;
class KPrFindReplace;
class KCommand;
/**
 * This class implements the 'find' functionality ( the "search next, prompt" loop )
 * and the 'replace' functionality. Same class, to allow centralizing the code that
 * finds the framesets and paragraphs to look into.
 */
class KPrFindReplace : public KoFindReplace
{
    Q_OBJECT
public:
    /**
     * The parent widget can't be the canvas, otherwise the 'find next' dialog
     * scrolls up/down when using the scrollbars (hehe, cool effect).
     */
    KPrFindReplace( QWidget* parent, KPrCanvas * canvas, KoSearchDia * dialog,
                    const Q3ValueList<KoTextObject *> & lstObjects, KPrTextView *textView );
    KPrFindReplace( QWidget* parent, KPrCanvas * canvas, KoReplaceDia * dialog,
                    const Q3ValueList<KoTextObject *> & lstObjects, KPrTextView *textView );
    ~KPrFindReplace();

    virtual void emitNewCommand(KCommand *);
    virtual void highlightPortion(KoTextParag * parag, int index, int length, KoTextDocument *textdoc, KDialogBase* dialog);

private:
    KPrCanvas *m_canvas;
};

#endif
