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
#include "kozoomhandler.h"

#include <kdebug.h>
//#define DEBUG_FORMATTER

// Originally based on KoTextFormatterBaseBreakWords::format()
int KoTextFormatter::format( KoTextDocument *doc, KoTextParag *parag,
                             int start, const QMap<int, KoTextParagLineStart*> & )
{
    KoTextStringChar *c = 0;
    start = 0;
    if ( start == 0 )
	c = &parag->string()->at( 0 );

    KoTextStringChar *firstChar = 0;
    KoTextString *string = parag->string();
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
    int availableWidth = dw - initialRMargin; // 'w' in QRT
#ifdef DEBUG_FORMATTER
    qDebug( "KoTextFormatterBaseBreakWords::format left=%d initialHeight=%d initialLMargin=%d initialRMargin=%d w=%d", left, initialHeight, initialLMargin, initialRMargin, w );
#endif
    bool fullWidth = TRUE;
    int marg = left + initialRMargin;
    int minw = 0;
    int wused = 0;
    int tminw = marg;
    bool wrapEnabled = isWrapEnabled( parag );
    int linenr = 0;

    int i = start;
#ifdef DEBUG_FORMATTER
    qDebug( "Initial KoTextParagLineStart at y=%d", y );
#endif
    KoTextParagLineStart *lineStart = new KoTextParagLineStart( y, 0, 0 );
    insertLineStart( parag, 0, lineStart );
    int lastBreak = -1;
    int tmpBaseLine = 0, tmph = 0;
    bool lastWasNonInlineCustom = FALSE;

    int align = parag->alignment();
    if ( align == Qt::AlignAuto && doc && doc->alignment() != Qt::AlignAuto )
	align = doc->alignment();

    int col = 0;
    int ww = 0; // width in layout units

    KoZoomHandler *zh = doc->formattingZoomHandler();
    int pixelww = 0; // width in pixels
    int pixelx = zh->layoutUnitToPixelX( x );

    parag->tabCache().clear();

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

	if ( c->isCustom() && c->customItem()->placement() != KoTextCustomItem::PlaceInline )
	    lastWasNonInlineCustom = TRUE;
	else
	    lastWasNonInlineCustom = FALSE;

	if ( c->c != '\t' || c->isCustom() ) {
            // Instead of using the high-resolution font, let's use the 100%-zoom-font
            // and calculate the LU size it is equivalent to. This gives better results
            // at most usual zoom resolutions.
	    // ww = string->width( i );
            KoTextFormat *charFormat = c->format();
            if ( c->isCustom() ) {
                ww = c->customItem()->width;
                pixelww = zh->layoutUnitToPixelX( ww );
            } else {
                ww = charFormat->charWidthLU( c, parag, i );
                // Pixel size - we want the metrics of the font that's going to be used.
                pixelww = charFormat->charWidth( zh, true, c, parag, i );
            }

            // This was wrong - we paint the text word by word anyway
#if 0
            bool breakable = ( lineStart && ( isBreakable( string, i ) || parag->isNewLinesAllowed() && c->c == '\n' ) ); // same test as below

            int ww_topix = zh->layoutUnitToPixelX(ww);
            // We have to limit the difference between pixel-width and proportional width
            // to negative differences. Such differences can be compensated on spaces, making
            // them larger. Positive differences can lead to a 0-sized or even a negative
            // sized spaces, which we don't want. This doesn't apply to spaces themselves, of course.
            if ( !breakable && pixelww > ww_topix  ) {
#ifdef DEBUG_FORMATTER
                qDebug("pixelww (%d) bigger than lu2pixel(%d)=%d -> setting to %d",
                       pixelww, ww, ww_topix, ww_topix);
#endif
                pixelww = ww_topix;
            }
#endif
	} else { // tab
	    int nx = parag->nextTab( i, x );
	    if ( nx < x )
		ww = availableWidth - x;
	    else
		ww = nx - x + 1;
            pixelww = zh->layoutUnitToPixelX( ww );
	}
        c->width = ww;

        //Currently unused in KWord
#if 0
	// Custom item that forces a new line
	if ( c->isCustom() && c->customItem()->ownLine() ) {
            QTextCustomItem* ci = c->customItem();
	    x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), c->height(), left, 4 ) : left;
	    w = dw - ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), c->height(), rm, 4 ) : 0 );
	    KoTextParagLineStart *lineStart2 = koFormatLine( zh, parag, string, lineStart, firstChar, c-1, align, availableWidth - x );
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
	    int tw = ci->minimumWidth();
	    if ( tw < QWIDGETSIZE_MAX )
		tminw = tw;
	    else
		tminw = marg;
 	    wused = QMAX( wused, ci->width );
	    continue;
	} // else ... left/right custom items. Unused too atm.
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
	     ( wrapAtColumn() == -1 && x + ww > availableWidth && lastBreak != -1 ||
	       wrapAtColumn() == -1 && x + ww > availableWidth - 4 && lastBreak == -1 && allowBreakInWords() ||
	       wrapAtColumn() != -1 && col >= wrapAtColumn() ) ||
	       parag->isNewLinesAllowed() && lastChr == '\n' && lastBreak > -1 ) {
#ifdef DEBUG_FORMATTER
	    qDebug( "BREAKING" );
#endif
	    if ( wrapAtColumn() != -1 )
		minw = QMAX( minw, x + ww );
	    // No breakable char found -> break at current char
	    if ( lastBreak < 0 ) {
		Q_ASSERT( lineStart );
		//if ( lineStart ) {
		    // (combine lineStart and tmpBaseLine/tmph)
		    int belowBaseLine = QMAX( h - lineStart->baseLine, tmph - tmpBaseLine );
		    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
		    h = lineStart->baseLine + belowBaseLine;
		    lineStart->h = h;
		//}
		KoTextParagLineStart *lineStart2 = koFormatLine( zh, parag, string, lineStart, firstChar, c-1, align, availableWidth - x );
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
		availableWidth = dw - initialRMargin;
		if ( parag->isNewLinesAllowed() && c->c == '\t' ) {
		    int nx = parag->nextTab( i, x );
		    if ( nx < x )
			ww = availableWidth - x;
		    else
			ww = nx - x + 1;
		}
		if ( x != left || availableWidth != dw )
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
		tminw = marg; // not in QRT?
	    } else {
		// Breakable char was found
		i = lastBreak;
		KoTextParagLineStart *lineStart2 = koFormatLine( zh, parag, string, lineStart, firstChar, parag->at( lastBreak ), align,
		                                                availableWidth - string->at( i ).x - ( string->isRightToLeft() && lastChr == '\n'? (c - 1)->width: 0 ) );
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
		availableWidth = dw - initialRMargin;
		if ( x != left || availableWidth != dw )
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
		    availableWidth = dw - newRMargin;
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
                    // ww and pixelww already calculated and stored, no need to duplicate
                    // code like QRT does.
                    ww = c->width;
                    pixelww = c->pixelwidth;
		}
	    }

	    //qDebug(  " -> lineStart->baseLine/lineStart->h : %d/%d", lineStart->baseLine, lineStart->h );
	    if ( i < len - 2 || c->c != ' ' )
		lastBreak = i;

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

        bool breakable = ( lineStart && ( isBreakable( string, i ) || parag->isNewLinesAllowed() && c->c == '\n' ) ); // same test as above
        if ( breakable /* || ( lastBreak != -1 && i - lastBreak > 5 ) || ( lastBreak == -1 && i % 5 == 0 ) */ )
        {
            // Re-sync x and pixelx (this is how we steal white pixels in spaces to compensate for rounding errors)
            //pixelx = zh->layoutUnitToPixelX( x );
            // More complex than that. It's the _space_ that has to grow/shrink
#ifdef DEBUG_FORMATTER
            int oldpixelww = pixelww;
#endif
            pixelww -= pixelx - zh->layoutUnitToPixelX( x );
#ifdef DEBUG_FORMATTER
            qDebug("pixelww was %d, now %d. Adjusted by pixelx - x. x=%d pixelx=%d", oldpixelww, pixelww, zh->layoutUnitToPixelX( x ), pixelx);
#endif
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
	x += ww;
        pixelx += pixelww;
        if ( i < len - 1 )
            wused = QMAX( wused, x );
#ifdef DEBUG_FORMATTER
	qDebug("LU: added %d -> now x=%d ; PIX: added %d -> now pixelx=%d",ww,x,pixelww,pixelx);
#endif
    }

    // ### hack. The last char in the paragraph is always invisible, and somehow sometimes has a wrong format. It changes between
    // layouting and printing. This corrects some layouting errors in BiDi mode due to this.
    if ( len > 1 /*&& !c->isAnchor()*/ ) {
	c->format()->removeRef();
	c->setFormat( string->at( len - 2 ).format() );
	c->format()->addRef();
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
	if ( align == Qt::AlignJustify )
	    align = Qt::AlignAuto;
        int space = availableWidth - x + c->width; // don't count the trailing space (it breaks e.g. centering)
        KoTextParagLineStart *lineStart2 = koFormatLine( zh, parag, string, lineStart, firstChar, c, align, space );

	h += doc ? parag->lineSpacing( linenr++ ) : 0;
	lineStart->h = h;
	delete lineStart2;
    }

    minw = QMAX( minw, tminw );

    int m = parag->bottomMargin();
    if ( parag->next() && doc && !doc->addMargins() )
	m = QMAX( m, parag->next()->topMargin() );
    parag->setFullWidth( fullWidth );
    //if ( parag->next() && parag->next()->isLineBreak() )
    //    m = 0;
    //qDebug( "Adding h(%d) and bottomMargin(%d) to y(%d) => %d", h, m, y, y+h+m );
    y += h + m;

    wused += rm;
    if ( !wrapEnabled || wrapAtColumn() != -1  )
	minw = QMAX(minw, wused);
    thisminw = minw;
    thiswused = wused;
    return y;
}

// Helper for koFormatLine and koBidiReorderLine
void KoTextFormatter::moveChar( KoTextStringChar& chr, KoZoomHandler *zh,
                                int deltaX, int deltaPixelX )
{
    int pixelx = chr.pixelxadj + zh->layoutUnitToPixelX( chr.x );
    chr.x += deltaX;
    chr.pixelxadj = pixelx + deltaPixelX - zh->layoutUnitToPixelX( chr.x );
}

KoTextParagLineStart *KoTextFormatter::koFormatLine(
    KoZoomHandler *zh,
    KoTextParag *parag, KoTextString *string, KoTextParagLineStart *line,
    KoTextStringChar *startChar, KoTextStringChar *lastChar, int align, int space )
{
    if( string->isBidi() )
	return koBidiReorderLine( zh, parag, string, line, startChar, lastChar, align, space );
    space = QMAX( space, 0 ); // #### with nested tables this gets negative because of a bug I didn't find yet, so workaround for now. This also means non-left aligned nested tables do not work at the moment
    int start = (startChar - &string->at(0));
    int last = (lastChar - &string->at(0) );
    // do alignment Auto == Left in this case
    if ( align & Qt::AlignHCenter || align & Qt::AlignRight ) {
	if ( align & Qt::AlignHCenter )
	    space /= 2;
        int toAddPix = zh->layoutUnitToPixelX( space );
	for ( int j = last; j >= start; --j ) {
            KoTextStringChar &chr = string->at( j );
            //// Start at last tab, if any - BR #40472.
            if ( chr.c == '\t' ) {
                break;
            }
            moveChar( chr, zh, space, toAddPix );
        }
    } else if ( align & Qt::AlignJustify ) {
	int numSpaces = 0;
        // End at "last-1", the last space ends up with a width of 0
	for ( int j = last-1; j >= start; --j ) {
            //// Start at last tab, if any. BR #40472 specifies that justifying should start after the last tab.
            if ( string->at( j ).c == '\t' ) {
                start = j+1;
                break;
            }
	    if( isBreakable( string, j ) ) {
		numSpaces++;
	    }
	}
	int toAdd = 0;
        int toAddPix = 0;
	for ( int k = start + 1; k <= last; ++k ) {
            KoTextStringChar &chr = string->at( k );
            if ( toAdd != 0 )
                moveChar( chr, zh, toAdd, toAddPix );
	    if( isBreakable( string, k ) && numSpaces ) {
		int s = space / numSpaces;
		toAdd += s;
                toAddPix = zh->layoutUnitToPixelX( toAdd );
		space -= s;
		numSpaces--;
                chr.width += s;
                chr.pixelwidth += zh->layoutUnitToPixelX( s ); // ### rounding problem, recalculate
	    }
	}
    }

    if ( last >= 0 && last < string->length() )
	line->w = string->at( last ).x + string->at( last ).width; //string->width( last );
    else
	line->w = 0;

    return new KoTextParagLineStart();
}

// collects one line of the paragraph and transforms it to visual order
KoTextParagLineStart *KoTextFormatter::koBidiReorderLine(
    KoZoomHandler *zh,
    KoTextParag * /*parag*/, KoTextString *text, KoTextParagLineStart *line,
    KoTextStringChar *startChar, KoTextStringChar *lastChar, int align, int space )
{
    int start = (startChar - &text->at(0));
    int last = (lastChar - &text->at(0) );
#ifdef DEBUG_FORMATTER
    kdDebug(32500) << "*KoTextFormatter::koBidiReorderLine from " << start << " to " << last << " space=" << space << " startChar->x=" << startChar->x << endl;
#endif
    KoBidiControl *control = new KoBidiControl( line->context(), line->status );
    QString str;
    str.setUnicode( 0, last - start + 1 );
    // fill string with logically ordered chars.
    KoTextStringChar *ch = startChar;
    QChar *qch = (QChar *)str.unicode();
    while ( ch <= lastChar ) {
	*qch = ch->c;
	qch++;
	ch++;
    }
    int x = startChar->x;

    QPtrList<KoTextRun> *runs;
    runs = KoComplexText::bidiReorderLine(control, str, 0, last - start + 1,
					 (text->isRightToLeft() ? QChar::DirR : QChar::DirL) );

    // now construct the reordered string out of the runs...

    int numSpaces = 0;
    // set the correct alignment. This is a bit messy....
    if( align == Qt::AlignAuto ) {
	// align according to directionality of the paragraph...
	if ( text->isRightToLeft() )
	    align = Qt::AlignRight;
    }

    if ( align & Qt::AlignHCenter ) {
        if ( text->isRightToLeft() )
            x = space/2 - x;
        else
            x += space/2;
    } else if ( align & Qt::AlignRight ) {
        if ( text->isRightToLeft() )
            x = space - x;
        else
            x += space;
    } else if ( align & Qt::AlignJustify ) {
	for ( int j = last - 1; j >= start; --j ) {
            //// Start at last tab, if any. BR #40472 specifies that justifying should start after the last tab.
            if ( text->at( j ).c == '\t' ) {
                start = j+1;
                break;
            }
	    if( isBreakable( text, j ) ) {
		numSpaces++;
	    }
	}
    }
    int pixelx = zh->layoutUnitToPixelX( x );
    int toAdd = 0;
    int toAddPix = 0;
    bool first = TRUE;
    KoTextRun *r = runs->first();
    int xmax = -0xffffff;
    while ( r ) {
#ifdef DEBUG_FORMATTER
        qDebug("koBidiReorderLine level: %d",r->level);
#endif
	if(r->level %2) {
	    // odd level, need to reverse the string
	    int pos = r->stop + start;
	    while(pos >= r->start + start) {
		KoTextStringChar &chr = text->at(pos);
		if( numSpaces && !first && isBreakable( text, pos ) ) {
		    int s = space / numSpaces;
                    toAdd += s;
                    toAddPix = zh->layoutUnitToPixelX( toAdd );
		    space -= s;
		    numSpaces--;
                    chr.width += s;
                    chr.pixelwidth += zh->layoutUnitToPixelX( s ); // ### rounding problem, recalculate
		} else if ( first ) {
		    first = FALSE;
		    if ( chr.c == ' ' ) // trailing space
                    {
                        //x -= chr.format()->width( ' ' );
                        x -= chr.width;
                        pixelx -= chr.pixelwidth;
                    }
		}
		chr.x = x + toAdd;
                chr.pixelxadj = pixelx + toAddPix - zh->layoutUnitToPixelX( chr.x );
#ifdef DEBUG_FORMATTER
                qDebug("koBidiReorderLine: pos=%d x(LU)=%d toAdd(LU)=%d -> chr.x=%d pixelx=%d+%d, pixelxadj=%d", pos, x, toAdd, chr.x, pixelx, zh->layoutUnitToPixelX( toAdd ), pixelx+zh->layoutUnitToPixelX( toAdd )-zh->layoutUnitToPixelX( chr.x ));
#endif
		chr.rightToLeft = TRUE;
		chr.startOfRun = FALSE;
		int ww = chr.width;
		if ( xmax < x + toAdd + ww ) xmax = x + toAdd + ww;
		x += ww;
                pixelx += chr.pixelwidth;
#ifdef DEBUG_FORMATTER
                qDebug("              ww=%d adding to x, now %d. pixelwidth=%d adding to pixelx, now %d xmax=%d", ww, x, chr.pixelwidth, pixelx, xmax );
#endif
		pos--;
	    }
	} else {
	    int pos = r->start + start;
	    while(pos <= r->stop + start) {
		KoTextStringChar& chr = text->at(pos);
		if( numSpaces && !first && isBreakable( text, pos ) ) {
		    int s = space / numSpaces;
		    toAdd += s;
                    toAddPix = zh->layoutUnitToPixelX( toAdd );
		    space -= s;
		    numSpaces--;
		} else if ( first ) {
		    first = FALSE;
		    if ( chr.c == ' ' ) // trailing space
                    {
                        //x -= chr.format()->width( ' ' );
                        x -= chr.width;
                        pixelx -= chr.pixelwidth;
                    }
		}
		chr.x = x + toAdd;
                chr.pixelxadj = pixelx + toAddPix - zh->layoutUnitToPixelX( chr.x );
		chr.rightToLeft = FALSE;
		chr.startOfRun = FALSE;
		int ww = chr.width;
		//qDebug("setting char %d at pos %d", pos, chr.x);
		if ( xmax < x + toAdd + ww ) xmax = x + toAdd + ww;
		x += ww;
                pixelx += chr.pixelwidth;
		pos++;
	    }
	}
	text->at( r->start + start ).startOfRun = TRUE;
	r = runs->next();
    }

    line->w = xmax + 10;
    KoTextParagLineStart *ls = new KoTextParagLineStart( control->context, control->status );
    delete control;
    delete runs;
    return ls;
}
