//========================================================================
//
// TextOutputDev.h
//
// Copyright 1997-2002 Glyph & Cog, LLC
//
//========================================================================

#ifndef TEXTOUTPUTDEV_H
#define TEXTOUTPUTDEV_H

#include <aconf.h>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

#include <stdio.h>
#include "gtypes.h"
#include "GfxFont.h"
#include "OutputDev.h"

class GfxState;
class GString;
class TextBlock;
class TextLine;

#undef TEXTOUT_DO_SYMBOLS

//------------------------------------------------------------------------

typedef void (*TextOutputFunc)(void *stream, char *text, int len);


//------------------------------------------------------------------------
// TextString
//------------------------------------------------------------------------
namespace PDFImport {
    class String;
    class Page;
}

class TextString {
public:

  // Constructor.
  TextString(GfxState *state, double x0, double y0,
	     double fontSize);


  // Destructor.
  virtual ~TextString();

  // Add a character to the string.
  virtual void addChar(GfxState *state, double x, double y,
                       double dx, double dy, Unicode u);

protected:
  double xMin, xMax;		// bounding box x coordinates
  double yMin, yMax;		// bounding box y coordinates
  union {
    GBool marked;		// temporary flag used by coalesce()
    GBool spaceAfter;		// insert a space after this string?
  };
  Unicode *text;		// the text
  double *xRight;		// right-hand x coord of each char
  int len;			// length of text and xRight
  int size;			// size of text and xRight arrays
  TextString *next;

  friend class TextPage;
  friend class TextBlock;
  friend class PDFImport::String;
  friend class PDFImport::Page;
};


//------------------------------------------------------------------------
// TextBlock
//------------------------------------------------------------------------

class TextBlock {
public:

  TextBlock();
  ~TextBlock();

  double xMin, xMax;
  double yMin, yMax;
  TextString *strings;		// list of strings in the block
  TextBlock *next;		// next block in line
  TextBlock *xyNext;		// next block on xyBlocks list
  Unicode *text;		// Unicode text of the block, including
				//   spaces between strings
  double *xRight;		// right-hand x coord of each char
  int len;			// total number of Unicode characters
  int convertedLen;		// total number of converted characters
  int *col;			// starting column number for each
				//   Unicode character
};

//------------------------------------------------------------------------
// TextLine
//------------------------------------------------------------------------

class TextLine {
public:

  TextLine();
  ~TextLine();

  TextBlock *blocks;
  TextLine *next;
  double yMin, yMax;
};

//------------------------------------------------------------------------
// TextPage
//------------------------------------------------------------------------

class TextPage {
public:

  // Constructor.
  TextPage(GBool rawOrderA);

  // Destructor.
  virtual ~TextPage();

  // Update the current font.
  void updateFont(GfxState *state);


  // Begin a new string.
  virtual void beginString(GfxState *state, double x0, double y0);

  // Add a character to the current string.
  void addChar(GfxState *state, double x, double y,
	       double dx, double dy, Unicode *u, int uLen);

  // End the current string, sorting it into the list of strings.
  virtual void endString();

  // Add a string, sorting it into the list of strings.
  virtual void addString(TextString *str);


  // Coalesce strings that look like parts of the same line.
  void coalesce();

  // Find a string.  If <top> is true, starts looking at top of page;
  // otherwise starts looking at <xMin>,<yMin>.  If <bottom> is true,
  // stops looking at bottom of page; otherwise stops looking at
  // <xMax>,<yMax>.  If found, sets the text bounding rectange and
  // returns true; otherwise returns false.
  GBool findText(Unicode *s, int len,
		 GBool top, GBool bottom,
		 double *xMin, double *yMin,
		 double *xMax, double *yMax);

  // Get the text which is inside the specified rectangle.
  GString *getText(double xMin, double yMin,
		   double xMax, double yMax);

  // Dump contents of page to a file.
  void dump(void *outputStream, TextOutputFunc outputFunc);

  // Clear the page.
  virtual void clear();

private:

  GBool xyBefore(TextString *str1, TextString *str2);
  GBool xyBefore(TextBlock *blk1, TextBlock *blk2);
  GBool yxBefore(TextBlock *blk1, TextBlock *blk2);
  double coalesceFit(TextString *str1, TextString *str2);

  GBool rawOrder;		// keep strings in content stream order

  TextString *curStr;		// currently active string
  double fontSize;		// current font size

  TextString *xyStrings;	// strings in x-major order (before
				//   they're sorted into lines)
  TextString *xyCur1, *xyCur2;	// cursors for xyStrings list
  TextLine *lines;		// list of lines

  int nest;			// current nesting level (for Type 3 fonts)

  int nTinyChars;		// number of "tiny" chars seen so far

  friend class PDFImport::Page;
};

//------------------------------------------------------------------------
// TextOutputDev
//------------------------------------------------------------------------

class TextOutputDev: public OutputDev {
public:

  // Open a text output file.  If <fileName> is NULL, no file is
  // written (this is useful, e.g., for searching text).  If
  // <rawOrder> is true, the text is kept in content stream order.
  TextOutputDev(char *fileName, GBool rawOrderA, GBool append);

  // Create a TextOutputDev which will write to a generic stream.  If
  // <rawOrder> is true, the text is kept in content stream order.
  TextOutputDev(TextOutputFunc func, void *stream, GBool rawOrderA);

  // Destructor.
  virtual ~TextOutputDev();

  // Check if file was successfully created.
  virtual GBool isOk() { return ok; }

  //---- get info about output device

  // Does this device use upside-down coordinates?
  // (Upside-down means (0,0) is the top left corner of the page.)
  virtual GBool upsideDown() { return gTrue; }

  // Does this device use drawChar() or drawString()?
  virtual GBool useDrawChar() { return gTrue; }

  // Does this device use beginType3Char/endType3Char?  Otherwise,
  // text in Type 3 fonts will be drawn with drawChar/drawString.
  virtual GBool interpretType3Chars() { return gFalse; }

  // Does this device need non-text content?
  virtual GBool needNonText() { return gFalse; }

  //----- initialization and control

  // Start a page.
  virtual void startPage(int pageNum, GfxState *state);

  // End a page.
  virtual void endPage();

  //----- update text state
  virtual void updateFont(GfxState *state);

  //----- text drawing
  virtual void beginString(GfxState *state, GString *s);
  virtual void endString(GfxState *state);
  virtual void drawChar(GfxState *state, double x, double y,
			double dx, double dy,
			double originX, double originY,
			CharCode c, Unicode *u, int uLen);

  //----- path painting

  //----- special access

  // Find a string.  If <top> is true, starts looking at top of page;
  // otherwise starts looking at <xMin>,<yMin>.  If <bottom> is true,
  // stops looking at bottom of page; otherwise stops looking at
  // <xMax>,<yMax>.  If found, sets the text bounding rectange and
  // returns true; otherwise returns false.
  GBool findText(Unicode *s, int len,
		 GBool top, GBool bottom,
		 double *xMin, double *yMin,
		 double *xMax, double *yMax);

  // Get the text which is inside the specified rectangle.
  GString *getText(double xMin, double yMin,
		   double xMax, double yMax);

private:

  TextOutputFunc outputFunc;	// output function
  void *outputStream;		// output stream
  GBool needClose;		// need to close the output file?
				//   (only if outputStream is a FILE*)
  TextPage *text;		// text for the current page
  GBool rawOrder;		// keep text in content stream order
  GBool ok;			// set up ok?

};

#endif
