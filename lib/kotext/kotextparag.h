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

#ifndef kotextparag_h
#define kotextparag_h

#include "koparaglayout.h"
#include "qrichtext_p.h"
#include <qstring.h>
#include <qcolor.h>
#include "kotextformat.h"

using namespace Qt3;
class KoTextDocument;
class KoParagCounter;
class KoStyle;
class KoTextCustomItem;

/**
 * This class extends QTextParag with more (zoom-aware) features,
 * like linespacing, borders, counter, tabulators, etc.
 * It also implements WYSIWYG text drawing.
 */
class KoTextParag : public QTextParag
{
public:
    KoTextParag( QTextDocument *d, QTextParag *pr = 0, QTextParag *nx = 0, bool updateIds = TRUE );
    ~KoTextParag();

    KoTextDocument * textDocument() const;

    KoTextFormat * paragraphFormat() const
    { return static_cast<KoTextFormat *>( paragFormat() ); }

    /** Sets all or some parameters from a paraglayout struct.
     * @param flags selects which settings to apply, see KoParagLayout's enum. */
    virtual void setParagLayout( const KoParagLayout &layout, int flags = KoParagLayout::All );

    const KoParagLayout & paragLayout() { return m_layout; }

    // Margins
    double margin( QStyleSheetItem::Margin m ) { return m_layout.margins[m]; }
    const double * margins() const { return m_layout.margins; }
    void setMargin( QStyleSheetItem::Margin m, double _i );
    void setMargins( const double * _i );

    /** Line spacing in pt if >=0, can also be LS_ONEANDHALF or LS_DOUBLE */
    double kwLineSpacing() const { return m_layout.lineSpacing; }

    void setLineSpacing( double _i );

    /** Use this to change the paragraph alignment, not QTextParag::setAlignment ! */
    void setAlign( int align );

    // Borders
    KoBorder leftBorder() const { return m_layout.leftBorder; }
    KoBorder rightBorder() const { return m_layout.rightBorder; }
    KoBorder topBorder() const { return m_layout.topBorder; }
    KoBorder bottomBorder() const { return m_layout.bottomBorder; }
    bool hasBorder() const { return m_layout.hasBorder(); }

    void setLeftBorder( const KoBorder & _brd ) { m_layout.leftBorder = _brd; }
    void setRightBorder( const KoBorder & _brd ) { m_layout.rightBorder = _brd; }
    void setTopBorder( const KoBorder & _brd );
    void setBottomBorder( const KoBorder & _brd );

    // Counters are used to implement list and heading numbering/bullets.
    void setCounter( const KoParagCounter & counter );
    void setNoCounter();
    void setCounter( const KoParagCounter * pCounter )
    {
        if ( pCounter ) setCounter( *pCounter );
        else setNoCounter();
    }
    KoParagCounter *counter();

    /** The space required to draw the complete counter label (i.e. the Counter for this
     * paragraph, as well as the Counters for any paragraphs above us in the numbering
     * hierarchy). @see drawLabel(). */
    int counterWidth() const;

    /** Style used by this paragraph */
    KoStyle *style() const { return m_layout.style; }
    /** Sets the style in this paragraph, but doesn't _apply_ it, only sets a reference */
    void setStyle( KoStyle *style ) { m_layout.style = style; }
    /** Applies the style directly (without undo/redo! See KoTextObject for undo/redo) */
    void applyStyle( KoStyle *style );

    /** Get tabulator positions */
    const KoTabulatorList& tabList() const { return m_layout.tabList(); }
    /** Set tabulator positions */
    void setTabList( const KoTabulatorList &tabList );

    /** Set a @p custom item at position @p index, with format @p currentFormat (convenience method) */
    void setCustomItem( int index, KoTextCustomItem * custom, QTextFormat * currentFormat );
    /** Remove the custom item from position @p index, but doesn't delete it */
    void removeCustomItem( int index );

    /** Find a custom item that we know is somewhere in this paragraph
     * Returns the index in the paragraph */
    int findCustomItem( const QTextCustomItem * custom ) const;


    /** Reimplemented from QTextParag to implement non-left-aligned tabs */
    virtual int nextTab( int chnum, int x );

    virtual int topMargin() const;
    virtual int bottomMargin() const;
    virtual int leftMargin() const;
    virtual int firstLineMargin() const;
    virtual int rightMargin() const;
    virtual int lineSpacing( int line ) const;

    /** @return the parag rect, in pixels. This takes care of some rounding problems */
    QRect pixelRect() const;

protected:
    virtual void paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cusror = 0, bool drawSelections = FALSE,
			int clipx = -1, int clipy = -1, int clipw = -1, int cliph = -1 );

    /** Draw the complete label (i.e. heading/list numbers/bullets) for this paragraph. */
    virtual void drawLabel( QPainter* p, int x, int y, int w, int h, int base, const QColorGroup& cg );
    void invalidateCounters();
    void checkItem( QStyleSheetItem * & item, const char * name );

    /** Draw the cursor mark. Reimplemented from QTextParag to convert coordinates first. */
    virtual void drawCursor( QPainter &painter, QTextCursor *cursor, int curx, int cury, int curh, const QColorGroup &cg );
    /** Draw a set of characters with the same formattings.
     * Reimplemented from QTextParag to convert coordinates first, and call @ref drawFormattingChars. */
    virtual void drawParagString( QPainter &painter, const QString &s, int start, int len, int startX,
                                  int lastY, int baseLine, int bw, int h, bool drawSelections,
                                  QTextFormat *lastFormat, int i, const QMemArray<int> &selectionStarts,
                                  const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft );
    /** Hook for KWTextParag. Default implementation does nothing. */
    virtual void drawFormattingChars( QPainter &, const QString &, int, int, int,
                                      int , int, int, int, bool,
                                      QTextFormat *, int, const QMemArray<int> &,
                                      const QMemArray<int> &, const QColorGroup &, bool ) { }
protected:
    QStyleSheetItem * m_item;
    KoParagLayout m_layout;
};

#endif
