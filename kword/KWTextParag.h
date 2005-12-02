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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwtextparag_h
#define kwtextparag_h

#include "KoTextParag.h"

class KoStyleCollection;
class QDomDocument;
class KWTextFrameSet;
class KWTextDocument;
class KWDocument;
class KWTextParag;
class KoParagStyle;

// At the moment those are the same - to be discussed
typedef KoParagLayout KWParagLayout;

/**
 * This class extends KoTextParag for KWord-specific formatting stuff,
 * custom items, loading and saving.
 */
class KWTextParag : public KoTextParag
{
public:
    KWTextParag( KoTextDocument *d, KoTextParag *pr = 0, KoTextParag *nx = 0, bool updateIds = TRUE )
        : KoTextParag( d, pr, nx, updateIds ) { }
    ~KWTextParag() { }

    KWTextDocument * kwTextDocument() const;

    virtual void setParagLayout( const KoParagLayout &layout, int flags = KoParagLayout::All, int marginIndex = -1 );

    /** The type of page-breaking behaviour */
    void setPageBreaking( int pb ); // warning this sets all the flags!
    int pageBreaking() const { return m_layout.pageBreaking; }
    bool linesTogether() const { return m_layout.pageBreaking & KoParagLayout::KeepLinesTogether; }
    bool hardFrameBreakBefore() const { return m_layout.pageBreaking & KoParagLayout::HardFrameBreakBefore; }
    bool hardFrameBreakAfter() const { return m_layout.pageBreaking & KoParagLayout::HardFrameBreakAfter; }

    /** Public for loading a style (KWord-1.3 XML) */
    static KoTextFormat loadFormat( QDomElement &formatElem, KoTextFormat *refFormat, const QFont &defaultFont, const QString & defaultLanguage, bool hyphanation );
    /** Public for saving a style (KWord-1.3 XML) */
    static QDomElement saveFormat( QDomDocument &doc, KoTextFormat *curFormat, KoTextFormat *refFormat, int pos, int len );


    /** Save (to KWord-1.3 XML) the whole paragraph */
    void save( QDomElement &parentElem, bool saveAnchorsFramesets = false );

    /** Save (to KWord-1.3 XML) a portion of the paragraph */
    void save( QDomElement &parentElem, int from, int to, bool saveAnchorsFramesets = false );

    /** Load (from KWord-1.3 XML) the paragraph */
    void load( QDomElement &attributes );

    /** Load (from KWord-1.3 XML) and apply \<FORMAT\> tags (used by KWTextParag::load and by KWPasteCommand) */
    void loadFormatting( QDomElement &attributes, int offset = 0, bool loadFootNote=true );

    /** Load (from KWord-1.3 XML) and apply paragraph layout */
    void loadLayout( QDomElement & attributes );

    /** Load (from KWord-1.3 XML), optionally using styles from document. */
    static KoParagLayout loadParagLayout( QDomElement & parentElem, KWDocument *doc, bool useRefStyle );

    virtual void loadOasis( const QDomElement& e, KoOasisContext& context, KoStyleCollection *styleCollection, uint& pos );
    virtual void saveOasis( KoXmlWriter& writer, KoSavingContext& context,
                            int from, int to, bool saveAnchorsFramesets = false ) const;

    virtual void join( KoTextParag *parag );
protected:
    virtual void drawFormattingChars( QPainter &painter, int start, int len,
                                      int lastY_pix, int baseLine_pix, int h_pix, // in pixels
                                      bool drawSelections,
                                      KoTextFormat *lastFormat, const QMemArray<int> &selectionStarts,
                                      const QMemArray<int> &selectionEnds, const QColorGroup &cg,
                                      bool rightToLeft, int line, KoTextZoomHandler* zh,
                                      int whichFormattingChars );

};

#endif
