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

#ifndef searchdia_h
#define searchdia_h

#include "qrichtext_p.h"
#include "kptextobject.h"
#include <koSearchDia.h>
using namespace Qt3;

class Page;
class KMacroCommand;
class KPTextView;
class KoTextObject;
class KoFindReplace;
class KoReplaceDia;
class KoSearchDia;
class KPTextView;
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
    KPrFindReplace( Page * page, KoSearchDia * dialog , KPTextView *textView ,const QPtrList<KoTextObject> & lstObject);
    KPrFindReplace( Page * page, KoReplaceDia * dialog, KPTextView *textView,const QPtrList<KoTextObject> & lstObject);
    ~KPrFindReplace();
    
    virtual void emitNewCommand(KCommand *);
    virtual void highlightPortion(Qt3::QTextParag * parag, int index, int length, KoTextDocument *textdoc);

private:
    Page *m_page;
};

#endif
