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
#include "kohyphen/kohyphen.h"

#include <kdebug.h>
//#define DEBUG_FORMATTER

/////// keep in sync with kotextformat.cc !
//#define REF_IS_LU

KoTextFormatter::KoTextFormatter()
{
    try {
        m_hyphenator = KoHyphenator::self();
    } catch ( KoHyphenatorException& e )
    {
        m_hyphenator = 0L;
    }
}

KoTextFormatter::~KoTextFormatter()
{
}

// Originally based on QTextFormatterBreakWords::format()
int KoTextFormatter::format( KoTextDocument *doc, KoTextParag *parag,
                             int start, const QMap<int, KoTextParagLineStart*> & )
{
    KoTextStringChar *c = 0;
    start = 0;
    if ( start == 0 )
	c = &parag->string()->at( 0 );

    KoTextStringChar *firstChar = 0;
    KoTextString *string = parag->string();
    bool rtl = string->isRightToLeft();
    int left = doc ? parag->leftMargin() + doc->leftMargin() : 0;
    int x = left;
    if ( doc && !rtl )
        x += parag->firstLineMargin();
    int curLeft = left;
    int y = doc && doc->addMargins() ? parag->topMargin() : 0;
    // #57555, top margin doesn't apply if parag at top of page
    // (but a portion of the margin can be needed, to complete the prev page)
    // So we apply formatVertically() on the top margin, to find where to break it.
    if ( !parag->prev() )
        y = 0; // no top margin on very first parag
    else if ( parag->breakableTopMargin() )
    {
        int shift = doc->flow()->adjustFlow( parag->rect().y(),
                                             0 /*w, unused*/,
                                             parag->breakableTopMargin() );
        if ( shift > 0 )
        {
            // The shift is in fact the amount of top-margin that should remain
            // The remaining portion should be eaten away.
            y = shift;
        }

    }
    // Now add the rest of the top margin (e.g. the one for the border)
    y += parag->topMargin() - parag->breakableTopMargin();
    int lineHeight = 0;
    int len = parag->length();

    int initialHeight = c->height(); // remember what adjustMargins was called with

    int currentRightMargin = parag->rightMargin(); // 'rm' in QRT
    if ( doc && rtl )
        currentRightMargin += parag->firstLineMargin();
    int initialRMargin = currentRightMargin;
    int dw = 0;

    if (doc)
        doc->flow()->adjustMargins( y + parag->rect().y(), initialHeight, x, initialRMargin, dw, parag );
    else // never the case in kotext
        dw = parag->documentVisibleWidth();

    int initialLMargin = x;
    curLeft = x;

    int maxY = doc ? doc->flow()->availableHeight() : -1;

    int availableWidth = dw - initialRMargin; // 'w' in QRT
#ifdef DEBUG_FORMATTER
    kdDebug(32500) << "KoTextFormatter::format left=" << left << " initialHeight=" << initialHeight << " initialLMargin=" << initialLMargin << " initialRMargin=" << initialRMargin << " availableWidth=" << availableWidth << endl;
#endif
    bool fullWidth = TRUE;
    int marg = left + initialRMargin;
    int minw = 0;
    int wused = 0;
    int tminw = marg;
    bool wrapEnabled = isWrapEnabled( parag );

    int i = start;
#ifdef DEBUG_FORMATTER
    kdDebug(32500) << "Initial KoTextParagLineStart at y=" << y << endl;
#endif
    KoTextParagLineStart *lineStart = new KoTextParagLineStart( y, 0, 0 );
    insertLineStart( parag, 0, lineStart );
    int lastBreak = -1;
    // tmph and tmpBaseLine are used after the last breakable char
    // we don't know yet if we'll break there, or later.
    int tmpBaseLine = 0, tmph = 0;
    bool lastWasNonInlineCustom = FALSE;

    int align = parag->alignment();
    if ( align == Qt::AlignAuto && doc && doc->alignment() != Qt::AlignAuto )
	align = doc->alignment();

    int col = 0;
    int ww = 0; // width in layout units

    KoZoomHandler *zh = doc->formattingZoomHandler();
#ifndef REF_IS_LU
    int pixelww = 0; // width in pixels
#endif
    int pixelx = zh->layoutUnitToPixelX( x );
    int lastPixelx = 0;

    parag->tabCache().clear();

    KoTextStringChar* lastChr = 0;
    for ( ; i < len; ++i, ++col ) {
	if ( c )
	    lastChr = c;
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
            KoTextFormat *charFormat = c->format();
            if ( c->isCustom() ) {
                ww = c->customItem()->width;
#ifndef REF_IS_LU
                pixelww = zh->layoutUnitToPixelX( ww );
#endif
            } else {
                ww = charFormat->charWidthLU( c, parag, i );
#ifndef REF_IS_LU
                // Pixel size - we want the metrics of the font that's going to be used.
                pixelww = charFormat->charWidth( zh, true, c, parag, i );
#endif
            }
	} else { // tab
	    int nx = parag->nextTab( i, x );
	    if ( nx < x )
		ww = availableWidth - x;
	    else
		ww = nx - x + 1;
#ifndef REF_IS_LU
            pixelww = zh->layoutUnitToPixelX( ww );
#endif
	}
        c->width = ww;

        //code from qt-3.1beta2
	if ( c->isCustom() && c->customItem()->ownLine() ) {
#ifdef DEBUG_FORMATTER
            kdDebug(32500) << "i=" << i << "/" << len << " custom item with ownline" << endl;
#endif
            int rightMargin = currentRightMargin;
            x = left;
            if ( doc )
                doc->flow()->adjustMargins( y + parag->rect().y(), parag->rect().height(), x, rightMargin, dw, parag );
	    int w = dw - rightMargin;
            c->customItem()->resize( w - x );
	    y += lineHeight;
	    lineHeight = c->height();
	    lineStart = new KoTextParagLineStart( y, lineHeight, lineHeight );
            // Added for kotext (to be tested)
            lineStart->lineSpacing = doc ? parag->lineSpacing( (int)parag->lineStartList().count()-1 ) : 0;
            lineStart->h += lineStart->lineSpacing;
            lineStart->w = dw;
	    insertLineStart( parag, i, lineStart );
	    c->lineStart = 1;
	    firstChar = c;
	    x = 0xffffff;
	    continue;
	}

#ifdef DEBUG_FORMATTER
	kdDebug(32500) << "c='" << c->c << "' i=" << i << "/" << len << " x=" << x << " ww=" << ww << " availableWidth=" << availableWidth << " (test is x+ww>aW) lastBreak=" << lastBreak << " isBreakable=" << isBreakable(string, i) << endl;
#endif
	// Wrapping at end of line - one big if :)
	if ( wrapEnabled
             // Check if should break (i.e. we are after the max X for the end of the line)
	     && ( /*wrapAtColumn() == -1 &&*/ x + ww > availableWidth &&
                  ( lastBreak != -1 || allowBreakInWords() )
                  /*|| wrapAtColumn() != -1 && col >= wrapAtColumn()*/ )

	     // Allow two breakable chars next to each other (e.g. '  ') but not more
	     && ( !isBreakable( string, i ) ||
                  ( i > 1 && lastBreak == i-1 && isBreakable( string, i-2 ) ) ||
                  lastBreak == -2 ) // ... used to be a special case...

             // Ensure that there is at least one char per line, otherwise, on
             // a very narrow document and huge chars, we could loop forever.
             // checkVerticalBreak takes care of moving down the lines where no
             // char should be, anyway.
             // Hmm, it doesn't really do so. To be continued...
             /////////// && ( firstChar != c )

             // Or maybe we simply encountered a '\n'
             || lastChr->c == '\n' && parag->isNewLinesAllowed() && lastBreak > -1 )
        {
#ifdef DEBUG_FORMATTER
	    kdDebug(32500) << "BREAKING" << endl;
#endif
	    //if ( wrapAtColumn() != -1 )
	    //    minw = QMAX( minw, x + ww );

            bool hyphenated = false;
            // Hyphenation: check if we can break somewhere between lastBreak and i
            if ( m_hyphenator )
            {
                int wordStart = QMAX(0, lastBreak+1);
                int maxlen = i - wordStart + 1; // we can't accept to break after maxlen
                QString word = string->mid( wordStart, maxlen );
                int wordEnd = i + 1;
                // but we need to compose the entire word, to hyphenate it
                while ( wordEnd < len && !isBreakable( string, wordEnd ) ) {
                    word += string->at(wordEnd).c;
                    wordEnd++;
                }
                if ( word.length() > 1 ) // don't call the hyphenator for empty or one-letter words
                {
                    QString lang = string->at(wordStart).format()->language();
                    char * hyphens = m_hyphenator->hyphens( word, lang );
#ifdef DEBUG_FORMATTER
                    kdDebug(32500) << "Hyphenation: word=" << word << " lang=" << lang << " hyphens=" << hyphens << " maxlen=" << maxlen << endl;
                    kdDebug(32500) << "Parag indexes: wordStart=" << wordStart << " lastBreak=" << lastBreak << " i=" << i << endl;
#endif
                    int hylen = strlen(hyphens);
                    Q_ASSERT( maxlen <= hylen );
                    // If this word was already hyphenated (at the previous line),
                    // don't break it there again. We can only break after firstChar.
                    int minPos = QMAX( 0, (firstChar - &string->at(0)) - wordStart );

                    // Check hyphenation positions from the end
                    for ( int hypos = maxlen-1 ; hypos >= minPos ; --hypos )
                        if ( ( hyphens[hypos] % 2 ) // odd number -> can break there...
                               && string->at(hypos + wordStart).format()->hyphenation() ) // ...if the user is ok with that
                        {
                            lineStart->hyphenated = true;
                            lastBreak = hypos + wordStart;
                            hyphenated = true;
#ifdef DEBUG_FORMATTER
                            kdDebug(32500) << "Hyphenation: will break at " << lastBreak << endl;
#endif
                            break;
                        }
                    delete[] hyphens;
                }
            }

	    // No breakable char found -> break at current char (i.e. before 'i')
	    if ( lastBreak < 0 ) {
                // (combine lineStart->baseLine/h and tmpBaseLine/tmph)
                int belowBaseLine = QMAX( lineHeight - lineStart->baseLine, tmph - tmpBaseLine );
                lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
                lineHeight = lineStart->baseLine + belowBaseLine;
                lineStart->h = lineHeight;
                lineStart->w = dw;

		KoTextParagLineStart *lineStart2 = koFormatLine( zh, parag, string, lineStart, firstChar, c-1, align, availableWidth - x );
                lineStart->lineSpacing = doc ? parag->lineSpacing( (int)parag->lineStartList().count()-1 ) : 0;
		lineStart->h += lineStart->lineSpacing;
		y += lineStart->h;
#ifdef DEBUG_FORMATTER
                int linenr = parag->lineStartList().count()-1;
                kdDebug(32500) << "line " << linenr << " done (breaking at current char). y now " << y << endl;
#endif

		lineStart = lineStart2;
		tmph = c->height();
		lineHeight = 0;

                initialRMargin = currentRightMargin;
                x = left;
                if ( doc )
                    doc->flow()->adjustMargins( y + parag->rect().y(), tmph, x, initialRMargin, dw, parag );

                pixelx = zh->layoutUnitToPixelX( x );
		initialHeight = tmph;
		initialLMargin = x;
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
                // recalc everything for 'i', it might still not be ok where it is...
                // (e.g. if there's no room at all on this line)
                // But we don't want to do this forever, so we check against maxY (if known)
                if ( maxY > -1 )
                {
                    if ( y < maxY )
                    {
                        --i; // so that the ++i in for() is a noop
                        continue;
                    }
                    else // we're after maxY, time to stop. Hopefully KWord will create more pages...
                        break;
                }
                // maxY not known -> keep going ('i' remains where it is)
                // (that's the initial QRT behaviour)
	    } else {
		// Breakable char was found
		i = lastBreak;
		c = &string->at( i ); // The last char in the last line
                int spaceAfterLine = availableWidth - c->x;
                // ?? AFAICS we should always deduce the char's width from the available space....
                //if ( string->isRightToLeft() && lastChr->c == '\n' )
                spaceAfterLine -= c->width;

                //else
                if ( c->c.unicode() == 0xad || hyphenated ) // soft hyphen or hyphenation
                {
                    // Recalculate its width, the hyphen will appear finally (important for the parag rect)
                    int width = KoTextZoomHandler::ptToLayoutUnitPt( c->format()->refFontMetrics().width( QChar(0xad) ) );
                    if ( c->c.unicode() == 0xad )
                        c->width = width;
                    spaceAfterLine -= width;
                }
		KoTextParagLineStart *lineStart2 = koFormatLine( zh, parag, string, lineStart, firstChar, c, align, spaceAfterLine );
                lineStart->lineSpacing = doc ? parag->lineSpacing( (int)parag->lineStartList().count()-1 ) : 0;
                lineStart->w = dw;
		lineStart->h += lineStart->lineSpacing;
		y += lineStart->h;
		lineStart = lineStart2;
#ifdef DEBUG_FORMATTER
		kdDebug(32500) << "Breaking at a breakable char (" << i << "). linenr=" << parag->lineStartList().count()-1 << " y=" << y << endl;
#endif

		c = &string->at( i + 1 ); // The first char in the new line
#ifdef DEBUG_FORMATTER
		kdDebug(32500) << "Next line will start at i+1=" << i+1 << ", char=" << c->c << endl;
#endif
		tmph = c->height();
		lineHeight = tmph;

                initialRMargin = currentRightMargin;
                x = left;
                if ( doc )
                    doc->flow()->adjustMargins( y + parag->rect().y(), tmph, x, initialRMargin, dw, parag );

                pixelx = zh->layoutUnitToPixelX( x );
		initialHeight = lineHeight;
		initialLMargin = x;
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
                // If we're after maxY, time to stop. Hopefully KWord will create more pages.
                if ( maxY > -1 && y >= maxY )
                    break;
		continue;
	    }
	} else if ( lineStart && ( isBreakable( string, i ) || parag->isNewLinesAllowed() && c->c == '\n' ) ) {
	    // Breakable character
	    if ( len <= 2 || i < len - 1 ) {
		//kdDebug(32500) << " Breakable character (i=" << i << " len=" << len << "): combining " << //	tmpBaseLine << "/" << tmph << " with " << c->ascent() << "/" << c->height() << endl;
		// (combine tmpBaseLine/tmph and this character)
		int belowBaseLine = QMAX( tmph - tmpBaseLine, c->height() - c->ascent() );
		tmpBaseLine = QMAX( tmpBaseLine, c->ascent() );
		tmph = tmpBaseLine + belowBaseLine;
		//kdDebug(32500) << " -> tmpBaseLine/tmph : " << tmpBaseLine << "/" << tmph << endl;
	    }
	    minw = QMAX( minw, tminw );
	    tminw = marg + ww;
	    // (combine lineStart and tmpBaseLine/tmph)
	    //kdDebug(32500) << "Breakable character: combining " << lineStart->baseLine << "/" << h << " with " << tmpBaseLine << "/" << tmph << endl;
	    int belowBaseLine = QMAX( lineHeight - lineStart->baseLine, tmph - tmpBaseLine );
	    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
	    lineHeight = lineStart->baseLine + belowBaseLine;
	    lineStart->h = lineHeight;
            lineStart->w = dw;
	    // if h > initialHeight,  call adjustMargins, and if the result is != initial[LR]Margin,
	    // format this line again
	    if ( doc && lineHeight > initialHeight )
	    {
		int lm = left + ( ( firstChar == &string->at(0) && doc && !rtl ) ? parag->firstLineMargin() : 0 );
#ifdef DEBUG_FORMATTER
                kdDebug(32500) << "left=" << left << ", firstlinepargin=" << (( firstChar == &string->at(0) && doc ) ? parag->firstLineMargin() : 0) << " => lm=" << lm << endl;
#endif
		int newLMargin = lm;
		int newRMargin = currentRightMargin - ( ( firstChar == &string->at(0) && doc && rtl ) ? parag->firstLineMargin() : 0 );
                int newPageWidth = dw;
                doc->flow()->adjustMargins( y + parag->rect().y(), lineHeight, newLMargin, newRMargin, newPageWidth, parag );

		initialHeight = lineHeight;
#ifdef DEBUG_FORMATTER
		kdDebug(32500) << "new height: " << lineHeight << " => left=" << left << " lm=" << lm << " first-char=" << (firstChar==&string->at(0)) << " newLMargin=" << newLMargin << " newRMargin=" << newRMargin << endl;
#endif
		if ( newLMargin != initialLMargin || newRMargin != initialRMargin || newPageWidth != dw )
		{
#ifdef DEBUG_FORMATTER
		    kdDebug(32500) << "formatting again" << endl;
#endif
		    i = (firstChar - &string->at(0));
		    x = newLMargin;
                    pixelx = zh->layoutUnitToPixelX( x );
		    availableWidth = dw - newRMargin;
		    initialLMargin = newLMargin;
		    initialRMargin = newRMargin;
                    dw = newPageWidth;
		    c = &string->at( i );
		    tmph = c->height();
		    lineHeight = tmph;
		    tmpBaseLine = c->ascent();
		    lineStart->h = lineHeight;
		    lineStart->baseLine = tmpBaseLine;
		    curLeft = x;
		    lastBreak = -1;
		    col = 0;
#ifdef DEBUG_FORMATTER
		    kdDebug(32500) << "Restarting with i=" << i << " x=" << x << " y=" << y << " lineHeight=" << lineHeight << " initialHeight=" << initialHeight << " initialLMargin=" << initialLMargin << " initialRMargin=" << initialRMargin << " y=" << y << endl;
#endif
                    // ww and pixelww already calculated and stored, no need to duplicate
                    // code like QRT does.
                    ww = c->width;
#ifndef REF_IS_LU
                    pixelww = c->pixelwidth;
#endif
		}
	    }

	    //kdDebug(32500) << " -> lineStart->baseLine/lineStart->h : " << lineStart->baseLine << "/" << lineStart->h << endl;
	    if ( i < len - 2 || c->c != ' ' )
		lastBreak = i;

	} else {
	    // Non-breakable character
	    tminw += ww;
	    //kdDebug(32500) << " Non-breakable character: combining " << tmpBaseLine << "/" << tmph << " with " << c->ascent() << "/" << c->height() << endl;
	    // (combine tmpBaseLine/tmph and this character)
	    int belowBaseLine = QMAX( tmph - tmpBaseLine, c->height() - c->ascent() );
	    tmpBaseLine = QMAX( tmpBaseLine, c->ascent() );
	    tmph = tmpBaseLine + belowBaseLine;
	    //kdDebug(32500) << " -> tmpBaseLine/tmph : " << tmpBaseLine << "/" << tmph << endl;
	}

	c->x = x;
        // pixelxadj is the adjustement to add to lu2pixel(x), to find pixelx
        // (pixelx would be too expensive to store directly since it would require an int)
        c->pixelxadj = pixelx - zh->layoutUnitToPixelX( x );
        //c->pixelwidth = pixelww;
#ifdef DEBUG_FORMATTER
        kdDebug(32500) << "LU: x=" << x << " [equiv. to pix=" << zh->layoutUnitToPixelX( x ) << "] ; PIX: x=" << pixelx << "  --> adj=" << c->pixelxadj << endl;
#endif

        if ( i > 0 )
            lastChr->pixelwidth = pixelx - lastPixelx;
        if ( i < len - 1 )
            wused = QMAX( wused, x );
        else // trailing space
            c->pixelwidth = zh->layoutUnitToPixelX( ww ); // was: pixelww;

	x += ww;
        lastPixelx = pixelx;
#ifdef REF_IS_LU
        pixelx = zh->layoutUnitToPixelX( x ); // no accumulating rounding errors anymore
#else
        pixelx += pixelww;
#endif
#ifdef DEBUG_FORMATTER
	kdDebug(32500) << "LU: added " << ww << " -> now x=" << x << " ; PIX: added " << pixelww << " -> now pixelx=" << pixelx << endl;
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
	kdDebug(32500) << "Last Line.... linenr=" << (int)parag->lineStartList().count()-1 << endl;
#endif
        //kdDebug(32500) << "Combining " << lineStart->baseLine << "/" << lineHeight << " with " << tmpBaseLine << "/" << tmph << endl;
	// (combine lineStart and tmpBaseLine/tmph)
	int belowBaseLine = QMAX( lineHeight - lineStart->baseLine, tmph - tmpBaseLine );
	lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
	lineHeight = lineStart->baseLine + belowBaseLine;
        lineStart->w = dw;
	//kdDebug(32500) << " -> lineStart->baseLine/lineStart->h : " << lineStart->baseLine << "/" << lineStart->h << endl;
	// last line in a paragraph is not justified
	if ( align == Qt::AlignJustify )
	    align = Qt::AlignAuto;
        int space = availableWidth - x + c->width; // don't count the trailing space (it breaks e.g. centering)
        KoTextParagLineStart *lineStart2 = koFormatLine( zh, parag, string, lineStart, firstChar, c, align, space );
        lineStart->lineSpacing = doc ? parag->lineSpacing( (int)parag->lineStartList().count()-1 ) : 0;
	lineHeight += lineStart->lineSpacing;
	lineStart->h = lineHeight;
	delete lineStart2;
    }

    minw = QMAX( minw, tminw );

    int m = parag->bottomMargin();
    // ##### Does OOo add margins or does it max them?
    //if ( parag->next() && doc && !doc->addMargins() )
    //	m = QMAX( m, parag->next()->topMargin() );
    parag->setFullWidth( fullWidth );
    //if ( parag->next() && parag->next()->isLineBreak() )
    //    m = 0;
#ifdef DEBUG_FORMATTER
    kdDebug(32500) << "Adding lineHeight(" << lineHeight << ") and bottomMargin(" << m << ") to y(" << y << ") => " << y+lineHeight+m << endl;
#endif
    y += lineHeight + m;

    wused += currentRightMargin;
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
#ifndef REF_IS_LU
    int pixelx = chr.pixelxadj + zh->layoutUnitToPixelX( chr.x );
#endif
    chr.x += deltaX;
#ifndef REF_IS_LU
    chr.pixelxadj = pixelx + deltaPixelX - zh->layoutUnitToPixelX( chr.x );
#endif
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
	    if( isStretchable( string, j ) ) {
		numSpaces++;
	    }
	}
	int toAdd = 0;
        int toAddPix = 0;
	for ( int k = start + 1; k <= last; ++k ) {
            KoTextStringChar &chr = string->at( k );
            if ( toAdd != 0 )
                moveChar( chr, zh, toAdd, toAddPix );
	    if( isStretchable( string, k ) && numSpaces ) {
		int s = space / numSpaces;
		toAdd += s;
                toAddPix = zh->layoutUnitToPixelX( toAdd );
		space -= s;
		numSpaces--;
                chr.width += s;
#ifndef REF_IS_LU
                chr.pixelwidth += zh->layoutUnitToPixelX( s ); // ### rounding problem, recalculate
#endif
	    }
	}
    }
    int current=0;
    int nc=0; // Not double, as we check it against 0 and to avoid gcc warnings
    KoTextFormat refFormat( *string->at(0).format() ); // we need a ref format, doesn't matter where it comes from
    for(int i=start;i<=last;++i)
    {
	KoTextFormat* format=string->at(i).format();
        // End of underline
	if ( (((!format->underline())&&
               (!format->doubleUnderline())&&
               (!format->waveUnderline())&&
               (format->underlineType()!=KoTextFormat::U_SIMPLE_BOLD))
              || i == last)
             && nc )
	{
	    double avg=static_cast<double>(current)/nc;
	    avg/=18.0;
            // Apply underline width "avg" from i-nc to i
            refFormat.setUnderLineWidth( avg );
            parag->setFormat( i-nc, i, &refFormat, true, KoTextFormat::UnderLineWidth );
	    nc=0;
	    current=0;
	}
        // Inside underline
	else if(format->underline()||
                format->waveUnderline()||
                format->doubleUnderline()||
                (format->underlineType() == KoTextFormat::U_SIMPLE_BOLD))
	{
	    ++nc;
	    current += format->pointSize(); //pointSize() is independent of {Sub,Super}Script in contrast to height()
	}
    }
#if 0
    if ( last >= 0 && last < string->length() ) {
        KoTextStringChar &chr = string->at( last );
	line->w = chr.x + chr.width; //string->width( last );
        // Add width of hyphen (so that it appears)
        if ( line->hyphenated )
            line->w += KoTextZoomHandler::ptToLayoutUnitPt( chr.format()->refFontMetrics().width( QChar(0xad) ) );
    } else
	line->w = 0;
#endif

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
        x += space/2;
    } else if ( align & Qt::AlignRight ) {
        x += space;
    } else if ( align & Qt::AlignJustify ) {
	for ( int j = last - 1; j >= start; --j ) {
            //// Start at last tab, if any. BR #40472 specifies that justifying should start after the last tab.
            if ( text->at( j ).c == '\t' ) {
                start = j+1;
                break;
            }
	    if( isStretchable( text, j ) ) {
		numSpaces++;
	    }
	}
    }
// TODO #ifndef REF_IS_LU or remove
    int pixelx = zh->layoutUnitToPixelX( x );
    int toAdd = 0;
    int toAddPix = 0;
    bool first = TRUE;
    KoTextRun *r = runs->first();
    int xmax = -0xffffff;
    while ( r ) {
#ifdef DEBUG_FORMATTER
        kdDebug(32500) << "koBidiReorderLine level: " << r->level << endl;
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
                kdDebug(32500) << "koBidiReorderLine: pos=" << pos << " x(LU)=" << x << " toAdd(LU)=" << toAdd << " -> chr.x=" << chr.x << " pixelx=" << pixelx << "+" << zh->layoutUnitToPixelX( toAdd ) << ", pixelxadj=" << pixelx+zh->layoutUnitToPixelX( toAdd )-zh->layoutUnitToPixelX( chr.x ) << endl;
#endif
		chr.rightToLeft = TRUE;
		chr.startOfRun = FALSE;
		int ww = chr.width;
		if ( xmax < x + toAdd + ww ) xmax = x + toAdd + ww;
		x += ww;
                pixelx += chr.pixelwidth;
#ifdef DEBUG_FORMATTER
                kdDebug(32500) << "              ww=" << ww << " adding to x, now " << x << ". pixelwidth=" << chr.pixelwidth << " adding to pixelx, now " << pixelx << " xmax=" << xmax << endl;
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
		//kdDebug(32500) << "setting char " << pos << " at pos " << chr.x << endl;
		if ( xmax < x + toAdd + ww ) xmax = x + toAdd + ww;
		x += ww;
                pixelx += chr.pixelwidth;
		pos++;
	    }
	}
	text->at( r->start + start ).startOfRun = TRUE;
	r = runs->next();
    }

    //line->w = xmax /*+ 10*/; // Why +10 ?
    KoTextParagLineStart *ls = new KoTextParagLineStart( control->context, control->status );
    delete control;
    delete runs;
    return ls;
}

void KoTextFormatter::postFormat( KoTextParag* parag )
{
    parag->fixParagWidth( viewFormattingChars() );
}
