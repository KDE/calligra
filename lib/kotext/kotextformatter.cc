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

#include "kotextformatter.h"
#include "kotextformat.h"
#include "kotextdocument.h"
#include "kozoomhandler.h"

//#define DEBUG_FORMATTER

// Heavily based on QTextFormatterBreakWords::format()
int KoTextFormatter::format( QTextDocument *doc, QTextParag *parag,
                             int start, const QMap<int, QTextParagLineStart*> & )
{
    QTextStringChar *c = 0;
    start = 0;
    if ( start == 0 )
	c = &parag->string()->at( 0 );

    QTextStringChar *firstChar = 0;
    QTextString *string = parag->string();
    int left = doc ? parag->leftMargin() + doc->leftMargin() : 0;
    int x = left + ( doc ? parag->firstLineMargin() : 0 );
    int curLeft = left;
    int y = doc && doc->addMargins() ? parag->topMargin() : 0;
    int h = 0;
    int len = parag->length();

    int initialHeight = h + c->height(); // remember what adjustLMargin was called with
    if ( doc )
	x = doc->flow()->adjustLMargin( y + parag->rect().y(), h + c->height(), x, 4 );
    int initialLMargin = x;	      // and remember the resulting adjustement we got
    int dw = parag->documentVisibleWidth() - ( doc ? ( left != x ? 0 : doc->rightMargin() ) : -4 );

    curLeft = x;
    int rm = parag->rightMargin();
    int initialRMargin = doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), h + c->height(), rm, 4 ) : 0;
    int w = dw - initialRMargin;
#ifdef DEBUG_FORMATTER
    qDebug( "QTextFormatterBreakWords::format left=%d initialHeight=%d initialLMargin=%d initialRMargin=%d w=%d", left, initialHeight, initialLMargin, initialRMargin, w );
#endif
    bool fullWidth = TRUE;
    int marg = left + initialRMargin;
    int minw = 0;
    int tminw = marg;
    bool wrapEnabled = isWrapEnabled( parag );
    int linenr = 0;

    int i = start;
#ifdef DEBUG_FORMATTER
    qDebug( "Initial QTextParagLineStart at y=%d", y );
#endif
    QTextParagLineStart *lineStart = new QTextParagLineStart( y, 0, 0 );
    insertLineStart( parag, 0, lineStart );
    int lastBreak = -1;
    int tmpBaseLine = 0, tmph = 0;
    bool lastWasNonInlineCustom = FALSE;

    int align = parag->alignment();
    if ( align == Qt3::AlignAuto && doc && doc->alignment() != Qt3::AlignAuto )
	align = doc->alignment();

    int col = 0;
    int ww = 0; // width in layout units

    KoTextDocument *textdoc = static_cast<KoTextDocument *>(doc);
    KoZoomHandler *zh = textdoc->zoomHandler();
    int pixelww = 0; // width in pixels
    int pixelx = zh->layoutUnitToPixelX( x );

    QChar lastChr;
    for ( ; i < len; ++i, ++col ) {
	if ( c )
	    lastChr = c->c;
	c = &string->at( i );
	if ( i > 0 && (x > curLeft || ww == 0) || lastWasNonInlineCustom ) {
	    c->lineStart = 0;
	} else {
	    c->lineStart = 1;
	    firstChar = c;
	}

	if ( c->isCustom() && c->customItem()->placement() != QTextCustomItem::PlaceInline )
	    lastWasNonInlineCustom = TRUE;
	else
	    lastWasNonInlineCustom = FALSE;

	if ( c->c.unicode() >= 32 || c->isCustom() ) {
	    ww = string->width( i );

            // Pixel size - need to change the font temporarily (same code as KoTextParag::drawParagString)
            QTextStringChar *c = &string->at( i );
            KoTextFormat *origFormat = static_cast<KoTextFormat *>(c->format()); // remember it
            KoTextFormat tmpFormat( *origFormat );  // make a copy
            tmpFormat.setPointSizeFloat( zh->layoutUnitToFontSize( tmpFormat.font().pointSize(), false /* TODO forPrint*/ ) );
            c->setFormat( &tmpFormat );
            pixelww = string->width( i );
            c->setFormat( origFormat );
	} else if ( c->c == '\t' ) {
	    int nx = parag->nextTab( i, x );
	    if ( nx < x )
		ww = w - x;
	    else
		ww = nx - x + 1;
            pixelww = zh->layoutUnitToPixelX( ww );
	} else {
	    ww = c->format()->width( ' ' );
            pixelww = zh->layoutUnitToPixelX( ww );
	}
        c->width = ww;

        //Currently unused in KWord
#if 0
	// Custom item that forces a new line
	if ( c->isCustom() && c->customItem()->ownLine() ) {
	    x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), c->height(), left, 4 ) : left;
	    w = dw - ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), c->height(), rm, 4 ) : 0 );
	    QTextParagLineStart *lineStart2 = formatLineKo( zh, parag, string, lineStart, firstChar, c-1, align, w - x );
	    c->customItem()->resize( parag->painter(), dw );
	    if ( x != left || w != dw )
		fullWidth = FALSE;
	    curLeft = x;
	    if ( i == 0 || !isBreakable( string, i - 1 ) || string->at( i - 1 ).lineStart == 0 ) {
		// Create a new line for this custom item
		lineStart->h += doc ? parag->lineSpacing( linenr++ ) : 0;
		y += QMAX( h, tmph );
		tmph = c->height();
		h = tmph;
		lineStart = lineStart2;
		lineStart->y = y;
		insertLineStart( parag, i, lineStart );
		c->lineStart = 1;
		firstChar = c;
	    } else {
		// No need for a new line, already at beginning of line
		tmph = c->height();
		h = tmph;
		delete lineStart2;
	    }
	    lineStart->h = h;
	    lineStart->baseLine = h;
	    tmpBaseLine = lineStart->baseLine;
	    lastBreak = -2;
	    x = 0xffffff;
	    minw = QMAX( minw, tminw );
	    int tw = QMAX( c->customItem()->minimumWidth(), QMIN( c->customItem()->widthHint(), c->customItem()->width ) );
	    if ( tw < 32000 )
		tminw = tw;
	    else
		tminw = marg;
	    continue;
	}
#endif

#ifdef DEBUG_FORMATTER
	qDebug("c='%c' i=%d/%d x=%d ww=%d w=%d (test is x+ww>w) lastBreak=%d isBreakable=%d",c->c.latin1(),i,len,x,ww,w,lastBreak,isBreakable(string,i));
#endif
	// Wrapping at end of line
	if ( wrapEnabled
	     // Allow '  ' but not more
	     && ( !isBreakable( string, i ) || ( i > 1 && lastBreak == i-1 && isBreakable( string, i-2 ) )
					    || lastBreak == -2 )
	     && ( lastBreak != -1 || allowBreakInWords() ) &&
	     ( wrapAtColumn() == -1 && x + ww > w && lastBreak != -1 ||
	       wrapAtColumn() == -1 && x + ww > w - 4 && lastBreak == -1 && allowBreakInWords() ||
	       wrapAtColumn() != -1 && col >= wrapAtColumn() ) ||
	       parag->isNewLinesAllowed() && lastChr == '\n' ) {
#ifdef DEBUG_FORMATTER
	    qDebug( "BREAKING" );
#endif
	    if ( wrapAtColumn() != -1 )
		minw = QMAX( minw, x + ww );
	    // No breakable char found -> break at current char
	    if ( lastBreak < 0 ) {
		ASSERT( lineStart );
		//if ( lineStart ) {
		    // (combine lineStart and tmpBaseLine/tmph)
		    int belowBaseLine = QMAX( h - lineStart->baseLine, tmph - tmpBaseLine );
		    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
		    h = lineStart->baseLine + belowBaseLine;
		    lineStart->h = h;
		//}
		QTextParagLineStart *lineStart2 = formatLineKo( zh, parag, string, lineStart, firstChar, c-1, align, w - x );
		lineStart->h += doc ? parag->lineSpacing( linenr++ ) : 0;
		y += lineStart->h;
#ifdef DEBUG_FORMATTER
                qDebug( "new line created, linenr now %d", linenr );
#endif

		lineStart = lineStart2;
		tmph = c->height();
		h = 0;
		x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), tmph, left, 4 ) : left;
                pixelx = zh->layoutUnitToPixelX( x );
		initialHeight = tmph;
		initialLMargin = x;
		initialRMargin = ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), tmph, rm, 4 ) : 0 );
		w = dw - initialRMargin;
		if ( parag->isNewLinesAllowed() && c->c == '\t' ) {
		    int nx = parag->nextTab( i, x );
		    if ( nx < x )
			ww = w - x;
		    else
			ww = nx - x + 1;
		}
		if ( x != left || w != dw )
		    fullWidth = FALSE;
		curLeft = x;
		lineStart->y = y;
		insertLineStart( parag, i, lineStart );
		lineStart->baseLine = c->ascent();
		lineStart->h = c->height();
		c->lineStart = 1;
		firstChar = c;
		tmpBaseLine = lineStart->baseLine;
		lastBreak = -1;
		col = 0;
		tminw = marg;
	    } else {
		// Breakable char was found
		i = lastBreak;
		QTextParagLineStart *lineStart2 = formatLineKo( zh, parag, string, lineStart, firstChar, parag->at( lastBreak ), align, w - string->at( i ).x );
		lineStart->h += doc ? parag->lineSpacing( linenr++ ) : 0;
		y += lineStart->h;
		lineStart = lineStart2;
#ifdef DEBUG_FORMATTER
		qDebug("Breaking at a breakable char (%d). linenr=%d y=%d",i,linenr,y);
#endif
		c = &string->at( i + 1 );
		tmph = c->height();
		h = tmph;
		x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), h, left, 4 ) : left;
                pixelx = zh->layoutUnitToPixelX( x );
		initialHeight = h;
		initialLMargin = x;
		initialRMargin = ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), h, rm, 4 ) : 0 );
		w = dw - initialRMargin;
		if ( x != left || w != dw )
		    fullWidth = FALSE;
		curLeft = x;
		lineStart->y = y;
		insertLineStart( parag, i + 1, lineStart );
		lineStart->baseLine = c->ascent();
		lineStart->h = c->height();
		c->lineStart = 1;
		firstChar = c;
		tmpBaseLine = lineStart->baseLine;
		lastBreak = -1;
		col = 0;
		tminw = marg;
		continue;
	    }
	} else if ( lineStart && ( isBreakable( string, i ) || parag->isNewLinesAllowed() && c->c == '\n' ) ) {
	    // Breakable character
	    if ( len <= 2 || i < len - 1 ) {
		//qDebug( " Breakable character (i=%d len=%d): combining %d/%d with %d/%d", i, len,
		//	tmpBaseLine, tmph, c->ascent(), c->height()+ls );
		// (combine tmpBaseLine/tmph and this character)
		int belowBaseLine = QMAX( tmph - tmpBaseLine, c->height() - c->ascent() );
		tmpBaseLine = QMAX( tmpBaseLine, c->ascent() );
		tmph = tmpBaseLine + belowBaseLine;
		//qDebug(  " -> tmpBaseLine/tmph : %d/%d", tmpBaseLine, tmph );
	    }
	    minw = QMAX( minw, tminw );
	    tminw = marg + ww;
	    // (combine lineStart and tmpBaseLine/tmph)
	    //qDebug( "Breakable character: combining %d/%d with %d/%d", lineStart->baseLine, h, tmpBaseLine, tmph );
	    int belowBaseLine = QMAX( h - lineStart->baseLine, tmph - tmpBaseLine );
	    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
	    h = lineStart->baseLine + belowBaseLine;
	    lineStart->h = h;
	    // if h > initialHeight,  call adjust[LR]Margin, and if the result is != initial[LR]Margin,
	    // format this line again
	    if ( doc && h > initialHeight )
	    {
		int lm = left + ( ( firstChar == &string->at(0) && doc ) ? parag->firstLineMargin() : 0 );
#ifdef DEBUG_FORMATTER
                qDebug( "left=%d, firstlinepargin=%d => lm=%d", left, ( firstChar == &string->at(0) && doc ) ? parag->firstLineMargin() : 0, lm );
#endif
		int newLMargin = doc->flow()->adjustLMargin( y + parag->rect().y(), h, lm, 4 );
		int newRMargin = doc->flow()->adjustRMargin( y + parag->rect().y(), h, rm, 4 );
		initialHeight = h;
#ifdef DEBUG_FORMATTER
		qDebug("new height: %d => left=%d lm=%d first-char=%d newLMargin=%d newRMargin=%d", h, left, lm, (firstChar==&string->at(0)), newLMargin, newRMargin);
#endif
		if ( newLMargin != initialLMargin || newRMargin != initialRMargin )
		{
#ifdef DEBUG_FORMATTER
		    qDebug("formatting again");
#endif
		    i = (firstChar - &string->at(0));
		    x = newLMargin;
                    pixelx = zh->layoutUnitToPixelX( x );
		    w = dw - newRMargin;
		    initialLMargin = newLMargin;
		    initialRMargin = newRMargin;
		    c = &string->at( i );
		    tmph = c->height();
		    h = tmph;
		    tmpBaseLine = c->ascent();
		    lineStart->h = h;
		    lineStart->baseLine = tmpBaseLine;
		    curLeft = x;
		    lastBreak = -1;
		    col = 0;
#ifdef DEBUG_FORMATTER
		    qDebug("Restarting with i=%d x=%d y=%d h=%d initialHeight=%d initialLMargin=%d initialRMargin=%d y=%d",i,x,y,h,initialHeight,initialLMargin,initialRMargin,y);
#endif
		    if ( c->c.unicode() >= 32 || c->isCustom() )
			ww = string->width( i );
		    else if ( parag->isNewLinesAllowed() && firstChar->c == '\t' ) {
			int nx = parag->nextTab( i, x );
			if ( nx < x )
			    ww = w - x;
			else
			    ww = nx - x + 1;
		    } else {
			ww = c->format()->width( ' ' );
		    }
		    //### minw ? tminw ?
		}
	    }

	    //qDebug(  " -> lineStart->baseLine/lineStart->h : %d/%d", lineStart->baseLine, lineStart->h );
	    if ( i < len - 2 || c->c != ' ' )
		lastBreak = i;

            // Re-sync x and pixelx (this is how we steal white pixels in spaces to compensate for rounding errors)
            //pixelx = zh->layoutUnitToPixelX( x );
            // More complex than that. It's the _space_ that has to grow/shrink
            pixelww -= pixelx - zh->layoutUnitToPixelX( x );
#ifdef DEBUG_FORMATTER
            qDebug("pixelww adjusted");
#endif
	} else {
	    // Non-breakable character
	    tminw += ww;
	    //qDebug( " Non-breakable character: combining %d/%d with %d/%d", tmpBaseLine, tmph, c->ascent(), c->height()+ls );
	    // (combine tmpBaseLine/tmph and this character)
	    int belowBaseLine = QMAX( tmph - tmpBaseLine, c->height() - c->ascent() );
	    tmpBaseLine = QMAX( tmpBaseLine, c->ascent() );
	    tmph = tmpBaseLine + belowBaseLine;
	    //qDebug(  " -> tmpBaseLine/tmph : %d/%d", tmpBaseLine, tmph );
	}

	c->x = x;
        // pixelxadj is the adjustement to add to lu2pixel(x), to find pixelx
        // (pixelx would be too expensive to store directly since it would require an int)
        c->pixelxadj = pixelx - zh->layoutUnitToPixelX( x );
        c->pixelwidth = pixelww;
#ifdef DEBUG_FORMATTER
        qDebug("LU: x=%d [equiv. to pix=%d] ; PIX: x=%d  --> adj=%d",
               x, zh->layoutUnitToPixelX( x ), pixelx, c->pixelxadj );
#endif
	if ( c->isCustom() )
	    c->customItem()->move( x, y );
	x += ww;
        pixelx += pixelww;
#ifdef DEBUG_FORMATTER
	qDebug("LU: added %d -> now x=%d ; PIX: added %d -> now pixelx=%d",ww,x,pixelww,pixelx);
#endif
    }

    // Finish formatting the last line
    if ( lineStart ) {
#ifdef DEBUG_FORMATTER
	qDebug( "Last Line.... linenr=%d", linenr );
#endif
        //qDebug( "Combining %d/%d with %d/%d", lineStart->baseLine, h, tmpBaseLine, tmph );
	// (combine lineStart and tmpBaseLine/tmph)
	int belowBaseLine = QMAX( h - lineStart->baseLine, tmph - tmpBaseLine );
	lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
	h = lineStart->baseLine + belowBaseLine;
	lineStart->h = h;
	//qDebug(  " -> lineStart->baseLine/lineStart->h : %d/%d", lineStart->baseLine, lineStart->h );
	// last line in a paragraph is not justified
	if ( align == Qt3::AlignJustify )
	    align = Qt3::AlignAuto;
	QTextParagLineStart *lineStart2 = formatLineKo( zh, parag, string, lineStart, firstChar, c, align, w - x );
	h += doc ? parag->lineSpacing( linenr++ ) : 0;
	lineStart->h = h;
	delete lineStart2;
    }

    minw = QMAX( minw, tminw );

    int m = parag->bottomMargin();
    if ( parag->next() && doc && !doc->addMargins() )
	m = QMAX( m, parag->next()->topMargin() );
    parag->setFullWidth( fullWidth );
    /*if ( is_printer( parag->painter() ) ) {
	QPaintDeviceMetrics metrics( parag->painter()->device() );
	double yscale = scale_factor( metrics.logicalDpiY() );
	m = (int)( (double)m * yscale );
    }*/
    //qDebug( "Adding h(%d) and bottomMargin(%d) to y(%d) => %d", h, m, y, y+h+m );
    y += h + m;

    /*
    if ( !wrapEnabled )
	minw = QMAX( minw, c->x + ww ); // #### Lars: Fix this for BiDi, please
    if ( doc ) {
	if ( minw < 32000 )
	    doc->setMinimumWidth( minw, parag );
    }*/

    return y;
}

QTextParagLineStart *KoTextFormatter::formatLineKo(
    KoZoomHandler *zh,
    QTextParag * /*parag*/, QTextString *string, QTextParagLineStart *line,
    QTextStringChar *startChar, QTextStringChar *lastChar, int align, int space )
{
//QT2HACK
//    if( string->isBidi() )
//	return bidiReorderLine( parag, string, line, startChar, lastChar, align, space );
    space = QMAX( space, 0 ); // #### with nested tables this gets negative because of a bug I didn't find yet, so workaround for now. This also means non-left aligned nested tables do not work at the moment
    int start = (startChar - &string->at(0));
    int last = (lastChar - &string->at(0) );
    // do alignment Auto == Left in this case
    if ( align & Qt::AlignHCenter || align & Qt::AlignRight ) {
	if ( align & Qt::AlignHCenter )
	    space /= 2;
	for ( int j = start; j <= last; ++j )
	    string->at( j ).x += space;
    } else if ( align & Qt3::AlignJustify ) {
	int numSpaces = 0;
	for ( int j = start; j < last; ++j ) {
	    if( isBreakable( string, j ) ) {
		numSpaces++;
	    }
	}
	int toAdd = 0;
	for ( int k = start + 1; k <= last; ++k ) {
            QTextStringChar &chr = string->at( k );
            if ( toAdd != 0 )
            {
                int pixelx = chr.pixelxadj + zh->layoutUnitToPixelX( chr.x );
                chr.x += toAdd;
                // Seems this is necessary. This means, pixelxadj really contains
                // some rounding-related values, that have to be recalculated when
                // pushing things to the right.
                pixelx += zh->layoutUnitToPixelX( toAdd );
                chr.pixelxadj = pixelx - zh->layoutUnitToPixelX( chr.x );
            }
	    if( isBreakable( string, k ) && numSpaces ) {
		int s = space / numSpaces;
		toAdd += s;
		space -= s;
		numSpaces--;
                chr.width += s;
                chr.pixelwidth += zh->layoutUnitToPixelX( s );
	    }
	}
    }

    if ( last >= 0 && last < string->length() )
	line->w = string->at( last ).x + string->width( last );
    else
	line->w = 0;

    return new QTextParagLineStart();
}
