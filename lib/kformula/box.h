/* This file is part of the KDE libraries
    Copyright (C) 1999 Ilya Baran (ibaran@mit.edu)

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

#ifndef BOX_H_INCLUDED
#define BOX_H_INCLUDED

#include <qpainter.h>
#include <qstring.h>
#include <qlist.h>
#include <qfont.h>
#include <kdebug.h> //will be included in practically all source files

#define DEFAULT_FONT_SIZE 25
#define MIN_FONT_SIZE  10

//pixels for spacing:
#define SPACE          (4)

//symbols have values above this
#define SYMBOL_ABOVE   20000

//references have values above this and below SYMBOL_ABOVE
#define MAX_REFERENCES 1000 //maximum references per formula
#define REFERENCE_ABOVE (SYMBOL_ABOVE - MAX_REFERENCES)
inline bool IS_REFERENCE(int r) { return (r >= REFERENCE_ABOVE && r < SYMBOL_ABOVE); }
inline int REFERENCE_NUM(int r) { return (r < MAX_REFERENCES ? r + REFERENCE_ABOVE : /*error*/ 0); }

//can be set to 0 if you want to print the string out somewhere.
//UNUSED_OFFSET is only used in this file.
#define UNUSED_OFFSET  1000

//temporary workaround--otherwise it warns since
//there are two possible QChar constructors:
//#define QChar(__x) QChar((int)(__x))
// Commented out by David. This makes it impossible to call QChar()
// (the constructor with no argument).

struct charinfo;

//each BoxType must be equal to the character that represents it in
//the string.  The parser depends on that.

typedef int SymbolType;
typedef int BoxType;

/**
 * @internal
 */
namespace Box {

    const BoxType PLUS = '+';
    const BoxType MINUS = '-';
    const BoxType TIMES = '*';
    const BoxType DIVIDE = '\\' + UNUSED_OFFSET;
    const BoxType POWER  = '^' + UNUSED_OFFSET; //just a test to see if it works
    const BoxType SQRT = '@' + UNUSED_OFFSET;
    const BoxType TEXT = 't';
    const BoxType CAT = '#' + UNUSED_OFFSET;
    const BoxType SUB = '_' + UNUSED_OFFSET;
    const BoxType LSUP = '6' + UNUSED_OFFSET;
    const BoxType LSUB = '%' + UNUSED_OFFSET;
    const BoxType PAREN = '(';
    const BoxType EQUAL = '=';
    const BoxType MORE = '>';
    const BoxType LESS = '<';
    const BoxType ABS = '|';
    const BoxType BRACKET = '[';
    const BoxType SLASH = '/';
    const BoxType MATRIX = 'm' + UNUSED_OFFSET;
    const BoxType SEPARATOR = '&' + UNUSED_OFFSET; // separator for matrices
    const BoxType ABOVE = ')' + UNUSED_OFFSET; //something useless
    const BoxType BELOW = ']' + UNUSED_OFFSET;
    const BoxType SYMBOL = 's' + UNUSED_OFFSET; // whatever
    // char for keeping track of cursor position in undo/redo:
    const BoxType CURSOR = 'c' + UNUSED_OFFSET;
};

namespace Box {
    const SymbolType INTEGRAL = SYMBOL_ABOVE + 0; // symbols have values above that
    const SymbolType SUM      = SYMBOL_ABOVE + 1;
    const SymbolType PRODUCT  = SYMBOL_ABOVE + 2;
    const SymbolType ARROW    = SYMBOL_ABOVE + 3;
    // elements of the symbol font are their own codes + SYMBOL_ABOVE
};

//a box is a unit of a formula.
//it has up to two children, and routines
//for arranging and displaying them.
/**
 * The box class is used for internal calculations.
 *
 * After a formula is parsed, it is converted to a
 * tree of boxes which can then be "calculated" (arranged)
 * and drawn.  You probably don't want to use this class
 * in your own programs.  If you do, see the header file.
 *
 * @short Formula layout helper.
 *
 * @internal
 */
class box {
friend class KFormula;
friend class matrixbox;

protected:
  BoxType type; //the box type
  QString text; //if a TEXT box, the text
  QFont lastFont; //the last font used
  QRect rect; //its bounding rectangle
  int relx;
  int rely; //used internally for storing calculated locations
  int offsx;
  int offsy; //its offset from its parent
  int fontsize; //its base fontsize
  box *b1;
  box *b2; //the children
  box *parent;  //duh
  QList<box> refParents; //boxes referring to this one
  int b1x;
  int b1y;
  int b2x;
  int b2y; //the offsets of the children
  int dirty; //number of days since last shower.  No--whether it's been
             //changed since calculate was last called on it.

  void makeDirty();
  void offset(int xoffs, int yoffs);

  static QRect symbolRect(QPainter &p, SymbolType s, int size);
  static void drawSymbol(QPainter &p, SymbolType s, int size, int x, int y);

public:
  box();
  box(int setNum);
  box(QString setText);
  box(int setType, box * setB1 = NULL, box * setB2 = NULL);

  virtual ~box();

  void setText(QString newText);
  virtual void calculate(QPainter &p, int setFontsize = -1,
			 QFont *f = 0, QColor *bc = 0, QColor *fc = 0 );
  QRect getRect() { return rect; }
  int getType() { return type; }
  box *getParent() { return parent; }
  QString getText() { return text; }
  virtual void draw(QPainter &p, int x, int y, QFont *f = 0, QColor *bc = 0, QColor *fc = 0);

  virtual QRect getCursorPos(charinfo i, int x, int y);
  QRect getLastRect();
};


#endif // BOX_H_INCLUDED










