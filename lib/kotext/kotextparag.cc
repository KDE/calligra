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

#include "kotextdocument.h"
#include "koparagcounter.h"
#include "kozoomhandler.h"
#include "kostyle.h"
#include <kglobal.h>
#include <klocale.h>
#include <assert.h>
#include <kdebug.h>
#include "kovariable.h"
#include <kooasiscontext.h>
#include <koxmlwriter.h>
#include <koGenStyles.h>

//#define DEBUG_PAINT

// Return the counter associated with this paragraph.
KoParagCounter *KoTextParag::counter()
{
    if ( !m_layout.counter )
        return 0L;

    // Garbage collect un-needed counters.
    if ( m_layout.counter->numbering() == KoParagCounter::NUM_NONE )
        setNoCounter();
    return m_layout.counter;
}

void KoTextParag::setMargin( QStyleSheetItem::Margin m, double _i )
{
    //kdDebug(32500) << "KoTextParag::setMargin " << m << " margin " << _i << endl;
    m_layout.margins[m] = _i;
    if ( m == QStyleSheetItem::MarginTop && prev() )
        prev()->invalidate(0);     // for top margin (post-1.1: remove this, not necessary anymore)
    invalidate(0);
}

void KoTextParag::setMargins( const double * margins )
{
    for ( int i = 0 ; i < 5 ; ++i )
        m_layout.margins[i] = margins[i];
    invalidate(0);
}

void KoTextParag::setAlign( int align )
{
    Q_ASSERT( align <= Qt::AlignJustify );
    align &= Qt::AlignHorizontal_Mask;
    setAlignment( align );
    m_layout.alignment = align;
}

int KoTextParag::resolveAlignment() const
{
    if ( m_layout.alignment == Qt::AlignAuto )
        return string()->isRightToLeft() ? Qt::AlignRight : Qt::AlignLeft;
    return m_layout.alignment;
}

void KoTextParag::setLineSpacing( double _i )
{
    m_layout.setLineSpacingValue(_i);
    invalidate(0);
}

void KoTextParag::setLineSpacingType( KoParagLayout::SpacingType _type )
{
    m_layout.lineSpacingType = _type;
    invalidate(0);
}

void KoTextParag::setTopBorder( const KoBorder & _brd )
{
    m_layout.topBorder = _brd;
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
    // #### Possible optimization: since any invalidation propagates down,
    // it's enough to stop at the first paragraph with an already-invalidated counter, isn't it?
    // This is only true if nobody else calls counter->invalidate...
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
// This is called by KoTextParag::paint.
void KoTextParag::drawLabel( QPainter* p, int xLU, int yLU, int /*wLU*/, int /*hLU*/, int baseLU, const QColorGroup& /*cg*/ )
{
    if ( !m_layout.counter ) // shouldn't happen
        return;

    if ( m_layout.counter->numbering() == KoParagCounter::NUM_NONE )
    {   // Garbage collect unnneeded counter.
        delete m_layout.counter;
        m_layout.counter = 0L;
        return;
    }

    int counterWidthLU = m_layout.counter->width( this );

    // We use the formatting of the first char as the formatting of the counter
    KoTextFormat *format = KoParagCounter::counterFormat( this );
    p->save();

    QColor textColor( format->color() );
    if ( !textColor.isValid() ) // Resolve the color at this point
        textColor = KoTextFormat::defaultTextColor( p );
    p->setPen( QPen( textColor ) );

    KoZoomHandler * zh = textDocument()->paintingZoomHandler();
    assert( zh );
    //bool forPrint = ( p->device()->devType() == QInternal::Printer );

    bool rtl = str->isRightToLeft(); // when true, we put suffix+counter+prefix at the RIGHT of the paragraph.
    int xLeft = zh->layoutUnitToPixelX( xLU - (rtl ? 0 : counterWidthLU) );
    int y = zh->layoutUnitToPixelY( yLU );
    //int h = zh->layoutUnitToPixelY( yLU, hLU );
    int base = zh->layoutUnitToPixelY( yLU, baseLU );
    int counterWidth = zh->layoutUnitToPixelX( xLU, counterWidthLU );
    int height = zh->layoutUnitToPixelY( yLU, format->height() );

    QFont font( format->screenFont( zh ) );
    // Footnote numbers are in superscript (in WP and Word, not in OO)
    if ( m_layout.counter->numbering() == KoParagCounter::NUM_FOOTNOTE )
    {
        int pointSize = ( ( font.pointSize() * 2 ) / 3 );
        font.setPointSize( pointSize );
        y -= ( height - QFontMetrics(font).height() );
    }
    p->setFont( font );

    // Now draw any bullet that is required over the space left for it.
    if ( m_layout.counter->isBullet() )
    {
	int xBullet = xLeft + zh->layoutUnitToPixelX( m_layout.counter->bulletX() );

        //kdDebug(32500) << "KoTextParag::drawLabel xLU=" << xLU << " counterWidthLU=" << counterWidthLU << endl;
	// The width and height of the bullet is the width of one space
        int width = zh->layoutUnitToPixelX( xLeft, format->width( ' ' ) );

        //kdDebug(32500) << "Pix: xLeft=" << xLeft << " counterWidth=" << counterWidth
        //          << " xBullet=" << xBullet << " width=" << width << endl;

        QString prefix = m_layout.counter->prefix();
        if ( !prefix.isEmpty() )
        {
            if ( rtl )
                prefix.prepend( ' ' /*the space before the bullet in RTL mode*/ );
            KoTextParag::drawFontEffects( p, format, zh, format->screenFont( zh ), textColor, xLeft, base, width, y, height, prefix[0] );

            int posY =y + base - format->offsetFromBaseLine();
            //we must move to bottom text because we create
            //shadow to 'top'.
            int sy = format->shadowY( zh );
            if ( sy < 0)
                posY -= sy;

            p->drawText( xLeft, posY, prefix );
        }

        QRect er( xBullet + (rtl ? width : 0), y + height / 2 - width / 2, width, width );
        // Draw the bullet.
        int posY = 0;
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
            {
                // The user has selected a symbol from a special font. Override the paragraph
                // font with the given family. This conserves the right size etc.
                if ( !m_layout.counter->customBulletFont().isEmpty() )
                {
                    QFont bulletFont( p->font() );
                    bulletFont.setFamily( m_layout.counter->customBulletFont() );
                    p->setFont( bulletFont );
                }
                KoTextParag::drawFontEffects( p, format, zh, format->screenFont( zh ), textColor, xBullet, base, width, y, height, ' ' );

                posY = y + base- format->offsetFromBaseLine();
                //we must move to bottom text because we create
                //shadow to 'top'.
                int sy = format->shadowY( zh );
                if ( sy < 0)
                    posY -= sy;

                p->drawText( xBullet, posY, m_layout.counter->customBulletCharacter() );
                break;
            }
            default:
                break;
        }

        QString suffix = m_layout.counter->suffix();
        if ( !suffix.isEmpty() )
        {
            if ( !rtl )
                suffix += ' ' /*the space after the bullet*/;

            KoTextParag::drawFontEffects( p, format, zh, format->screenFont( zh ), textColor, xBullet + width, base, counterWidth, y,height, suffix[0] );

            int posY =y + base- format->offsetFromBaseLine();
            //we must move to bottom text because we create
            //shadow to 'top'.
            int sy = format->shadowY( zh );
            if ( sy < 0)
                posY -= sy;

            p->drawText( xBullet + width, posY, suffix, -1 );
        }
    }
    else
    {
        QString counterText = m_layout.counter->text( this );
        // There are no bullets...any parent bullets have already been suppressed.
        // Just draw the text! Note: one space is always appended.
        if ( !counterText.isEmpty() )
        {
            KoTextParag::drawFontEffects( p, format, zh, format->screenFont( zh ), textColor, xLeft, base, counterWidth, y, height, counterText[0] );

            counterText += ' ' /*the space after the bullet (before in RTL mode)*/;

            int posY =y + base - format->offsetFromBaseLine();
            //we must move to bottom text because we create
            //shadow to 'top'.
            int sy = format->shadowY( zh );
            if ( sy < 0)
                posY -= sy;

            p->drawText( xLeft, posY , counterText, -1 );
        }
    }
    p->restore();
}

int KoTextParag::breakableTopMargin() const
{
    KoZoomHandler * zh = textDocument()->formattingZoomHandler();
    return zh->ptToLayoutUnitPixY(
        m_layout.margins[ QStyleSheetItem::MarginTop ] );
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
        + m_layout.leftBorder.width() );
}

int KoTextParag::rightMargin() const
{
    KoZoomHandler * zh = textDocument()->formattingZoomHandler();
    int cw=0;
    if( m_layout.counter && str->isRightToLeft() &&
    	(( m_layout.counter->alignment() == Qt::AlignRight ) || ( m_layout.counter->alignment() == Qt::AlignAuto )))
	    cw = counterWidth();

    return zh->ptToLayoutUnitPixX(
        m_layout.margins[ QStyleSheetItem::MarginRight ]
        + m_layout.rightBorder.width() )
        + cw; /* in layout units already */
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
    // TODO add shadow in KoTextFormatter!
    int shadow = 0; //QABS( zh->ptToLayoutUnitPixY( shadowDistanceY() ) );
    if ( m_layout.lineSpacingType == KoParagLayout::LS_SINGLE )
        return shadow;
    else if ( m_layout.lineSpacingType == KoParagLayout::LS_CUSTOM )
        return zh->ptToLayoutUnitPixY( m_layout.lineSpacingValue() ) + shadow;
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
        if ( isValid() )
            return (*it)->lineSpacing;

        int height = ( *it )->h;
        //kdDebug(32500) << " line spacing type: " << m_layout.lineSpacingType << " value:" << m_layout.lineSpacingValue() << " line_height=" << height << endl;
        switch ( m_layout.lineSpacingType )
        {
        case KoParagLayout::LS_MULTIPLE:
        {
            double n = QMAX( m_layout.lineSpacingValue() - 1.0, 0.0 );
            return shadow + qRound( n * height );
        }
        case KoParagLayout::LS_ONEANDHALF:
        {
            // Special case of LS_MULTIPLE, with n=1.5
            return shadow + height / 2;
        }
        case KoParagLayout::LS_DOUBLE:
        {
            // Special case of LS_MULTIPLE, with n=1
            return shadow + height;
        }
        case KoParagLayout::LS_AT_LEAST:
        {
            int atLeast = zh->ptToLayoutUnitPixY( m_layout.lineSpacingValue() );
            int h = QMAX( height, atLeast );
            // height is now the required total height
            return shadow + h - height;
        }
        case KoParagLayout::LS_FIXED:
        {
            return shadow + zh->ptToLayoutUnitPixY( m_layout.lineSpacingValue() ) - height;
        }
        // Silence compiler warnings
        case KoParagLayout::LS_SINGLE:
        case KoParagLayout::LS_CUSTOM:
            break;
        }
    }
    kdWarning() << "Unhandled linespacing type : " << m_layout.lineSpacingType << endl;
    return 0+shadow;
}

QRect KoTextParag::pixelRect( KoZoomHandler *zh ) const
{
    QRect rct( zh->layoutUnitToPixel( rect() ) );
    //kdDebug(32500) << "   pixelRect for parag " << paragId()
    //               << ": rect=" << rect() << " pixelRect=" << rct << endl;

    // After division we almost always end up with the top overwriting the bottom of the parag above
    if ( prev() )
    {
        QRect prevRect( zh->layoutUnitToPixel( prev()->rect() ) );
        if ( rct.top() < prevRect.bottom() + 1 )
        {
            //kdDebug(32500) << "   pixelRect: rct.top() adjusted to " << prevRect.bottom() + 1 << " (was " << rct.top() << ")" << endl;
            rct.setTop( prevRect.bottom() + 1 );
        }
    }
    return rct;
}

// Paint this paragraph. This is called by KoTextDocument::drawParagWYSIWYG
// (KoTextDocument::drawWithoutDoubleBuffer when printing)
void KoTextParag::paint( QPainter &painter, const QColorGroup &cg, KoTextCursor *cursor, bool drawSelections,
                         int clipx, int clipy, int clipw, int cliph )
{
#ifdef DEBUG_PAINT
    kdDebug(32500) << "KoTextParag::paint =====  id=" << paragId() << " clipx=" << clipx << " clipy=" << clipy << " clipw=" << clipw << " cliph=" << cliph << endl;
    kdDebug(32500) << " clipw in pix (approx) : " << textDocument()->paintingZoomHandler()->layoutUnitToPixelX( clipw ) << " cliph in pix (approx) : " << textDocument()->paintingZoomHandler()->layoutUnitToPixelX( cliph ) << endl;
#endif

    // Let's call drawLabel ourselves, rather than having to deal with QStyleSheetItem to get paintLines to call it!
    if ( m_layout.counter && m_layout.counter->numbering() != KoParagCounter::NUM_NONE && m_lineChanged <= 0 )
    {
        int cy, h, baseLine;
        lineInfo( 0, cy, h, baseLine );
        int xLabel = at(0)->x;
        if ( str->isRightToLeft() )
            xLabel += at(0)->width;
        drawLabel( &painter, xLabel, cy, 0, 0, baseLine, cg );
    }

    paintLines( painter, cg, cursor, drawSelections, clipx, clipy, clipw, cliph );

    // Now draw paragraph border
    if ( m_layout.hasBorder() )
    {
        KoZoomHandler * zh = textDocument()->paintingZoomHandler();
        assert(zh);

        QRect r;
        // Old solution: stick to the text
        //r.setLeft( at( 0 )->x - counterWidth() - 1 );
        //r.setRight( rect().width() - rightMargin() - 1 );

        // New solution: occupy the full width
        // Note that this is what OpenOffice does too.
        // For something closer to the text, we need a border feature in KoTextFormat, I guess.

        // drawBorders paints outside the give rect, so we need to 'subtract' the border
        // width on all sides.
        r.setLeft( KoBorder::zoomWidthX( m_layout.leftBorder.width(), zh, 0 ) );
        // The +1 is because if border is 1 pixel, nothing to subtract. 2 pixels -> subtract 1.
        r.setRight( zh->layoutUnitToPixelX(rect().width()) - KoBorder::zoomWidthX( m_layout.rightBorder.width(), zh, 0 ) );
        r.setTop( zh->layoutUnitToPixelY(lineY( 0 )) );

        int lastLine = lines() - 1;
        // We need to start from the pixelRect, to make sure the bottom border is entirely painted.
        // This is a case where we DO want to subtract pixels to pixels...
        int paragBottom = pixelRect(zh).height()-1;
        // If we don't have a bottom border, we need go as low as possible ( to touch the next parag's border ).
        // If we have a bottom border, then we rather exclude the linespacing. Looks nicer. OO does that too.
        if ( m_layout.bottomBorder.width() > 0 )
            paragBottom -= zh->layoutUnitToPixelY( lineSpacing( lastLine ) );
        paragBottom -= KoBorder::zoomWidthY( m_layout.bottomBorder.width(), zh, 0 );
        //kdDebug(32500) << "Parag border: paragBottom=" << paragBottom
        //               << " bottom border width = " << KoBorder::zoomWidthY( m_layout.bottomBorder.width(), zh, 0 ) << endl;
        r.setBottom( paragBottom );

        //kdDebug(32500) << "KoTextParag::paint documentWidth=" << documentWidth() << " LU (" << zh->layoutUnitToPixelX(documentWidth()) << " pixels) bordersRect=" << r << endl;
        KoBorder::drawBorders( painter, zh, r,
                               m_layout.leftBorder, m_layout.rightBorder, m_layout.topBorder, m_layout.bottomBorder,
                               0, QPen() );
    }
}


void KoTextParag::paintLines( QPainter &painter, const QColorGroup &cg, KoTextCursor *cursor, bool drawSelections,
			int clipx, int clipy, int clipw, int cliph )
{
    if ( !visible )
	return;
    //KoTextStringChar *chr = at( 0 );
    //if (!chr) { kdDebug(32500) << "paragraph " << (void*)this << " " << paragId() << ", can't paint, EMPTY !" << endl;

    // This is necessary with the current code, but in theory it shouldn't
    // be necessary, if Xft really gives us fully proportionnal chars....
#define CHECK_PIXELXADJ

    int curx = -1, cury = 0, curh = 0, curline = 0;
    int xstart, xend = 0;

    QString qstr = str->toString();
    qstr.replace( QChar(0x00a0U), ' ' ); // Not all fonts have non-breakable-space glyph

    const int nSels = doc ? doc->numSelections() : 1;
    QMemArray<int> selectionStarts( nSels );
    QMemArray<int> selectionEnds( nSels );
    if ( drawSelections ) {
	bool hasASelection = FALSE;
	for ( int i = 0; i < nSels; ++i ) {
	    if ( !hasSelection( i ) ) {
		selectionStarts[ i ] = -1;
		selectionEnds[ i ] = -1;
	    } else {
		hasASelection = TRUE;
		selectionStarts[ i ] = selectionStart( i );
		int end = selectionEnd( i );
		if ( end == length() - 1 && n && n->hasSelection( i ) )
		    end++;
		selectionEnds[ i ] = end;
	    }
	}
	if ( !hasASelection )
	    drawSelections = FALSE;
    }

    // Draw the lines!
    int line = m_lineChanged;
    if (line<0) line = 0;

    int numLines = lines();
#ifdef DEBUG_PAINT
    kdDebug(32500) << " paintLines: from line " << line << " to " << numLines-1 << endl;
#endif
    for( ; line<numLines ; line++ )
    {
	// get the start and length of the line
	int nextLine;
        int startOfLine;
    	lineStartOfLine(line, &startOfLine);
	if (line == numLines-1 )
            nextLine = length();
	else
            lineStartOfLine(line+1, &nextLine);

	// init this line
        int cy, h, baseLine;
	lineInfo( line, cy, h, baseLine );
	if ( clipy != -1 && cy > clipy - r.y() + cliph ) // outside clip area, leave
	    break;

        // Vars related to the current "run of text"
	int paintStart = startOfLine;
	KoTextStringChar* chr = at(startOfLine);
        KoTextStringChar* nextchr = chr;

	// okay, paint the line!
	for(int i=startOfLine;i<nextLine;i++)
	{
            chr = nextchr;
            if ( i < nextLine-1 )
                nextchr = at( i+1 );

            // we flush at end of line
            bool flush = ( i == nextLine - 1 );
            // Optimization note: QRT uses "flush |=", which doesn't have shortcut optimization

            // we flush on format changes
	    flush = flush || ( nextchr->format() != chr->format() );
	    // we flush on link changes
	    //flush = flush || ( nextchr->isLink() != chr->isLink() );
            // we flush on small caps changes
            if ( !flush && chr->format()->attributeFont() == KoTextFormat::ATT_SMALL_CAPS )
            {
                bool isLowercase = chr->c.upper() != chr->c;
                bool nextLowercase = nextchr->c.upper() != nextchr->c;
                flush = isLowercase != nextLowercase;
            }
	    // we flush on start of run
	    flush = flush || nextchr->startOfRun;
	    // we flush on bidi changes
	    flush = flush || ( nextchr->rightToLeft != chr->rightToLeft );
#ifdef CHECK_PIXELXADJ
            // we flush when the value of pixelxadj changes
            flush = flush || ( nextchr->pixelxadj != chr->pixelxadj );
#endif
	    // we flush before and after tabs
	    flush = flush || ( chr->c == '\t' || nextchr->c == '\t' );
	    // we flush on soft hypens
	    flush = flush || ( chr->c.unicode() == 0xad );
	    // we flush on custom items
	    flush = flush || chr->isCustom();
	    // we flush before custom items
	    flush = flush || nextchr->isCustom();
	    // when painting justified we flush on spaces
	    if ((alignment() & Qt::AlignJustify) == Qt::AlignJustify )
		//flush = flush || QTextFormatter::isBreakable( str, i );
                flush = flush || chr->c.isSpace();
	    // when underlining or striking "word by word" we flush before/after spaces
	    if (!flush && chr->format()->wordByWord() && chr->format()->isStrikedOrUnderlined())
                flush = flush || chr->c.isSpace() || nextchr->c.isSpace();
	    // we flush when the string is getting too long
	    flush = flush || ( i - paintStart >= 256 );
	    // we flush when the selection state changes
	    if ( drawSelections ) {
                // check if selection state changed - TODO update from QRT
		bool selectionChange = FALSE;
		if ( drawSelections ) {
		    for ( int j = 0; j < nSels; ++j ) {
			selectionChange = selectionStarts[ j ] == i+1 || selectionEnds[ j ] == i+1;
			if ( selectionChange )
			    break;
		    }
		}
                flush = flush || selectionChange;
            }

            // check for cursor mark
            if ( cursor && this == cursor->parag() && i == cursor->index() ) {
                curx = cursor->x();
                curline = line;
                KoTextStringChar *c = chr;
                if ( i > 0 )
                    --c;
                curh = c->height();
                cury = cy + baseLine - c->ascent();
            }

            if ( flush ) {  // something changed, draw what we have so far

                KoTextStringChar* cStart = at( paintStart );
                if ( chr->rightToLeft ) {
                    xstart = chr->x;
                    xend = cStart->x + cStart->width;
                } else {
                    xstart = cStart->x;
                        if ( i < length() - 1 && !str->at( i + 1 ).lineStart &&
                         str->at( i + 1 ).rightToLeft == chr->rightToLeft )
                        xend = str->at( i + 1 ).x;
                    else
                        xend = chr->x + chr->width;
                }

                if ( (clipx == -1 || clipw == -1) || (xend >= clipx && xstart <= clipx + clipw) ) {
                    if ( !chr->isCustom() ) {
                        drawParagString( painter, qstr, paintStart, i - paintStart + 1, xstart, cy,
                                         baseLine, xend-xstart, h, drawSelections,
                                         chr->format(), selectionStarts, selectionEnds,
                                         cg, chr->rightToLeft, line );
                    }
                    else
                        if ( chr->customItem()->placement() == KoTextCustomItem::PlaceInline ) {
                            chr->customItem()->draw( &painter, chr->x, cy + baseLine - chr->customItem()->ascent(), clipx - r.x(), clipy - r.y(), clipw, cliph, cg,
                                                     drawSelections && nSels && selectionStarts[ 0 ] <= i && selectionEnds[ 0 ] > i );
                        }
                }
                paintStart = i+1;
            }
        } // end of character loop
    } // end of line loop

    // if we should draw a cursor, draw it now
    if ( curx != -1 && cursor ) {
        drawCursor( painter, cursor, curx, cury, curh, cg );
    }
}

// Called by KoTextParag::paintLines
// Draw a set of characters with the same formattings.
// Reimplemented here to convert coordinates first, and call @ref drawFormattingChars.
void KoTextParag::drawParagString( QPainter &painter, const QString &str, int start, int len, int startX,
                                   int lastY, int baseLine, int bw, int h, bool drawSelections,
                                   KoTextFormat *format, const QMemArray<int> &selectionStarts,
                                   const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft, int line )
{
    KoZoomHandler * zh = textDocument()->paintingZoomHandler();
    assert(zh);

#ifdef DEBUG_PAINT
    kdDebug(32500) << "KoTextParag::drawParagString drawing from " << start << " to " << start+len << endl;
    kdDebug(32500) << " startX in LU: " << startX << " lastY in LU:" << lastY
                   << " baseLine in LU:" << baseLine << endl;
#endif

    // Calculate offset (e.g. due to shadow on left or top)
    // Important: don't use the 2-args methods here, offsets are not heights
    // (0 should be 0, not 1) (#63256)
    int shadowOffsetX_pix = zh->layoutUnitToPixelX( format->offsetX() );
    int shadowOffsetY_pix = zh->layoutUnitToPixelY( format->offsetY() );

    // Calculate startX in pixels
    int startX_pix = zh->layoutUnitToPixelX( startX ) /* + at( rightToLeft ? start+len-1 : start )->pixelxadj */;
#ifdef DEBUG_PAINT
    kdDebug(32500) << "KoTextParag::drawParagString startX in pixels : " << startX_pix /*<< " adjustment:" << at( rightToLeft ? start+len-1 : start )->pixelxadj*/ << " bw=" << bw << endl;
#endif

    int bw_pix = zh->layoutUnitToPixelX( startX, bw );
    int lastY_pix = zh->layoutUnitToPixelY( lastY );
    int baseLine_pix = zh->layoutUnitToPixelY( lastY, baseLine ); // 2 args=>+1. Is that correct?
    int h_pix = zh->layoutUnitToPixelY( lastY, h );
#ifdef DEBUG_PAINT
    kdDebug(32500) << "KoTextParag::drawParagString h(LU)=" << h << " lastY(LU)=" << lastY
                   << " h(PIX)=" << h_pix << " lastY(PIX)=" << lastY_pix
                   << " baseLine(PIX)=" << baseLine_pix << endl;
#endif

    if ( format->textBackgroundColor().isValid() )
        painter.fillRect( startX_pix, lastY_pix, bw_pix, h_pix, format->textBackgroundColor() );

    // don't want to draw line breaks but want them when drawing formatting chars
    int draw_len = len;
    int draw_startX = startX;
    int draw_bw = bw_pix;
    if ( at( start + len - 1 )->c == '\n' )
    {
        draw_len--;
        draw_bw -= at( start + len - 1 )->pixelwidth;
        if ( rightToLeft && draw_len > 0 )
            draw_startX = at( start + draw_len - 1 )->x;
    }

    // Draw selection (moved here to do it before applying the offset from the shadow)
    // (and because it's not part of the shadow drawing)
    if ( drawSelections ) {
        bool inSelection = false;
	const int nSels = doc ? doc->numSelections() : 1;
	for ( int j = 0; j < nSels; ++j ) {
	    if ( start >= selectionStarts[ j ] && start < selectionEnds[ j ] ) {
                inSelection = true;
		if ( j == KoTextDocument::Standard )
		    painter.fillRect( startX_pix, lastY_pix, bw_pix, h_pix, cg.color( QColorGroup::Highlight ) );
		else
		    painter.fillRect( startX_pix, lastY_pix, bw_pix, h_pix, doc ? doc->selectionColor( j ) : cg.color( QColorGroup::Highlight ) );
                break;
	    }
	}
        if ( !inSelection )
            drawSelections = false; // save time in drawParagStringInternal
    }

    if ( draw_len > 0 )
    {
        int draw_startX_pix = zh->layoutUnitToPixelX( draw_startX ) /* + at( rightToLeft ? start+draw_len-1 : start )->pixelxadj*/;
        draw_startX_pix += shadowOffsetX_pix;
        lastY_pix += shadowOffsetY_pix;

        if ( format->shadowDistanceX() != 0 || format->shadowDistanceY() != 0 ) {
            int sx = format->shadowX( zh );
            int sy = format->shadowY( zh );
            if ( sx != 0 || sy != 0 )
            {
                painter.save();
                painter.translate( sx, sy );
                drawParagStringInternal( painter, str, start, draw_len, draw_startX_pix,
                                         lastY_pix, baseLine_pix,
                                         draw_bw,
                                         h_pix, FALSE /*drawSelections*/,
                                         format, selectionStarts,
                                         selectionEnds, cg, rightToLeft, line, zh, true );
                painter.restore();
            }
        }

        drawParagStringInternal( painter, str, start, draw_len, draw_startX_pix,
                                 lastY_pix, baseLine_pix,
                                 draw_bw,
                                 h_pix, drawSelections, format, selectionStarts,
                                 selectionEnds, cg, rightToLeft, line, zh, false );
    }

    bool forPrint = ( painter.device()->devType() == QInternal::Printer );
    if ( textDocument()->drawFormattingChars() && !forPrint )
    {
        drawFormattingChars( painter, start, len,
                             lastY_pix, baseLine_pix, h_pix,
                             drawSelections,
                             format, selectionStarts,
                             selectionEnds, cg, rightToLeft,
                             line, zh, AllFormattingChars );
    }
}

// Copied from the original KoTextParag
// (we have to copy it here, so that color & font changes don't require changing
// a local copy of the text format)
// And we have to keep it separate from drawParagString to avoid s/startX/startX_pix/ etc.
void KoTextParag::drawParagStringInternal( QPainter &painter, const QString &s, int start, int len, int startX,
                                   int lastY, int baseLine, int bw, int h, bool drawSelections,
                                   KoTextFormat *format, const QMemArray<int> &selectionStarts,
                                   const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft, int line, KoZoomHandler* zh, bool drawingShadow )
{
#ifdef DEBUG_PAINT
    kdDebug(32500) << "KoTextParag::drawParagStringInternal start=" << start << " len=" << len << " : '" << s.mid(start,len) << "'" << endl;
    kdDebug(32500) << "In pixels:  startX=" << startX << " lastY=" << lastY << " baseLine=" << baseLine
                   << " bw=" << bw << " h=" << h << " rightToLeft=" << rightToLeft << endl;
#endif
    if ( drawingShadow && format->shadowDistanceX() == 0 && format->shadowDistanceY() == 0 )
        return;
    // 1) Sort out the color
    QColor textColor( drawingShadow ? format->shadowColor() : format->color() );
    if ( !textColor.isValid() ) // Resolve the color at this point
        textColor = KoTextFormat::defaultTextColor( &painter );

    // 2) Sort out the font
    QFont font( format->screenFont( zh ) );
    if ( format->attributeFont() == KoTextFormat::ATT_SMALL_CAPS && s[start].upper() != s[start] )
        font = format->smallCapsFont( zh, true );

#if 0
    QFontInfo fi( font );
    kdDebug(32500) << "KoTextParag::drawParagStringInternal requested font " << font.pointSizeFloat() << " using font " << fi.pointSize() << "pt (format font: " << format->font().pointSizeFloat() << "pt)" << endl;
    QFontMetrics fm( font );
    kdDebug(32500) << "Real font: " << fi.family() << ". Font height in pixels: " << fm.height() << endl;
#endif

    // 3) Paint
    QString str( s );
    if ( str[ (int)str.length() - 1 ].unicode() == 0xad )
        str.remove( str.length() - 1, 1 );
    painter.setPen( QPen( textColor ) );
    painter.setFont( font );

    KoTextDocument* doc = document();

    if ( drawSelections ) {
	const int nSels = doc ? doc->numSelections() : 1;
	for ( int j = 0; j < nSels; ++j ) {
	    if ( start >= selectionStarts[ j ] && start < selectionEnds[ j ] ) {
		if ( !doc || doc->invertSelectionText( j ) )
		    textColor = cg.color( QColorGroup::HighlightedText );
		    painter.setPen( QPen( textColor ) );
                    break;
            }
        }
    }

    QPainter::TextDirection dir = rightToLeft ? QPainter::RTL : QPainter::LTR;

    if ( dir != QPainter::RTL && start + len == length() ) // don't draw the last character (trailing space)
    {
       len--;
       if ( len <= 0 )
           return;
       bw-=at(length()-1)->pixelwidth;
    }
    KoTextParag::drawFontEffects( &painter, format, zh, font, textColor, startX, baseLine, bw, lastY, h, str[start] );

    if ( str[ start ] != '\t' && str[ start ].unicode() != 0xad ) {
        str = format->displayedString( str ); // #### This converts the whole string, instead of from start to start+len!
	if ( format->vAlign() == KoTextFormat::AlignNormal ) {
            int posY = lastY + baseLine - format->offsetFromBaseLine();
            //we must move to bottom text because we create
            //shadow to 'top'.
            int sy = format->shadowY( zh );
            if ( sy < 0)
                posY -= sy;
	    painter.drawText( startX, posY, str, start, len, dir );
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
	} else if ( format->vAlign() == KoTextFormat::AlignSuperScript ) {
            int posY =lastY + baseLine - ( painter.fontMetrics().height() / 2 )-format->offsetFromBaseLine();
            //we must move to bottom text because we create
            //shadow to 'top'.
            int sy = format->shadowY( zh );
            if ( sy < 0)
                posY -= sy;
	    painter.drawText( startX, posY, str, start, len, dir );
	} else if ( format->vAlign() == KoTextFormat::AlignSubScript ) {
            int posY =lastY + baseLine + ( painter.fontMetrics().height() / 6 )-format->offsetFromBaseLine();
            //we must move to bottom text because we create
            //shadow to 'top'.
            int sy = format->shadowY( zh );
            if ( sy < 0)
                posY -= sy;
	    painter.drawText( startX, posY, str, start, len, dir );
	}
    }
    if ( str[ start ] == '\t' && m_tabCache.contains( start ) ) {
	painter.save();
	KoZoomHandler * zh = textDocument()->paintingZoomHandler();
	const KoTabulator& tab = m_layout.tabList()[ m_tabCache[ start ] ];
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

    if ( start+len < length() && at( start+len )->lineStart )
    {
#ifdef DEBUG_PAINT
        //kdDebug(32500) << "we are drawing the end of line " << line << ". Auto-hyphenated: " << lineHyphenated( line ) << endl;
#endif
        bool drawHyphen = at( start+len-1 )->c.unicode() == 0xad;
        drawHyphen = drawHyphen || lineHyphenated( line );
        if ( drawHyphen ) {
#ifdef DEBUG_PAINT
            kdDebug(32500) << "drawing hyphen at x=" << startX+bw << endl;
#endif
            painter.drawText( startX + bw, lastY + baseLine, "-" ); // \xad gives squares with some fonts (!?)
        }
    }

    // Paint a zigzag line for "wrong" background spellchecking checked words:
    if(
		painter.device()->devType() != QInternal::Printer &&
		format->isMisspelled() &&
		!drawingShadow &&
		textDocument()->drawingMissingSpellLine() )
	{
		painter.save();
		painter.setPen( QPen( Qt::red, 1 ) );

		// Draw 3 pixel lines with increasing offset and distance 4:
		for( int zigzag_line = 0; zigzag_line < 3; ++zigzag_line )
		{
			for( int zigzag_x = zigzag_line; zigzag_x < bw; zigzag_x += 4 )
			{
				painter.drawPoint(
					startX + zigzag_x,
					lastY + baseLine + h/12 - 1 + zigzag_line );
			}
		}

		// "Double" the pixel number for the middle line:
		for( int zigzag_x = 3; zigzag_x < bw; zigzag_x += 4 )
		{
			painter.drawPoint(
				startX + zigzag_x,
				lastY + baseLine + h/12 );
		}

		painter.restore();
	}
}

bool KoTextParag::lineHyphenated( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	kdWarning() << "KoTextParag::lineHyphenated: line " << l << " out of range!" << endl;
	return false;
    }

    if ( !isValid() )
	const_cast<KoTextParag*>(this)->format();

    QMap<int, KoTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->hyphenated;
}

/** Draw the cursor mark. Reimplemented from KoTextParag to convert coordinates first. */
void KoTextParag::drawCursor( QPainter &painter, KoTextCursor *cursor, int curx, int cury, int curh, const QColorGroup &cg )
{
    KoZoomHandler * zh = textDocument()->paintingZoomHandler();
    int x = zh->layoutUnitToPixelX( curx ) /*+ cursor->parag()->at( cursor->index() )->pixelxadj*/;
    //kdDebug(32500) << "  drawCursor: LU: [cur]x=" << curx << ", cury=" << cury << " -> PIX: x=" << x << ", y=" << zh->layoutUnitToPixelY( cury ) << endl;
    KoTextParag::drawCursorDefault( painter, cursor, x,
                            zh->layoutUnitToPixelY( cury ),
                            zh->layoutUnitToPixelY( cury, curh ), cg );
}

// Reimplemented from KoTextParag
void KoTextParag::copyParagData( KoTextParag *parag )
{
    // Style of the previous paragraph
    KoParagStyle * style = parag->style();
    // Obey "following style" setting
    bool styleApplied = false;
    if ( style )
    {
        KoParagStyle * newStyle = style->followingStyle();
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
        if ( m_layout.counter && m_layout.counter->numbering() == KoParagCounter::NUM_FOOTNOTE )
            setNoCounter();
        // Do not copy 'restart numbering at this paragraph' option (would be silly)
        if ( m_layout.counter )
            m_layout.counter->setRestartCounter(false);

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
            //kdDebug(32500) << "KoTextParag::nextTab tArray[" << i << "]=" << tArray[i] << " type " << m_layout.tabList()[i].type << endl;
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
                    // Look for the next tab (or EOL), and for alignChar
                    // Default to right-aligned if no decimal point found (behavior from msword)
                    int c = chnum + 1;
                    int w = 0;
                    while ( c < string()->length()-1 && string()->at( c ).c != '\t' && string()->at( c ).c != '\n' )
                    {
                        KoTextStringChar & ch = string()->at( c );
                        if ( ch.c == m_layout.tabList()[i].alignChar )
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
    }
    // No tab list, use tab-stop-width. qrichtext.cpp has the code :)
    return KoTextParag::nextTabDefault( chnum, x );
}

void KoTextParag::applyStyle( KoParagStyle *style )
{
    setParagLayout( style->paragLayout() );
    KoTextFormat *newFormat = &style->format();
    setFormat( 0, string()->length(), newFormat );
    setFormat( newFormat );
}

void KoTextParag::setParagLayout( const KoParagLayout & layout, int flags )
{
    //kdDebug(32500) << "KoTextParag::setParagLayout flags=" << flags << endl;
    if ( flags & KoParagLayout::Alignment )
        setAlign( layout.alignment );
    if ( flags & KoParagLayout::Margins )
        setMargins( layout.margins );
    if ( flags & KoParagLayout::LineSpacing )
    {
        setLineSpacingType( layout.lineSpacingType );
        setLineSpacing( layout.lineSpacingValue() );
    }
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
    {
        setDirection( static_cast<QChar::Direction>(layout.direction) );
        // Don't call applyStyle from here, it would overwrite any paragraph-specific settings
        setStyle( layout.style );
    }
}

void KoTextParag::setCustomItem( int index, KoTextCustomItem * custom, KoTextFormat * currentFormat )
{
    //kdDebug(32500) << "KoTextParag::setCustomItem " << index << "  " << (void*)custom
    //               << "  currentFormat=" << (void*)currentFormat << endl;
    if ( currentFormat )
        setFormat( index, 1, currentFormat );
    at( index )->setCustomItem( custom );
    //addCustomItem();
    document()->registerCustomItem( custom, this );
    custom->recalc(); // calc value (e.g. for variables) and set initial size
    invalidate( 0 );
    setChanged( true );
}

void KoTextParag::removeCustomItem( int index )
{
    Q_ASSERT( at( index )->isCustom() );
    KoTextCustomItem * item = at( index )->customItem();
    at( index )->loseCustomItem();
    //KoTextParag::removeCustomItem();
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
    kdDebug(32500) << "Paragraph " << this << " (" << paragId() << ") [changed="
              << hasChanged() << ", valid=" << isValid()
              << ", needsSpellCheck=" << string()->needsSpellCheck()
              << ", wasMovedDown=" << wasMovedDown()
              // not used << ", lastInFrame=" << isLastInFrame()
              << "] ------------------ " << endl;
    if ( prev() && prev()->paragId() + 1 != paragId() )
        kdWarning() << "  Previous paragraph " << prev() << " has ID " << prev()->paragId() << endl;
    if ( next() && next()->paragId() != paragId() + 1 )
        kdWarning() << "  Next paragraph " << next() << " has ID " << next()->paragId() << endl;
    //if ( !next() )
    //    kdDebug(32500) << "  next is 0L" << endl;
    /*
      static const char * const dm[] = { "DisplayBlock", "DisplayInline", "DisplayListItem", "DisplayNone" };
      QPtrVector<QStyleSheetItem> vec = styleSheetItems();
      for ( uint i = 0 ; i < vec.size() ; ++i )
      {
      QStyleSheetItem * item = vec[i];
      kdDebug(32500) << "  StyleSheet Item " << item << " '" << item->name() << "'" << endl;
      kdDebug(32500) << "        italic=" << item->fontItalic() << " underline=" << item->fontUnderline() << " fontSize=" << item->fontSize() << endl;
      kdDebug(32500) << "        align=" << item->alignment() << " leftMargin=" << item->margin(QStyleSheetItem::MarginLeft) << " rightMargin=" << item->margin(QStyleSheetItem::MarginRight) << " topMargin=" << item->margin(QStyleSheetItem::MarginTop) << " bottomMargin=" << item->margin(QStyleSheetItem::MarginBottom) << endl;
      kdDebug(32500) << "        displaymode=" << dm[item->displayMode()] << endl;
      }*/
    kdDebug(32500) << "  Style: " << style() << " " << ( style() ? style()->name().local8Bit().data() : "NO STYLE" ) << endl;
    kdDebug(32500) << "  Text: '" << string()->toString() << "'" << endl;
    if ( info == 0 ) // paragraph info
    {
        if ( m_layout.counter )
        {
            QString additionalInfo;
            if ( m_layout.counter->restartCounter() )
                additionalInfo = "[restartCounter]";
            static const char * const s_numbering[] = { "List", "Chapter", "None", "Footnote" };
            kdDebug(32500) << "  Counter style=" << m_layout.counter->style()
                      << " numbering=" << s_numbering[ m_layout.counter->numbering() ]
                      << " depth=" << m_layout.counter->depth()
                      << " number=" << m_layout.counter->number( this )
                      << " text='" << m_layout.counter->text( this ) << "'"
                      << " width=" << m_layout.counter->width( this )
                      << additionalInfo << endl;
        }
        static const char * const s_align[] = { "Auto", "Left", "Right", "ERROR", "HCenter", "ERR", "ERR", "ERR", "Justify", };
        static const char * const s_linespacing[] = { "Single", "1.5", "2", "custom", "atLeast", "Multiple", "Fixed" };
        static const char * const s_dir[] = { "DirL", "DirR", "DirEN", "DirES", "DirET", "DirAN", "DirCS", "DirB", "DirS", "DirWS", "DirON", "DirLRE", "DirLRO", "DirAL", "DirRLE", "DirRLO", "DirPDF", "DirNSM", "DirBN" };
        kdDebug(32500) << "  align: " << s_align[alignment()] << "  resolveAlignment: " << s_align[resolveAlignment()]
                  << "  isRTL:" << string()->isRightToLeft()
                  << "  dir: " << s_dir[direction()] << endl;
        QRect pixr = pixelRect( textDocument()->paintingZoomHandler() );
        kdDebug(32500) << "  rect() : " << DEBUGRECT( rect() )
                  << "  pixelRect() : " << DEBUGRECT( pixr ) << endl;
        kdDebug(32500) << "  topMargin()=" << topMargin() << " bottomMargin()=" << bottomMargin()
                  << " leftMargin()=" << leftMargin() << " firstLineMargin()=" << firstLineMargin()
                  << " rightMargin()=" << rightMargin() << endl;
        if ( kwLineSpacingType() != KoParagLayout::LS_SINGLE )
            kdDebug(32500) << "  linespacing type=" << s_linespacing[ -kwLineSpacingType() ]
                           << " value=" << kwLineSpacing() << endl;

        static const char * const tabtype[] = { "T_LEFT", "T_CENTER", "T_RIGHT", "T_DEC_PNT", "error!!!" };
        KoTabulatorList tabList = m_layout.tabList();
        if ( tabList.isEmpty() ) {
            if ( string()->toString().find( '\t' ) != -1 )
                kdDebug(32500) << "Tab width: " << textDocument()->tabStopWidth() << endl;
        } else {
            KoTabulatorList::Iterator it = tabList.begin();
            for ( ; it != tabList.end() ; it++ )
                kdDebug(32500) << "Tab type:" << tabtype[(*it).type] << " at: " << (*it).ptPos << endl;
        }
    } else if ( info == 1 ) // formatting info
    {
        kdDebug(32500) << "  Paragraph format=" << paragFormat() << " " << paragFormat()->key()
                  << " fontsize:" << dynamic_cast<KoTextFormat *>(paragFormat())->pointSize() << endl;

        for ( int line = 0 ; line < lines(); ++ line ) {
            int y, h, baseLine;
            lineInfo( line, y, h, baseLine );
            int startOfLine;
            lineStartOfLine( line, &startOfLine );
            kdDebug(32500) << "  Line " << line << " y=" << y << " height=" << h << " baseLine=" << baseLine << " startOfLine(index)=" << startOfLine << endl;
        }
        kdDebug(32500) << endl;
        KoTextString * s = string();
        int lastX = 0; // pixels
        int lastW = 0; // pixels
        for ( int i = 0 ; i < s->length() ; ++i )
        {
            KoTextStringChar & ch = s->at(i);
            int pixelx =  textDocument()->formattingZoomHandler()->layoutUnitToPixelX( ch.x )
                          + ch.pixelxadj;
            if ( ch.lineStart )
                kdDebug(32500) << "LINESTART" << endl;
            kdDebug(32500) << i << ": '" << QString(ch.c) << "' (" << ch.c.unicode() << ")"
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

	    // Check that the format is in the collection (i.e. its defaultFormat or in the dict)
	    if ( ch.format() != textDocument()->formatCollection()->defaultFormat() )
                Q_ASSERT( textDocument()->formatCollection()->dict()[ch.format()->key()] );

            if ( !string()->isBidi() && !ch.lineStart )
                Q_ASSERT( lastX + lastW == pixelx ); // looks like some rounding problem with justified spaces
            lastX = pixelx;
            lastW = ch.pixelwidth;
            if ( ch.isCustom() )
            {
                KoTextCustomItem * item = ch.customItem();
                kdDebug(32500) << " - custom item " << item
                          << " ownline=" << item->ownLine()
                          << " size=" << item->width << "x" << item->height
                          << " ascent=" << item->ascent()
                          << endl;
            }
        }
    }
}
#endif

void KoTextParag::drawFontEffects( QPainter * p, KoTextFormat *format, KoZoomHandler *zh, QFont font, const QColor & color, int startX, int baseLine, int bw, int lastY, int /*h*/, QChar firstChar )
{
    // This is about drawing underlines and strikeouts
    // So abort immediately if there's none to draw.
    if ( !format->isStrikedOrUnderlined() )
        return;
    //kdDebug(32500) << "drawFontEffects wordByWord=" << format->wordByWord() <<
    //    " firstChar='" << QString(firstChar) << "'" << endl;
    // paintLines ensures that we're called word by word if wordByWord is true.
    if ( format->wordByWord() && firstChar.isSpace() )
        return;

    double dimd;
    int y;
    int offset = 0;
    if (format->vAlign() == KoTextFormat::AlignSubScript )
        offset = p->fontMetrics().height() / 6;
    else if (format->vAlign() == KoTextFormat::AlignSuperScript )
        offset = -p->fontMetrics().height() / 2;

    dimd = KoBorder::zoomWidthY( format->underLineWidth(), zh, 1 );
    if((format->vAlign() == KoTextFormat::AlignSuperScript) ||
	(format->vAlign() == KoTextFormat::AlignSubScript ))
	dimd*=format->relativeTextSize();
    y = lastY + baseLine + offset - format->offsetFromBaseLine();

    if ( format->doubleUnderline())
    {
        QColor col = format->textUnderlineColor().isValid() ? format->textUnderlineColor(): color ;
	int dim=static_cast<int>(0.75*dimd);
	dim=dim?dim:1; //width of line should be at least 1
        p->save();

        switch( format->underlineStyle())
        {
        case KoTextFormat::U_SOLID:
            p->setPen( QPen( col, dim, Qt::SolidLine ) );
            break;
        case KoTextFormat::U_DASH:
            p->setPen( QPen( col, dim, Qt::DashLine ) );
            break;
        case KoTextFormat::U_DOT:
            p->setPen( QPen( col, dim, Qt::DotLine ) );
            break;
        case KoTextFormat::U_DASH_DOT:
            p->setPen( QPen( col, dim, Qt::DashDotLine ) );
            break;
        case KoTextFormat::U_DASH_DOT_DOT:
            p->setPen( QPen( col, dim, Qt::DashDotDotLine ) );
            break;
        default:
            p->setPen( QPen( color, dim, Qt::SolidLine ) );
        }

        y += static_cast<int>(1.125*dimd); // slightly under the baseline if possible
        p->drawLine( startX, y, startX + bw, y );
        y += static_cast<int>(1.5*dimd);
        p->drawLine( startX, y, startX + bw, y );
        p->restore();
        if ( font.underline() ) { // can this happen?
            font.setUnderline( FALSE );
            p->setFont( font );
        }
    }
    else if ( format->underline() ||
                format->underlineType() == KoTextFormat::U_SIMPLE_BOLD)
    {

        QColor col = format->textUnderlineColor().isValid() ? format->textUnderlineColor(): color ;
        p->save();
	int dim=(format->underlineType() == KoTextFormat::U_SIMPLE_BOLD)?static_cast<int>(2*dimd):static_cast<int>(dimd);
	dim=dim?dim:1; //width of line should be at least 1
        y += static_cast<int>(1.875*dimd);

        switch( format->underlineStyle() )
        {
        case KoTextFormat::U_SOLID:
            p->setPen( QPen( col, dim, Qt::SolidLine ) );
            break;
        case KoTextFormat::U_DASH:
            p->setPen( QPen( col, dim, Qt::DashLine ) );
            break;
        case KoTextFormat::U_DOT:
            p->setPen( QPen( col, dim, Qt::DotLine ) );
            break;
        case KoTextFormat::U_DASH_DOT:
            p->setPen( QPen( col, dim, Qt::DashDotLine ) );
            break;
        case KoTextFormat::U_DASH_DOT_DOT:
            p->setPen( QPen( col, dim, Qt::DashDotDotLine ) );
            break;
        default:
            p->setPen( QPen( col, dim, Qt::SolidLine ) );
        }

        p->drawLine( startX, y, startX + bw, y );
        p->restore();
        font.setUnderline( FALSE );
        p->setFont( font );
    }
    else if ( format->waveUnderline() )
    {
	int dim=static_cast<int>(dimd);
	dim=dim?dim:1; //width of line should be at least 1
        y += dim;
        QColor col = format->textUnderlineColor().isValid() ? format->textUnderlineColor(): color ;
        p->save();
	int offset = 2 * dim;
	QPen pen(col, dim, Qt::SolidLine);
	pen.setCapStyle(Qt::RoundCap);
	p->setPen(pen);
	Q_ASSERT(offset);
	double anc=acos(1.0-2*(static_cast<double>(offset-(startX)%offset)/static_cast<double>(offset)))/3.1415*180;
	int pos=1;
	//set starting position
	if(2*((startX/offset)/2)==startX/offset)
	    pos*=-1;
	//draw first part of wave
	p->drawArc( (startX/offset)*offset, y, offset, offset, 0, -qRound(pos*anc*16) );
        //now the main part
	int zigzag_x = (startX/offset+1)*offset;
	for ( ; zigzag_x + offset <= bw+startX; zigzag_x += offset)
        {
	    p->drawArc( zigzag_x, y, offset, offset, 0, pos*180*16 );
	    pos*=-1;
        }
	//and here we finish
	anc=acos(1.0-2*(static_cast<double>((startX+bw)%offset)/static_cast<double>(offset)))/3.1415*180;
	p->drawArc( zigzag_x, y, offset, offset, 180*16, -qRound(pos*anc*16) );
	p->restore();
        font.setUnderline( FALSE );
        p->setFont( font );
    }

    dimd = KoBorder::zoomWidthY( static_cast<double>(format->pointSize())/18.0, zh, 1 );
    if((format->vAlign() == KoTextFormat::AlignSuperScript) ||
	(format->vAlign() == KoTextFormat::AlignSubScript ))
	dimd*=format->relativeTextSize();
    y = lastY + baseLine + offset - format->offsetFromBaseLine();

    if ( format->strikeOutType() == KoTextFormat::S_SIMPLE
         || format->strikeOutType() == KoTextFormat::S_SIMPLE_BOLD)
    {
        unsigned int dim = (format->strikeOutType() == KoTextFormat::S_SIMPLE_BOLD)? static_cast<int>(2*dimd) : static_cast<int>(dimd);
        p->save();

        switch( format->strikeOutStyle() )
        {
        case KoTextFormat::S_SOLID:
            p->setPen( QPen( color, dim, Qt::SolidLine ) );
            break;
        case KoTextFormat::S_DASH:
            p->setPen( QPen( color, dim, Qt::DashLine ) );
            break;
        case KoTextFormat::S_DOT:
            p->setPen( QPen( color, dim, Qt::DotLine ) );
            break;
        case KoTextFormat::S_DASH_DOT:
            p->setPen( QPen( color, dim, Qt::DashDotLine ) );
            break;
        case KoTextFormat::S_DASH_DOT_DOT:
            p->setPen( QPen( color, dim, Qt::DashDotDotLine ) );
            break;
        default:
            p->setPen( QPen( color, dim, Qt::SolidLine ) );
        }

        y -= static_cast<int>(5*dimd);
        p->drawLine( startX, y, startX + bw, y );
        p->restore();
        font.setStrikeOut( FALSE );
        p->setFont( font );
    }
    else if ( format->strikeOutType() == KoTextFormat::S_DOUBLE )
    {
        unsigned int dim = static_cast<int>(dimd);
        p->save();

        switch( format->strikeOutStyle() )
        {
        case KoTextFormat::S_SOLID:
            p->setPen( QPen( color, dim, Qt::SolidLine ) );
            break;
        case KoTextFormat::S_DASH:
            p->setPen( QPen( color, dim, Qt::DashLine ) );
            break;
        case KoTextFormat::S_DOT:
            p->setPen( QPen( color, dim, Qt::DotLine ) );
            break;
        case KoTextFormat::S_DASH_DOT:
            p->setPen( QPen( color, dim, Qt::DashDotLine ) );
            break;
        case KoTextFormat::S_DASH_DOT_DOT:
            p->setPen( QPen( color, dim, Qt::DashDotDotLine ) );
            break;
        default:
            p->setPen( QPen( color, dim, Qt::SolidLine ) );
        }

	y -= static_cast<int>(4*dimd);
        p->drawLine( startX, y, startX + bw, y);
	y -= static_cast<int>(2*dimd);
        p->drawLine( startX, y, startX + bw, y);
        p->restore();
        font.setStrikeOut( FALSE );
        p->setFont( font );
    }

}

// ### is this method correct for RTL text?
QString KoTextParag::toString( int from, int length ) const
{
    QString str;
    if ( from == 0 && m_layout.counter )
        str += m_layout.counter->text( this ) + ' ';
    if ( length == -1 )
        length = this->length() - 1 /*trailing space*/ - from;
    for ( int i = from ; i < (length+from) ; ++i )
    {
        KoTextStringChar *ch = at( i );
        if ( ch->isCustom() )
        {
            KoVariable * var = dynamic_cast<KoVariable *>(ch->customItem());
            if ( var )
                str += var->text(true);
            else //frame inline
                str +=' ';
        }
        else
            str += ch->c;
    }
    return str;
}

void KoTextParag::loadOasisSpan( const QDomElement& parent, KoOasisContext& context, uint& pos )
{
    // parse every child node of the parent
    for( QDomNode node ( parent.firstChild() ); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement ts ( node.toElement() );
        QString textData;
        QString tagName( ts.tagName() );
        //kdDebug() << k_funcinfo << tagName << " isText:" << node.isText() << endl;
        bool textFoo = tagName.startsWith( "text:" );
        QString afterText = tagName.mid( 5 );
        KoTextCustomItem* customItem = 0;

        // allow loadSpanTag to modify the stylestack
        context.styleStack().save();

        // Try to keep the order of the tag names by probability of happening
        if ( node.isText() )
        {
            QDomText t ( node.toText() );
            textData = t.data();
        }
        else if ( afterText == "span" ) // text:span
        {
            context.styleStack().save();
            context.fillStyleStack( ts, "text:style-name" );
            loadOasisSpan( ts, context, pos ); // recurse
            context.styleStack().restore();
        }
        else if ( textFoo && afterText == "s" ) // text:s
        {
            int howmany = 1;
            if (ts.hasAttribute("text:c"))
                howmany = ts.attribute("text:c").toInt();

            textData.fill(32, howmany);
        }
        else if ( textFoo && afterText == "tab" ) // text:tab (it's tab-stop in OO-1.1 but tab in oasis)
        {
            textData = '\t';
        }
        else if ( textFoo && afterText == "line-break" ) // text:line-break
        {
            textData = '\n';
        }
        else
        {
            bool handled = false;
            if ( textFoo )
            {
                // Check if it's a variable
                KoVariable* var = context.variableCollection().loadOasisField( textDocument(), ts, context );
                if ( var )
                {
                    textData = "#";     // field placeholder
                    customItem = var;
                    handled = true;
                }
            }
            if ( !handled )
            {
                handled = textDocument()->loadSpanTag( ts, context,
                                                       this, pos,
                                                       textData, customItem );
                if ( !handled )
                {
                    kdWarning(32500) << "Ignoring tag " << ts.tagName() << endl;
                    context.styleStack().restore();
                    continue;
                }
            }
        }

        const uint length = textData.length();
        if ( length )
        {
            append( textData );
            if ( customItem )
                setCustomItem( pos, customItem, 0 );
            KoTextFormat f;
            f.load( context );
            //kdDebug(32500) << "loadOasisSpan: applying formatting from " << pos << " to " << pos+length << "\n   format=" << f.key() << endl;
            setFormat( pos, length, document()->formatCollection()->format( &f ), TRUE );
            pos += length;
        }
        context.styleStack().restore();
    }
}

KoParagLayout KoTextParag::loadParagLayout( KoOasisContext& context, KoStyleCollection *styleCollection, bool findStyle )
{
    KoParagLayout layout;

    // Only when loading paragraphs, not when loading styles
    if ( findStyle )
    {
        KoParagStyle *style;
        // Name of the style. If there is no style, then we do not supply
        // any default!
        QString styleName = context.styleStack().userStyleName();
        if ( !styleName.isEmpty() )
        {
            style = styleCollection->findStyle( styleName );
            if (!style)
            {
                kdError(32001) << "Cannot find style \"" << styleName << "\" - using Standard" << endl;
                style = styleCollection->findStyle( "Standard" );
            }
            //else kdDebug() << "KoParagLayout::KoParagLayout setting style to " << style << " " << style->name() << endl;
        }
        else
        {
            kdError(32001) << "No style name !? - using Standard" << endl;
            style = styleCollection->findStyle( "Standard" );
        }
        Q_ASSERT(style);
        layout.style = style;
    }

    KoParagLayout::loadOasisParagLayout( layout, context );

    return layout;
}

void KoTextParag::loadOasis( const QDomElement& parent, KoOasisContext& context, KoStyleCollection *styleCollection )
{
    // first load layout from style
    KoParagLayout paragLayout = loadParagLayout( context, styleCollection, true );
    setParagLayout( paragLayout );

    // Load default format from style.
    KoTextFormat *defaultFormat = style() ? &style()->format() : 0L;
    setFormat( document()->formatCollection()->format( defaultFormat ) );

    uint pos = 0;
    loadOasisSpan( parent, context, pos );

    setChanged( true );
    invalidate( 0 );
}

void KoTextParag::writeSpanText( KoXmlWriter& writer, const QString& text ) const
{
    uint len = text.length();
    int nrSpaces = 0; // number of consecutive spaces
    QString str;
    str.reserve( len );
    // Accumulate chars either in str or in nrSpaces (for spaces).
    // Flush str when writing a subelement (for spaces or for another reason)
    // Flush nrSpaces when encountering two or more consecutive spaces
    for ( uint i = 0; i < len ; ++i ) {
        QChar ch = text[i];
        if ( ch != ' ' ) {
            if ( nrSpaces > 0 ) {
                // For the first space we use ' '.
                // "it is good practice to use (text:s) for the second and all following SPACE characters in a sequence."
                str += ' ';
                --nrSpaces;
                if ( nrSpaces > 0 ) { // there are more spaces
                    if ( !str.isEmpty() )
                        writer.addTextNode( str );
                    str = QString::null;
                    writer.startElement( "text:s" );
                    if ( nrSpaces > 1 ) // it's 1 by default
                        writer.addAttribute( "text:c", nrSpaces );
                    writer.endElement();
                }
            }
            nrSpaces = 0;
        }
        switch ( ch.unicode() ) {
        case '\t':
            if ( !str.isEmpty() )
                writer.addTextNode( str );
            str = QString::null;
            writer.startElement( "text:tab" );
            if ( m_tabCache.contains( i ) )
                writer.addAttribute( "text:tab-ref", m_tabCache[i] + 1 );
            writer.endElement();
            break;
        case '\n':
            if ( !str.isEmpty() )
                writer.addTextNode( str );
            str = QString::null;
            writer.startElement( "text:line-break" );
            writer.endElement();
            break;
        case ' ':
            ++nrSpaces;
            break;
        default:
            str += text[i];
            break;
        }
    }
    // either we still have text in str or we have spaces in nrSpaces
    if ( nrSpaces > 0 ) {
        str += ' ';
        --nrSpaces;
    }
    if ( !str.isEmpty() ) {
        writer.addTextNode( str );
    }
    if ( nrSpaces > 0 ) { // there are more spaces
        Q_ASSERT( str.isEmpty() );
        writer.startElement( "text:s" );
        if ( nrSpaces > 1 ) // it's 1 by default
            writer.addAttribute( "text:c", nrSpaces );
        writer.endElement();
    }
}

void KoTextParag::saveOasis( KoXmlWriter& writer, KoSavingContext& context,
                             int from /* default 0 */, int to /* default -1 i.e. length()-2 */,
                             bool saveAnchorsFramesets /* default false */ ) const
{
    KoGenStyles& mainStyles = context.mainStyles();

    // Write paraglayout to styles (with parent == the parag's style)
    QString parentStyleName;
    if ( m_layout.style )
        parentStyleName = context.styleAutoName( m_layout.style );

    KoGenStyle autoStyle( KoGenStyle::STYLE_AUTO, "paragraph", parentStyleName );
    paragFormat()->save( autoStyle );
    m_layout.saveOasis( autoStyle );

    QString autoParagStyleName = mainStyles.lookup( autoStyle, "P", true );

    bool outline = m_layout.style && m_layout.style->isOutline();

    KoParagCounter* paragCounter = const_cast<KoTextParag *>( this )->counter(); // should be const!
    if ( paragCounter )
    {
        writer.startElement( "text:numbered-paragraph" );
        writer.addAttribute( "text:level", (int)paragCounter->depth() + 1 );
        if ( paragCounter->restartCounter() )
            writer.addAttribute( "text:start-value", paragCounter->startNumber() );

        KoGenStyle listStyle( KoGenStyle::STYLE_LIST /*, no family*/ );
        paragCounter->saveOasis( listStyle );

        QString autoListStyleName = mainStyles.lookup( listStyle, "L", true );
        writer.addAttribute( "text:style-name", autoListStyleName );
    }
    writer.startElement( outline ? "text:h" : "text:p", false /*no indent inside this tag*/ );
    if ( outline && paragCounter ) {
        writer.addAttribute( "text:outline-level", (int)m_layout.counter->depth() + 1 );
    }
    writer.addAttribute( "text:style-name", autoParagStyleName );

    if ( paragCounter ) {
        // This is to help export filters
        writer.startElement( "text:number" );
        writer.addTextNode( m_layout.counter->text( this ) );
        writer.endElement();
    }

    if ( to == -1 ) {
        // Save the whole parag. If length() == 1 (only trailing space),
        // then to will be set to -1, and we'll save no characters, as intended.
        to = length() - 2;
    }
    QString text = string()->toString();
    Q_ASSERT( text.right(1)[0] == ' ' );

    //kdDebug() << k_funcinfo << "'" << text << "' from=" << from << " to=" << to << endl;

    // A helper method would need no less than 7 params...
#define WRITESPAN( next ) { \
        if ( curFormat == paragFormat() ) {                             \
            writeSpanText( writer, text.mid( startPos, next - startPos ) ); \
        } else {                                                        \
            KoGenStyle gs( KoGenStyle::STYLE_AUTO, "text", autoParagStyleName ); \
            curFormat->save( gs );                                      \
            QString autoStyleName = mainStyles.lookup( gs, "T" );       \
            writer.startElement( "text:span" );                         \
            writer.addAttribute( "text:style-name", autoStyleName );    \
            writeSpanText( writer, text.mid( startPos, next - startPos ) ); \
            writer.endElement();                                        \
        }                                                               \
    }

    KoTextFormat *curFormat = 0;
    int startPos = from;
    // Once we have a span, we need to keep using them. Otherwise we might generate
    // <text:span>foo</text:span> <text:span>bar</text:span> and lose the space between
    // the two spans.
    for ( int i = from; i <= to; ++i ) {
        KoTextStringChar & ch = string()->at(i);
        KoTextFormat * newFormat = static_cast<KoTextFormat *>( ch.format() );
        if ( !curFormat )
            curFormat = newFormat;
        if ( ch.isCustom() ) {
            // TODO implement saving custom items
        } else {
            if ( newFormat != curFormat ) { // Format changed, save previous one.
                WRITESPAN( i )
                startPos = i;
                curFormat = newFormat;
            }
        }
    }

    //kdDebug() << k_funcinfo << "startPos=" << startPos << " to=" << to << " curFormat=" << curFormat << endl;

    if ( to > startPos ) { // Save last format
        WRITESPAN( to + 1 )
    }

    writer.endElement(); // text:p or text:h
    if ( paragCounter )
        writer.endElement(); // text:numbered-paragraph
}

void KoTextParag::applyListStyle( KoOasisContext& context, int restartNumbering, bool orderedList, bool heading, int level )
{
    delete m_layout.counter;
    m_layout.counter = new KoParagCounter;
    m_layout.counter->loadOasis( context, restartNumbering, orderedList, heading, level );
    // need to call invalidateCounters() ? Not during loading at least.
}

int KoTextParag::documentWidth() const
{
    return doc ? doc->width() : 0; //docRect.width();
}

//int KoTextParag::documentVisibleWidth() const
//{
//    return doc ? doc->visibleWidth() : 0; //docRect.width();
//}

int KoTextParag::documentX() const
{
    return doc ? doc->x() : 0; //docRect.x();
}

int KoTextParag::documentY() const
{
    return doc ? doc->y() : 0; //docRect.y();
}

void KoTextParag::fixParagWidth( bool viewFormattingChars )
{
    // Fixing the parag rect for the formatting chars (only CR here, KWord handles framebreak).
    if ( viewFormattingChars && lineStartList().count() == 1 ) // don't use lines() here, parag not formatted yet
    {
        KoTextFormat * lastFormat = at( length() - 1 )->format();
        setWidth( QMIN( rect().width() + lastFormat->width('x'), doc->width() ) );
    }
    // Warning, if adding anything else here, adjust KWTextFrameSet::fixParagWidth
}

// Called by KoTextParag::drawParagString - all params are in pixel coordinates
void KoTextParag::drawFormattingChars( QPainter &painter, int start, int len,
                                       int lastY_pix, int baseLine_pix, int h_pix, // in pixels
                                       bool /*drawSelections*/,
                                       KoTextFormat * /*lastFormat*/, const QMemArray<int> &/*selectionStarts*/,
                                       const QMemArray<int> &/*selectionEnds*/, const QColorGroup &cg,
                                       bool rightToLeft, int /*line*/, KoZoomHandler* zh,
                                       int whichFormattingChars )
{
    if ( !whichFormattingChars )
        return;
    painter.save();
    QPen pen( cg.color( QColorGroup::Highlight ) );
    painter.setPen( pen );
    //kdDebug() << "KWTextParag::drawFormattingChars start=" << start << " len=" << len << " length=" << length() << endl;
    if ( start + len == length() && ( whichFormattingChars & FormattingEndParag ) )
    {
        // drawing the end of the parag
        KoTextStringChar &ch = string()->at( length() - 1 );
        KoTextFormat* format = static_cast<KoTextFormat *>( ch.format() );
        int w = format->charWidth( zh, true, &ch, this, 'X' );
        int size = QMIN( w, h_pix * 3 / 4 );
        // x,y is the bottom right corner of the ¶
        //kdDebug() << "startX=" << startX << " bw=" << bw << " w=" << w << endl;
        int x;
        if ( rightToLeft )
            x = zh->layoutUnitToPixelX( ch.x ) /*+ ch.pixelxadj*/ + ch.pixelwidth - 1;
        else
            x = zh->layoutUnitToPixelX( ch.x ) /*+ ch.pixelxadj*/ + w;
        int y = lastY_pix + baseLine_pix;
        //kdDebug() << "KWTextParag::drawFormattingChars drawing CR at " << x << "," << y << endl;
        painter.drawLine( (int)(x - size * 0.2), y - size, (int)(x - size * 0.2), y );
        painter.drawLine( (int)(x - size * 0.5), y - size, (int)(x - size * 0.5), y );
        painter.drawLine( x, y, (int)(x - size * 0.7), y );
        painter.drawLine( x, y - size, (int)(x - size * 0.5), y - size);
        painter.drawArc( x - size, y - size, size, (int)(size / 2), -90*16, -180*16 );
#ifdef DEBUG_FORMATTING
        painter.setPen( Qt::blue );
        painter.drawRect( zh->layoutUnitToPixelX( ch.x ) /*+ ch.pixelxadj*/ - 1, lastY_pix, ch.pixelwidth, baseLine_pix );
        QPen pen( cg.color( QColorGroup::Highlight ) );
        painter.setPen( pen );
#endif
    }

    // Now draw spaces, tabs and newlines
    if ( (whichFormattingChars & FormattingSpace) ||
         (whichFormattingChars & FormattingTabs) ||
         (whichFormattingChars & FormattingBreak) )
    {
        int end = QMIN( start + len, length() - 1 ); // don't look at the trailing space
        for ( int i = start ; i < end ; ++i )
        {
            KoTextStringChar &ch = string()->at(i);
#ifdef DEBUG_FORMATTING
            painter.setPen( (i % 2)? Qt::red: Qt::green );
            painter.drawRect( zh->layoutUnitToPixelX( ch.x ) /*+ ch.pixelxadj*/ - 1, lastY_pix, ch.pixelwidth, baseLine_pix );
            QPen pen( cg.color( QColorGroup::Highlight ) );
            painter.setPen( pen );
#endif
            if ( ch.isCustom() )
                continue;
            if ( (ch.c == ' ' || ch.c.unicode() == 0x00a0U)
                 && (whichFormattingChars & FormattingSpace))
            {
                // Don't use ch.pixelwidth here. We want a square with
                // the same size for all spaces, even the justified ones
                int w = zh->layoutUnitToPixelX( ch.format()->width( ' ' ) );
                int height = zh->layoutUnitToPixelY( ch.ascent() );
                int size = QMAX( 2, QMIN( w/2, height/3 ) ); // Enfore that it's a square, and that it's visible
                int x = zh->layoutUnitToPixelX( ch.x ); // + ch.pixelxadj;
                QRect spcRect( x + (ch.pixelwidth - size) / 2, lastY_pix + baseLine_pix - (height - size) / 2, size, size );
                if ( ch.c == ' ' )
                    painter.drawRect( spcRect );
                else // nbsp
                    painter.fillRect( spcRect, pen.color() );
            }
            else if ( ch.c == '\t' && (whichFormattingChars & FormattingTabs) )
            {
                /*KoTextStringChar &nextch = string()->at(i+1);
                  int nextx = (nextch.x > ch.x) ? nextch.x : rect().width();
                  //kdDebug() << "tab x=" << ch.x << " nextch.x=" << nextch.x
                  //          << " nextx=" << nextx << " startX=" << startX << " bw=" << bw << endl;
                  int availWidth = nextx - ch.x - 1;
                  availWidth=zh->layoutUnitToPixelX(availWidth);*/

                int availWidth = ch.pixelwidth;

                KoTextFormat* format = ch.format();
                int x = zh->layoutUnitToPixelX( ch.x ) /*+ ch.pixelxadj*/ + availWidth / 2;
                int charWidth = format->screenFontMetrics( zh ).width( 'W' );
                int size = QMIN( availWidth, charWidth ) / 2 ; // actually the half size
                int y = lastY_pix + baseLine_pix - zh->layoutUnitToPixelY( ch.ascent()/2 );
                int arrowsize = zh->zoomItY( 2 );
                painter.drawLine( x - size, y, x + size, y );
                if ( rightToLeft )
                {
                    painter.drawLine( x - size, y, x - size + arrowsize, y - arrowsize );
                    painter.drawLine( x - size, y, x - size + arrowsize, y + arrowsize );
                }
                else
                {
                    painter.drawLine( x + size, y, x + size - arrowsize, y - arrowsize );
                    painter.drawLine( x + size, y, x + size - arrowsize, y + arrowsize );
                }
            }
            else if ( ch.c == '\n' && (whichFormattingChars & FormattingBreak) )
            {
                // draw line break
                KoTextFormat* format = static_cast<KoTextFormat *>( ch.format() );
                int w = format->charWidth( zh, true, &ch, this, 'X' );
                int size = QMIN( w, h_pix * 3 / 4 );
                int arrowsize = zh->zoomItY( 2 );
                // x,y is the bottom right corner of the reversed L
                //kdDebug() << "startX=" << startX << " bw=" << bw << " w=" << w << endl;
                int y = lastY_pix + baseLine_pix - arrowsize;
                //kdDebug() << "KWTextParag::drawFormattingChars drawing Line Break at " << x << "," << y << endl;
                if ( rightToLeft )
                {
                    int x = zh->layoutUnitToPixelX( ch.x ) /*+ ch.pixelxadj*/ + ch.pixelwidth - 1;
                    painter.drawLine( x - size, y - size, x - size, y );
                    painter.drawLine( x - size, y, (int)(x - size * 0.3), y );
                    // Now the arrow
                    painter.drawLine( (int)(x - size * 0.3), y, (int)(x - size * 0.3 - arrowsize), y - arrowsize );
                    painter.drawLine( (int)(x - size * 0.3), y, (int)(x - size * 0.3 - arrowsize), y + arrowsize );
                }
                else
                {
                    int x = zh->layoutUnitToPixelX( ch.x ) /*+ ch.pixelxadj*/ + w - 1;
                    painter.drawLine( x, y - size, x, y );
                    painter.drawLine( x, y, (int)(x - size * 0.7), y );
                    // Now the arrow
                    painter.drawLine( (int)(x - size * 0.7), y, (int)(x - size * 0.7 + arrowsize), y - arrowsize );
                    painter.drawLine( (int)(x - size * 0.7), y, (int)(x - size * 0.7 + arrowsize), y + arrowsize );
                }
            }
        }
        painter.restore();
    }
}
