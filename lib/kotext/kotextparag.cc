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

#include "kotextparag.h"
#include "kotextdocument.h"
#include "kotextformat.h"
#include "koparagcounter.h"
#include "kozoomhandler.h"
#include "kostyle.h"
#include <kglobal.h>
#include <klocale.h>
#include <assert.h>
#include <koGlobal.h>
#include <kdebug.h>

// Create a default KoParagLayout.
KoParagLayout::KoParagLayout()
{
    initialise();
}

void KoParagLayout::operator=( const KoParagLayout &layout )
{
    alignment = layout.alignment;
    for ( int i = 0 ; i < 5 ; ++i )
        margins[i] = layout.margins[i];
    pageBreaking = layout.pageBreaking;
    leftBorder = layout.leftBorder;
    rightBorder = layout.rightBorder;
    topBorder = layout.topBorder;
    bottomBorder = layout.bottomBorder;
    if ( layout.counter )
        counter = new KoParagCounter( *layout.counter );
    else
        counter = 0L;
    lineSpacing = layout.lineSpacing;
    style = layout.style;
    setTabList( layout.tabList() );
}

int KoParagLayout::compare( const KoParagLayout & layout ) const
{
    int flags = 0;
    if ( alignment != layout.alignment )
        flags |= Alignment;
    for ( int i = 0 ; i < 5 ; ++i )
        if ( margins[i] != layout.margins[i] )
        {
            flags |= Margins;
            break;
        }
    if ( pageBreaking != layout.pageBreaking )
        flags |= PageBreaking;
    if ( leftBorder != layout.leftBorder
         || rightBorder != layout.rightBorder
         || topBorder != layout.topBorder
         || bottomBorder != layout.bottomBorder )
        flags |= Borders;

    if ( layout.counter )
    {
        if ( counter )
        {
            if ( ! ( *layout.counter == *counter ) )
                flags |= BulletNumber;
        } else
            if ( layout.counter->numbering() != KoParagCounter::NUM_NONE )
                flags |= BulletNumber;
    }
    else
        if ( counter && counter->numbering() != KoParagCounter::NUM_NONE )
            flags |= BulletNumber;

    if ( lineSpacing != layout.lineSpacing )
        flags |= LineSpacing;
    //if ( style != layout.style )
    //    flags |= Style;
    if ( m_tabList != m_tabList )
        flags |= Tabulator;
    return flags;
}

void KoParagLayout::initialise()
{
    alignment = Qt::AlignLeft;
    for ( int i = 0 ; i < 5 ; ++i ) // use memset ?
        margins[i] = 0;
    lineSpacing = 0;
    counter = 0L;
    leftBorder.ptWidth = 0;
    rightBorder.ptWidth = 0;
    topBorder.ptWidth = 0;
    bottomBorder.ptWidth = 0;
    pageBreaking = 0;
    style = 0L;
    m_tabList.clear();
}

KoParagLayout::~KoParagLayout()
{
    delete counter;
}

/////

KoTextParag::KoTextParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds)
    : QTextParag( d, pr, nx, updateIds )
{
    //kdDebug() << "KoTextParag::KoTextParag " << this << endl;
    m_item = 0L;
}

KoTextParag::~KoTextParag()
{
    if ( !textDocument()->isDestroying() )
        invalidateCounters();
    //kdDebug() << "KoTextParag::~KoTextParag " << this << endl;
    delete m_item;
}

KoTextDocument * KoTextParag::textDocument() const
{
    return static_cast<KoTextDocument *>( document() );
}

// There is one QStyleSheetItems per paragraph, created on demand,
// in order to set the DisplayMode for counters.
void KoTextParag::checkItem( QStyleSheetItem * & item, const char * name )
{
    if ( !item )
    {
        item = new QStyleSheetItem( 0, QString::fromLatin1(name) /* For debugging purposes only */ );
        QVector<QStyleSheetItem> vec = styleSheetItems();
        vec.resize( vec.size() + 1 );
        vec.insert( vec.size() - 1, item );
        //kdDebug() << "KoTextParag::checkItem inserting QStyleSheetItem " << name << " at position " << vec.size()-1 << endl;
        setStyleSheetItems( vec );
    }
}

// Return the counter associated with this paragraph.
KoParagCounter *KoTextParag::counter()
{
    if ( !m_layout.counter )
        return 0L;

    // Garbage collect unnneeded counters.
    if ( m_layout.counter->numbering() == KoParagCounter::NUM_NONE )
        setNoCounter();
    return m_layout.counter;
}

void KoTextParag::setMargin( QStyleSheetItem::Margin m, double _i )
{
    //kdDebug() << "KoTextParag::setMargin " << m << " margin " << _i << endl;
    m_layout.margins[m] = _i;
    if ( m == QStyleSheetItem::MarginTop && prev() )
        prev()->invalidate(0);     // for top margin (post-1.1: remove this, not necessary anymore)
    invalidate(0);
}

void KoTextParag::setMargins( const double * margins )
{
    for ( int i = 0 ; i < 5 ; ++i )
        m_layout.margins[i] = margins[i];
    if ( prev() )
        prev()->invalidate(0);     // for top margin (post-1.1: remove this, not necessary anymore)
    invalidate(0);
}

void KoTextParag::setAlign( int align )
{
    setAlignment( align );
    m_layout.alignment = align;
}

void KoTextParag::setLineSpacing( double _i )
{
    m_layout.lineSpacing = _i;
    invalidate(0);
}

void KoTextParag::setTopBorder( const KoBorder & _brd )
{
    m_layout.topBorder = _brd;
    if ( prev() )
        prev()->invalidate(0);     // for top margin (post-1.1: remove this, not necessary anymore)
    invalidate(0);
}

void KoTextParag::setBottomBorder( const KoBorder & _brd )
{
    m_layout.bottomBorder = _brd;
    invalidate(0);
}

void KoTextParag::setNoCounter()
{
    delete m_layout.counter;
    m_layout.counter = 0L;
    invalidateCounters();
}

void KoTextParag::setCounter( const KoParagCounter & counter )
{
    // Garbage collect unnneeded counters.
    if ( counter.numbering() == KoParagCounter::NUM_NONE )
    {
        setNoCounter();
    }
    else
    {
        delete m_layout.counter;
        m_layout.counter = new KoParagCounter( counter );

        checkItem( m_item, "m_item" );
        // Set the display mode (in order for drawLabel to get called by QTextParag)
        m_item->setDisplayMode( QStyleSheetItem::DisplayListItem );

        // Invalidate the counters
        invalidateCounters();
    }
}

void KoTextParag::invalidateCounters()
{
    // Invalidate this paragraph and all the following ones
    // (Numbering may have changed)
    invalidate( 0 );
    if ( m_layout.counter )
        m_layout.counter->invalidate();
    KoTextParag *s = static_cast<KoTextParag *>( next() );
    while ( s ) {
        if ( s->m_layout.counter )
            s->m_layout.counter->invalidate();
        s->invalidate( 0 );
        s = static_cast<KoTextParag *>( s->next() );
    }
}

int KoTextParag::counterWidth() const
{
    if ( !m_layout.counter )
        return 0;

    return m_layout.counter->width( this );
}

// Draw the counter/bullet for a paragraph
void KoTextParag::drawLabel( QPainter* p, int x, int y, int /*w*/, int h, int base, const QColorGroup& /*cg*/ )
{
    if ( !m_layout.counter ) // shouldn't happen
        return;

    if ( m_layout.counter->numbering() == KoParagCounter::NUM_NONE )
    {   // Garbage collect unnneeded counter.
        delete m_layout.counter;
        m_layout.counter = 0L;
        return;
    }

    int size = m_layout.counter->width( this );

    // Draw the complete label.
    QTextFormat *format = at( 0 )->format(); // paragFormat();
    p->save();
    QPen tmpPen=p->pen();

    QColor newColor(format->color());
    QFont newFont(format->font());

    p->setFont( newFont );
    tmpPen.setColor( newColor);
    p->setPen(tmpPen);

    // Now draw any bullet that is required over the space left for it.
    if ( m_layout.counter->isBullet() )
    {
        // Modify x offset.
        QString suffix = m_layout.counter->suffix() + ' ' /*the trailing space*/;
        for ( unsigned int i = 0; i < suffix.length(); i++ )
            x -= format->width( suffix, i );
        int width = format->width( ' ' );
        int height = format->height();
        QRect er( x - width, y - h + height / 2 - width / 2, width, width );
        // Draw the bullet.
        switch ( m_layout.counter->style() )
        {
            case KoParagCounter::STYLE_DISCBULLET:
                p->setBrush( QBrush(newColor) );
                p->drawEllipse( er );
                p->setBrush( Qt::NoBrush );
                break;
            case KoParagCounter::STYLE_SQUAREBULLET:
                p->fillRect( er , QBrush(newColor) );
                break;
            case KoParagCounter::STYLE_CIRCLEBULLET:
                p->drawEllipse( er );
                break;
            case KoParagCounter::STYLE_CUSTOMBULLET:
                // The user has selected a symbol from a special font. Override the paragraph
                // font with the given family. This conserves the right size etc.
                if ( !m_layout.counter->customBulletFont().isEmpty() )
                {
                    newFont.setFamily( m_layout.counter->customBulletFont() );
                    p->setFont( newFont );
                }
                p->drawText( x - width, y - h + base, m_layout.counter->customBulletCharacter() );
                break;
            default:
                break;
        }
        // TODO draw suffix text ?
    }
    else
    {
        // There are no bullets...any parent bullets have already been suppressed.
        // Just draw the text! Note: one space is always appended.
        QString counterText = m_layout.counter->text( this );
        if ( !counterText.isEmpty() )
        {
            //code from qt3stuff
            if (format->vAlign() == QTextFormat::AlignSuperScript )
            {
                QFont tmpFont( p->font() );
                tmpFont.setPointSize( ( tmpFont.pointSize() * 2 ) / 3 );
                p->setFont( tmpFont );
                base=base*2/3;
            }
            else if ( format->vAlign() == QTextFormat::AlignSubScript )
            {
                QFont tmpFont( p->font() );
                tmpFont.setPointSize( ( tmpFont.pointSize() * 2 ) / 3 );
                p->setFont( tmpFont );
            }
            p->drawText( x - size, y - h + base, counterText + ' ' );
        }
    }
    p->restore();
}

int KoTextParag::topMargin() const
{
    KoZoomHandler * zh = textDocument()->zoomHandler();
    return zh->ptToLayoutUnit( m_layout.margins[ QStyleSheetItem::MarginTop ] )
        + KoBorder::zoomWidthY( m_layout.topBorder.ptWidth, zh, 0 );
}

int KoTextParag::bottomMargin() const
{
    KoZoomHandler * zh = textDocument()->zoomHandler();
    return zh->ptToLayoutUnit( m_layout.margins[ QStyleSheetItem::MarginBottom ] )
        + KoBorder::zoomWidthY( m_layout.bottomBorder.ptWidth, zh, 0 );
}

int KoTextParag::leftMargin() const
{
    KoZoomHandler * zh = textDocument()->zoomHandler();
    return zh->ptToLayoutUnit( m_layout.margins[ QStyleSheetItem::MarginLeft ] )
        + KoBorder::zoomWidthX( m_layout.leftBorder.ptWidth, zh, 0 )
        + counterWidth() /* in layout units already */;
}

int KoTextParag::rightMargin() const
{
    KoZoomHandler * zh = textDocument()->zoomHandler();
    return zh->ptToLayoutUnit( m_layout.margins[ QStyleSheetItem::MarginRight ] )
        + KoBorder::zoomWidthX( m_layout.rightBorder.ptWidth, zh, 0 );
}

int KoTextParag::firstLineMargin() const
{
    return textDocument()->zoomHandler()->ptToLayoutUnit(
        m_layout.margins[ QStyleSheetItem::MarginFirstLine ] );
}

int KoTextParag::lineSpacing( int line ) const
{
    if ( m_layout.lineSpacing >= 0 )
        return textDocument()->zoomHandler()->ptToLayoutUnit(
            m_layout.lineSpacing );
    else {
        KoTextParag * that = const_cast<KoTextParag *>(this);
        if( line >= (int)that->lineStartList().count() )
        {
            kdError() << "KoTextParag::lineSpacing assert(line<lines) failed: line=" << line << " lines=" << that->lineStartList().count() << endl;
            return 0;
        }
        QMap<int, QTextParagLineStart*>::ConstIterator it = that->lineStartList().begin();
        while ( line-- > 0 )
            ++it;
        int height = ( *it )->h;
        //kdDebug() << " line height=" << height << " valid=" << isValid() << endl;

        if ( m_layout.lineSpacing == KoParagLayout::LS_ONEANDHALF )
        {
            // Tricky. During formatting height doesn't include the linespacing,
            // but afterwards (e.g. when drawing the cursor), it does !
            return isValid() ? height / 3 : height / 2;
        }
        else if ( m_layout.lineSpacing == KoParagLayout::LS_DOUBLE )
        {
            return isValid() ? height / 2 : height;
        }
    }
    kdWarning() << "Unhandled linespacing value : " << m_layout.lineSpacing << endl;
    return 0;
}

// Reimplemented from QTextParag
void KoTextParag::paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cursor, bool drawSelections,
                         int clipx, int clipy, int clipw, int cliph )
{
#if 1
    // We force the alignment to justify during drawing, so that drawParagString is called
    // for at most one word at a time, never more. This allows to make the spaces slightly
    // bigger to compensate for the rounding problems.
    int realAlignment = alignment();
    setAlignment( Qt3::AlignJustify );
#endif
    //qDebug("KoTextParag::paint %p", this);
    QTextParag::paint( painter, cg, cursor, drawSelections, clipx, clipy, clipw, cliph );

#if 1
    setAlignment( realAlignment );
#endif

    // Now draw paragraph border
    if ( m_layout.hasBorder() )
    {
        KoZoomHandler * zh = textDocument()->zoomHandler();

        QRect r;
        // Old solution: stick to the text
        //r.setLeft( at( 0 )->x - counterWidth() - 1 );
        //r.setRight( rect().width() - rightMargin() - 1 );

        // New solution: occupy the full width
        r.setLeft( KoBorder::zoomWidthX( m_layout.leftBorder.ptWidth, zh, 0 ) );
        // ## documentWidth breaks with variable width. Maybe use currentDrawnFrame() ?
        r.setRight( documentWidth() - 2 - KoBorder::zoomWidthX( m_layout.rightBorder.ptWidth, zh, 0 ) );
        r.setTop( lineY( 0 ) );
        int lastLine = lines() - 1;
        r.setBottom( static_cast<int>( lineY( lastLine ) + lineHeight( lastLine ) ) );
        // If we don't have a bottom border, we need go as low as possible ( to touch the next parag's border ).
        // If we have a bottom border, then we rather exclude the linespacing. Just looks nicer IMHO.
        if ( m_layout.bottomBorder.ptWidth > 0 )
            r.rBottom() -= lineSpacing( lastLine ) + 1;

        //kdDebug() << "KoTextParag::paint documentWidth=" << documentWidth() << " r=" << DEBUGRECT( r ) << endl;
        KoBorder::drawBorders( painter, zh, r,
                               m_layout.leftBorder, m_layout.rightBorder, m_layout.topBorder, m_layout.bottomBorder,
                               0, QPen() );
    }
}

// Reimplemented from QTextParag
void KoTextParag::drawParagString( QPainter &painter, const QString &s, int start, int len, int startX,
                                   int lastY, int baseLine, int bw, int h, bool drawSelections,
                                   QTextFormat *lastFormat, int i, const QMemArray<int> &selectionStarts,
                                   const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft )
{
    KoTextFormat localFormat( *static_cast<KoTextFormat *>(lastFormat) );
    if ( !lastFormat->color().isValid() ) // Resolve the color at this point
        localFormat.setColor( KoTextFormat::defaultTextColor( &painter ) );

    bool forPrint = ( painter.device()->devType() == QInternal::Printer );
    KoZoomHandler * zh = textDocument()->zoomHandler();
    localFormat.setPointSizeFloat( zh->layoutUnitToFontSize( localFormat.font().pointSize(), forPrint ) );
    QFontInfo fi( localFormat.font() );
    //kdDebug() << "KoTextParag::drawParagString requested font " << localFormat.font().pointSizeFloat() << " using font " << fi.pointSize() << " (pt for layout-unit size " << lastFormat->font().pointSizeFloat() << ")" << endl;
    //kdDebug() << "KoTextParag::drawParagString in pixelsizes : " << localFormat.font().pixelSize() << " (" << lastFormat->font().pixelSize() << " for lu)" << endl;

    lastFormat = &localFormat;

    //kdDebug() << "startX in LU: " << startX << " layoutUnitToPt( startX )*zoomedResolutionX : " << zh->layoutUnitToPt( startX ) << "*" << zh->zoomedResolutionX() << endl;
    startX = zh->layoutUnitToPixelX( startX ) + at( start )->pixelxadj;
    //kdDebug() << "KoTextParag::drawParagString startX in pixels : " << startX << endl;

    lastY = zh->layoutUnitToPixelY( lastY );
    baseLine = zh->layoutUnitToPixelY( baseLine );
    bw = zh->layoutUnitToPixelX( bw );
    h = zh->layoutUnitToPixelY( h );

    // From now on lastFormat, startX, lastY etc. have zoom-related (pixel) sizes.
    QTextParag::drawParagString( painter, s, start, len, startX,
                                 lastY, baseLine, bw, h, drawSelections,
                                 lastFormat, i, selectionStarts,
                                 selectionEnds, cg, rightToLeft );

    drawFormattingChars( painter, s, start, len, startX,
                         lastY, baseLine, bw, h, drawSelections,
                         lastFormat, i, selectionStarts,
                         selectionEnds, cg, rightToLeft );
}

// Reimplemented from QTextParag
void KoTextParag::drawCursor( QPainter &painter, QTextCursor *cursor, int curx, int cury, int curh, const QColorGroup &cg )
{
    KoZoomHandler * zh = textDocument()->zoomHandler();
    int x = zh->layoutUnitToPixelX( curx ) + cursor->parag()->at( cursor->index() )->pixelxadj;
    qDebug("  drawCursor: LU: [cur]x=%d -> PIX: x=%d", curx, x );
    QTextParag::drawCursor( painter, cursor, x,
                            zh->layoutUnitToPixelY( cury ),
                            zh->layoutUnitToPixelY( curh ), cg );
}

void KoTextParag::setTabList( const KoTabulatorList &tabList )
{
    KoTabulatorList lst( tabList );
    m_layout.setTabList( lst );
    if ( !tabList.isEmpty() )
    {
        KoZoomHandler * zh = textDocument()->zoomHandler();
        int * tabs = new int[ tabList.count() + 1 ]; // will be deleted by ~QTextParag
        KoTabulatorList::Iterator it = lst.begin();
        unsigned int i = 0;
        for ( ; it != lst.end() ; ++it, ++i )
            tabs[i] = zh->ptToLayoutUnit( (*it).ptPos );
        tabs[i] = 0;
        assert( i == tabList.count() );
        setTabArray( tabs );
    } else
    {
        setTabArray( 0 );
    }
    invalidate( 0 );
}

int KoTextParag::nextTab( int chnum, int x )
{
    if ( !m_layout.tabList().isEmpty() )
    {
        // Fetch the zoomed and sorted tab positions from QTextParag
        // We stored them there for faster access
        int * tArray = tabArray();
        int i = 0;
        while ( tArray[ i ] ) {
            //kdDebug() << "KoTextParag::nextTab tArray[" << i << "]=" << tArray[i] << " type " << m_layout.tabList()[i].type << endl;
            if ( tArray[ i ] >= x ) {
                int type = m_layout.tabList()[i].type;
                switch ( type ) {
                case T_RIGHT:
                case T_CENTER:
                {
                    // Look for the next tab (or EOL)
                    int c = chnum + 1;
                    int w = 0;
                    // We include the trailing space in the calculation because QRT actually formats it
                    while ( c < string()->length() - 1 && string()->at( c ).c != '\t' )
                    {
                        QTextStringChar & ch = string()->at( c );
                        // Determine char width (same code as the one in QTextFormatterBreak[In]Words::format())
                        if ( ch.c.unicode() >= 32 || ch.isCustom() )
                            w += string()->width( c );
                        else
                            w += ch.format()->width( ' ' );
                        ++c;
                    }
                    if ( type == T_RIGHT )
                        return tArray[ i ] - w - 1; // -1 is due to qrt's nx-x+1
                    else // T_CENTER
                        return tArray[ i ] - w/2 - 1; // -1 is due to qrt's nx-x+1
                }
                case T_DEC_PNT:
                {
                    // Look for the next tab (or EOL), and for <digit><dot>
                    // Default to right-aligned if no decimal point found (behavior from msword)
                    int c = chnum + 1;
                    int w = 0;
                    int decimalPoint = KGlobal::locale()->decimalSymbol()[0].unicode();
                    bool digitFound = false;
                    while ( c < string()->length()-1 && string()->at( c ).c != '\t' )
                    {
                        QTextStringChar & ch = string()->at( c );
                        if ( ch.c.isDigit() )
                            digitFound = true;
                        else if ( digitFound && ( ch.c == '.' || ch.c.unicode() == decimalPoint ) )
                        {
                            w += string()->width( c ) / 2; // center around the decimal point
                            break;
                        }
                        else
                            digitFound = false; // The digit has to be right before the dot

                        // Determine char width (same code as the one in QTextFormatterBreak[In]Words::format())
                        if ( ch.c.unicode() >= 32 || ch.isCustom() )
                            w += string()->width( c );
                        else
                            w += ch.format()->width( ' ' );

                        ++c;
                    }
                    return tArray[ i ] - w - 1; // -1 is due to qrt's nx-x+1
                }
                default: // case T_LEFT:
                    return tArray[ i ] - 1; // -1 is due to qrt's nx-x+1
                }
            }
            ++i;
        }
    }
    // No tab list (or no more tabs), use tab-stop-width. QTextParag has the code :)
    return QTextParag::nextTab( chnum, x );
}

void KoTextParag::applyStyle( KoStyle *style )
{
    setParagLayout( style->paragLayout() );
    KoTextFormat *newFormat = &style->format();
    setFormat( 0, string()->length(), newFormat, true );
    setFormat( newFormat );
}

void KoTextParag::setParagLayout( const KoParagLayout & layout, int flags )
{
    //kdDebug() << "KoTextParag::setParagLayout flags=" << flags << endl;
    if ( flags & KoParagLayout::Alignment )
        setAlign( layout.alignment );
    if ( flags & KoParagLayout::Margins )
         setMargins( layout.margins );
    if ( flags & KoParagLayout::LineSpacing )
        setLineSpacing( layout.lineSpacing );
    if ( flags & KoParagLayout::Borders )
    {
        setLeftBorder( layout.leftBorder );
        setRightBorder( layout.rightBorder );
        setTopBorder( layout.topBorder );
        setBottomBorder( layout.bottomBorder );
    }
    if ( flags & KoParagLayout::BulletNumber )
        setCounter( layout.counter );
    if ( flags & KoParagLayout::Tabulator )
        setTabList( layout.tabList() );

    if ( flags == KoParagLayout::All )
        // Don't call setStyle from here, it would overwrite any paragraph-specific settings
        setStyle( layout.style );
}
