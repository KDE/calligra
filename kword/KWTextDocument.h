/* This file is part of the KDE project
   Copyright (C) 2001-2005 David Faure <faure@kde.org>

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

#ifndef kwtextdocument_h
#define kwtextdocument_h

#include "KoRichText.h"
#include <KoTextDocument.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3MemArray>

class KWFrame;
class KWTextFrameSet;
class KoTextFormatCollection;
class KoTextDocCommand;

/**
 * This is our KoTextDocument reimplementation, to create KWTextParag instead of KoTextParags,
 * and to relate it to the text frameset it's in.
 */
class KWTextDocument : public KoTextDocument
{
    Q_OBJECT
public:
    /** A real text document inside a frameset */
    KWTextDocument( KWTextFrameSet * textfs, KoTextFormatCollection *fc, KoTextFormatter *formatter = 0L );
    /** A standalone text document, for a preview */
    KWTextDocument( KoTextZoomHandler * zoomHandler );

    ~KWTextDocument();

    virtual KoTextParag * createParag( KoTextDocument *d, KoTextParag *pr = 0, KoTextParag *nx = 0, bool updateIds = true );

    /** Return the text frameset in which this document is.
     * Note that this can be 0L (e.g. for paragraphs in the paragdia preview) */
    KWTextFrameSet * textFrameSet() const { return m_textfs; }

    virtual KoTextDocCommand *deleteTextCommand( KoTextDocument *textdoc, int id, int index, const Q3MemArray<KoTextStringChar> & str, const CustomItemsMap & customItemsMap, const Q3ValueList<KoParagLayout> & oldParagLayouts );

    /// Extensions to KoTextParag::loadOasisSpan
    virtual bool loadSpanTag( const QDomElement& tag, KoOasisContext& context,
                              KoTextParag* parag, uint pos,
                              QString& textData, KoTextCustomItem* & customItem );
    /// Extensions to KoTextDocument::loadOasisText
    virtual bool loadOasisBodyTag( const QDomElement& tag, KoOasisContext& context,
                                   KoTextParag* & lastParagraph, KoStyleCollection * styleColl,
                                   KoTextParag* nextParagraph );
protected:
    void init();
private:
    void appendBookmark( KoTextParag* parag, int pos, KoTextParag* endParag, int endPos, const QString& name );
    void loadOasisFootnote( const QDomElement& tag, KoOasisContext& context,
                            KoTextCustomItem* & customItem );
    void loadOasisTOC( const QDomElement& tag, KoOasisContext& context, KoTextParag* & lastParagraph, KoStyleCollection* styleColl, KoTextParag* nextParagraph );

    KWTextFrameSet * m_textfs;
};

#endif
