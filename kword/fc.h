#ifndef line_h
#define line_h

class KWFormatContext;
class KWordDocument;
class KWCharAttribute;

#include <qpainter.h>

#include "format.h"
#include "parag.h"

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
  enum LayoutError { COLUMN_TOO_TALL, PAPER_HEIGHT_TOO_SMALL, NO_ERROR };

  KWFormatContext(KWordDocument *_doc,unsigned int _frameSet);
  ~KWFormatContext();

  void init( KWParag *_parag, QPainter &_painter, bool _updateCounters = true, bool _fromStart = true,
	     int _frame = -1, int _page = -1 );
  void enterNextParag( QPainter &_painter, bool _updateCounters = true );
  void skipCurrentParag( QPainter &_painter );
  void gotoStartOfParag( QPainter &_painter );
  void cursorGotoLine( unsigned int _textpos, QPainter &_painter );
  /**
   * Assumes that '_textpos' is in the current line. It sets the cursor
   * to this position.
   */
  void cursorGotoPos( unsigned int _textpos, QPainter &_painter );
  /**
   * Move cursor to 'mx' and 'my' whis is given in pixels.
   */
  void cursorGotoPixelLine(unsigned int mx,unsigned int my,QPainter &_painter);
  void cursorGotoPixelInLine(unsigned int mx,unsigned int my,QPainter &_painter);
  KWCharAttribute* getObjectType(unsigned int mx,unsigned int my,QPainter &_painter);
  /**
   * Move the cursor to the next character very fast. Return -1 if the next character
   * is a special object like an image or such, returns 0 if there is a character
   * but a format change and returns 1 if there is a character without a format change.
   * Returns -2 if we are at the line end.
   *
   * This function does not leave the current line.
   */
  int cursorGotoNextChar( QPainter &_painter );
  void cursorGotoRight( QPainter &_painter );
  void cursorGotoRight( QPainter &_painter, int _pos );
  void cursorGotoLeft( QPainter &_painter );
  void cursorGotoUp( QPainter &_painter );
  void cursorGotoDown( QPainter &_painter );
  void cursorGotoLineStart( QPainter &_painter );
  /**
   * Sets the cursor BEHIND the last character of the current line
   * if the line is the last one of the current paragrph.
   * Otherwise the cursor is set ON the last character.
   * The only exception are empty lines or lines which contain a word
   * that is longer then an entire line. In these cases the cursor
   * is positioned BEHIN the last character, too.
   */
  void cursorGotoLineEnd( QPainter &_painter );
  void cursorGotoNextLine(QPainter &_painter);
  void cursorGotoPrevLine(QPainter &_painter);
  bool makeNextLineLayout( QPainter &_painter );
  bool makeLineLayout( QPainter &_painter, bool _checkIntersects = true, bool _checkTabs = true );
  void makeCounterLayout( QPainter &_painter );

  bool isCursorAtParagStart();
  bool isCursorAtLineStart();
  bool isCursorInFirstLine();
  bool isCursorAtParagEnd();
  bool isCursorAtLineEnd();
  /**
   * @return true if the cursor is currently positioned at the last character of
   *         a line. This is one character before the linedEndPos.
   */
  bool isCursorAtLastChar();
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
  unsigned int getFrameSet() { return frameSet; }
  unsigned int getFrame() { return frame; }

  /**
   * @return the text that represents the counter
   */
  QString getCounterText() { return counterText; }
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
  void setTextPos(unsigned int _pos) { textPos = _pos; }

  unsigned int getLineHeight();

  void apply( KWFormat &_format );

  void selectWord(KWFormatContext &_fc1,KWFormatContext &_fc2,QPainter &painter);

  void setFrameSet(unsigned int _frameSet) { frameSet = _frameSet; }

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
  /**
   * The document we are in right now.
   */
  KWordDocument *document;

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

};

#endif
