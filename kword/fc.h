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

#ifndef line_h
#define line_h

#include <qregion.h>

#include "format.h"
#include "parag.h"

class KWFormatContext;
class KWordDocument;
class KWCharAttribute;
class KWFrame;
class KWFrameSet;

/******************************************************************/
/* Class: KWFormatContext                                         */
/******************************************************************/

/**
 * @short The context of the cursors position in the text.
 *
 * A format context ( fc ) is used to represent the font, position, layout etc. of
 * the cursors position. Every @ref KWordDocument has at least one @ref KWFormatContext.
 * Whenever the cursor moves, the fc is updated. This way we always know in
 * which line we are, what the current font is, etc. This class does not print or
 * display the cursor. It just represents the context of the cursors position.
 * It is legal to have multiple FCs, but mention that a call to one FCs functions
 * may invalidate another one. So dont use multiple ones if you dont know
 * what you are doing.
 */
class KWFormatContext : public KWFormat
{
public:
    enum LayoutError {COLUMN_TOO_TALL, PAPER_HEIGHT_TOO_SMALL, NO_ERROR};

    KWFormatContext( KWordDocument *_doc, unsigned int _frameSet );
    KWFormatContext &operator=( const KWFormatContext &fc ) const;
    virtual ~KWFormatContext() {}

    virtual bool operator>( const KWFormatContext &fc ) const;
    virtual bool operator<( const KWFormatContext &fc ) const;
    virtual bool operator==( const KWFormatContext &fc ) const;

    void init( KWParag *_parag, bool _fromStart = TRUE,
               int _frame = -1, int _page = -1 );
    void enterNextParag();
    void skipCurrentParag();
    void gotoStartOfParag();
    void cursorGotoLine( unsigned int _textpos );
    /**
     * Assumes that '_textpos' is in the current line. It sets the cursor
     * to this position.
     */
    void cursorGotoPos( unsigned int _textpos );
    /**
     * Move cursor to 'mx' and 'my' whis is given in pixels.
     */
    void cursorGotoPixelLine( unsigned int mx, unsigned int my );
    void cursorGotoPixelInLine( unsigned int mx, unsigned int my );
    KWCharAttribute* getObjectType( unsigned int mx, unsigned int my );
    /**
     * Move the cursor to the next character very fast. Return -1 if the next character
     * is a special object like an image or such, returns 0 if there is a character
     * but a format change and returns 1 if there is a character without a format change.
     * Returns -2 if we are at the line end.
     *
     * This function does not leave the current line.
     */
    int cursorGotoNextChar();
    void cursorGotoRight();
    void cursorGotoRight( int _pos );
    void cursorGotoLeft();
    void cursorGotoUp();
    void cursorGotoDown();
    void cursorGotoLineStart();
    /**
     * Sets the cursor BEHIND the last character of the current line
     * if the line is the last one of the current paragrph.
     * Otherwise the cursor is set ON the last character.
     * The only exception are empty lines or lines which contain a word
     * that is longer then an entire line. In these cases the cursor
     * is positioned BEHIN the last character, too.
     */
    void cursorGotoLineEnd();
    void cursorGotoNextLine();
    void cursorGotoPrevLine();
    bool makeNextLineLayout( bool redrawBackgroundWhenAppendPage = TRUE );
    bool makeLineLayout( bool _checkIntersects = TRUE, bool _checkTabs = TRUE, bool redrawBackgroundWhenAppendPage = TRUE );
    void makeCounterLayout();

    bool isCursorAtParagStart();
    bool isCursorAtLineStart();
    bool isCursorInFirstLine();
    bool isCursorAtParagEnd();
    bool isCursorAtLineEnd();
    /**
     * @return TRUE if the cursor is currently positioned at the last character of
     *         a line. This is one character before the linedEndPos.
     */
    bool isCursorAtLastChar();
    bool isCursorInLastLine();

    KWDisplayFont& getDisplayFont() const { return *displayFont; }

    /**
     * @return the paragraph we are in currently.
     *
     * @see #parag
     */
    KWParag* getParag() const { return parag; }
    /**
     * @return the page we are currently on.
     *
     * @see #pahe
     */
    unsigned int getPage() const { return page; }
    /**
     * @return the colum we are in currently.
     *
     * @see #column
     */
    unsigned int getFrameSet() const { return frameSet; }
    unsigned int getFrame() const { return frame; }

    /**
     * @return the text that represents the counter
     */
    QString getCounterText() const { return counterText; }
    /**
     * @return the width that is needed to display the counter.
     */
    unsigned int getPTCounterWidth() const { return ptCounterWidth; }
    /**
     * @return the position that is needed to display the counter.
     */
    unsigned int getPTCounterPos() const { return ptCounterPos; }
    /**
     * @return the current ascender. This is always <= ptMaxAscender.
     */
    unsigned int getPTAscender() const { return ptAscender; }
    /**
     * @return the current descender. This is always <= ptMaxDescender.
     */
    unsigned int getPTDescender() const { return ptDescender; }
    /**
     * @return the maximal ascender in the current line.
     */
    unsigned int getPTMaxAscender() const { return ptMaxAscender; }
    /**
     * @return the maximal descender in the current line.
     */
    unsigned int getPTMaxDescender() const { return ptMaxDescender; }
    /**
     * @return the Y position of the cursor relative to the upper left
     *         corner of the first page.
     */
    unsigned int getPTY() const { return ptY; }
    /**
     * @return the X position of the cursor relative to the left edge
     *         of the paper.
     */
    unsigned int getPTPos() const { return ptPos; }

    unsigned int getPTTextLen() const { return ptTextLen; }

    unsigned int getPTLeft() const { return ptLeft; }

    unsigned int getPTWidth() const { return ptWidth; }

    float getPTSpacing() const { return ptSpacing; }

    unsigned int getLineStartPos() const { return lineStartPos; }
    unsigned int getLineEndPos() const { return lineEndPos; }
    unsigned int getTextPos() const { return textPos; }
    void setTextPos( unsigned int _pos ) { textPos = _pos; }

    unsigned int getLineHeight() const;

    void apply( const KWFormat &_format );

    bool selectWord( KWFormatContext &_fc1, KWFormatContext &_fc2 );

    void setFrameSet( unsigned int _frameSet );
    void setFrame( unsigned int _frame );

    QRegion getEmptyRegion() const;

protected:
    unsigned int ptTextLen;
    unsigned int ptAscender;
    unsigned int ptDescender;
    unsigned int ptMaxAscender;
    unsigned int ptMaxDescender;
    unsigned int ptY;
    unsigned int ptCellSpacing;
    unsigned int ptPos;
    unsigned int WantedPtPos;
    bool during_vertical_cursor_movement;
    unsigned int specialHeight;
    unsigned int frameSet;
    unsigned int frame;

    unsigned int page;
    //unsigned int column;

    unsigned int lineStartPos;
    KWFormat lineStartFormat;

    unsigned int lineEndPos;
    unsigned int textPos;

    /**
     * Amount of spaces in the line. This is needed if we have "Blocksatz".
     * We need this to calculate the width of a single space character.
     */
    unsigned short spaces;

    /**
     * Additional offsets to the left and right.
     * Counters cause them to become different from 0.
     */
    unsigned int ptLeft;
    unsigned int ptWidth;
    unsigned int ptStartPos;
    unsigned int ptCounterPos;

    /**
     * We use this instance for font loading stuff.
     */
    KWFormat tmpFormat;

    /**
     * The paragraph we are currently in.
     */
    KWParag *parag;

    LayoutError error;

    /**
     * This is the counter text of this paragraph.
     */
    QString counterText;
    /**
     * The width needed to display the counter.
     */
    unsigned int ptCounterWidth;
    unsigned int ptCounterAscender;
    unsigned int ptCounterDescender;

    /**
     * This is the font we are using.
     */
    KWDisplayFont *displayFont;

    /**
     * The amount of points that you have to add to every spaces width to
     * get the BLOCK flow.
     */
    float ptSpacing;

    /**
     * The error we make in 'block' mode, by assigning spaces
     * an integer size, but a float size would be correct.
     */
    float spacingError;

    bool compare_formats;
    bool outOfFrame;
    bool offsetsAdded;

    QRegion emptyRegion;

    KWFrameSet *pFrameSet;
    KWFrame *pFrame;

};

#endif
