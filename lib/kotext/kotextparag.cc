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

//#include "kotextparag.h"
//#include "kotextdocument.h"
#include "koparagcounter.h"
#include "kozoomhandler.h"
#include "kostyle.h"
#include <kglobal.h>
#include <klocale.h>
#include <assert.h>
#include <kdebug.h>

/////

// There is one QStyleSheetItems per paragraph, created on demand,
// in order to set the DisplayMode for counters.
void KoTextParag::checkItem( QStyleSheetItem * & item, const char * name )
{
    // ### Note: now that we forked QRT, we could get rid of this
    // (useless memory allocation) and call drawLabel directly
    if ( !item )
    {
        item = new QStyleSheetItem( 0, QString::fromLatin1(name) /* For debugging purposes only */ );
        QPtrVector<QStyleSheetItem> vec = styleSheetItems();
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
        // Set the display mode (in order for drawLabel to get called by KoTextParag)
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
    KoTextParag *s = next();
    while ( s ) {
        if ( s->m_layout.counter )
            s->m_layout.counter->invalidate();
        s->invalidate( 0 );
        s = s->next();
    }
}

int KoTextParag::counterWidth() const
{
    if ( !m_layout.counter )
        return 0;

    return m_layout.counter->width( this );
}

// Draw the complete label (i.e. heading/list numbers/bullets) for this paragraph.
// This is called by KoTextParag::paintDefault.
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

    // We use the formatting of the first char as the formatting of the counter
    KoTextFormat *format = at( 0 )->format(); // paragFormat();
    p->save();

    //QColor textColor( format->color() );
    QColor textColor( textDocument()->drawingShadow() ? shadowColor():format->color());
    if ( !textColor.isValid() ) // Resolve the color at this point
        textColor = KoTextFormat::defaultTextColor( p );
    p->setPen( QPen( textColor ) );

    KoZoomHandler * zh = textDocument()->paintingZoomHandler();
    assert( zh );
    //bool forPrint = ( p->device()->devType() == QInternal::Printer );
    p->setFont( format->screenFont( zh ) );

    x = zh->layoutUnitToPixelX( x );
    y = zh->layoutUnitToPixelY( y );
    h = zh->layoutUnitToPixelY( y, h );
    base = zh->layoutUnitToPixelY( y, base );
    size = zh->layoutUnitToPixelX( size );

    int height = format->height();

    height = zh->layoutUnitToPixelY( y, height );

    // Now draw any bullet that is required over the space left for it.
    if ( m_layout.counter->isBullet() )
    {
        // Modify x offset.
        QString suffix = m_layout.counter->suffix() + ' ' /*the trailing space*/;
        for ( unsigned int i = 0; i < suffix.length(); i++ )
            x -= zh->layoutUnitToPixelX(format->width( suffix, i ));

        int width = format->width( ' ' );

        width = zh->layoutUnitToPixelX( width );
        QString prefix = m_layout.counter->prefix();
	if ( !prefix.isEmpty() )
	{
	    prefix += ' '/*the trailing space, part of the prefix*/;
            int posPrefix=0;
            for ( unsigned int i = 0; i < prefix.length(); i++ )
                posPrefix += zh->layoutUnitToPixelX(format->width( prefix,i));

            KoTextParag::drawUnderlineDoubleUnderline( p, format, zh, format->screenFont( zh ) , textColor, x , base, width, y - h, height );

            p->drawText( x-posPrefix, y - h + base, prefix );
        }

        QRect er( x - width, y - h + height / 2 - width / 2, width, width );
        // Draw the bullet.
        switch ( m_layout.counter->style() )
        {
            case KoParagCounter::STYLE_DISCBULLET:
                p->setBrush( QBrush(textColor) );
                p->drawEllipse( er );
                p->setBrush( Qt::NoBrush );
                break;
            case KoParagCounter::STYLE_SQUAREBULLET:
                p->fillRect( er, QBrush(textColor) );
                break;
            case KoParagCounter::STYLE_BOXBULLET:
                p->drawRect( er );
                break;
            case KoParagCounter::STYLE_CIRCLEBULLET:
                p->drawEllipse( er );
                break;
            case KoParagCounter::STYLE_CUSTOMBULLET:
                // The user has selected a symbol from a special font. Override the paragraph
                // font with the given family. This conserves the right size etc.
                if ( !m_layout.counter->customBulletFont().isEmpty() )
                {
                    QFont bulletFont( p->font() );
                    bulletFont.setFamily( m_layout.counter->customBulletFont() );
                    p->setFont( bulletFont );
                }
                KoTextParag::drawUnderlineDoubleUnderline( p, format, zh, format->screenFont( zh ) , textColor, x - width , base, width, y - h ,height );

                p->drawText( x - width, y - h + base, m_layout.counter->customBulletCharacter() );
                break;
            default:
                break;
        }
	if ( !suffix.isEmpty() )
        {
            KoTextParag::drawUnderlineDoubleUnderline( p, format, zh, format->screenFont( zh ) , textColor, x , base, size, y - h,height );

            p->drawText( x , y - h + base, suffix );
        }
    }
    else
    {
        // There are no bullets...any parent bullets have already been suppressed.
        // Just draw the text! Note: one space is always appended.
        KoTextParag::drawUnderlineDoubleUnderline( p, format, zh, format->screenFont( zh ) , textColor, x - size , base, size, y - h, height);


        QString counterText = m_layout.counter->text( this );
        if ( !counterText.isEmpty() )
            p->drawText( x - size, y - h + base, counterText + ' ' );
    }
    p->restore();
}

int KoTextParag::topMargin() const
{
    KoZoomHandler * zh = textDocument()->formattingZoomHandler();
    return zh->ptToLayoutUnitPixY(
        m_layout.margins[ QStyleSheetItem::MarginTop ]
        + m_layout.topBorder.width() );
}

int KoTextParag::bottomMargin() const
{
    KoZoomHandler * zh = textDocument()->formattingZoomHandler();
    return zh->ptToLayoutUnitPixY(
        m_layout.margins[ QStyleSheetItem::MarginBottom ]
        + m_layout.bottomBorder.width() );
}

int KoTextParag::leftMargin() const
{
    KoZoomHandler * zh = textDocument()->formattingZoomHandler();
    return zh->ptToLayoutUnitPixX(
        m_layout.margins[ QStyleSheetItem::MarginLeft ]
        + m_layout.leftBorder.width() )
        + counterWidth() /* in layout units already */;
}

int KoTextParag::rightMargin() const
{
    KoZoomHandler * zh = textDocument()->formattingZoomHandler();
    return zh->ptToLayoutUnitPixX(
        m_layout.margins[ QStyleSheetItem::MarginRight ]
        + m_layout.rightBorder.width() );
}

int KoTextParag::firstLineMargin() const
{
    KoZoomHandler * zh = textDocument()->formattingZoomHandler();
    return zh->ptToLayoutUnitPixY(
        m_layout.margins[ QStyleSheetItem::MarginFirstLine ] );
}

int KoTextParag::lineSpacing( int line ) const
{
    KoZoomHandler * zh = textDocument()->formattingZoomHandler();
    int shadow = QABS( zh->ptToLayoutUnitPixY( shadowDistanceY() ) );
    if ( m_layout.lineSpacing >= 0 )
        return zh->ptToLayoutUnitPixY( m_layout.lineSpacing ) + shadow;
    else {
        KoTextParag * that = const_cast<KoTextParag *>(this);
        if( line >= (int)that->lineStartList().count() )
        {
            kdError() << "KoTextParag::lineSpacing assert(line<lines) failed: line=" << line << " lines=" << that->lineStartList().count() << endl;
            return 0+shadow;
        }
        QMap<int, KoTextParagLineStart*>::ConstIterator it = that->lineStartList().begin();
        while ( line-- > 0 )
            ++it;
        int height = ( *it )->h;
        //kdDebug() << " line height=" << height << " valid=" << isValid() << endl;

        if ( m_layout.lineSpacing == KoParagLayout::LS_ONEANDHALF )
        {
            // Tricky. During formatting height doesn't include the linespacing,
            // but afterwards (e.g. when drawing the cursor), it does !
            return shadow + (isValid() ? height / 3 : height / 2);
        }
        else if ( m_layout.lineSpacing == KoParagLayout::LS_DOUBLE )
        {
            return shadow + (isValid() ? height / 2 : height);
        }
    }
    kdWarning() << "Unhandled linespacing value : " << m_layout.lineSpacing << endl;
    return 0+shadow;
}

QRect KoTextParag::pixelRect( KoZoomHandler *zh ) const
{
    QRect rct( zh->layoutUnitToPixel( rect() ) );
    // Both of those workarounds don't appear to be necessary anymore
    // after the qRound->int fix in KoZoomHandler::layoutUnitToPixel.
    // To be checked.
#if 0
    // QRect-semantics and rounding problem... The height is wrong.
    rct.setHeight( zh->layoutUnitToPixelY( rect().height() ) );

    kdDebug() << "KoTextParag::pixelRect was:" << zh->layoutUnitToPixel( rect() ).height()
              << " is now:" << rct.height() << endl;
#endif
#if 0
    // After division we almost always end up with the top overwriting the bottom of the parag above
    if ( prev() )
    {
        QRect prevRect( zh->layoutUnitToPixel( prev()->rect() ) );
        if ( rct.top() < prevRect.bottom() + 1 )
        {
            kdDebug() << "pixelRect: rct.top() adjusted to " << prevRect.bottom() + 1 << " (was " << rct.top() << ")" << endl;
            rct.setTop( prevRect.bottom() + 1 );
        }
    }
#endif
    return rct;
}

// Reimplemented from KoTextParag, called by KoTextDocument::drawParagWYSIWYG
// (KoTextDocument::drawWithoutDoubleBuffer when printing)
void KoTextParag::paint( QPainter &painter, const QColorGroup &cg, KoTextCursor *cursor, bool drawSelections,
                         int clipx, int clipy, int clipw, int cliph )
{
    //kdDebug() << "KoTextParag::paint clipx=" << clipx << " clipy=" << clipy << " clipw=" << clipw << " cliph=" << cliph << endl;
    //kdDebug() << " clipw in pix (approx) : " << textDocument()->paintingZoomHandler()->layoutUnitToPixelX( clipw ) << endl;
    //kdDebug() << " cliph in pix (approx) : " << textDocument()->paintingZoomHandler()->layoutUnitToPixelX( cliph ) << endl;

    // We force the alignment to justify during drawing, so that drawParagString is called
    // for at most one word at a time, never more. This allows to make the spaces slightly
    // bigger to compensate for the rounding problems.
    int realAlignment = alignment();
    setAlignmentDirect( Qt::AlignJustify );

    //qDebug("KoTextParag::paint %p", this);
    KoTextParag::paintDefault( painter, cg, cursor, drawSelections, clipx, clipy, clipw, cliph );

    setAlignmentDirect( realAlignment );

    // Now draw paragraph border
    if ( m_layout.hasBorder() &&!textDocument()->drawingShadow())
    {
        KoZoomHandler * zh = textDocument()->paintingZoomHandler();
        assert(zh);

        QRect r;
        // Old solution: stick to the text
        //r.setLeft( at( 0 )->x - counterWidth() - 1 );
        //r.setRight( rect().width() - rightMargin() - 1 );

        // New solution: occupy the full width
        r.setLeft( KoBorder::zoomWidthX( m_layout.leftBorder.width(), zh, 0 ) );
        // ## documentWidth breaks with variable width. Maybe use currentDrawnFrame() ?
        r.setRight( zh->layoutUnitToPixelX(documentWidth()) - 2 - KoBorder::zoomWidthX( m_layout.rightBorder.width(), zh, 0 ) );
        r.setTop( zh->layoutUnitToPixelY(lineY( 0 )) );
        int lastLine = lines() - 1;
        r.setBottom( static_cast<int>( zh->layoutUnitToPixelY(lineY( lastLine ) + lineHeight( lastLine ) ) )+QABS( shadowY( zh ) ));
        // If we don't have a bottom border, we need go as low as possible ( to touch the next parag's border ).
        // If we have a bottom border, then we rather exclude the linespacing. Just looks nicer IMHO.
        if ( m_layout.bottomBorder.width() > 0 )
            r.rBottom() -= zh->layoutUnitToPixelY(lineSpacing( lastLine )) + 1;
        //kdDebug() << "KoTextParag::paint documentWidth=" << documentWidth() << " r=" << DEBUGRECT( r ) << endl;
        KoBorder::drawBorders( painter, zh, r,
                               m_layout.leftBorder, m_layout.rightBorder, m_layout.topBorder, m_layout.bottomBorder,
                               0, QPen() );
    }
}

// Called by KoTextParag::paintDefault
// Draw a set of characters with the same formattings.
// Reimplemented here to convert coordinates first, and call @ref drawFormattingChars.
void KoTextParag::drawParagString( QPainter &painter, const QString &s, int start, int len, int startX,
                                   int lastY, int baseLine, int bw, int h, bool drawSelections,
                                   KoTextFormat *lastFormat, int i, const QMemArray<int> &selectionStarts,
                                   const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft )
{
    KoZoomHandler * zh = textDocument()->paintingZoomHandler();
    assert(zh);

    //kdDebug() << "startX in LU: " << startX << " layoutUnitToPt( startX )*zoomedResolutionX : " << zh->layoutUnitToPt( startX ) << "*" << zh->zoomedResolutionX() << endl;

    // Calculate startX in pixels (using the xadj value of the corresponding char)
    int startX_pix = zh->layoutUnitToPixelX( startX ) + at( rightToLeft ? start+len-1 : start )->pixelxadj;
    //kdDebug() << "KoTextParag::drawParagString startX in pixels : " << startX_pix << " bw=" << bw << endl;

    int lastY_pix = zh->layoutUnitToPixelY( lastY );
    int baseLine_pix = zh->layoutUnitToPixelY( lastY, baseLine );
    int h_pix = zh->layoutUnitToPixelY( lastY, h );
    //kdDebug() << "KoTextParag::drawParagString h(LU)=" << h << " lastY(LU)=" << lastY
    //        << " h(PIX)=" << h_pix << " lastY(PIX)=" << lastY_pix << endl;

    if ( lastFormat->textBackgroundColor().isValid() )
        painter.fillRect( startX_pix, lastY_pix, bw, h_pix, lastFormat->textBackgroundColor() );

    // don't want to draw line breaks but want them when drawing formatting chars
    int draw_len = len;
    int draw_startX = startX;
    int draw_bw = bw;
    if ( at( start + len - 1 )->c == '\n' )
    {
      draw_len--;
      draw_bw -= at( start + len - 1 )->pixelwidth;
      if ( rightToLeft )
        draw_startX = at( start + draw_len - 1 )->x;
    }
    int draw_startX_pix = zh->layoutUnitToPixelX( draw_startX ) + at( rightToLeft ? start+draw_len-1 : start )->pixelxadj;

    drawParagStringInternal( painter, s, start, draw_len, draw_startX_pix,
                             lastY_pix, baseLine_pix,
                             draw_bw, // Note that bw is already in pixels (see KoTextParag::paint)
                             h_pix, drawSelections, lastFormat, i, selectionStarts,
                             selectionEnds, cg, rightToLeft, zh );

    if ( !textDocument()->drawingShadow() && textDocument()->drawFormattingChars() )
    {
        drawFormattingChars( painter, s, start, len,
                             startX, lastY, baseLine, h,
                             startX_pix, lastY_pix, baseLine_pix, bw, h_pix,
                             drawSelections,
                             lastFormat, i, selectionStarts,
                             selectionEnds, cg, rightToLeft );
    }
}

// Copied from the original KoTextParag
// (we have to copy it here, so that color & font changes don't require changing
// a local copy of the text format)
// And we have to keep it separate from drawParagString to avoid s/startX/startX_pix/ etc.
void KoTextParag::drawParagStringInternal( QPainter &painter, const QString &s, int start, int len, int startX,
                                   int lastY, int baseLine, int bw, int h, bool drawSelections,
                                   KoTextFormat *lastFormat, int i, const QMemArray<int> &selectionStarts,
                                   const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft, KoZoomHandler* zh )
{
    //kdDebug(32001) << "KoTextParag::drawParagStringInternal start=" << start << " len=" << len << endl;
    //kdDebug(32001) << "In pixels:  startX=" << startX << " lastY=" << lastY << " baseLine=" << baseLine
    //          << " bw=" << bw << " h=" << h << endl;

    // 1) Sort out the color
    QColor textColor( lastFormat->color() );
    if ( textDocument()->drawingShadow() ) // Use shadow color if drawing a shadow
        textColor = shadowColor();
    if ( !textColor.isValid() ) // Resolve the color at this point
        textColor = KoTextFormat::defaultTextColor( &painter );

    // 2) Sort out the font
    //bool forPrint = ( painter.device()->devType() == QInternal::Printer );
    QFont font( lastFormat->screenFont( zh ) );
#if 0
    QFontInfo fi( font );
    kdDebug() << "KoTextParag::drawParagString requested font " << font.pointSizeFloat() << " using font " << fi.pointSize() << " (pt for layout-unit size " << lastFormat->font().pointSizeFloat() << ")" << endl;
    QFontMetrics fm( font );
    kdDebug() << "Real font: " << fi.family() << ". Font height in pixels: " << fm.height() << endl;
#endif

    // 3) Go (almost verbatim from the original KoTextFormat::drawParagString)
    QString str( s );
    if ( str[ (int)str.length() - 1 ].unicode() == 0xad )
	str.remove( str.length() - 1, 1 );
    painter.setPen( QPen( textColor ) );
    painter.setFont( font );

    KoTextDocument* doc = document();
#if 0
    if ( doc && lastFormat->isAnchor() && !lastFormat->anchorHref().isEmpty() && lastFormat->useLinkColor() ) {
        if ( doc->linkColor.isValid() )
            painter.setPen( doc->linkColor );
        else
	painter.setPen( QPen( cg.link()  ) );
	if ( doc->underlineLinks() ) {
	    font.setUnderline( TRUE );
	    painter.setFont( font );
	}
    }
#endif

    if ( drawSelections ) {
	const int nSels = doc ? doc->numSelections() : 1;
	const int startSel = painter.device()->devType() != QInternal::Printer ? 0 : 1;
	for ( int j = startSel; j < nSels; ++j ) {
	    if ( i > selectionStarts[ j ] && i <= selectionEnds[ j ] ) {
		if ( !doc || doc->invertSelectionText( j ) )
		    textColor = cg.color( QColorGroup::HighlightedText );
		    painter.setPen( QPen( textColor ) );
		if ( j == KoTextDocument::Standard )
		    painter.fillRect( startX, lastY, bw, h, cg.color( QColorGroup::Highlight ) );
		else
		    painter.fillRect( startX, lastY, bw, h, doc ? doc->selectionColor( j ) : cg.color( QColorGroup::Highlight ) );
	    }
	}
    }

    KoTextParag::drawUnderlineDoubleUnderline( &painter, lastFormat, zh, font, textColor, startX, baseLine, bw, lastY, h);

    QPainter::TextDirection dir = rightToLeft ? QPainter::RTL : QPainter::LTR;

    if ( dir != QPainter::RTL && start + len == length() ) // don't draw the last character (trailing space)
       len--;

    if ( str[ start ] != '\t' && str[ start ].unicode() != 0xad ) {
	if ( lastFormat->vAlign() == KoTextFormat::AlignNormal ) {
	    painter.drawText( startX, lastY + baseLine, str, start, len, dir );
#ifdef BIDI_DEBUG
	    painter.save();
	    painter.setPen ( Qt::red );
	    painter.drawLine( startX, lastY, startX, lastY + baseLine );
	    painter.drawLine( startX, lastY + baseLine/2, startX + 10, lastY + baseLine/2 );
	    int w = 0;
	    int i = 0;
	    while( i < len )
		w += painter.fontMetrics().charWidth( str, start + i++ );
	    painter.setPen ( Qt::blue );
	    painter.drawLine( startX + w - 1, lastY, startX + w - 1, lastY + baseLine );
	    painter.drawLine( startX + w - 1, lastY + baseLine/2, startX + w - 1 - 10, lastY + baseLine/2 );
	    painter.restore();
#endif
	} else if ( lastFormat->vAlign() == KoTextFormat::AlignSuperScript ) {
	    painter.drawText( startX, lastY + baseLine - ( h - painter.fontMetrics().height() ), str, start, len, dir );
	} else if ( lastFormat->vAlign() == KoTextFormat::AlignSubScript ) {
	    painter.drawText( startX, lastY + baseLine + ( painter.fontMetrics().height() / 6 ), str, start, len, dir );
	}
    }
    if ( str[ start ] == '\t' && m_tabCache.contains( start ) ) {
	painter.save();
	KoZoomHandler * zh = textDocument()->paintingZoomHandler();
	KoTabulator tab = m_layout.tabList()[ m_tabCache[ start ] ];
	int lineWidth = zh->zoomItY( tab.ptWidth );
	switch ( tab.filling ) {
	    case TF_DOTS:
		painter.setPen( QPen( textColor, lineWidth, Qt::DotLine ) );
		painter.drawLine( startX, lastY + baseLine, startX + bw, lastY + baseLine );
		break;
	    case TF_LINE:
		painter.setPen( QPen( textColor, lineWidth, Qt::SolidLine ) );
		painter.drawLine( startX, lastY + baseLine, startX + bw, lastY + baseLine );
            case TF_DASH:
		painter.setPen( QPen( textColor, lineWidth, Qt::DashLine ) );
		painter.drawLine( startX, lastY + baseLine, startX + bw, lastY + baseLine );
		break;
            case TF_DASH_DOT:
		painter.setPen( QPen( textColor, lineWidth, Qt::DashDotLine ) );
		painter.drawLine( startX, lastY + baseLine, startX + bw, lastY + baseLine );
		break;
            case TF_DASH_DOT_DOT:
		painter.setPen( QPen( textColor, lineWidth, Qt::DashDotDotLine ) );
		painter.drawLine( startX, lastY + baseLine, startX + bw, lastY + baseLine );
		break;

            default:
                break;
	}
	painter.restore();
    }
    if ( i + 1 < length() && at( i + 1 )->lineStart && at( i )->c.unicode() == 0xad ) {
	painter.drawText( startX + bw, lastY + baseLine, "\xad" );
    }
    if ( painter.device()->devType() != QInternal::Printer && lastFormat->isMisspelled() && !textDocument()->drawingShadow() && textDocument()->drawingMissingSpellLine()) {
	painter.save();
	painter.setPen( QPen( Qt::red, 1, Qt::DotLine ) );
	painter.drawLine( startX, lastY + baseLine + 1, startX + bw, lastY + baseLine + 1 );
	painter.restore();
    }
#if 0
    i -= len;
    if ( doc && lastFormat->isAnchor() && !lastFormat->anchorHref().isEmpty() &&
         doc->focusIndicator.parag == this &&
         doc->focusIndicator.start >= i &&
         doc->focusIndicator.start + doc->focusIndicator.len <= i + len ) {
	painter.drawWinFocusRect( QRect( startX, lastY, bw, h ) );
    }
#endif
}

/** Draw the cursor mark. Reimplemented from KoTextParag to convert coordinates first. */
void KoTextParag::drawCursor( QPainter &painter, KoTextCursor *cursor, int curx, int cury, int curh, const QColorGroup &cg )
{
    if ( textDocument()->drawingShadow() )
        return; // No shadow of the cursor ;)
    KoZoomHandler * zh = textDocument()->paintingZoomHandler();
    int x = zh->layoutUnitToPixelX( curx ) + cursor->parag()->at( cursor->index() )->pixelxadj;
    //qDebug("  drawCursor: LU: [cur]x=%d, cury=%d -> PIX: x=%d, y=%d", curx, cury, x, zh->layoutUnitToPixelY( cury ) );
    KoTextParag::drawCursorDefault( painter, cursor, x,
                            zh->layoutUnitToPixelY( cury ),
                            zh->layoutUnitToPixelY( cury, curh ), cg );
}

// Reimplemented from KoTextParag
void KoTextParag::copyParagData( KoTextParag *parag )
{
    // Style of the previous paragraph
    KoStyle * style = parag->style();
    // Obey "following style" setting
    bool styleApplied = false;
    if ( style )
    {
        KoStyle * newStyle = style->followingStyle();
        if ( newStyle && style != newStyle ) // if same style, keep paragraph-specific changes as usual
        {
            setParagLayout( newStyle->paragLayout() );
            KoTextFormat * format = &newStyle->format();
            setFormat( format );
            format->addRef();
            string()->setFormat( 0, format, true ); // prepare format for text insertion
            styleApplied = true;
        }
    }
    // This should never happen in KWord, but it happens in KPresenter
    //else
    //    kdWarning() << "Paragraph has no style " << paragId() << endl;

    // No "following style" setting, or same style -> copy layout & format of previous paragraph
    if (!styleApplied)
    {
        setParagLayout( parag->paragLayout() );
        // Remove pagebreak flags from initial parag - they got copied to the new parag
        parag->m_layout.pageBreaking &= ~KoParagLayout::HardFrameBreakBefore;
        parag->m_layout.pageBreaking &= ~KoParagLayout::HardFrameBreakAfter;
        // Remove footnote counter text from second parag
        if ( m_layout.counter && m_layout.counter->numbering() == KoParagCounter::NUM_FIXEDTEXT )
            setNoCounter();

        // set parag format to the format of the trailing space of the previous parag
        setFormat( parag->at( parag->length()-1 )->format() );
        // KoTextCursor::splitAndInsertEmptyParag takes care of setting the format
        // for the chars in the new parag
    }

    // Note: we don't call the original KoTextParag::copyParagData on purpose.
    // We don't want setListStyle to get called - it ruins our stylesheetitems
    // And we don't care about copying the stylesheetitems directly,
    // applying the parag layout will create them
}

void KoTextParag::setTabList( const KoTabulatorList &tabList )
{
    KoTabulatorList lst( tabList );
    m_layout.setTabList( lst );
    if ( !tabList.isEmpty() )
    {
        KoZoomHandler* zh = textDocument()->formattingZoomHandler();
        int * tabs = new int[ tabList.count() + 1 ]; // will be deleted by ~KoTextParag
        KoTabulatorList::Iterator it = lst.begin();
        unsigned int i = 0;
        for ( ; it != lst.end() ; ++it, ++i )
            tabs[i] = zh->ptToLayoutUnitPixX( (*it).ptPos );
        tabs[i] = 0;
        assert( i == tabList.count() );
        setTabArray( tabs );
    } else
    {
        setTabArray( 0 );
    }
    invalidate( 0 );
}

/** "Reimplemented" from KoTextParag to implement non-left-aligned tabs */
int KoTextParag::nextTab( int chnum, int x )
{
    if ( !m_layout.tabList().isEmpty() )
    {
        // Fetch the zoomed and sorted tab positions from KoTextParag
        // We stored them there for faster access
        int * tArray = tabArray();
        int i = 0;
        if ( string()->isRightToLeft() )
            i = m_layout.tabList().size() - 1;

        while ( i >= 0 && i < (int)m_layout.tabList().size() ) {
            //kdDebug() << "KoTextParag::nextTab tArray[" << i << "]=" << tArray[i] << " type " << m_layout.tabList()[i].type << endl;
            int tab = tArray[ i ];
            if ( string()->isRightToLeft() )
                tab = rect().width() - tab;

            if ( tab > x ) {
                int type = m_layout.tabList()[i].type;

                // fix the tab type for right to left text
                if ( string()->isRightToLeft() )
                    if ( type == T_RIGHT )
                        type = T_LEFT;
                    else if ( type == T_LEFT )
                        type = T_RIGHT;

                switch ( type ) {
                case T_RIGHT:
                case T_CENTER:
                {
                    // Look for the next tab (or EOL)
                    int c = chnum + 1;
                    int w = 0;
                    while ( c < string()->length() - 1 && string()->at( c ).c != '\t' && string()->at( c ).c != '\n' )
                    {
                        KoTextStringChar & ch = string()->at( c );
                        // Determine char width
                        // This must be done in the same way as in KoTextFormatter::format() or there can be different rounding errors.
                        if ( ch.isCustom() )
                            w += ch.customItem()->width;
                        else
                        {
                            KoTextFormat *charFormat = ch.format();
                            int ww = charFormat->charWidth( textDocument()->formattingZoomHandler(), false, &ch, this, c );
                            ww = KoTextZoomHandler::ptToLayoutUnitPt( ww );
                            w += ww;
                        }
                        ++c;
                    }

                    m_tabCache[chnum] = i;

                    if ( type == T_RIGHT )
                        return tab - w;
                    else // T_CENTER
                        return tab - w/2;
                }
                case T_DEC_PNT:
                {
                    // Look for the next tab (or EOL), and for <digit><dot>
                    // Default to right-aligned if no decimal point found (behavior from msword)
                    int c = chnum + 1;
                    int w = 0;
                    int decimalPoint = KGlobal::locale()->decimalSymbol()[0].unicode();
                    bool digitFound = false;
                    while ( c < string()->length()-1 && string()->at( c ).c != '\t' && string()->at( c ).c != '\n' )
                    {
                        KoTextStringChar & ch = string()->at( c );

                        if ( ch.c.isDigit() )
                            digitFound = true;
                        else if ( digitFound && ( ch.c == '.' || ch.c.unicode() == decimalPoint ) )
                        {
                            if ( string()->isRightToLeft() )
                            {
                                w = ch.width /*string()->width( c )*/ / 2; // center around the decimal point
                                ++c;
                                continue;
                            }
                            else
                            {
                                w += ch.width /*string()->width( c )*/ / 2; // center around the decimal point
                                break;
                            }
                        }
                        else
                            digitFound = false; // The digit has to be right before the dot

                        // Determine char width
                        if ( ch.isCustom() )
                            w += ch.customItem()->width;
                        else
                        {
                            KoTextFormat *charFormat = ch.format();
                            int ww = charFormat->charWidth( textDocument()->formattingZoomHandler(), false, &ch, this, c );
                            ww = KoTextZoomHandler::ptToLayoutUnitPt( ww );
                            w += ww;
                        }

                        ++c;
                    }
                    m_tabCache[chnum] = i;
                    return tab - w;
                }
                default: // case T_LEFT:
                    m_tabCache[chnum] = i;
                    return tab;
                }
            }
            if ( string()->isRightToLeft() )
                --i;
            else
                ++i;
        }
        // No more tabs
        return tArray[0];
    }
    // No tab list, use tab-stop-width. qrichtext.cpp has the code :)
    return KoTextParag::nextTabDefault( chnum, x );
}

void KoTextParag::setShadow( double dist, short int direction, const QColor &col )
{
    m_layout.shadowDistance = dist;
    m_layout.shadowDirection = direction;
    m_layout.shadowColor = col;
    invalidate(0);
}

double KoTextParag::shadowDistanceY() const
{
    switch ( m_layout.shadowDirection )
    {
    case KoParagLayout::SD_LEFT_UP:
    case KoParagLayout::SD_UP:
    case KoParagLayout::SD_RIGHT_UP:
        return - m_layout.shadowDistance;
    case KoParagLayout::SD_LEFT:
    case KoParagLayout::SD_RIGHT:
        return 0;
    case KoParagLayout::SD_LEFT_BOTTOM:
    case KoParagLayout::SD_BOTTOM:
    case KoParagLayout::SD_RIGHT_BOTTOM:
        return m_layout.shadowDistance;
    }
    return 0;
}


int KoTextParag::shadowX( KoZoomHandler *zh ) const
{
    switch ( m_layout.shadowDirection )
    {
    case KoParagLayout::SD_LEFT_BOTTOM:
    case KoParagLayout::SD_LEFT:
    case KoParagLayout::SD_LEFT_UP:
        return - zh->zoomItX( m_layout.shadowDistance );
    case KoParagLayout::SD_UP:
    case KoParagLayout::SD_BOTTOM:
        return 0;
    case KoParagLayout::SD_RIGHT_UP:
    case KoParagLayout::SD_RIGHT:
    case KoParagLayout::SD_RIGHT_BOTTOM:
        return zh->zoomItX( m_layout.shadowDistance );
    }
    return 0;
}

int KoTextParag::shadowY( KoZoomHandler *zh ) const
{
    return zh->zoomItY(shadowDistanceY());
}

void KoTextParag::applyStyle( KoStyle *style )
{
    setParagLayout( style->paragLayout() );
    KoTextFormat *newFormat = &style->format();
    setFormat( 0, string()->length(), newFormat );
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
    if ( flags & KoParagLayout::Shadow )
        setShadow( layout.shadowDistance, layout.shadowDirection,layout.shadowColor );
    if ( flags == KoParagLayout::All )
        // Don't call applyStyle from here, it would overwrite any paragraph-specific settings
        setStyle( layout.style );
}

void KoTextParag::setCustomItem( int index, KoTextCustomItem * custom, KoTextFormat * currentFormat )
{
    kdDebug(32001) << "KoTextParag::setCustomItem " << index << "  " << (void*)custom
                   << "  currentFormat=" << (void*)currentFormat << endl;
    if ( currentFormat )
        setFormat( index, 1, currentFormat );
    at( index )->setCustomItem( custom );
    addCustomItem();
    document()->registerCustomItem( custom, this );
    custom->resize();
    invalidate( 0 );
}

void KoTextParag::removeCustomItem( int index )
{
    Q_ASSERT( at( index )->isCustom() );
    KoTextCustomItem * item = at( index )->customItem();
    at( index )->loseCustomItem();
    KoTextParag::removeCustomItem();
    document()->unregisterCustomItem( item, this );
}


int KoTextParag::findCustomItem( const KoTextCustomItem * custom ) const
{
    int len = string()->length();
    for ( int i = 0; i < len; ++i )
    {
        KoTextStringChar & ch = string()->at(i);
        if ( ch.isCustom() && ch.customItem() == custom )
            return i;
    }
    kdWarning() << "KoTextParag::findCustomItem custom item " << (void*)custom
              << " not found in paragraph " << paragId() << endl;
    return 0;
}

#ifndef NDEBUG
void KoTextParag::printRTDebug( int info )
{
    kdDebug() << "Paragraph " << this << " (" << paragId() << ") [changed="
              << hasChanged() << ", valid=" << isValid()
              << ", needsSpellCheck=" << string()->needsSpellCheck()
              << ", wasMovedDown=" << wasMovedDown()
              << "] ------------------ " << endl;
    if ( prev() && prev()->paragId() + 1 != paragId() )
        kdWarning() << "  Previous paragraph " << prev() << " has ID " << prev()->paragId() << endl;
    if ( next() && next()->paragId() != paragId() + 1 )
        kdWarning() << "  Next paragraph " << next() << " has ID " << next()->paragId() << endl;
    //if ( !next() )
    //    kdDebug() << "  next is 0L" << endl;
    /*
      static const char * dm[] = { "DisplayBlock", "DisplayInline", "DisplayListItem", "DisplayNone" };
      QPtrVector<QStyleSheetItem> vec = styleSheetItems();
      for ( uint i = 0 ; i < vec.size() ; ++i )
      {
      QStyleSheetItem * item = vec[i];
      kdDebug() << "  StyleSheet Item " << item << " '" << item->name() << "'" << endl;
      kdDebug() << "        italic=" << item->fontItalic() << " underline=" << item->fontUnderline() << " fontSize=" << item->fontSize() << endl;
      kdDebug() << "        align=" << item->alignment() << " leftMargin=" << item->margin(QStyleSheetItem::MarginLeft) << " rightMargin=" << item->margin(QStyleSheetItem::MarginRight) << " topMargin=" << item->margin(QStyleSheetItem::MarginTop) << " bottomMargin=" << item->margin(QStyleSheetItem::MarginBottom) << endl;
      kdDebug() << "        displaymode=" << dm[item->displayMode()] << endl;
      }*/
    kdDebug() << "  Style: " << style() << " " << ( style() ? style()->name().local8Bit().data() : "NO STYLE" ) << endl;
    kdDebug() << "  Text: '" << string()->toString() << "'" << endl;
    if ( info == 0 ) // paragraph info
    {
        if ( counter() )
            kdDebug() << "  Counter style=" << counter()->style()
                      << " numbering=" << counter()->numbering()
                      << " depth=" << counter()->depth()
                      << " text='" << m_layout.counter->text( this ) << "'"
                      << " width=" << m_layout.counter->width( this ) << endl;
        kdDebug() << "  align: " << alignment() << endl;
        QRect pixr = pixelRect( textDocument()->paintingZoomHandler() );
        kdDebug() << "  rect() : " << DEBUGRECT( rect() )
                  << "  pixelRect() : " << DEBUGRECT( pixr ) << endl;
        kdDebug() << "  topMargin()=" << topMargin() << " bottomMargin()=" << bottomMargin()
                  << " leftMargin()=" << leftMargin() << " firstLineMargin()=" << firstLineMargin()
                  << " rightMargin()=" << rightMargin() << endl;

        static const char * tabtype[] = { "T_LEFT", "T_CENTER", "T_RIGHT", "T_DEC_PNT", "error!!!" };
        KoTabulatorList tabList = m_layout.tabList();
        if ( tabList.isEmpty() ) {
            if ( string()->toString().find( '\t' ) != -1 )
                kdDebug() << "Tab width: " << textDocument()->tabStopWidth() << endl;
        } else {
            KoTabulatorList::Iterator it = tabList.begin();
            for ( ; it != tabList.end() ; it++ )
                kdDebug() << "Tab type:" << tabtype[(*it).type] << " at: " << (*it).ptPos << endl;
        }
    } else if ( info == 1 ) // formatting info
    {
        kdDebug() << "  Paragraph format=" << paragFormat() << " " << paragFormat()->key()
                  << " fontsize:" << dynamic_cast<KoTextFormat *>(paragFormat())->font().pointSize() << endl;

        KoTextString * s = string();
        int lastX = 0; // pixels
        int lastW = 0; // pixels
        for ( int i = 0 ; i < s->length() ; ++i )
        {
            KoTextStringChar & ch = s->at(i);
            int pixelx =  textDocument()->formattingZoomHandler()->layoutUnitToPixelX( ch.x )
                          + ch.pixelxadj;
            kdDebug() << i << ": '" << QString(ch.c) << "' (" << ch.c.unicode() << ")"
                      << " x(LU)=" << ch.x
                      << " w(LU)=" << ch.width//s->width(i)
                      << " x(PIX)=" << pixelx
                      << " (xadj=" << + ch.pixelxadj << ")"
                      << " w(PIX)=" << ch.pixelwidth
                      << " height=" << ch.height()
                //      << " format=" << ch.format()
                      << " \"" << ch.format()->key() << "\" "
                //<< " fontsize:" << dynamic_cast<KoTextFormat *>(ch.format())->pointSize()
                      << endl;
            Q_ASSERT( textDocument()->formatCollection()->dict()[ch.format()->key()] );
            if ( !string()->isBidi() && !ch.lineStart )
                Q_ASSERT( lastX + lastW == pixelx );
            lastX = pixelx;
            lastW = ch.pixelwidth;
            if ( ch.isCustom() )
            {
                KoTextCustomItem * item = ch.customItem();
                kdDebug() << " - custom item " << item
                          << " ownline=" << item->ownLine()
                          << " size=" << item->width << "x" << item->height
                          << endl;
            }
        }
    }
}
#endif


void KoTextParag::drawUnderlineDoubleUnderline( QPainter * p, KoTextFormat *format, KoZoomHandler *zh, QFont font, const QColor & color, int startX, int baseLine, int bw, int lastY,  int h )
{
    if ( format->doubleUnderline() )
    {
        // For double-underlining, both lines are of width 0.5 (*zoom), in the hopes
	// to have room for both.
	// Another solution would be to increase the descent, but this would have to be
	// done in the formatter
	// ### TODO scale the painter to do this, especially when printing, to gain more resolution
        //kdDebug() << "KoTextParag::drawParagStringInternal double underline. lastY=" << lastY << " baseLine=" << baseLine << " 0.5pix=" << KoBorder::zoomWidthY( 0.5, zh, 0 ) << " 1pix=" << KoBorder::zoomWidthY( 1, zh, 0 ) << " descent=(LU:" << lastFormat->descent() << " pix:" << zh->layoutUnitToPixelY( lastFormat->descent() ) << ")" << endl;
        QColor col = format->textUnderlineColor().isValid() ? format->textUnderlineColor(): color ;

	int y = lastY + baseLine + KoBorder::zoomWidthY( 0.2, zh, 0 ); // slightly under the baseline if possible
        p->save();
        switch( format->lineType() )
        {
        case KoTextFormat::SOLID:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::SolidLine ) );
            break;
        case KoTextFormat::DASH:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashLine ) );
            break;
        case KoTextFormat::DOT:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DotLine ) );
            break;
        case KoTextFormat::DASH_DOT:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashDotLine ) );
            break;
        case KoTextFormat::DASH_DOT_DOT:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashDotDotLine ) );

            break;
        default:
            p->setPen( QPen( color, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::SolidLine ) );
        }

        p->drawLine( startX, y, startX + bw, y );
        //kdDebug() << "KoTextParag::drawParagStringInternal drawing first line at " << y << endl;
	y = lastY + baseLine + zh->layoutUnitToPixelY( format->descent() ) /*- KoBorder::zoomWidthY( 1, zh, 0 )*/;
        //kdDebug() << "KoTextParag::drawParagStringInternal drawing second line at " << y << endl;
	p->drawLine( startX, y, startX + bw, y );
        p->restore();
        if ( font.underline() ) { // can this happen?
            font.setUnderline( FALSE );
            p->setFont( font );
        }
    }
    else if ( format->underline() )
    {
        int y = lastY + baseLine + KoBorder::zoomWidthY( 1, zh, 0 );
        QColor col = format->textUnderlineColor().isValid() ? format->textUnderlineColor(): color ;
        p->save();

        switch( format->lineType() )
        {
        case KoTextFormat::SOLID:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::SolidLine ) );
            break;
        case KoTextFormat::DASH:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashLine ) );
            break;
        case KoTextFormat::DOT:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashDotLine ) );
            break;
        case KoTextFormat::DASH_DOT:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashDotLine ) );
            break;
        case KoTextFormat::DASH_DOT_DOT:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashDotDotLine ) );

            break;
        default:
            p->setPen( QPen( col, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::SolidLine ) );
        }

        p->drawLine( startX, y, startX + bw, y );
        p->restore();
        font.setUnderline( FALSE );
        p->setFont( font );
    }

    if ( font.strikeOut() )
    {

        p->save();
        switch( format->strikeOutType() )
        {
        case KoTextFormat::SOLID:
            p->setPen( QPen( color, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::SolidLine ) );
            break;
        case KoTextFormat::DASH:
            p->setPen( QPen( color, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashLine ) );
            break;
        case KoTextFormat::DOT:
            p->setPen( QPen( color, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashDotLine ) );
            break;
        case KoTextFormat::DASH_DOT:
            p->setPen( QPen( color, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashDotLine ) );
            break;
        case KoTextFormat::DASH_DOT_DOT:
            p->setPen( QPen( color, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::DashDotDotLine ) );

            break;
        default:
            p->setPen( QPen( color, KoBorder::zoomWidthY( 1, zh, 1 ), Qt::SolidLine ) );
        }

        int y = lastY + baseLine + KoBorder::zoomWidthY( 1, zh, 0 );
        p->drawLine( startX, y - h/2 + 2 , startX + bw, y- h/2 +2 );
        p->restore();
        font.setStrikeOut( FALSE );
        p->setFont( font );
    }
}
