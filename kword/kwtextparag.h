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

#ifndef kwtextparag_h
#define kwtextparag_h

#include <kotextparag.h>

class QDomDocument;
class KWTextFrameSet;
class KWTextDocument;
class KWDocument;
class KWTextParag;
class KWStyle;

// At the moment those are the same - to be discussed
typedef KoParagLayout KWParagLayout;

/**
 * This class extends KoTextParag for KWord-specific formatting stuff,
 * custom items, loading and saving.
 */
class KWTextParag : public KoTextParag
{
public:
    KWTextParag( QTextDocument *d, QTextParag *pr = 0, QTextParag *nx = 0, bool updateIds = TRUE )
        : KoTextParag( d, pr, nx, updateIds ) { }
    ~KWTextParag() { }

    KWTextDocument * kwTextDocument() const;

    virtual void setParagLayout( const KoParagLayout &layout, int flags = KoParagLayout::All );

    /** The type of page-breaking behaviour */
    void setPageBreaking( int pb ); // warning this sets all the flags!
    int pageBreaking() const { return m_layout.pageBreaking; }
    bool linesTogether() const { return m_layout.pageBreaking & KoParagLayout::KeepLinesTogether; }
    bool hardFrameBreakBefore() const { return m_layout.pageBreaking & KoParagLayout::HardFrameBreakBefore; }
    bool hardFrameBreakAfter() const { return m_layout.pageBreaking & KoParagLayout::HardFrameBreakAfter; }

    /** Public for loading a style */
    static KoTextFormat loadFormat( QDomElement &formatElem, KoTextFormat *refFormat, const QFont &defaultFont, KoZoomHandler *zh );
    /** Public for saving a style */
    static QDomElement saveFormat( QDomDocument &doc, KoTextFormat *curFormat, KoTextFormat *refFormat, int pos, int len, KoZoomHandler *zh );


    /** Save the whole paragraph */
    void save( QDomElement &parentElem, bool saveAnchorsFramesets = false )
    { save( parentElem, 0, length()-2, saveAnchorsFramesets ); }

    /** Save a portion of the paragraph */
    void save( QDomElement &parentElem, int from, int to, bool saveAnchorsFramesets = false );

    /** Load the paragraph */
    void load( QDomElement &attributes );

    /** Load and apply <FORMAT> tags (used by KWTextParag::load and by KWPasteCommand) */
    void loadFormatting( QDomElement &attributes, int offset = 0 );

    /** Load and apply paragraph layout */
    void loadLayout( QDomElement & attributes );

    /** Load from XML, optionally using styles from document. */
    static KoParagLayout loadParagLayout( QDomElement & parentElem, KWDocument *doc, bool useRefStyle );
    static void saveParagLayout( const KoParagLayout& layout, QDomElement & parentElem );

#ifndef NDEBUG
    void printRTDebug( int );
#endif

protected:
    virtual void copyParagData( QTextParag *_parag );
    virtual void drawFormattingChars( QPainter &painter, const QString &s, int start, int len, int startX,
                                      int lastY, int baseLine, int bw, int h, bool drawSelections,
                                      QTextFormat *lastFormat, int i, const QMemArray<int> &selectionStarts,
                                      const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft );

};

#endif
