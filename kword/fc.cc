/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>, Torben Weis <weis@kde.org>

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

#include <stdio.h>
#include <math.h>

#include "fc.h"
#include "kword_doc.h"
#include "paraglayout.h"
#include "char.h"
#include "autoformat.h"
#include "font.h"

/******************************************************************/
/* Class: KWFormatContext                                         */
/******************************************************************/

/*================================================================*/
KWFormatContext::KWFormatContext( KWordDocument *_doc, unsigned int _frameSet )
    : KWFormat()
{
    displayFont = 0;
    setDefaults( _doc );

    doc = _doc;

    during_vertical_cursor_movement = FALSE;

    spacingError = 0;
    ptTextLen = 0;
    textPos = 0;

    lineStartPos = 0;
    lineEndPos = 0;
    specialHeight = 0;
    ptMaxAscender = 0;
    ptMaxDescender = 0;
    setFrameSet( _frameSet );
    setFrame( 1 );
    compare_formats = TRUE;
    outOfFrame = FALSE;
    offsetsAdded = FALSE;
}

/*================================================================*/
KWFormatContext &KWFormatContext::operator=( const KWFormatContext &fc )
{
    // KWFormat
    userFont = fc.getUserFont();
    ptFontSize = fc.getPTFontSize();
    weight = fc.getWeight();
    italic = fc.getItalic();
    underline = fc.getUnderline();
    vertAlign = fc.getVertAlign();
    color = fc.getColor();
    ref = 0;
    if ( !doc )
        doc =  fc.doc;

    // KWFormatContext
    ptTextLen = fc.ptTextLen;
    ptAscender = fc.ptAscender;
    ptDescender = fc.ptDescender;
    ptMaxAscender = fc.ptMaxAscender;
    ptMaxDescender = fc.ptMaxDescender;
    ptY = fc.ptY;
    ptCellSpacing = fc.ptCellSpacing;
    ptPos = fc.ptPos;
    WantedPtPos = fc.WantedPtPos;
    during_vertical_cursor_movement = fc.during_vertical_cursor_movement;
    specialHeight = fc.specialHeight;
    frameSet = fc.frameSet;
    frame = fc.frame;
    page = fc.page;
    lineStartPos = fc.lineStartPos;
    lineStartFormat = fc.lineStartFormat;
    lineEndPos = fc.lineEndPos;
    textPos = fc.textPos;
    spaces = fc.spaces;
    ptLeft = fc.ptLeft;
    ptWidth = fc.ptWidth;
    ptStartPos = fc.ptStartPos;
    ptCounterPos = fc.ptCounterPos;
    tmpFormat = fc.tmpFormat;
    parag = fc.parag;
    error = fc.error;
    counterText = fc.counterText;
    ptCounterWidth = fc.ptCounterWidth;
    ptCounterAscender = fc.ptCounterAscender;
    ptCounterDescender = fc.ptCounterDescender;
    displayFont = fc.displayFont;
    ptSpacing = fc.ptSpacing;
    spacingError = fc.spacingError;
    compare_formats = fc.compare_formats;
    outOfFrame = fc.outOfFrame;
    offsetsAdded = fc.offsetsAdded;
    emptyRegion = fc.emptyRegion;
    pFrameSet = fc.pFrameSet;
    pFrame = fc.pFrame;

    return *this;
}

/*================================================================*/
bool KWFormatContext::operator<( const KWFormatContext &fc ) const
{
    if ( fc.parag == parag )
        return fc.textPos > textPos;

    // Reggie: That's unefficient, but I have no better idea :-(
    KWParag *p = parag->getNext();
    while ( p ) {
        if ( p == fc.parag )
            return TRUE;
        p = p->getNext();
    }

    return FALSE;
}

/*================================================================*/
bool KWFormatContext::operator>( const KWFormatContext &fc ) const
{
    if ( fc.parag == parag )
        return fc.textPos < textPos;

    // Reggie: That's unefficient, but I have no better idea :-(
    KWParag *p = parag->getPrev();
    while ( p ) {
        if ( p == fc.parag )
            return TRUE;
        p = p->getPrev();
    }

    return FALSE;
}

/*================================================================*/
bool KWFormatContext::operator==( const KWFormatContext &fc ) const
{
    if ( fc.parag == parag )
        return fc.textPos == textPos;

    return FALSE;
}

/*================================================================*/
void KWFormatContext::init( KWParag *_parag, bool _fromStart,
                            int _frame, int _page )
{
    outOfFrame = FALSE;
    specialHeight = 0;
    ptMaxAscender = 0;
    ptMaxDescender = 0;

    if ( _fromStart ) {

        // Offset from the top of the page
        if ( pFrameSet->getNumFrames() > 0 )
            ptY = static_cast<int>(pFrameSet->getFrame( 0 )->top() + pFrameSet->getFrame( 0 )->getBTop().pt());
        else
            ptY = 0;
        if ( _frame != -1 && _page != -1 && pFrameSet->getFrameInfo() != FI_BODY ) {
            setFrame( _frame );
            page = _page;
            ptY = static_cast<int>(pFrame->top() + pFrame->getBTop().pt());
        } else {
            setFrame( 1 );
            page = 1;
        }

        // Enter the first paragraph
        parag = 0L;
        enterNextParag();

        // Loop until we got the paragraph
        while ( parag != _parag ) {
            // Skip the current paragraph
            skipCurrentParag();
            // Go to the next one
            enterNextParag();
        }

    } else {
        parag = _parag;
        ptY = parag->getPTYStart();

        lineStartPos = 0;
        setFrame( parag->getStartFrame() );
        page = pFrameSet->getPageOfFrame( frame - 1 ) + 1;

        makeLineLayout();
    }
}

/*================================================================*/
void KWFormatContext::enterNextParag()
{
    // Set the context to the given paragraph
    if ( parag != 0L ) {
        parag->setPTYEnd( ptY );
        parag = parag->getNext();
        if ( !parag ) {
            qWarning( "ERROR: Parag not found! Strange things may happen" );
            return;
        }
    } else
        parag = dynamic_cast<KWTextFrameSet*>( pFrameSet )->getFirstParag();
    // On which page are we now ...
    parag->setStartPage( page );
    parag->setEndPage( page );
    // In which frame ...
    parag->setStartFrame( frame );
    parag->setEndFrame( frame );
    // Vertical position ...
    parag->setPTYStart( ptY );

  // We are at the beginning of our paragraph
    lineStartPos = 0;

    // Reset font size, color etc. to the documents default
    setDefaults( doc );
    // Change fonts & stuff to match the paragraphs layout
    compare_formats = FALSE;
    apply( parag->getParagLayout()->getFormat() );
    compare_formats = TRUE;

    // Calculate everything about the line we are in.
    makeLineLayout();
}

/*================================================================*/
void KWFormatContext::skipCurrentParag()
{
    bool ret;

    // Iterate over all lines
    do {
        // returns FALSE if we are at the end of
        // the paragraph after the call returns.
        ret = makeLineLayout();
        // Skip the line
        ptY += getLineHeight();
        // next line
        lineStartPos = lineEndPos;
    } while ( ret );
}

/*================================================================*/
void KWFormatContext::gotoStartOfParag()
{
}

/*================================================================*/
bool KWFormatContext::isCursorAtLastChar()
{
    if ( textPos >= lineEndPos - 1 )
        return TRUE;

    return FALSE;
}

/*================================================================*/
bool KWFormatContext::isCursorInFirstLine()
{
    // If we are in the first line, return TRUE
    return ( lineStartPos == 0 );
}

/*================================================================*/
bool KWFormatContext::isCursorAtParagStart()
{
    return ( textPos == 0 );
}

/*================================================================*/
bool KWFormatContext::isCursorAtLineStart()
{
    return ( textPos == lineStartPos );
}

/*================================================================*/
bool KWFormatContext::isCursorInLastLine()
{
    // If we are in the last line, return TRUE
    return ( lineEndPos == parag->getTextLen() );
}

/*================================================================*/
bool KWFormatContext::isCursorAtParagEnd()
{
    if ( !isCursorInLastLine() )
        return FALSE;

    // Are we behind the paragraphs last character now ?
    if ( textPos == parag->getTextLen() )
        return TRUE;

    return FALSE;
}

/*================================================================*/
bool KWFormatContext::isCursorAtLineEnd()
{
    // Are we behind the last lines character ?
    // Torben: I commented this out since it looks strange
    return (textPos == lineEndPos /* || textPos == lineEndPos - 1 */);
}

/*================================================================*/
void KWFormatContext::cursorGotoRight()
{
    during_vertical_cursor_movement = FALSE;

    // Are we at the end of a paragraph ?
    if ( isCursorAtParagEnd() )
    {
        // The last paragraph ?
        if ( parag->getNext() == 0L )
            return;
        // Skip the current line
        ptY += getLineHeight();
        // Enter the next paragraph
        enterNextParag();
        cursorGotoLineStart();
        return;
    }
    else if ( isCursorInLastLine() )
    {
        textPos++;
        cursorGotoPos( textPos );
        return;
    }

    // If the cursor is in the last line of some paragraph,
    // then we should not care here.
    if ( isCursorAtLastChar() )
    {
        lineStartPos = lineEndPos;
        ptY += getLineHeight();
        makeLineLayout();
        cursorGotoLineStart();
        return;
    }

    textPos++;

    cursorGotoPos( textPos );
}

/*================================================================*/
void KWFormatContext::cursorGotoRight( int _pos )
{
    during_vertical_cursor_movement = FALSE;

    for ( int i = 0; i < _pos; i++ )
    {
        // Are we at the end of a paragraph ?
        if ( isCursorAtParagEnd() )
        {
            // The last paragraph ?
            if ( parag->getNext() == 0L )
                return;
            // Skip the current line
            ptY += getLineHeight();
            // Enter the next paragraph
            enterNextParag();
            cursorGotoLineStart();
            continue;
        }
        else if ( isCursorInLastLine() )
        {
            cursorGotoNextChar();
            continue;
        }

        // If the cursor is in the last line of some paragraph,
        // then we should not care here.
        if ( isCursorAtLastChar() )
        {
            lineStartPos = lineEndPos;
            ptY += getLineHeight();
            makeLineLayout();
            cursorGotoLineStart();
            continue;
        }

        cursorGotoNextChar();
    }
}

/*================================================================*/
void KWFormatContext::cursorGotoLeft()
{
    during_vertical_cursor_movement = FALSE;

    if ( isCursorAtParagStart() )
    {
        // The first paragraph ?
        if ( parag->getPrev() == 0L )
            return;
        // Enter the prev paragraph
        init( parag->getPrev(), FALSE );
        int ret;
        do
        {
            ret = makeLineLayout();
            if ( ret ) {
                // Skip the line
                ptY += getLineHeight();
                // next line
                lineStartPos = lineEndPos;
            }
        }
        while ( ret );

        cursorGotoLineEnd();
        return;
    }

    // If the cursor is in the first line of some paragraph
    if ( isCursorAtLineStart() )
    {
        unsigned int tmpPos = lineStartPos;
        init( parag, FALSE );
        do
        {
            makeLineLayout();
            if ( lineEndPos < tmpPos )
            {
                ptY += getLineHeight();
                // next line
                lineStartPos = lineEndPos;
            }
        }
        while ( lineEndPos < tmpPos );
        cursorGotoLineEnd();
        return;
    }

    textPos--;

    cursorGotoPos( textPos );
}

/*================================================================*/
void KWFormatContext::cursorGotoUp()
{
    unsigned int frm = frame;

    if ( !during_vertical_cursor_movement )
        WantedPtPos = ptPos;

    if ( isCursorInFirstLine() )
    {
        // The firstparagraph ?
        if ( parag->getPrev() == 0L )
            return;
        // Enter the prev paragraph

        init( parag->getPrev(), FALSE );
        int ret;
        do
        {
            ret = makeLineLayout();
            if ( ret )
            {
                // Skip the line
                ptY += getLineHeight();
                // next line
                lineStartPos = lineEndPos;
            }
        }
        while ( ret );

        if ( frm != frame )
            WantedPtPos = ptStartPos;

        cursorGotoLineStart();
        while ( ptPos < WantedPtPos && textPos < lineEndPos && !isCursorAtLineEnd() )
            cursorGotoRight();
    }
    else
    {
        // Re-Enter the current paragraph
        unsigned int tmpPos = lineStartPos;
        init ( parag, FALSE );
        do
        {
            makeLineLayout();
            if ( lineEndPos < tmpPos )
            {
                ptY += getLineHeight();
                // next line
                lineStartPos = lineEndPos;
            }
        }
        while( lineEndPos < tmpPos );

        if ( frm != frame )
            WantedPtPos = ptStartPos;

        cursorGotoLineStart();
        while ( ptPos < WantedPtPos && textPos < lineEndPos - 1 && !isCursorAtLineEnd() )
            cursorGotoRight();
    }
}

/*================================================================*/
void KWFormatContext::cursorGotoDown()
{
    unsigned int frm = frame;
    // Save the position where we started going down
    if ( !during_vertical_cursor_movement )
        WantedPtPos = ptPos;

    if ( isCursorInLastLine() )
    {
        // The last paragraph ?
        if ( parag->getNext() == 0L )
            return;
        // Skip the current line
        ptY += getLineHeight();
        // Enter the next paragraph
        enterNextParag();
    }
    else
    {
        lineStartPos = lineEndPos;
        ptY += getLineHeight();
        makeLineLayout();
    }

    if ( frm != frame )
        WantedPtPos = ptStartPos;

    cursorGotoLineStart();
    while ( ptPos < WantedPtPos && !isCursorAtLineEnd() )
        cursorGotoRight();
}

/*================================================================*/
void KWFormatContext::cursorGotoLineStart()
{
    during_vertical_cursor_movement = FALSE;
    spacingError = 0;
    cursorGotoPos( lineStartPos );
}

/*================================================================*/
void KWFormatContext::cursorGotoLineEnd()
{
    during_vertical_cursor_movement = FALSE;

    if ( isCursorInLastLine() )
    {
        cursorGotoPos( lineEndPos );
        return;
    }

    // Is the last character a space ? => it is not really displayed
    if ( lineEndPos > lineStartPos && parag->getText()[ lineEndPos - 1 ].c == ' ' )
    {
        // Go to the last character only.
        cursorGotoPos( lineEndPos - 1 );
        return;
    }

    // The line is empty or it contains a word which is longer then a line.
    // In both cases we can go behind the last character
    cursorGotoPos( lineEndPos );
}

/*================================================================*/
void KWFormatContext::cursorGotoNextLine()
{
    during_vertical_cursor_movement = FALSE;

    lineStartPos = lineEndPos;
    ptY += getLineHeight();
    makeLineLayout();
    cursorGotoLineStart();
}

/*================================================================*/
void KWFormatContext::cursorGotoPrevLine()
{
    // Re-Enter the current paragraph
    unsigned int tmpPos = lineStartPos;
    init( parag, FALSE );
    do
    {
        makeLineLayout();
        if ( lineEndPos < tmpPos )
        {
            ptY += getLineHeight();
            // next line
            lineStartPos = lineEndPos;
        }
    }
    while ( lineEndPos < tmpPos );

    cursorGotoLineStart();

    during_vertical_cursor_movement = FALSE;
}

/*================================================================*/
void KWFormatContext::cursorGotoLine( unsigned int _textpos )
{
    if ( _textpos < lineStartPos ) {
        gotoStartOfParag();
        makeLineLayout();
    } else if ( _textpos >= lineStartPos && _textpos < lineEndPos ) {
        cursorGotoPos( _textpos );
        return;
    }

    bool ret;
    do {
        if ( _textpos >= lineStartPos && _textpos < lineEndPos ) {
            cursorGotoPos( _textpos );
            return;
        }
        ret = makeNextLineLayout();
    } while ( ret );

    if ( !doc->isEmpty() )
    {
      qWarning( "ERROR: Textpos behind content of parag! Strange things may happen now!" );
      qWarning( "(If you just opened an empty document you can IGNORE this message!!!" );
    }
}

/*================================================================*/
void KWFormatContext::cursorGotoPos( unsigned int _textpos )
{
    KWChar *text = parag->getText();
    KWParagLayout *lay = parag->getParagLayout();

    unsigned int pos = lineStartPos;
    spacingError = 0;
    ptPos = ptStartPos;
    compare_formats = FALSE;
    apply( lineStartFormat );
    compare_formats = TRUE;

    // test this to avoid crashes!!
    if ( _textpos > parag->getKWString()->size() )
        _textpos =  parag->getKWString()->size();

    bool _break = FALSE;
    while ( pos < _textpos && !_break ) {
        if ( text[ pos ].c == KWSpecialChar ) {
            // Handle specials here
            switch ( text[ pos ].attrib->getClassId() ) {
            case ID_KWCharImage: {
                ptPos += ( ( KWCharImage* )text[ pos ].attrib )->getImage()->width();
                specialHeight = QMAX( specialHeight,
                                      (unsigned int)( (KWCharImage*) text[pos].attrib )->getImage()->height() );
                pos++;
            } break;
            case ID_KWCharVariable: {
                KWCharVariable *v = dynamic_cast<KWCharVariable*>( text[ pos ].attrib );
                v->getVar()->setInfo( frameSet, frame,
                                      doc->getPageNum( ptY + doc->getPTPaperHeight() ), parag );
                v->getVar()->recalc();
                apply( *v->getFormat() );
                ptPos += displayFont->getPTWidth( v->getText() );
                pos++;
            } break;
            case ID_KWCharFootNote: {
                KWCharFootNote *fn = dynamic_cast<KWCharFootNote*>( text[ pos ].attrib );
                apply( *fn->getFormat() );
                ptPos += displayFont->getPTWidth( fn->getText() );
                pos++;
            } break;
            case ID_KWCharAnchor: {
                pos++;
            } break;
            case ID_KWCharTab: {
                unsigned int tabPos = 0;
                KoTabulators tabType;
                if ( parag->getParagLayout()->getNextTab( ptPos, pFrame->left() +
                                                          pFrame->getBLeft().pt(),
                                                          pFrame->right() -
                                                          pFrame->getBRight().pt(), tabPos, tabType ) ) {
                    // next tab is in this line
                    if ( tabPos > ptPos ) {
                        switch ( tabType ) {
                        case T_LEFT:
                            ptPos = tabPos;
                            break;
                        case T_RIGHT: {
                            if ( ptPos + ( ptTextLen - ( ptPos - ptStartPos ) ) < tabPos )
                                ptPos = tabPos - ( ptTextLen - ( ptPos - ptStartPos ) );
                        } break;
                        case T_CENTER: {
                            if ( ptPos + ( ptTextLen - ( ptPos - ptStartPos ) ) / 2 < tabPos )
                                ptPos = tabPos - ( ptTextLen - ( ptPos - ptStartPos ) ) / 2;
                        } break;
                        default: break;
                        }
                    } else { // next tab is in the next line
                        _break = TRUE;
                    }
                }
                pos++;
            } break;
            }
        } else {
            if ( text[ pos ].attrib ) {
                // Change text format here
                assert( text[ pos ].attrib->getClassId() == ID_KWCharFormat );
                KWCharFormat *f = ( KWCharFormat* )text[ pos ].attrib;
                apply( *f->getFormat() );
            }

            if ( text[ pos ].c == ' ' && lay->getFlow() == KWParagLayout::BLOCK &&
                 lineEndPos != parag->getTextLen() ) {
                float sp = ptSpacing + spacingError;

                float dx = floor( sp );
                spacingError = sp - dx;

                ptPos += ( unsigned int )dx + displayFont->getPTWidth( text[ pos ].c );

                pos++;
            } else {
                ptPos += displayFont->getPTWidth( text[ pos ].c );
                pos++;
            }
        }
    }

    textPos = _textpos;
}

/*================================================================*/
void KWFormatContext::cursorGotoPixelLine( unsigned int mx, unsigned int my )
{
    textPos = 0;

    KWParag *_p = dynamic_cast<KWTextFrameSet*>( pFrameSet )->getFirstParag();
    init( _p );

    if ( (ptY <= my) && (ptY + getLineHeight() >= my) &&
         (ptLeft <= mx) && (ptLeft + ptWidth >= mx) )
    {
        init( _p, FALSE );
        textPos = lineStartPos;
        cursorGotoLineStart();
        return;
    }

    while ( (_p->getPTYEnd() < my) && _p->getNext() )
        _p = _p->getNext();

    if ( _p->getPrev() ) _p = _p->getPrev();

    init( _p, FALSE );

    bool found = FALSE;
    unsigned int pg = page;
    while ( makeNextLineLayout() ) {
        if ( (ptY <= my) && (ptY + getLineHeight() >= my) &&
             (ptLeft <= mx) && (ptLeft + ptWidth >= mx) )
        {
            found = TRUE;
            break;
        }
        if ( (ptY > my + 20) && (pg != page) )
            break;
    }

    if ( !found )
    {
       ptY -= getLineHeight();
    }

    textPos = lineStartPos;
    cursorGotoLineStart();
}


/*================================================================*/
void KWFormatContext::cursorGotoPixelInLine( unsigned int mx, unsigned int )
{
    if ( isCursorAtLineEnd() ) return;
    unsigned int oldPTPos, oldDist = 0, dist = 0;

    while ( !isCursorAtLineEnd() )
    {
        oldPTPos = ptPos;
        cursorGotoRight();
        dist = ( ptPos - oldPTPos ) / 3;
        if ( mx >= oldPTPos - oldDist && mx <= ptPos - dist || textPos == lineStartPos )
        {
            cursorGotoLeft();
            return;
        }
        oldDist = dist;
    }
}

/*================================================================*/
KWCharAttribute* KWFormatContext::getObjectType( unsigned int mx, unsigned int my )
{
    KWFormatContext fc( doc, frameSet );
    //fc = *this;

    fc.cursorGotoPixelLine( mx, my );

    if ( isCursorAtLineEnd() ) return 0L;
    unsigned int oldPTPos, oldDist = 0, dist = 0;

    while ( !fc.isCursorAtLineEnd() )
    {
        oldPTPos = fc.getPTPos();
        fc.cursorGotoRight();
        dist = ( fc.getPTPos() - oldPTPos ) / 3;
        if ( mx >= oldPTPos - oldDist && mx <= fc.getPTPos() - dist || fc.getTextPos() == fc.getLineStartPos() )
        {
            fc.cursorGotoLeft();
            return fc.getParag()->getKWString()->data()[ fc.getTextPos() ].attrib;
        }
        oldDist = dist;
    }

    return fc.getParag()->getKWString()->data()[ fc.getTextPos() ].attrib;
}

/*================================================================*/
int KWFormatContext::cursorGotoNextChar()
{
    // If we are already at lineend, then we wont move further
    if ( isCursorAtLineEnd() )
        return -2;

    KWChar *text = parag->getText();
    KWParagLayout *lay = parag->getParagLayout();
    int pos = textPos;
    if ( text[ pos ].c == KWSpecialChar ) {
        // Handle specials here
        switch ( text[ pos ].attrib->getClassId() ) {
        case ID_KWCharImage: {
            ptPos += ( ( KWCharImage* )text[ pos ].attrib )->getImage()->width();
            specialHeight = QMAX( specialHeight,
                                  (unsigned int)( (KWCharImage*) text[ pos ].attrib )->getImage()->height() );
            pos++;
        } break;
        case ID_KWCharVariable: {
            KWCharVariable *v = dynamic_cast<KWCharVariable*>( text[ pos ].attrib );
            v->getVar()->setInfo( frameSet, frame, doc->getPageNum( ptY + doc->getPTPaperHeight() ), parag );
            v->getVar()->recalc();
            apply( *v->getFormat() );
            ptPos += displayFont->getPTWidth( v->getText() );
            pos++;
        } break;
        case ID_KWCharFootNote: {
            KWCharFootNote *fn = dynamic_cast<KWCharFootNote*>( text[ pos ].attrib );
            apply( *fn->getFormat() );
            ptPos += displayFont->getPTWidth( fn->getText() );
            pos++;
        } break;
        case ID_KWCharAnchor: {
            pos++;
        } break;
        case ID_KWCharTab: {
            unsigned int tabPos = 0;
            KoTabulators tabType;
            if ( parag->getParagLayout()->getNextTab( ptPos, pFrame->left() +
                                                      pFrame->getBLeft().pt(),
                                                      pFrame->right() -
                                                      pFrame->getBRight().pt(), tabPos, tabType ) ) {
                // next tab is in this line
                if ( tabPos > ptPos ) {
                    switch ( tabType ) {
                    case T_LEFT:
                        ptPos = tabPos;
                        break;
                    case T_RIGHT: {
                        //if ( !_checkTabs ) break;
                        if ( ptPos + ( ptTextLen - ( ptPos - ptStartPos ) ) < tabPos )
                            ptPos = tabPos - ( ptTextLen - ( ptPos - ptStartPos ) );
                    } break;
                    case T_CENTER: {
                        //if ( !_checkTabs ) break;
                        if ( ptPos + ( ptTextLen - ( ptPos - ptStartPos ) ) / 2 < tabPos )
                            ptPos = tabPos - ( ptTextLen - ( ptPos - ptStartPos ) ) / 2;
                    } break;
                    default: break;
                    }
                } else { // next tab is in the next line
                    pos++;
                    return -2;
                }
            }
            pos++;
        } break;
        }
    } else {
        if ( text[ pos ].attrib ) {
            // Change text format here
            assert( text[ pos ].attrib->getClassId() == ID_KWCharFormat );
            KWCharFormat *f = ( KWCharFormat* )text[ pos ].attrib;
            apply( *f->getFormat() );
        }

        if ( text[ pos ].c == ' ' && lay->getFlow() == KWParagLayout::BLOCK &&
             lineEndPos != parag->getTextLen() ) {
            float sp = ptSpacing + spacingError;
            float dx = floor( sp );
            spacingError = sp - dx;
            ptPos += ( unsigned int )dx + displayFont->getPTWidth( text[ pos ].c );
            pos++;
        } else {
            ptPos += displayFont->getPTWidth( text[ pos ].c );
            pos++;
        }
    }
    textPos = pos;

    if ( isCursorAtLineEnd() )
        return -2;

    if ( parag->getText()[ textPos ].c != KWSpecialChar &&
         parag->getText()[ textPos ].attrib == 0 ||
         (int)textPos - 1 >= 0 && parag->getText()[ textPos - 1 ].c != KWSpecialChar &&
         *( ( KWCharFormat* )parag->getText()[ textPos - 1 ].attrib ) ==
         *( ( KWCharFormat* )parag->getText()[ textPos ].attrib ) )
        return 1;

    if ( parag->getText()[ textPos ].c != KWSpecialChar )
        return 0;

    assert( parag->getText()[ textPos ].attrib != 0L );
    return -1;
}


/*================================================================*/
bool KWFormatContext::makeNextLineLayout( bool redrawBackgroundWhenAppendPage )
{
    if ( !pFrameSet->isVisible() )
        return FALSE;

    if ( !parag ) {
        //qDebug( "1" );
        doc->checkNumberOfPages( this );
        return FALSE;
    }

    if ( lineEndPos == parag->getTextLen() ) {
        if ( parag->getNext() == 0L || outOfFrame ) {
            ptY += getLineHeight();
            parag->setPTYEnd( ptY );
            outOfFrame = FALSE;
            if ( !outOfFrame )
                doc->checkNumberOfPages( this );
            return FALSE;
        }

        ptY += getLineHeight();
        enterNextParag();
    } else {
        lineStartPos = lineEndPos;
        ptY += getLineHeight();
        makeLineLayout( TRUE, TRUE, redrawBackgroundWhenAppendPage );
    }

    return TRUE;
}

/*================================================================*/
bool KWFormatContext::makeLineLayout( bool _checkIntersects, bool _checkTabs,
                                      bool redrawBackgroundWhenAppendPage )
{
    if ( !pFrameSet->isVisible() )
        return FALSE;

    int _left = 0, _right = 0;

    // Reggie: DAMN SLOW HACK !!!!!!!
    if ( parag->getParagLayout()->hasSpecialTabs() && _checkTabs && _checkIntersects )
        makeLineLayout( TRUE, FALSE, redrawBackgroundWhenAppendPage );

    if ( _checkIntersects ) {
        if ( pFrame->hasIntersections() )
            makeLineLayout( FALSE, TRUE, redrawBackgroundWhenAppendPage );
    }

    if ( pFrame->hasIntersections() ) {
        _left = pFrame->getLeftIndent( ptY, getLineHeight() );
        _right = pFrame->getRightIndent( ptY, getLineHeight() );
    }

    compare_formats = FALSE;
    unsigned int _ptTextLen = ptTextLen;
    ptTextLen = 0;
    specialHeight = 0;

    ptPos = 0;
    spaces = 0;
    textPos = lineStartPos;
    lineEndPos = lineStartPos;
    ptMaxAscender = 0;
    ptMaxDescender = 0;
    ptAscender = 0;
    ptDescender = 0;
    offsetsAdded = TRUE;

    unsigned int tmpPTWidth = 0;
    unsigned int tmpPTAscender;
    unsigned int tmpPTDescender;
    unsigned int tmpSpaces = 0;

    unsigned left = 0;
    unsigned int right = 0;

    KWChar *text = parag->getText();

    makeCounterLayout(); // !!! HACK !!!

    // The indentation of the line. This is only the indentation
    // the user selected.
    unsigned int indent;
    if ( lineStartPos == 0 ) {
        // Reset font size, color etc. to the documents default
        setDefaults( doc );
        // Change fonts & stuff to match the paragraphs layout
        apply( parag->getParagLayout()->getFormat() );

        indent = static_cast<int>(parag->getParagLayout()->getFirstLineLeftIndent().pt()*doc->getZoom())/100;
    } else {
        indent = static_cast<int>(parag->getParagLayout()->getLeftIndent().pt()*doc->getZoom())/100;
    }

    indent += _left;

    // Calculate the shift for the first visible character. This may be the counter, too
    unsigned int xShift = static_cast<int>(pFrame->left() + pFrame->getBLeft().pt() + indent);

    ptLeft = xShift;
    ptWidth = static_cast<int>(pFrame->width() -
              pFrame->getBLeft().pt() -
              pFrame->getBRight().pt() - indent - _right);

    // First line ? Draw the couter ?
    if ( lineStartPos == 0 && parag->getParagLayout()->getCounterType() != KWParagLayout::CT_NONE ) {
        KWFormat counterfm( doc, *this );
        counterfm.apply( parag->getParagLayout()->getFormat() );
//         if ( parag->getParagLayout()->getCounterType() == KWParagLayout::CT_BULLET )
//             counterfm.setUserFont( document->findUserFont( parag->getParagLayout()->getBulletFont() ) );
//         _painter.setFont( *( counterfm.loadFont( document ) ) );
//         _painter.setPen( counterfm.getColor() );

        left += ptCounterWidth;
    }


    //ptPos = 0;

    // Calculate the first characters position in screen coordinates
    ptPos = xShift + left;

    ptStartPos = ptPos;

    // Calculate the counter position
    // Is the counter fixed to the left side ?
    ptCounterPos = ptStartPos - ptCounterWidth;

    apply( *this );

    // Get ascender/descender of the font we are starting with
    tmpPTAscender = displayFont->getPTAscender();
    tmpPTDescender = displayFont->getPTDescender();

    lineStartFormat = *this;

    if ( static_cast<int>( ptWidth ) < doc->getRastX() ) {
        int newptY = pFrame->getNextFreeYPos( ptY, getLineHeight() );
        if(ptY != newptY) {
            ptY = newptY+2;
            return makeLineLayout( TRUE, TRUE, redrawBackgroundWhenAppendPage );
        } else return TRUE;
    }

    bool _broken = FALSE;
    bool _break = FALSE;

    doc->getAutoFormat().startAutoFormat( parag, this );

    compare_formats = TRUE;
    // Loop until we reach the end of line
    while ( !_break && ( ptPos < xShift + ( pFrame->width() -
                                            pFrame->getBLeft().pt() -
                                            pFrame->getBRight().pt() ) - indent - _right ||
                         !_broken ) &&
            textPos < parag->getTextLen() ) {
        QChar c = text[ textPos ].c;

        if ( c != KWSpecialChar && text[ textPos ].attrib ) {
            // Handle font formats here.
            //assert( text[ textPos ].attrib->getClassId() == ID_KWCharFormat );
            KWCharFormat *f = ( KWCharFormat* )text[ textPos ].attrib;
            apply( *f->getFormat() );
            if ( textPos == lineStartPos )
                lineStartFormat = *this;
        }

        // do the autoformat stuff
        if ( c != KWSpecialChar ) {
            doc->getAutoFormat().doSpellCheck( parag, this );
            doc->getAutoFormat().doTypographicQuotes( parag, this );
            doc->getAutoFormat().doAutoFormat( parag, this );
            doc->getAutoFormat().doUpperCase( parag, this );
        }

        // if we will not fit into the line anymore, let us leave the loop
        if ( c != KWSpecialChar ) {
            if ( ptPos + displayFont->getPTWidth( c ) >=
                 xShift + ( pFrame->width() -
                            pFrame->getBLeft().pt() -
                            pFrame->getBRight().pt() ) - indent - _right && _broken )
                break;
        } else if ( c == KWSpecialChar && text[ textPos ].attrib->getClassId() == ID_KWCharImage ) {
            if ( ( ( KWCharImage* )text[ textPos ].attrib )->getImage()->width() + ptPos >=
                 xShift + ( pFrame->width() -
                            pFrame->getBLeft().pt() -
                            pFrame->getBRight().pt() ) - indent - _right && _broken )
                break;
        } else if ( c == KWSpecialChar && text[ textPos ].attrib->getClassId() == ID_KWCharVariable ) {
            if ( displayFont->getPTWidth( dynamic_cast<KWCharVariable*>( text[ textPos ].attrib )->getText() ) +
                 ptPos >=
                 xShift + ( pFrame->width() -
                            pFrame->getBLeft().pt() -
                            pFrame->getBRight().pt() ) - indent - _right && _broken )
                break;
        } else if ( c == KWSpecialChar && text[ textPos ].attrib->getClassId() == ID_KWCharFootNote ) {
            if ( displayFont->getPTWidth( dynamic_cast<KWCharFootNote*>( text[ textPos ].attrib )->getText() ) +
                 ptPos >=
                 xShift + ( pFrame->width() -
                            pFrame->getBLeft().pt() -
                            pFrame->getBRight().pt() ) - indent - _right && _broken )
                break;
        }

        // Is it a space character
        if ( c == ' ' ) {
            // This is the correct point to make a line break
            lineEndPos = textPos + 1;
            // If we break here, then the line has the following width ...
            ptTextLen = tmpPTWidth;
            spaces = tmpSpaces;
            // ... or one more space if we dont break the line here.
            tmpSpaces++;
            _broken = TRUE;
        }

        // Do we have some format definition here ?
        if ( c == KWSpecialChar ) {
            switch ( text[ textPos ].attrib->getClassId() ) {
            case ID_KWCharImage: {
                int w = ( ( KWCharImage* )text[ textPos ].attrib )->getImage()->width();
                ptPos += w;
                tmpPTWidth += w;
                specialHeight = QMAX( specialHeight,
                                      (unsigned int)( (KWCharImage*)text[ textPos ].attrib )->getImage()->height() );
                textPos++;
            } break;
            case ID_KWCharVariable: {
                KWCharVariable *v = dynamic_cast<KWCharVariable*>( text[ textPos ].attrib );
                v->getVar()->setInfo( frameSet, frame,
                                      doc->getPageNum( ptY + doc->getPTPaperHeight() ), parag );
                v->getVar()->recalc();
                apply( *v->getFormat() );
                int w = displayFont->getPTWidth( v->getText() );
                ptPos += w;
                tmpPTWidth += w;
                textPos++;
            } break;
            case ID_KWCharFootNote: {
                KWCharFootNote *fn = dynamic_cast<KWCharFootNote*>( text[ textPos ].attrib );
                apply( *fn->getFormat() );
                int w = displayFont->getPTWidth( fn->getText() );
                ptPos += w;
                tmpPTWidth += w;
                textPos++;
            } break;
            case ID_KWCharAnchor: {
                // An anchor does not consume any display width.
                textPos++;
            } break;
            case ID_KWCharTab: {
                unsigned int tabPos = 0;
                KoTabulators tabType;
                if ( parag->getParagLayout()->getNextTab( ptPos, pFrame->left() +
                                                          pFrame->getBLeft().pt(),
                                                          pFrame->right() -
                                                          pFrame->getBRight().pt(),
                                                          tabPos, tabType ) ) {
                    // next tab is in this line
                    if ( tabPos > ptPos ) {
                        switch ( tabType ) {
                        case T_LEFT:
                            ptPos = tabPos;
                            break;
                        case T_RIGHT: {
                            if ( !_checkTabs ) break;
                            if ( ptPos + ( _ptTextLen - ( ptPos - ptStartPos ) ) < tabPos )
                                ptPos = tabPos - ( _ptTextLen - ( ptPos - ptStartPos ) );
                        } break;
                        case T_CENTER: {
                            if ( !_checkTabs ) break;
                            if ( ptPos + ( _ptTextLen - ( ptPos - ptStartPos ) ) / 2 < tabPos )
                                ptPos = tabPos - ( _ptTextLen - ( ptPos - ptStartPos ) ) / 2;
                        } break;
                        default: break;
                        }
                    } else { // next tab is in the next line
                        lineEndPos = textPos;
                        textPos--;
                        ptTextLen = tmpPTWidth;
                        spaces = tmpSpaces;
                        tmpSpaces++;
                        _broken = TRUE;
                        _break = TRUE;
                    }
                }
                textPos++;
            } break;
            }
        } else { // An usual character ...
            // Go right ...
            ptPos += displayFont->getPTWidth( c );
            // Increase the lines width
            tmpPTWidth += displayFont->getPTWidth( c );
            // One more character
            textPos++;
        }
    }

    doc->getAutoFormat().endAutoFormat( parag, this );

    // Are we at the paragraphs end ?
    if ( textPos == parag->getTextLen() ) {
        // We have to take the last possible linebreak
        lineEndPos = textPos;
        ptTextLen = tmpPTWidth;
        ptAscender = tmpPTAscender;
        ptDescender = tmpPTDescender;
        if ( ptAscender > ptMaxAscender )
            ptMaxAscender = ptAscender;
        if ( ptDescender > ptMaxDescender )
            ptMaxDescender = ptDescender;
        spaces = tmpSpaces;
    }

    if ( parag->getParagLayout()->getFlow() == KWParagLayout::CENTER ) {
        ptPos = static_cast<int>(xShift + ( ( pFrame->width() -
                             pFrame->getBLeft().pt() -
                             pFrame->getBRight().pt() ) -
                           indent - left - right - ptTextLen - _right ) / 2);
        ptStartPos = ptPos;
    } else if ( parag->getParagLayout()->getFlow() == KWParagLayout::RIGHT ) {
        ptPos = static_cast<int>(xShift + ( pFrame->width() -
                           pFrame->getBLeft().pt() -
                           pFrame->getBRight().pt() ) -
                right - ptTextLen - indent - _right);
        ptStartPos = ptPos;
    }

    // Calculate the space between words if we have "block" formating.
    if ( parag->getParagLayout()->getFlow() == KWParagLayout::BLOCK && spaces > 0 )
        ptSpacing = static_cast<float>( ( pFrame->width() -
                                          pFrame->getBLeft().pt() -
                                          pFrame->getBRight().pt() ) - ptTextLen -
                                        indent - _right ) / static_cast<float>( spaces );
    else
        ptSpacing = 0;

    offsetsAdded = FALSE;
    compare_formats = FALSE;
    apply( *( ( KWFormat* )this ) );
    compare_formats = TRUE;

    // Does this line still fit in this frame or do we have to make a hard break?
    if ( !doc->isPTYInFrame( frameSet - 1, frame - 1, ptY + getLineHeight() ) ||
         ( parag->hasHardBreak() && isCursorInFirstLine() ) &&
         parag->getPrev() && parag->getPrev()->getEndPage() == page ) {
        // Are we a header or footer?
        if ( isAHeader( pFrameSet->getFrameInfo() ) ||
             isAFooter( pFrameSet->getFrameInfo() ) ) {
            int diff = static_cast<int>(( ptY + getLineHeight() ) - ( pFrame->bottom() -
                                                     pFrame->getBBottom().pt() ));

            if ( doc->canResize( pFrameSet,
                                      pFrame,
                                      pFrameSet->getPageOfFrame( frame - 1 ), diff + 1 ) ) {
                pFrame->setHeight( pFrame->height() + diff + 1 );

                if ( pFrameSet->getGroupManager() ) {
                    pFrameSet->getGroupManager()->deselectAll();
                    pFrame->setSelected( TRUE );
                    pFrameSet->getGroupManager()->recalcRows();
                }

                doc->recalcFrames( FALSE, TRUE );
                doc->updateAllFrames();
                doc->setNeedRedraw( TRUE );
            } else {
                outOfFrame = TRUE;
                return FALSE;
            }
        } else if ( frame < pFrameSet->getNumFrames() ) { // Can we jump to the next frame ?
            setFrame( frame + 1 );
            if ( static_cast<int>( pFrame->top() + pFrame->getBTop().pt() ) >
                 static_cast<int>( page ) * static_cast<int>( doc->getPTPaperHeight() ) )
                page++;
            parag->setEndPage( page );
            parag->setEndFrame( frame );
            ptY = static_cast<int>(pFrame->top() + pFrame->getBTop().pt());
            return makeLineLayout( TRUE, TRUE, redrawBackgroundWhenAppendPage );
        } else { // append a page or resize frame
            if ( pFrame->getFrameBehaviour() == AutoExtendFrame && !parag->hasHardBreak() ) { // Resize frame
                int diff = static_cast<int>(( ptY + getLineHeight() ) - ( pFrame->bottom() -
                                                         pFrame->getBBottom().pt() ));

                if ( doc->canResize( pFrameSet,
                                          pFrame,
                                          pFrameSet->getPageOfFrame( frame - 1 ), diff + 1 ) ) {
                    pFrame->setHeight( pFrame->height() + diff + 1 );

                    if ( pFrameSet->getGroupManager() ) {
                        pFrameSet->getGroupManager()->deselectAll();
                        pFrame->setSelected( TRUE );
                        pFrameSet->getGroupManager()->recalcRows();
                    }

                    doc->recalcFrames( FALSE, TRUE );
                    doc->updateAllFrames();
                    doc->setNeedRedraw( TRUE );
                    doc->drawAllBorders( );
                } else {
                    outOfFrame = TRUE;
                    return FALSE;
                }
            } else if ( parag->hasHardBreak() ||
                        ( !(pFrame->getFrameSet()->getGroupManager()) &&
                          pFrame->getFrameBehaviour() == AutoCreateNewFrame &&
                          pFrame->getNewFrameBehaviour()==Reconnect) ) {
                if(pFrame->getPageNum()+1==doc->getPages())  { // Append page
                    doc->appendPage( page - 1, redrawBackgroundWhenAppendPage );
                }
                else {
                    // make a new frame.
                    KWFrame *frm = new KWFrame(pFrame->getFrameSet(), pFrame->x(), pFrame->y() + doc->getPTPaperHeight(),
                                               pFrame->width(), pFrame->height(), pFrame->getRunAround(), pFrame->getRunAroundGap() );
                    frm->setLeftBorder( pFrame->getLeftBorder2() );
                    frm->setRightBorder( pFrame->getRightBorder2() );
                    frm->setTopBorder( pFrame->getTopBorder2() );
                    frm->setBottomBorder( pFrame->getBottomBorder2() );
                    frm->setBLeft( pFrame->getBLeft() );
                    frm->setBRight( pFrame->getBRight() );
                    frm->setBTop( pFrame->getBTop() );
                    frm->setBBottom( pFrame->getBBottom() );
                    frm->setBackgroundColor( QBrush( pFrame->getBackgroundColor() ) );
                    frm->setPageNum( pFrame->getPageNum()+1 );
                    frm->setNewFrameBehaviour(pFrame->getNewFrameBehaviour());
                    frm->setFrameBehaviour(pFrame->getFrameBehaviour());
                    frm->setSheetSide(pFrame->getSheetSide());
                    pFrame->getFrameSet()->addFrame( frm );
                    doc->updateAllFrames();
                }
                page++;
                setFrame( frame + 1 );
                parag->setEndPage( page );
                parag->setEndFrame( frame );
                ptY = static_cast<int>(pFrame->top() +
                                       pFrame->getBTop().pt());
                return makeLineLayout( TRUE, TRUE, redrawBackgroundWhenAppendPage );
            }
        }
    }

    compare_formats = TRUE;
    // If we are in the last line, return FALSE
    if ( lineEndPos == parag->getTextLen() )
        return FALSE;

    return TRUE;
}

/*================================================================*/
unsigned int KWFormatContext::getLineHeight() const
{
    unsigned int hei = ptMaxAscender + ptMaxDescender;
    unsigned int plus = 0;

    return QMAX( hei, specialHeight ) + static_cast<int>(getParag()->getParagLayout()->getLineSpacing().pt()) + plus;
}

/*================================================================*/
void KWFormatContext::makeCounterLayout()
{
    if ( parag->getParagLayout()->getCounterType() == KWParagLayout::CT_NONE ) {
        ptCounterWidth=0;
        return;
    }

    KWFormat format( doc, parag->getParagLayout()->getFormat() );
    format.apply( parag->getParagLayout()->getFormat() );
    if ( parag->getParagLayout()->getCounterType() == KWParagLayout::CT_BULLET )
        format.setUserFont( doc->findUserFont( parag->getParagLayout()->getBulletFont() ) );
    KWDisplayFont *font = format.loadFont( doc );

    counterText = parag->getCounterText();

    ptCounterWidth = QMAX( font->getPTWidth( parag->getCounterWidth() ),
                           font->getPTWidth( parag->getCounterText() ) );
    ptCounterAscender = font->getPTAscender();
    ptCounterDescender = font->getPTDescender();
}


/*================================================================*/
void KWFormatContext::apply( const KWFormat &_format )
{
    if ( compare_formats && _format == *( ( KWFormat* )this ) )
        return;

    KWFormat::apply( _format );
    if ( _format.getVertAlign() != VA_NORMAL )
        displayFont = doc->findDisplayFont( userFont, ( 2 * _format.getPTFontSize() ) / 3, _format.getWeight(),
                                                 _format.getItalic(), _format.getUnderline() );
    else
        displayFont = loadFont( doc );

    ptAscender = displayFont->getPTAscender();
    ptDescender = displayFont->getPTDescender();
    ptMaxAscender = QMAX( ptAscender, ptMaxAscender );
    ptMaxDescender = QMAX( ptDescender, ptMaxDescender );

    if ( _format.getVertAlign() == KWFormat::VA_SUB )
        ptMaxDescender = QMAX( ptMaxDescender, (unsigned int)( (2 * _format.getPTFontSize() ) / 3) / 2 );
    else if ( _format.getVertAlign() == KWFormat::VA_SUPER )
        ptMaxAscender = QMAX( ptMaxAscender, (unsigned int)( (2 * _format.getPTFontSize() ) / 3) / 2);

    if ( !offsetsAdded ) {
        if ( isCursorInLastLine()  && getParag() && getParag()->getParagLayout()->getParagFootOffset().pt() != 0 )
            ptMaxDescender += static_cast<int>(getParag()->getParagLayout()->getParagFootOffset().pt());
        if ( isCursorInFirstLine() && getParag() && getParag()->getParagLayout()->getParagHeadOffset().pt() != 0 )
            ptMaxAscender += static_cast<int>(getParag()->getParagLayout()->getParagHeadOffset().pt());
        offsetsAdded = TRUE;
    }
}

/*================================================================*/
bool KWFormatContext::selectWord( KWFormatContext &_fc1, KWFormatContext &_fc2 )
{
    KWChar *text = parag->getText();
    bool goLeft = FALSE;

    if ( !text || parag->getKWString()->size() <= textPos )
        return FALSE;

    KWFormatContext fc( doc, frameSet );
    fc = *this;

    if ( text[ textPos ].c == ' ' ) {
        _fc1 = *this;
        cursorGotoRight();
    } else {
        unsigned int i = textPos;
        while ( i > 0 && i > lineStartPos && text[ i ].c != ' ' ) {
            cursorGotoLeft();
            i = textPos;
        }
        if ( i == lineStartPos )
            goLeft = TRUE;
        cursorGotoRight();
        _fc1 = *this;
    }

    if ( text[ textPos ].c == ' ' )
        _fc2 = *this;
    else {
        unsigned int i = textPos;
        while ( i < parag->getTextLen() && i < lineEndPos && text[ i ].c != ' ' ) {
            cursorGotoRight();
            i = textPos;
        }
        _fc2 = *this;
    }
    if ( goLeft ) _fc1.cursorGotoLeft();

    return TRUE;
}

/*================================================================*/
void KWFormatContext::setFrameSet( unsigned int _frameSet )
{
    // #### just a bad workaround. Fix the source of the problem!
    if ( _frameSet == 0 )
        ++_frameSet;

    frameSet = _frameSet;
    if ( doc )
        pFrameSet = doc->getFrameSet( frameSet - 1 );
    else
        pFrameSet = 0;

//     if ( !pFrameSet )
//      qWarning( "KWFormatContext::setFrameSet: pFrameSet is NULL, KWord will crash soon or at least behave strange!" );

    setFrame( 1 );
}

/*================================================================*/
void KWFormatContext::setFrame( unsigned int _frame )
{
    // #### just a bad workaround. Fix the source of the problem!
    if ( _frame == 0 )
        ++_frame;

    frame = _frame;
    if ( doc && pFrameSet ) {
        pFrame = pFrameSet->getFrame( frame - 1 );
        if(pFrame)
            emptyRegion = pFrame->getEmptyRegion();
    } else
        pFrame = 0;

//     if ( !pFrame )
//      qWarning( "KWFormatContext::setFrame: pFrame is NULL, KWord will crash soon or at least behave strange!" );
}

/*================================================================*/
QRegion KWFormatContext::getEmptyRegion() const
{
    return emptyRegion;
}
