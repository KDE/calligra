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

#include "qrichtext_p.h"
#include "defs.h"
#include "border.h"

#include <qstring.h>
#include <qcolor.h>
#include <kwformat.h>
#include <koRuler.h>

using namespace Qt3;
class QDomDocument;
class KWTextFrameSet;
class KWTextDocument;
class KWTextCustomItem;
class KWDocument;
class KWTextParag;
class KWStyle;
class Counter;

/**
 * This class holds the paragraph-specific formatting information
 * It's separated from KWTextParag so that it can be copied in
 * the undo/redo history, and in KWStyle.
 */
class KWParagLayout
{
public:
    KWParagLayout();
    KWParagLayout( const KWParagLayout &layout ) { operator=( layout ); }
    // Load from XML, optionally using styles from document.
    KWParagLayout( QDomElement & parentElem, KWDocument *doc, bool useRefStyle );

    ~KWParagLayout();

    // Save to XML.
    void save( QDomElement & parentElem );

    // This enum is used to mark parts of a KWParagLayout as changed
    // (i.e. when changing them in the dialog/stylist)
    enum { Alignment = 1,
           BulletNumber = 2, // Can't use Counter : )
           Margins = 4,
           LineSpacing = 8,
           Borders = 16,
           Tabulator = 32,
           PageBreaking = 64,
           /* Style is maybe missing */
           All = Alignment | BulletNumber | Margins | LineSpacing | Borders | Tabulator | PageBreaking
    } Flags;

    enum {
        BreakBetweenLines = 0, // default
        KeepLinesTogether = 1,
        HardFrameBreakBefore = 2,      // incompatible with KeepWithPrevious
        HardFrameBreakAfter = 4,       // incompatible with KeepWithNext
        KeepWithPrevious = 8,          // incompatible with HardFrameBreakBefore
        KeepWithNext = 16              // incompatible with HardFrameBreakAfter
    }; // Page breaking flags

    // This class is used as a struct, which explains the public vars :)
    int alignment;
    double margins[5]; // left, right, top, bottom, firstLineSpacing - in pt
    enum { LS_ONEANDHALF = -1, LS_DOUBLE = -2 };
    double lineSpacing;
    Border leftBorder, rightBorder, topBorder, bottomBorder;
    Counter* counter; // can be 0 if no counter set
    int pageBreaking;

    KWStyle* style;

    void setTabList( const KoTabulatorList & tabList ) { m_tabList = tabList; }
    KoTabulatorList tabList() const { return m_tabList; }

    void operator=( const KWParagLayout & );

    // Return a set of flags showing the differences between this and 'layout'
    int compare( const KWParagLayout & layout ) const;

private:
    KoTabulatorList m_tabList;

    // Common setup.
    void initialise();
};

/**
 * This class extends QTextParag for KWord-specific formatting stuff
 */
class KWTextParag : public QTextParag
{
public:
    KWTextParag( QTextDocument *d, QTextParag *pr = 0, QTextParag *nx = 0, bool updateIds = TRUE );
    ~KWTextParag();

    KWTextDocument * textDocument() const;

    KWTextFormat * paragraphFormat() const
    { return static_cast<KWTextFormat *>( paragFormat() ); }

    // Sets all the parameters from a paraglayout struct
    void setParagLayout( const KWParagLayout &layout, int flags = KWParagLayout::All );
    const KWParagLayout & paragLayout() { return m_layout; }

    // Margins
    double margin( QStyleSheetItem::Margin m ) { return m_layout.margins[m]; }
    const double * margins() const { return m_layout.margins; }
    void setMargin( QStyleSheetItem::Margin m, double _i );
    void setMargins( const double * _i );

    // Line spacing in pt if >=0, can also be LS_ONEANDHALF or LS_DOUBLE
    double kwLineSpacing() const { return m_layout.lineSpacing; }

    void setLineSpacing( double _i );

    void setAlign( int align ); // use this, not QTextParag::setAlignment !

    // Borders
    Border leftBorder() const { return m_layout.leftBorder; }
    Border rightBorder() const { return m_layout.rightBorder; }
    Border topBorder() const { return m_layout.topBorder; }
    Border bottomBorder() const { return m_layout.bottomBorder; }

    void setLeftBorder( const Border & _brd ) { m_layout.leftBorder = _brd; }
    void setRightBorder( const Border & _brd ) { m_layout.rightBorder = _brd; }
    void setTopBorder( const Border & _brd );
    void setBottomBorder( const Border & _brd );

    // Counters are used to implement list and heading numbering/bullets.
    void setCounter( const Counter & counter );
    void setNoCounter();
    void setCounter( const Counter * pCounter )
    {
        if ( pCounter ) setCounter( *pCounter );
        else setNoCounter();
    }
    Counter *counter();

    // The space required to draw the complete counter label (i.e. the Counter for this
    // paragraph, as well as the Counters for any paragraphs above us in the numbering
    // hierarchy). See also drawLabel().
    int counterWidth() const;

    // Style
    KWStyle *style() const { return m_layout.style; }
    void setStyle( KWStyle *style ) { m_layout.style = style; } // doesn't _apply_ the style
    void applyStyle( KWStyle *style ); // applies the style (without undo/redo!)

    // The type of page-breaking behaviour
    void setPageBreaking( int pb ); // warning this sets all the flags!
    int pageBreaking() const { return m_layout.pageBreaking; }
    bool linesTogether() const { return m_layout.pageBreaking & KWParagLayout::KeepLinesTogether; }
    bool hardFrameBreakBefore() const { return m_layout.pageBreaking & KWParagLayout::HardFrameBreakBefore; }
    bool hardFrameBreakAfter() const { return m_layout.pageBreaking & KWParagLayout::HardFrameBreakAfter; }
    bool isPageBreakParag() const { return m_bPageBreak; }
    void setPageBreakParag( bool b );

    // Get and set tabulator positions
    KoTabulatorList tabList() const { return m_layout.tabList(); }
    void setTabList( const KoTabulatorList &tabList );

    // Public for KWStyle
    static QDomElement saveFormat( QDomDocument & doc, KWTextFormat * curFormat, KWTextFormat * refFormat, int pos, int len );
    static KWTextFormat loadFormat( QDomElement &formatElem, KWTextFormat * refFormat, const QFont & defaultFont );

    void save( QDomElement &parentElem )
    { save( parentElem, 0, length()-2 ); }
    void save( QDomElement &parentElem, int from, int to, bool saveAnchorsFramesets = false );

    void load( QDomElement &attributes );

    // Load and apply <FORMAT> tags (used by KWTextParag::load and by KWPasteCommand)
    void loadFormatting( QDomElement &attributes, int offset = 0 );

    // Load and apply paragraph layout
    void loadLayout( QDomElement & attributes );

    // Set a @p custom item at position @p index, with format @p currentFormat (convenience method)
    void setCustomItem( int index, KWTextCustomItem * custom, QTextFormat * currentFormat );
    // Remove the custom item from position @p index, but doesn't delete it
    void removeCustomItem( int index );

    // Find a custom item that we know is somewhere in this paragraph
    // Returns the index in the paragraph
    int findCustomItem( const QTextCustomItem * custom ) const;

#ifndef NDEBUG
    void printRTDebug( int );
#endif

    // Reimplemented from QTextParag to implement non-left-aligned tabs
    virtual int nextTab( int chnum, int x );

protected:
    // This is public in QTextParag but it should be internal to KWTextParag,
    // because it's in pixels.
    virtual int topMargin() const;
    virtual int bottomMargin() const;
    virtual int leftMargin() const;
    virtual int firstLineMargin() const;
    virtual int rightMargin() const;
    virtual int lineSpacing( int line ) const;

    virtual void paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cusror = 0, bool drawSelections = FALSE,
			int clipx = -1, int clipy = -1, int clipw = -1, int cliph = -1 );

    virtual void drawParagString( QPainter &painter, const QString &s, int start, int len, int startX,
                                  int lastY, int baseLine, int bw, int h, bool drawSelections,
                                  QTextFormat *lastFormat, int i, const QMemArray<int> &selectionStarts,
                                  const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft );

    // Draw the complete label (i.e. heading/list numbers/bullets) for this paragrpah.
    virtual void drawLabel( QPainter* p, int x, int y, int w, int h, int base, const QColorGroup& cg );
    virtual void copyParagData( QTextParag *_parag );
    void invalidateCounters();
    void checkItem( QStyleSheetItem * & item, const char * name );

private:
    QStyleSheetItem * m_item;
    KWParagLayout m_layout;
    bool m_bPageBreak;
};

#endif
