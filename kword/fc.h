#ifndef line_h
#define line_h

class KWFormatContext;
class KWordDocument_impl;

#include <qpainter.h>

#include "format.h"
#include "parag.h"

/**
 * @short The context of the cursors position in the text.
 *
 * A format context ( fc ) is used to represent the font, position, layout etc. of
 * the cursors position. Every @ref KWordDocument_impl has at least one @ref KWFormatContext.
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
    enum LayoutError { COLUMN_TOO_TALL, PAPER_HEIGHT_TOO_SMALL, NO_ERROR };

    KWFormatContext( KWordDocument_impl *_doc );
    ~KWFormatContext();

    void init( KWParag *_parag, QPainter &_painter );
    void enterNextParag( QPainter &_painter );
    void skipCurrentParag( QPainter &_painter );
    void gotoStartOfParag( QPainter &_painter );
    void cursorGotoLine( unsigned int _textpos, QPainter &_painter );
    /**
     * Assumes that '_textpos' is in the current line. It sets the cursor
     * to this position.
     */
    void cursorGotoPos( unsigned int _textpos, QPainter &_painter );
    /**
     * Move the cursor to the next character very fast. Return True
     * if there was a font change.
     */
    bool cursorGotoNextChar( QPainter &_painter );
    void cursorGotoRight( QPainter &_painter );
    void cursorGotoLeft( QPainter &_painter );
    void cursorGotoUp( QPainter &_painter );
    void cursorGotoDown( QPainter &_painter );
    void cursorGotoLineStart( QPainter &_painter );
    void cursorGotoLineEnd( QPainter &_painter );
    void cursorGotoNextLine(QPainter &_painter);
    bool makeNextLineLayout( QPainter &_painter );
    bool makeLineLayout( QPainter &_painter );
    void makeCounterLayout( QPainter &_painter );

    bool isCursorAtParagStart();
    bool isCursorAtLineStart();
    bool isCursorInFirstLine();
    bool isCursorAtParagEnd();
    bool isCursorAtLineEnd();
    bool isCursorInLastLine();
    
    KWDisplayFont& getDisplayFont() { return *displayFont; }

    /**
     * @return the paragraph we are in currently.
     *
     * @see #parag
     */
    KWParag* getParag() { return parag; }
    /**
     * @return the page we are currently on.
     *
     * @see #pahe
     */
    unsigned int getPage() { return page; }
    /**
     * @return the colum we are in currently.
     *
     * @see #column
     */
    unsigned int getColumn() { return column; }

    /**
     * @return the text that represents the counter
     */
    const char* getCounterText() { return counterText.data(); }
    /**
     * @return the width that is needed to display the counter.
     */
    unsigned int getPTCounterWidth() { return ptCounterWidth; }
    /**
     * @return the position that is needed to display the counter.
     */
    unsigned int getPTCounterPos() { return ptCounterPos; }
    /**
     * @return the current ascender. This is always <= ptMaxAscender.
     */
    unsigned int getPTAscender() { return ptAscender; }
    /**
     * @return the current descender. This is always <= ptMaxDescender.
     */
    unsigned int getPTDescender() { return ptDescender; }
    /**
     * @return the maximal ascender in the current line.
     */
    unsigned int getPTMaxAscender() { return ptMaxAscender; }
    /**
     * @return the maximal descender in the current line.
     */
    unsigned int getPTMaxDescender() { return ptMaxDescender; }
    /**
     * @return the Y position of the cursor relative to the upper left
     *         corner of the first page.
     */
    unsigned int getPTY() { return ptY; }
    /**
     * @return the X position of the cursor relative to the left edge
     *         of the paper.
     */
    unsigned int getPTPos() { return ptPos; }

    unsigned int getPTTextLen() { return ptTextLen; }

    unsigned int getPTLeft() { return ptLeft; }

    unsigned int getPTWidth() { return ptWidth; }

    float getPTSpacing() { return ptSpacing; }

    unsigned int getLineStartPos() { return lineStartPos; }
    unsigned int getLineEndPos() { return lineEndPos; }
    unsigned int getTextPos() { return textPos; }

    unsigned short getCounter( unsigned int _counternr, unsigned int _depth );

    unsigned int getLineHeight() { return ptMaxAscender + ptMaxDescender; }
    
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
    
    unsigned int page;
    unsigned int column;
    
    unsigned int lineStartPos;
    unsigned int lineEndPos;
    unsigned int textPos;
    
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
    /**
     * The document we are in right now.
     */
    KWordDocument_impl *document;    

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

    /**
     * We store all paragraph counters in this array.
     */
    unsigned short counters[8][8];
};

#endif
