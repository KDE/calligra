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
#include "kwunit.h"
#include "border.h"

#include <qstring.h>
#include <qcolor.h>
#include <koRuler.h>

using namespace Qt3;
class QDomDocument;
class KWTextFrameSet;
class KWDocument;
class KWTextParag;
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
    KWParagLayout( const KWParagLayout &parag );
    // Load from XML, optionally using styles from document.
    KWParagLayout( QDomElement & parentElem, KWDocument *doc = 0L );

    ~KWParagLayout();

    // Save to XML.
    void save( QDomElement & parentElem );

    // This class used to be a struct, which explains the public vars :)
    int alignment;
    KWUnit margins[5]; // left, right, top, bottom, firstLineSpacing
    KWUnit lineSpacing;
    Border leftBorder, rightBorder, topBorder, bottomBorder;
    Counter* counter; // can be 0 if no counter set
    bool samePage;

    void setStyleName( const QString &styleName );
    QString styleName() const { return m_styleName; }

    void setTabList( const KoTabulatorList & tabList ) { m_tabList = tabList; }
    KoTabulatorList tabList() const { return m_tabList; }

private:
    QString m_styleName;
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

    // Creates a KWParagLayout structure from our info (proper+inherited), for undo/redo purposes
    const KWParagLayout & paragLayout() { return m_layout; }

    // Sets all the parameters from a paraglayout struct
    void setParagLayout( const KWParagLayout &layout );

    // Margins
    KWUnit margin( QStyleSheetItem::Margin m ) { return m_layout.margins[m]; }
    const KWUnit * margins() const { return m_layout.margins; }
    KWUnit kwLineSpacing() const { return m_layout.lineSpacing; }

    void setMargin( QStyleSheetItem::Margin m, KWUnit _i );
    void setMargins( const KWUnit * _i );
    void setLineSpacing( KWUnit _i );
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
    QString styleName() const { return m_layout.styleName(); }
    void setStyleName( const QString & style ) { m_layout.setStyleName( style ); }

    // Public for KWStyle
    static QDomElement saveFormat( QDomDocument & doc, QTextFormat * curFormat, QTextFormat * refFormat, int pos, int len );
    static QTextFormat loadFormat( QDomElement &formatElem, QTextFormat * refFormat );

    void save( QDomElement &parentElem, int from = 0, int to = -1 );
    void load( QDomElement &attributes );

    // Load and apply <FORMAT> tags (used by KWTextParag::load and by KWPasteCommand)
    void loadFormatting( QDomElement &attributes, int offset = 0 );

    // Load and apply paragraph layout
    void loadLayout( QDomElement & attributes );

    void setCustomItem( int index, QTextCustomItem * custom, QTextFormat * currentFormat );

    KoTabulatorList tabList() const { return m_layout.tabList(); }
    void setTabList( const KoTabulatorList &tabList ) { m_layout.setTabList( tabList ); }

    void printRTDebug();

protected:
    // This is public in QTextParag but it should be internal to KWTextParag,
    // because it's in pixels.
    virtual int topMargin() const;
    virtual int bottomMargin() const;
    virtual int leftMargin() const;
    virtual int firstLineMargin() const;
    virtual int rightMargin() const;
    virtual int lineSpacing() const;

    virtual void paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cusror = 0, bool drawSelections = FALSE,
			int clipx = -1, int clipy = -1, int clipw = -1, int cliph = -1 );

    // Draw the complete label (i.e. heading/list numbers/bullets) for this paragrpah.
    virtual void drawLabel( QPainter* p, int x, int y, int w, int h, int base, const QColorGroup& cg );
    virtual void copyParagData( QTextParag *_parag );
    void invalidateCounters();
    void checkItem( QStyleSheetItem * & item, const char * name );

private:
    QStyleSheetItem * m_item;
    KWParagLayout m_layout;
};

/**
 * This is our QTextDocument reimplementation, to create KWTextParag instead of QTextParags,
 * and to relate it to the text frameset it's in.
 */
class KWTextDocument : public QTextDocument
{
    Q_OBJECT
public:
    KWTextDocument( KWTextFrameSet * textfs, QTextDocument *p ) : QTextDocument( p ), m_textfs( textfs ) {
        // QTextDocument::QTextDocument creates a parag, but too early for our createParag to get called !
        // So we have to get rid of it.
        clear( true );
        // Using clear( false ) is a bit dangerous, since we don't always check cursor->parag() for != 0
    }

    virtual QTextParag * createParag( QTextDocument *d, QTextParag *pr = 0, QTextParag *nx = 0, bool updateIds = TRUE )
    {
        return new KWTextParag( d, pr, nx, updateIds );
    }

    KWTextFrameSet * textFrameSet() const { return m_textfs; }

private:
    KWTextFrameSet * m_textfs;
};

#endif
