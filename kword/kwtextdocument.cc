/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kwtextdocument.h"
#include "kwtextparag.h"
#include "kwdoc.h"
#include "kwtextframeset.h"
#include <kdebug.h>
#include "kwcommand.h"

KWTextDocument::KWTextDocument( KWTextFrameSet * textfs, KoTextFormatCollection *fc, KoTextFormatter *formatter )
    : KoTextDocument( textfs->kWordDocument(), fc, formatter, false ), m_textfs( textfs )
{
    init();
}

KWTextDocument::KWTextDocument( KoZoomHandler * zoomHandler )
    : KoTextDocument( zoomHandler, new KoTextFormatCollection( QFont("helvetica") /*unused*/, QColor() ), 0L, false ),
      m_textfs( 0 )
{
    init();
}

void KWTextDocument::init()
{
    // Create initial paragraph as a KWTextParag
    clear( true );
}

KWTextDocument::~KWTextDocument()
{
}

KoTextParag * KWTextDocument::createParag( KoTextDocument *d, KoTextParag *pr, KoTextParag *nx, bool updateIds )
{
    return new KWTextParag( static_cast<KoTextDocument *>(d), static_cast<KoTextParag *>(pr), static_cast<KoTextParag *>(nx), updateIds );
}

KoTextDocCommand *KWTextDocument::deleteTextCommand( KoTextDocument *textdoc, int id, int index, const QMemArray<KoTextStringChar> & str, const CustomItemsMap & customItemsMap, const QValueList<KoParagLayout> & oldParagLayouts )
{
    //kdDebug()<<" KoTextDocument::deleteTextCommand************\n";
    return new KWTextDeleteCommand( textdoc, id, index, str, customItemsMap, oldParagLayouts );
}


#include "kwtextdocument.moc"
