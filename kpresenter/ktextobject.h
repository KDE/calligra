/******************************************************************/
/* KTextObject - (c) by Reginald Stadlbauer 1998                  */
/* Version: 0.0.4                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KTextObject is under GNU GPL                                   */
/******************************************************************/
/* Module: Text Object (header)                                   */
/******************************************************************/

#ifndef KTEXTOBJECT_H
#define KTEXTOBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <qkeycode.h>
#include <qtablevw.h>
#include <qcolor.h>
#include <qfont.h>
#include <qpainter.h>
#include <qwidget.h>
#include <qstring.h>
#include <qlist.h>
#include <krect.h>
#include <kpoint.h>
#include <qfontmet.h>
#include <qevent.h>
#include <qcursor.h>
#include <qpicture.h>
#include <qscrbar.h>
#include <qfile.h>
#include <qtstream.h>
#include <qregexp.h>
#include <qpixmap.h>
#include <qclipbrd.h>
#include <qpopmenu.h>

#include <kapp.h>

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

class TxtObj;
class TxtLine;
class TxtParagraph;
class KTextObject;

/******************************************************************/
/* class TxtCursor - Text Cursor                                  */
/******************************************************************/

/**
 * The TxtCusor class manages everything which has to do with the
 * position of the textcursor of the KTextObject.<br>
 * <b>You should <i>never</i> need to do anything with this class. It's the
 * best, if you only access methodes of the KTextObject.</b>
 * @short Class for managing a textcursor.
 * @author Reginald Stadlbauer <reggie@kde.org>
 * @version 0.0.4
 */

class TxtCursor
{
public:

  /**
   * The constructor needs a reference to "his" KTextObject.
   */
  TxtCursor(KTextObject*);
  TxtCursor();
  //~TxtCursor() {}

  /**
   * Get the current character (to the right of the caret).
   **/
  QChar character();

  /**
   * Move the cursor a char forward.
   */
  void charForward();

  /**
   * Move the cursor a char backwards.
   */
  void charBackward();

  /**
   * Move the cursor a line up.
   */
  void lineUp();

  /**
   * Move the cursor a line down.
   */
  void lineDown();

  /**
   * Move the cursor a word forward.
   */
  void wordForward();

  /**
   * Move the cursor a word backwards.
   */
  void wordBackward();

  /**
   * Move the cursor a line forward.
   */
  void lineForward();

  /**
   * Move the cursor a line backwards.
   */
  void lineBackward();

  /**
   * Move the cursor a paragraph forward.
   */
  void paragraphForward();

  /**
   * Move the cursor a paragraph backwards.
   */
  void paragraphBackward();

  /**
   * Move the cursor to the beginning.
   */
  void pos1();

  /**
   * Move the cursor to the end.
   */
  void end();

  /**
   * Set the absolute position of the cursor in the text (calculated in chars).
   */
  void setPositionAbs(unsigned int pos);

  /**
   * Set the position of the cursor in a paragraph (calculated in chars).
   */
  void setPositionParagraph(unsigned int paragraph,unsigned int pos);

  /**
   * Set the position of the cursor in a line in a paragraph (calculated in chars).
   */
  void setPositionParagraph(unsigned int paragraph,unsigned int line,unsigned int pos);

  /**
   * Set the absolute position of the cursor in a line (calculated in chars).
   */
  void setPositionLine(unsigned int line,unsigned int pos);

  /**
   * Returns the cursorposition relative to the whole text.
   */
  unsigned int positionAbs() {return absPos;}

  /**
   * Returns the paragraph, in which the cursor is.
   */
  unsigned int positionParagraph() {return paragraph;}

  /**
   * Returns the cursorposition relative to the paragraph.
   */
  unsigned int positionInParagraph() {return inParagraph;}

  /**
   * Returns the line in the paragraph, in which the the cursor is
   */
  unsigned int positionLine() {return line;}

  /**
   * Returns the cursorposition relative to the line in the paragraph
   */
  unsigned int positionInLine() {return inLine;}

  /**
   * Set the maximal position of the cursor.
   */
  void setMaxPosition(unsigned int m) {objMaxPos = m;}

  /**
   * Returns the maximal position of the cursor.
   */
  unsigned int maxPosition() {return objMaxPos;}

  /**
   * (Re)Calculate the cursor position. This is called, if something in the text changes.
   */
  void calcPos();

  /**
   * Returns the cursor, whos position is smaller
   */
  TxtCursor* minCursor(TxtCursor*);

  /**
   * Returns the cursor, whos position is bigger
   */
  TxtCursor* maxCursor(TxtCursor*);

  /**
   * Set the x-position of the cursor in pixels
   */
  void setXPos(int xpos) {_xpos = xpos;}

  /**
   * Returns the x-position of the cursor in pixels
   */
  int xpos() {return _xpos;}

  /**
   * Set the y-position of the cursor in pixels
   */
  void setYPos(int ypos) {_ypos = ypos;}

  /**
   * Returns the y-position of the cursor in pixels
   */
  int ypos() {return _ypos;}

  /**
   * Set the height of the cursor in pixels
   */
  void setHeight(unsigned int _height) {__height = _height;}

  /**
   * Returns the height of the cursor in pixels
   */
  unsigned int height() {return __height;}

  /**
   * Set the KTextObject.
   */
  void setKTextObject(KTextObject* _txtObj) {txtObj = _txtObj;}

  /**
   * copy a TxtCursor and return a reference to it.
   */
  TxtCursor& copy(TxtCursor&);

protected:

  //*********** variables ***********

  unsigned int absPos;

  unsigned int paragraph;
  unsigned int inParagraph;

  unsigned int line;
  unsigned int inLine;

  unsigned int objMaxPos;
  unsigned int _xpos,_ypos,__height;

  KTextObject *txtObj;

  TxtParagraph *paragraphPtr;
  TxtLine *linePtr;
  TxtObj *objPtr;
};

/******************************************************************/
/* class TxtObj - Text Object                                     */
/******************************************************************/

/**
 * The class TxtObj is a class for a single object in the KTextObject.<br>
 * <b>You should <i>never</i> need to do anything with this class. It's the
 * best, if you only access methodes of the KTextObject.</b>
 * @short Class for a single object in the KTextObject.
 * @author Reginald Stadlbauer <reggie@kde.org>
 * @version 0.0.4
 */

class TxtObj
{
public:

  /**
   * type of the textobject
   */
  enum ObjType {TEXT,SEPARATOR};

  /**
   * vertical alignment
   */
  enum VertAlign {SUBSCRIPT,NORMAL,SUPERSCRIPT};

  /**
   * Default constructor.
   */
  TxtObj();

  /**
   * Overloaded constructor. With this constructor you can init a textobject.
   */
  TxtObj(const QString & t,QFont f,QColor,VertAlign,ObjType ot=TEXT);
  ~TxtObj() {};

  /**
   * Set object type.
   */
  void setType(ObjType ot) {objType = ot;}

  /**
   * Returns object type.
   */
  ObjType type() {return objType;}

  /**
   * Set the font.
   */
  void setFont(QFont f) {objFont = f;}

  /**
   * Returns the font.
   */
  QFont font() {return objFont;}

  /**
   * Set the color.
   */
  void setColor(QColor c) {objColor = c;}

  /**
   * Returns the color.
   */
  QColor color() {return objColor;}

  /**
   * Set the vertical alignment.
   */
  void setVertAlign(VertAlign va) {objVertAlign = va;}

  /**
   * Returns the vertical alignment.
   */
  VertAlign vertAlign() {return objVertAlign;}

  /**
   * Set the original size (needed for zooming the KTextObject).
   */
  void setOrigSize(int s) {_origSize = s;}

  /**
   * Returns the original size.
   */
  int origSize() {return _origSize;}

  /**
   * Inserts the string <i>text</i> at the position <i>index</i>.
   */
  void insert(unsigned int index,const QString & text) {objText.insert(index,text);}

  /**
   * Inserts the char <i>c</i> at the position <i>index</i>.
   */
  void insert(unsigned int index,QChar c) {objText.insert(index,c);}

  /**
   * Appends the string <i>text</i>.
   */
  void append(const QString & text) {objText.append(text);}

  /**
   * Appends the char <i>c</i>.
   */
  void append(QChar c) {objText.insert(0,c);}

  /**
   * Delete the character at the position <i>i</i>.
   */
  void deleteChar(unsigned int i) {objText.remove(i,1);}

  /**
   * Returns the text of the object.
   */
  QString text() {return objText;}

  /**
   * Returns the length of the text.
   */
  unsigned int textLength() {return objText.length();}

  /**
   * Returns the width of the text.
   */
  unsigned int width();

  /**
   * Returns the height of the text.
   */
  unsigned int height();

  /**
   * Returns the ascent of the text.
   */
  unsigned int ascent();

  /**
   * Returns the descent of the text.
   */
  unsigned int descent();

  /**
   * Returns the position (in chars) at which pos (in pixels) is.
   */
  int getPos(int pos);

protected:

  //*********** variables ***********

  ObjType objType;
  QColor objColor;
  QFont objFont;
  VertAlign objVertAlign;
  int _origSize;

  QString objText;

};

/******************************************************************/
/* class TxtLine - Text Line                                      */
/******************************************************************/

/**
 * The class Txt Line offers everything for a text line. It consists of a list
 * of text objects.<br>
 * <b>You should <i>never</i> need to do anything with this class. It's the
 * best, if you only access methodes of the KTextObject.</b>
 * @short Class for a text line in the KTextObject.
 * @author Reginald Stadlbauer <reggie@kde.org>
 * @version 0.0.4
 */

class TxtLine
{
public:

  /**
   * Constructor. If init is <i>true</i>, an empty textobject is inserted.
   */
  TxtLine(bool init = false);
  ~TxtLine() {objList.clear();};

  /**
   * Insert a text (with attributes) at <i>pos</i>.
   */
  void insert(unsigned int pos,const QString &,QFont,QColor,TxtObj::VertAlign);

  /**
   * Insert a character (with attributes) at <i>pos</i>.
   */
  void insert(unsigned int pos,char,QFont,QColor,TxtObj::VertAlign);

  /**
   * Insert a text object at <i>i</i>.
   */
  void insert(unsigned int i,TxtObj *to)
    {objList.insert(i,to);}

  /**
   * Append a text (with attributes).
   */
  void append(const QString &t,QFont f,QColor c,TxtObj::VertAlign va)
    {objList.append(new TxtObj(t,f,c,va));}

  /**
   * Append a character (with attributes).
   */
  void append(char t,QFont f,QColor c,TxtObj::VertAlign va)
    {objList.append(new TxtObj((char*)t,f,c,va));}

  /**
   * Append a text (with attributes).
   */
  void append(const QString & t,QFont f,QColor c,TxtObj::VertAlign va,TxtObj::ObjType ot)
    {objList.append(new TxtObj(t,f,c,va,ot));}

  /**
   * Append a character (with attributes).
   */
  void append(char t,QFont f,QColor c,TxtObj::VertAlign va,TxtObj::ObjType ot)
    {objList.append(new TxtObj((char*)t,f,c,va,ot));}

  /**
   * Append a text object.
   */
  void append(TxtObj *to)
    {objList.append(to);}

  /**
   * Delete the character at <i>pos</i>.
   */
  void deleteChar(unsigned int pos);

  /**
   * Delete the first character in the text object <i>obj</i>.
   */
  void deleteFirstChar(unsigned int obj);

  /**
   * Backspace the character at <i>pos</i>.
   */
  void backspaceChar(unsigned int pos);

  /**
   * Delete the last character in the text object <i>obj</i>.
   */
  void backspaceLastChar(unsigned int obj);

  /**
   * Delete the text object at the position <i>pos</i>.
   */
  void deleteItem(unsigned int pos) {objList.remove(pos);}

  /**
   * Returns the text object at the position <i>i</i>.
   */
  TxtObj *itemAt(unsigned int i) {return objList.at(i);}

  /**
   * Returns number of text objects.
   */
  unsigned int items() {return objList.count();}

  /**
   * Returns the length of the line (in chars).
   */
  unsigned int lineLength();

  /**
   * Returns the width of the line.
   */
  unsigned int width();

  /**
   * Returns the height of the line.
   */
  unsigned int height(TxtParagraph *_parag);

  /**
   * Returns the maximal ascent of the line.
   */
  unsigned int ascent(TxtParagraph *_parag);

  /**
   * Returns the maximal descent of the line.
   */
  unsigned int descent(TxtParagraph *_parag);

  /**
   * Assigns a textline.
   */
  TxtLine &operator=(TxtLine *l);

  /**
   * Adds a textline
   */
  TxtLine &operator+=(TxtLine *l);

  /**
   * Clears the line.
   */
  void clear() {objList.clear();}

  /**
   * Split a text object in the line at <i>pos</i> in two objects.
   */
  void splitObj(unsigned int pos);

  /**
   * Returns the number of the text object, in which <i>pos</i> is. If <i>pos</i> is not in a text object,
   * return -1.
   */
  int getInObj(unsigned int pos,int *startpos = 0L);

  /**
   * Returns the number of the text object, before which <i>pos</i> is. If <i>pos</i> is not before a text object,
   * return -1.
   */
  int getBeforeObj(unsigned int pos,int *startpos = 0L);

  /**
   * Returns the number of the text object, after which <i>pos</i> is. If <i>pos</i> is not after a text object,
   * return -1.
   */
  int getAfterObj(unsigned int pos,int *startpos = 0L);

  /**
   * Returns the text of a line.
   */
  QString getText() {return getPartOfText(0,lineLength());}

  /**
   * Returns the text of a part of a line.
   */
  QString getPartOfText(int _from,int _to);

  /**
   * Returns number of words in that line.
   */
  unsigned int words();

  /**
   * Returns the word at position <i>pos</i>. <i>ind</i> gives the index of the word.
   */
  QString wordAt(unsigned int pos,int &ind);

protected:

  //*********** variables ***********

  QList<TxtObj> objList;

  TxtObj *objPtr;

};

/******************************************************************/
/* class TxtParagraph - Text Paragraph                            */
/******************************************************************/

/**
 * The class TxtParagraph offers everything for a paragraph in the KTextObject. It consists
 * of a list of text lines.<br>
 * <b>You should <i>never</i> need to do anything with this class. It's the
 * best, if you only access methodes of the KTextObject.</b>
 * @short Class for a text paragraph in the KTextObject.
 * @author Reginald Stadlbauer <reggie@kde.org>
 * @version 0.0.4
 */

class TxtParagraph
{
public:

  /**
   * structure for RegExpMode
   */
  struct RegExpMode
  {
    QRegExp regexp;
    QFont font;
    QColor color;
  };

  /**
   * Horizontal alignment of the paragraph.
   */
  enum HorzAlign {LEFT,CENTER,RIGHT,BLOCK};

  /**
   * Constructor. If init is <i>true</i> an empty text line is inserted.
   */
  TxtParagraph(bool init = false);
  ~TxtParagraph() {lineList.clear();};

  /**
   * Insert a string with attributes at a cursor position.
   */
  void insert(TxtCursor,const QString &,QFont,QColor,TxtObj::VertAlign);

  /**
   * Insert a character with attributes at a cursor position.
   */
  void insert(TxtCursor,char,QFont,QColor,TxtObj::VertAlign);

  /**
   * Append a string with attributes.
   */
  void append(const QString &,QFont,QColor,TxtObj::VertAlign);

  /**
   * Append a character with attributes.
   */
  void append(char,QFont,QColor,TxtObj::VertAlign);

  /**
   * Insert a text line at the position <i>i</i>.
   */
  void insert(unsigned int i,TxtLine *l);

  /**
   * Append a text line.
   */
  void append(TxtLine*);

  /**
   * Insert a text object at the position <i>i</i>.
   */
  void insert(unsigned int i,TxtObj*);

  /**
   * Append a text object.
   */
  void append(TxtObj*);

  /**
   * Returns the text line at the position <i>i</i>.
   */
  TxtLine *lineAt(unsigned int i) {return lineList.at(i);}

  /**
   * Returns the number of text lines in the paragraph.
   */
  unsigned int lines() {return lineList.count();}

  /**
   * Returns the length of the text of the paragraph (in chars).
   */
  unsigned int paragraphLength();

  /**
   * Returns the width of the paragraph.
   */
  unsigned int width();

  /**
   * Returns the height of the paragraph.
   */
  unsigned int height();

  /**
   * Set the horizontal alignment of the paragraph
   */
  void setHorzAlign(HorzAlign ha) {objHorzAlign = ha;}

  /**
   * Returns the horizontal alignment of the paragraph
   */
  HorzAlign horzAlign() {return objHorzAlign;}

  /**
   * Breaks lines in a certain width (pixels). Returns the needed rect.<br>
   */
  KRect breakLines(unsigned int,bool regexpMode=false,bool composerMode=false);

  /**
   * Breaks lines in a certain width (chars).<br>
   */
  void break_Lines(unsigned int,bool regexpMode=false,bool composerMode=false);

  /**
   * Concate all lines of the paragraph and return the reference to the resulting line.
   */
  TxtLine* toOneLine();

  /**
   * Used for composer mode of the KTextObject.
   */
  void doComposerMode(QColor,QFont,QColor,QFont);

  /**
   * Used for regexp mode of the KTextObject.
   */
  void doRegExpMode(QList<RegExpMode>*,bool revert=true);

  /**
   * Returns number of TxtObjs in the paragraph.
   */
  unsigned int items();

  /**
   * Returns number of words in that line.
   */
  unsigned int words();

  /**
   * Returns the TxtObj at the position <i>pos</i>
   */
  TxtObj* itemAt(unsigned int pos);

  /**
   * Returns the word at position <i>pos</i>. <i>ind</i> gives the index of the word.
   */
  QString wordAt(unsigned int pos,int &ind);

  /**
   * Delete a line.
   */
  void deleteLine(unsigned int pos) {lineList.remove(pos);}

  /**
   * Set regexp-List.
   */
  void setRegExpList(QList<RegExpMode> *l) {regExpList = l;}

  /**
   * Delete region between <i>_start</i> and <i>_stop</i>.
   */
  void deleteRegion(int _start,int _stop);

  /**
   * Returns <i>true</i>, if the paragraph is empty, else <i>false</i>.
   */
  bool isEmpty();

  /**
   * Returns the depth of this paragraph.
   */
  int getDepth() { return _depth; }

  /**
   * Set the depth of this paragraph
   */
  void setDepth(int d);

  /**
   * Returns the left indent.
   */
  int getLeftIndent() { return _leftIndent; }

  /**
   * Set the left indent of this paragraph
   */
  void setLeftIndent(int l)
    { _leftIndent = l; }

  /**
   * Set the line spacing
   */
  void setLineSpacing(int l) { lineSpacing = l; }

  /**
   * Set distance before the parag.
   */
  void setDistBefore(int d) { distBefore = d; }

  /**
   * Set distance after the parag.
   */
  void setDistAfter(int d) { distAfter = d; }

  /**
   * Returns line spacing.
   */
  int getLineSpacing() { return lineSpacing; }

  /**
   * Returns distance before the parag.
   */
  int getDistBefore() { return distBefore; }

  /**
   * Returns distance after the parag.
   */
  int getDistAfter() { return distAfter; }

  /**
   * Search a line and return the index.
   */
  int find(TxtLine *l) { return lineList.findRef(l); }

protected:

  unsigned int widthToNextSep(unsigned int);
  unsigned int charsToNextSep(unsigned int);

  //*********** variables ***********

  QList<TxtLine> lineList;

  HorzAlign objHorzAlign;

  TxtLine *linePtr;
  TxtLine *lin;
  TxtLine *line;
  TxtObj *obj;
  QList<RegExpMode> *regExpList;
  int _depth,_leftIndent;
  int lineSpacing,distBefore,distAfter;

};

/******************************************************************/
/* class KTextObject - KTextObject                                */
/******************************************************************/

/**
 * The KTextObject is a widget for editing rich text. That means you can
 * edit text with different colors, fonts, sizes, etc. This widget was mainly
 * designed for editing a short text, but the speed should be ok for a few pages
 * too.<br>
 * If you need the text drawn transparent, you can get it using the function @ref #getPic.
 * This function returns a QPicture, which you can draw transparent.<br>
 * To set or get the current font, I'm using the signal/slot mechanism of Qt. To see, how
 * this works for the KTextObject, look at the demo-application<br>
 * To get the text of the KTextObject use @ref #toASCII or @ref #toHTML. To write it into a file
 * use @ref #saveASCII or @ref #saveHTML.<br>
 * To add a text use @ref #addText, to load an ASCII file use @ref #openASCII, and to "parse"
 * and set a HTML text, use @ref #parseHTML.<br>
 * You can use two types of linebreaking. Either dynamically linebreaking, which means that the
 * width of the text is always <= the with of the widget. You can also maximal number of
 * chars, which sould be displayed in a line. To set the linebreaking type use @ref #setLineBreak.
 * Default is dynamically linebreaking.
 * @short A widget for editing rich text.
 * @author Reginald Stadlbauer <reggie@kde.org>
 * @version 0.0.4
 */

class KTextObject : public QTableView
{
  Q_OBJECT

public:

  /**
   * structur for type of unsorted lists
   */
  struct UnsortListType
  {
    QList<QFont> *font;
    QList<QColor> *color;
    QList<QChar> *chr;
    QList<QFont> *ofont;
  };

  /**
   * structure for enum list
   */
  static const int NUMBER = 1;
  static const int ALPHABETH = 2;

  struct EnumListType
  {
    int type;
    QString before;
    QString after;
    int start;
    QFont font;
    QColor color;
    QFont ofont;
  };

  /**
   * structure for cell width/height
   */
  struct CellWidthHeight
  {
    unsigned int wh;
  };

  /**
   * structure for automatical replacing characters
   */
  struct AutoReplace
  {
    char c;
    QString replace;
  };

  /**
   * Type of the KTextObject. Default is PLAIN.
   */
  enum ObjType {PLAIN,ENUM_LIST,UNSORT_LIST,TABLE};

  /**
   * Constructor of the KTextObject.<br>
   * <i>QWidget *parent:</i> Parent widget<br>
   * <i>const char *name:</i> Name<br>
   * <i>@ref #ObjType ot:</i> Object Type<br>
   * <i>unsigned int c</i> and <i>unsigned int r</i> are not needed at the moment. They will be useful
   * in the future for a new feature.<br>
   * <i>_width</i>: Linebreak width. (< 1 means dynamically linebreak, >= 1 means max. _width chars in a line)<br>
   */
  KTextObject(QWidget *parent=0,const char *name=0,ObjType ot=PLAIN,unsigned int c=0,unsigned int r=0,int __width=0);
  ~KTextObject();

  /**
   * Set the object type of the KTextObject.
   */
  void setObjType(ObjType);

  /**
   * Returns the object type of the KTextObject.
   */
  ObjType objType() {return obType;}

  /**
   * Set the type for enumerated lists.<br>
   * The structure for the enumerated list types looks like that:
   * <pre>const int NUMBER = 1;
   * const int ALPHABETH = 2;
   *
   * struct EnumListType
   * {
   *  &nbsp;&nbsp;int type; // NUMBER or ALPHABETH
   *  &nbsp;&nbsp;QString before; // string before the counter
   *  &nbsp;&nbsp;QString after; // string after the counter
   *  &nbsp;&nbsp;int start; // start of the counter
   *  &nbsp;&nbsp;QFont font;
   *  &nbsp;&nbsp;QColor color;
   *  &nbsp;&nbsp;QFont ofont;
   * };
   * </pre>
   */
  void setEnumListType(EnumListType t) {objEnumListType = t; repaint(true);}

  /**
   * Returns the type of enumerated lists.<br>
   * The structure for the enumerated list types looks like that:
   * <pre>const int NUMBER = 1;
   * const int ALPHABETH = 2;
   *
   * struct EnumListType
   * {
   *  &nbsp;&nbsp;int type; // NUMBER or ALPHABETH
   *  &nbsp;&nbsp;QString before; // string before the counter
   *  &nbsp;&nbsp;QString after; // string after the counter
   *  &nbsp;&nbsp;int start; // start of the counter
   *  &nbsp;&nbsp;QFont font;
   *  &nbsp;&nbsp;QColor color;
   *  &nbsp;&nbsp;QFont ofont;
   * };
   * </pre>
   */
  EnumListType enumListType() {return objEnumListType;}

  /**
   * Set the type of unsorted lists.<br>
   * The structure for the enumerated list types looks like that:
   * <pre>struct UnsortListType
   * {
   *  &nbsp;&nbsp;QFont font;
   *  &nbsp;&nbsp;QColor color;
   *  &nbsp;&nbsp;int chr;
   *  &nbsp;&nbsp;QFont ofont;
   * };
   * </pre>
   */
  void setUnsortListType(UnsortListType t) {objUnsortListType = t; repaint(true);}

  /**
   * Returns the type of unsorted lists.<br>
   * <pre>struct UnsortListType
   * {
   *  &nbsp;&nbsp;QFont font;
   *  &nbsp;&nbsp;QColor color;
   *  &nbsp;&nbsp;int chr;
   *  &nbsp;&nbsp;QFont ofont;
   * };
   * </pre>
   */
  UnsortListType unsortListType() {return objUnsortListType;}

  /**
   * This function does nothing at the moment
   */
  void setRow(unsigned int r) {objRow = r;}

  /**
   * This function does nothing at the moment
   */
  unsigned int row() {return objRow;}

  /**
   * This function does nothing at the moment
   */
  void setCol(unsigned int c) {objCol = c;}

  /**
   * This function does nothing at the moment
   */
  unsigned int col() {return objCol;}

  /**
   * Show textcursor if s is <i>true</i>, else hide it.
   */
  void setShowCursor(bool s) {sCursor = s;}

  /**
   * Returns <i>true</i> if the textcursor is shown, els it returns <i>false</i>.
   */
  bool showCursor() {return sCursor;}

  /**
   * Returns the length of the whole text.
   */
  unsigned int textLength();

  /**
   * Returns the reference of a the paragraph number i. See @ref #TxtParagraph for more information.
   */
  TxtParagraph *paragraphAt(unsigned int i) {return paragraphList.at(i);}

  /**
   * Returns the number of paragraphs, which ar in this text.
   */
  unsigned int paragraphs() {return paragraphList.count();}

  /**
   * Set the current font.
   */
  void setFont(QFont f) {currFont = f; if (drawSelection) changeAttribs();}

  /**
   * Returns the current font.
   */
  QFont font() {return currFont;}

  /**
   * Set the current color.
   */
  void setColor(QColor c) {currColor = c; if (drawSelection) changeAttribs();}

  /**
   * Returns the current color.
   */
  QColor color() {return currColor;}

  /**
   * Set the horizontal alignment of the current paragraph. The current paragraph is the
   * paragraph, in which the cursor is. See @ref #TxtParagraph for more information.
   */
  void setHorzAlign(TxtParagraph::HorzAlign ha,int p = -1)
    {changeHorzAlign(ha,p);};

  /**
   * Returns the horizontal align of the paragraph <i>p</i>.
   */
  TxtParagraph::HorzAlign horzAlign(int p = -1);

  /**
   * Show a vertical and horizontal scrollbar.
   */
  void showScrollbar() {setTableFlags(Tbl_vScrollBar|Tbl_hScrollBar);}

  /**
   * Returns a QPicture of the text to draw it transparent.<br>
   * The four integers give the size of the QPicture<br>
   * If presMode is <i>true</i>, you can say that only the paragraphs between from and to are drawn.
   * If <i>_clip</i> is true, the clipping rect is set to <i>_x, _y, _w, _h</i>, else no clipping rect is set.
   */
  QPicture* getPic(int _x,int _y,int _w,int _h,bool presMode=false,int from=-1,int to=-1,bool _clip=true);
  void draw(QPainter &p,int _x,int _y,int _w,int _h,bool presMode=false,int from=-1,int to=-1,bool _clip=true,bool _drawempty=true);

  /**
   * Resize the KTextObject.
   */
  void resize(int w,int h)
    {QTableView::resize(w,h); recalc();}

  /**
   * Resize the KTextObject.
   */
  void resize(KSize s)
    {resize(s.width(),s.height());}

  /**
   * Zoom the text by the faktor which is given as the float argument.
   */
  void zoom(float _fakt);

  /**
   * Zoom the text back to it's original size.
   */
  void zoomOrig();

  /**
   * Add an empty paragraph and get the reference to it.
   */
  TxtParagraph* addParagraph();

  /**
   * Clear the KTextObject. If <i>init</i> is <i>true</i> a paragraph with one line is appended,
   * else nothing is appended.
   */
  void clear(bool init=true);

  /**
   * Returns the text of the KTextObject as ASCII-Text in a string.
   * If linebreak is <i>true</i> for each linebreak a \n is used. If <i>makelist</i> is true, lists
   * are converted to ASCII as well.
   */
  QString toASCII(bool linebreak=true,bool makelist=true);

  /**
   * For compatibility to QMultiLineEdit. It just calls @ref toASCII.
   */
  QString text(bool linebreak=true) {return toASCII(linebreak);}

  /**
   * Returns the text of the KTextObject as HTML-Text in a string.
   * If <i>clean</i> in <i>true</i> the fontsizes are calculated correctly as HTML, else
   * the real fontsize is used. If you only want to save and load the text in the KTextObject,
   * <i>clean</i> should be <i>false</i>, if you want to use the output for a HTML-Page, <i>clean</i> should
   * be <i>true</i>. If <i>onlyBody</i> is <i>true</i>, only the stuff which is between the body-begin
   * and body-end flag is returned, else a whole, valide HTML-document is returned.
   */
  QString toHTML(bool clean=false,bool onlyBody=false);

  /**
   * Saves the text of the KTextObject as ASCII-Text to the file
   * <i>filename</i>.<br>
   * If linebreak is <i>true</i> for each linebreak a \n is used.
   */
  void saveASCII(QString filename,bool linebreak=true);

  /**
   * Saves the text of the KTextObject in HTML-Text to the file
   * <i>filename</i>.<br>
   * If clean in <i>true</i> the fontsizes are calculated correctly as HTML, else
   * the real fontsize is used. If you only want to save and load the text in the KTextObject,
   * clean should be <i>false</i>, if you want to use the output for a HTML-Page, clean should
   * be <i>true</i>.
   */
  void saveHTML(QString filename,bool clean=false);

  /**
   * Adds the <i>text</i> in the given <i>color</i> and <i>font</i> to the KTextObject.
   * If <i>newParagraph</i> is <i>true</i> the text is added into a new paragraph, and with
   * <i>align</i> you can give the horizontal alignment of the paragraphs, which will be
   * added (paragraphs are seperated by '\n'). If <i>_recalc</i> is <i>true</i>, everything is
   * recalculated and redrawn, else no redraw/recalc is done. If <i>htmlMode</i> is true, nbsp's are
   * coverted to spaces.
   */
  void addText(QString text,QFont font,QColor color,
	       bool newParagraph=false,TxtParagraph::HorzAlign align=TxtParagraph::LEFT,
	       bool _recalc=true,bool htmlMode=false);

  /**
   * Parses a HTML text and sets it into the KTextObject.
   */
  void parseHTML(QString text);

  /**
   * Opens an ASCII file for editing in the KTextObject.
   */
  void openASCII(QString filename);

  /**
   * Opens an HTML file for editing in the KTextObject.
   */
  void openHTML(QString filename);

  /**
   * Set the type of the linebreaking. A <i>_width</i> < 1 means dynamically linebreaking.
   * If <i>_width</i> is >= 1, this argument sets the maximal number of chars of a line.
   */
  void setLineBreak(int _width);

  /**
   * With this function you can switch on a composer mode. If the composer-mode is
   * on, all lines, which begin with a char out of '>:|' will be displayed in <i>quoted_color</i>
   * and <i>quoted_font</i>. The rest is displayed in <i>normal_color</i> and <i>normal_font</i>.
   */
  void enableComposerMode(QColor quoted_color,QFont quoted_font,QColor normal_color,QFont normal_font);

  /**
   * With this function you can switch off the composer mode.
   */
  void disableComposerMode() {composerMode = false;}

  /**
   * Enable RegExpMode. In this mode you can give a list of @ref #RegExpMode, which is used for
   * syntax highliting. That means, if a part of the text string matches to a regexp of the list, this part
   * is drawn in the attributes of this list item.<br>
   * <b>IMPORTANT</b>: The first item of the list has to store the attributes of the default text. So the
   * regular expression of the first list-item is ignored!<br>
   * The structure TxtParagraph::RegExpMode looks like that:
   * <pre>struct RegExpMode
   * {
   * &nbsp;&nbsp;QRegExp regexp;
   * &nbsp;&nbsp;QFont font;
   * &nbsp;&nbsp;QColor color;
   * };
   * </pre>
   */
  void enableRegExpMode(QList<TxtParagraph::RegExpMode>&);

  /**
   * Disable RegExpMode.
   */
  void disableRegExpMode() {regexpMode = false;}

  /**
   * Set a replacement list. Here you can give a list of characters, which should be automatically converted to another
   * string, if the char is entered.<br>
   * The type AutoReplace looks like that:
   * <pre>struct AutoReplace
   * {
   * &nbsp;&nbsp;char c;
   * &nbsp;&nbsp;QString replace;
   * };
   * </pre>
   */
  void setAutoReplacement(QList<AutoReplace>&);

  /**
   * Returns a part of the text.
   */
  QString getPartOfText(TxtCursor*,TxtCursor*);

  /**
   * Copy selection to clipboard.
   */
  void copyRegion(bool hideSelection=false);

  /**
   * Copy selection to clipboard and delete selection (=cut).
   */
  void cutRegion();

  /**
   * Paste text form clipboard.
   */
  void paste();

  /**
   * Returns the number of TxtObjs in the KTextObject. <b>NOTE:</b> A TxtObj isn't a word. A
   * Word may consist of many TxtObs. To get the number of words, call @ref words.
   */
  unsigned int items();

  /**
   * Returns the number of TxtObjs in the line <i>line</i>.
   */
  unsigned int itemsInLine(int line);

  /**
   * Returns the number of TxtObjs in the line <i>line</i> in the paragraph <i>para</i>.
   */
  unsigned int itemsInLine(int line,int para);

  /**
   * Returns the number of TxtObjs in the paragraph <i>para</i>.
   */
  unsigned int itemsInParagraph(int para);

  /**
   * Returns the number of words in the KTextObject.
   */
  unsigned int words();

  /**
   * Returns the number of words in the line <i>line</i>.
   */
  unsigned int wordsInLine(int line);

  /**
   * Returns the number of words in the line <i>line</i> in the paragraph <i>para</i>.
   */
  unsigned int wordsInLine(int line,int para);

  /**
   * Returns the number of words in the paragraph <i>para</i>.
   */
  unsigned int wordsInParagraph(int para);

  /**
   * Returns the number of lines.
   */
  unsigned int lines();

  /**
   * Returns the number of lines in the paragraph <i>para</i>.
   */
  unsigned int linesInParagraph(int para);

  /**
   * Returns a pointer to the TxtObj at the position <i>pos</i> in the KTextObject.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the TxtObj number <i>pos</i>.
   */
  TxtObj* itemAt(int pos);

  /**
   * Returns a pointer to the TxtObj at the position <i>pos</i> in the line <i>line</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the TxtObj number <i>pos</i>.
   */
  TxtObj* itemAtLine(int pos,int line);

  /**
   * Returns a pointer to the TxtObj at the position <i>pos</i> in the paragraph <i>para</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the TxtObj number <i>pos</i>.
   */
  TxtObj* itemAtPara(int pos,int para);

  /**
   * Returns a pointer to the TxtObj at the position <i>pos</i> in the line <i>line</i><br>
   * in the paragraph <i>para</i>.
   * <i>pos</i> is <b>not</b> given in chars, it's the TxtObj number <i>pos</i>.
   */
  TxtObj* itemAt(int pos,int line,int para);


  /**
   * Returns the text of the word at the position <i>pos</i> in the KTextObject.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>. <i>ind</i> gives the index of the word.
   */
  QString wordAt(int pos,int &ind);

  /**
   * Returns the text of the word at the position <i>pos</i> in the line <i>line</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>. <i>ind</i> gives the index of the word.
   */
  QString wordAtLine(int pos,int line,int &ind);

  /**
   * Returns the text of the word at the position <i>pos</i> in the paragraph <i>para</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>. <i>ind</i> gives the index of the word.
   */
  QString wordAtPara(int pos,int para,int &ind);

  /**
   * Returns the text of the word at the position <i>pos</i> in the line <i>line</i>
   * in the paragraph <i>para</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>n. <i>ind</i> gives the index of the word.
   */
  QString wordAt(int pos,int line,int para,int &ind);

  /**
   * Returns a pointer to the line <i>line</i>.
   */
  TxtLine* lineAt(int line);

  /**
   * Returns a pointer to the line <i>line</i> in the paragraph <i>para</i>.
   */
  TxtLine* lineAt(int line,int para);

  /**
   * Returns the region between <i>_startCursor</i> and <i>_stopCursor</i>. The region
   * is packed into a list of TxtObjs.
   */
  QList<TxtObj>* regionAt(TxtCursor *_startCursor,TxtCursor *_stopCursor);

  /**
   * Delete the TxtObj at position <i>pos</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>n.
   */
  void deleteItem(int pos);

  /**
   * Delete the TxtObj in the line <i>line</i> at position <i>pos</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>n.
   */
  void deleteItemInLine(int pos,int line);

  /**
   * Delete the TxtObj in the paragraph <i>para</i> at position <i>pos</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>n.
   */
  void deleteItemInPara(int pos,int para);

  /**
   * Delete the TxtObj in the paragraph <i>para</i> in the line <i>line</i> at position <i>pos</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>n.
   */
  void deleteItem(int pos,int line,int para);

  /**
   * Delete the word at position <i>pos</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>n.
   */
  void deleteWord(int pos);

  /**
   * Delete the word in the line <i>line</i> at position <i>pos</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>n.
   */
  void deleteWordInLine(int pos,int line);

  /**
   * Delete the word in the paragraph <i>para</i> at position <i>pos</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>n.
   */
  void deleteWordInPara(int pos,int para);

  /**
   * Delete the word in the paragraph <i>para</i> in the line <i>line</i> at position <i>pos</i>.<br>
   * <i>pos</i> is <b>not</b> given in chars, it's the word number <i>pos</i>n.
   */
  void deleteWord(int pos,int line,int para);

  /**
   * Delete the line <i>line</i>.
   */
  void deleteLine(int line);

  /**
   * Delete the line <i>line</i> in the paragraph <i>para</i>.
   */
  void deleteLine(int line,int para);

  /**
   * Delete the paragraph <i>para</i>.
   */
  void deleteParagraph(int para,bool _update = true);

  /**
   * Delete the region between <i>_startCursor</i> and <i>_stopCursor</i>.
   */
  void deleteRegion(TxtCursor *_startCursor,TxtCursor *_stopCursor);

  /**
   * Insert the text <i>text</i> at the position <i>pos</i> (given in words).
   * The text is written in the font <i>font</i> and the color <i>color</i>.
   */
  void insertText(QString text,int pos,QFont font,QColor color);

  /**
   * Insert the text <i>text</i> in the line <i>line</i> at the position <i>pos</i> (given in words).
   * The text is written in the font <i>font</i> and the color <i>color</i>.
   */
  void insertTextInLine(QString text,int pos,int line,QFont font,QColor color);

  /**
   * Insert the text <i>text</i> in the paragraph <i>para</i> at the position <i>pos</i> (given in words).
   * The text is written in the font <i>font</i> and the color <i>color</i>.
   */
  void insertTextInPara(QString text,int pos,int para,QFont font,QColor color);

  /**
   * Insert the text <i>text</i> in the line <i>line</i> in the paragraph <i>para</i> at the position <i>pos</i> (given in words).
   * The text is written in the font <i>font</i> and the color <i>color</i>.
   */
  void insertText(QString text,int pos,int line,int para,QFont font,QColor color);

  /**
   * Insert the text <i>text</i> at the cursor position <i>_cursor</i>. The text is written in the font <i>font</i>
   * and the color <i>color</i>.
   */
  void insertText(QString text,TxtCursor *_cursor,QFont font,QColor color);

  /**
   * Insert items <i>items</i> at the position <i>pos</i> (given in items).
   */
  void insertItems(QList<TxtObj> *items,int pos);

  /**
   * Insert items <i>items</i> at the position <i>pos</i> (given in items) in the line <i>line</i>.
   */
  void insertItemsInLine(QList<TxtObj> *items,int pos,int line);

  /**
   * Insert items <i>items</i> at the position <i>pos</i> (given in items) in the paragraph <i>para</i>.
   */
  void insertItemsInPara(QList<TxtObj> *items,int pos,int para);

  /**
   * Insert items <i>items</i> at the position <i>pos</i> (given in items) in the line <i>line</i> in the
   * paragraph <i>para</i>.
   */
  void insertItems(QList<TxtObj> *items,int pos,int line,int para);

  /**
   * Insert items <i>items</i> at the cursor position <i>_cursor</i>.
   */
  void insertItems(QList<TxtObj> *items,TxtCursor *_cursor,bool redraw=true);

  /**
   * Replace <i>len</i> items beginning at position <i>pos</i> (given in items) with <i>items</i>.
   */
  void replaceItems(QList<TxtObj> *items,int pos,int len);

  /**
   * Replace <i>len</i> items beginning at position <i>pos</i> (given in items) in the line <i>line</i> with <i>items</i>.
   */
  void replaceItemsInLine(QList<TxtObj> *items,int pos,int line,int len);

  /**
   * Replace <i>len</i> items beginning at position <i>pos</i> (given in items) in the paragraph <i>para</i> with <i>items</i>.
   */
  void replaceItemsInPara(QList<TxtObj> *items,int pos,int para,int len);

  /**
   * Replace <i>len</i> items beginning at position <i>pos</i> (given in items) int the line <i>line</i>
   * in the paragraph <i>para</i> with <i>items</i>.
   */
  void replaceItems(QList<TxtObj> *items,int pos,int line,int para,int len);

  /**
   * Replace the word at position <i>pos</i> with the text <i>text</i>.
   * The text is written in the font <i>font</i> and the color <i>color</i>.
   */
  void replaceWord(QString text,int pos,QFont font,QColor color);

  /**
   * Replace the word at position <i>pos</i> in the line <i>line</i> with the text <i>text</i>.
   * The text is written in the font <i>font</i> and the color <i>color</i>.
   */
  void replaceWordInLine(QString text,int pos,int line,QFont font,QColor color);

  /**
   * Replace the word at position <i>pos</i> in the paragraph <i>para</i> with the text <i>text</i>.
   * The text is written in the font <i>font</i> and the color <i>color</i>.
   */
  void replaceWordInPara(QString text,int pos,int para,QFont font,QColor color);

  /**
   * Replace the word at position <i>pos</i> in the line <i>line</i> in the paragraph <i>para</i> with the text <i>text</i>.
   * The text is written in the font <i>font</i> and the color <i>color</i>.
   */
  void replaceWord(QString text,int pos,int line,int para,QFont font,QColor color);

  /**
   * Replace region between the cursors <i>_startCursor</i> and <i>_stopCursor</i> with the
   * items <i>items</i>.
   */
  void replaceRegion(QList<TxtObj> *items,TxtCursor *_startCursor,TxtCursor *_stopCursor);

  /**
   * Replace region between the cursors <i>_startCursor</i> and <i>_stopCursor</i> with the
   * text <i>text</i>. The text is written with the font <i>font</i> and the color <i>color</i>.
   */
  void replaceRegion(QString text,TxtCursor *_startCursor,TxtCursor *_stopCursor,QFont font,QColor color);

  /**
   * Change the attributes of the text between <i>_startCursor</i> and <i>_stopCursor</i> to <i>font</i> and <i>color</i>.
   */
  void changeRegionAttribs(TxtCursor *_startCursor,TxtCursor *_stopCursor,QFont font,QColor color);

  /**
   * Change the alignment of the paragraphs between <i>_startCursor</i> and <i>_stopCursor</i> to <i>align</i>.
   */
  void changeRegionAlign(TxtCursor *_startCursor,TxtCursor *_stopCursor,TxtParagraph::HorzAlign _align);

  /**
   * Returns absolute position (<i>x1</i> and <i>x2</i>) of the word at position <i>pos</i> in chars.
   */
  void getAbsPosOfWord(int pos,int &x1,int &x2);

  /**
   * Returns absolute position (<i>x1</i> and <i>x2</i>) of the word at position <i>pos</i> in the line <i>line</i> in chars.
   */
  void getAbsPosOfWordInLine(int pos,int line,int &x1,int &x2);

  /**
   * Returns absolute position (<i>x1</i> and <i>x2</i>) of the word at position <i>pos</i> in the paragraph <i>para</i> in chars.
   */
  void getAbsPosOfWordInPara(int pos,int para,int &x1,int &x2);

  /**
   * Returns absolute position (<i>x1</i> and <i>x2</i>) of the word at position <i>pos</i> in the line <i>line</i>
   * in the paragraph <i>para</i> in chars.
   */
  void getAbsPosOfWord(int pos,int line,int para,int &x1,int &x2);

  /**
   * Returns the line, in which <i>pos</i> is.
   */
  void getLine(int &pos,int &line);

  /**
   * Returns the line in the paragraph <i>para</i>, in which <i>pos</i> is.
   */
  void getLine(int &pos,int para,int &line);

  /**
   * Returns the paragraph, in which <i>line</i> is.
   */
  void getPara(int &line,int &para);

  /**
   * Returns the paragraph and the line in this paragraph, in which <i>pos</i> is.
   */
  void getPara(int &pos,int &line,int &para);

  /**
   * Returns text cursor.
   */
  TxtCursor* getCursor() {return txtCursor;}

  /**
   * Toggles modified status.
   */
  void toggleModified(bool m) {_modified = m;}

  /**
   * Returns modified status.
   */
  bool isModified() {return _modified;}

  /**
   * Assign <i>txtObj</i> to <i>this</i> and return a reference to <i>this</i>.
   */
  KTextObject& operator=(KTextObject &txtObj);

  /**
   * Returns the linebreak width.
   */
  int getLineBreakWidth() {return linebreak_width;}

  /**
   * Returns parent widget.
   */
  QWidget* getParent() {return _parent;}

  /**
   * Return reference to the text cursor.
   */
  TxtCursor *getTxtCursor() {return txtCursor;}

  /**
   * Start finding a string at the cursor position and goes forwards. If the string was found, this string gets selected.<br>
   * Returns <i>true</i> if the string was found, else it returns <i>false</i>. If <i>true</i> is
   * returned, <i>from</i> to <i>to</i> points to the position, where the string was found. Else <i>from</i> and <i>to</i> are
   * undefined.<br>
   * <i>caseSensitive</i> sets, if the search is case sensitive or not.
   */
  bool searchFirst(QString,TxtCursor *from,TxtCursor *to,bool caseSensitive);

  /**
   * Continue finding a string forwards. If the string was found, this string gets selected.<br>
   * Returns <i>true</i> if the string was found, else it returns <i>false</i>. If <i>true</i> is
   * returned, <i>from</i> to <i>to</i> points to the position, where the string was found. Else <i>from</i> and <i>to</i> are
   * undefined.<br>
   * <i>caseSensitive</i> sets, if the search is case sensitive or not.
   */
  bool searchNext(QString,TxtCursor *from,TxtCursor *to,bool caseSensitive);

  /**
   * Start finding a string at the cursor position and goes backwards. If the string was found, this string gets selected.<br>
   * Returns <i>true</i> if the string was found, else it returns <i>false</i>. If <i>true</i> is
   * returned, <i>from</i> to <i>to</i> points to the position, where the string was found. Else <i>from</i> and <i>to</i> are
   * undefined.<br>
   * <i>caseSensitive</i> sets, if the search is case sensitive or not.
   */
  bool searchFirstRev(QString,TxtCursor *from,TxtCursor *to,bool caseSensitive);

  /**
   * Continue finding a string backwards. If the string was found, this string gets selected.<br>
   * Returns <i>true</i> if the string was found, else it returns <i>false</i>. If <i>true</i> is
   * returned, <i>from</i> to <i>to</i> points to the position, where the string was found. Else <i>from</i> and <i>to</i> are
   * undefined.<br>
   * <i>caseSensitive</i> sets, if the search is case sensitive or not.
   */
  bool searchNextRev(QString,TxtCursor *from,TxtCursor *to,bool caseSensitive);

  /**
   * Set search index to te beginning.
   */
  void setSearchIndexToBegin()
    { searchIndexFrom.setPositionAbs(0); searchIndexTo.setPositionAbs(0); }

  /**
   * Set search index to the end.
   */
  void setSearchIndexToEnd()
    { searchIndexFrom.setPositionAbs(textLength()); searchIndexTo.setPositionAbs(textLength()); }

  /**
   * Start finding a string <i>search</i> and replacing it with <i>replace</i> at the current cursor position and goes forwards.
   * If the string was replaced, the replaced string is selected.<br>
   * Returns <i>true</i> if the string was found and replaced, else it returns <i>false</i>. If <i>true</i> is
   * returned, <i>from</i> to <i>to</i> points to the position, where the string was replaced. Else <i>from</i> and <i>to</i> are
   * undefined.<br>
   * <i>caseSensitive</i> sets, if the search/replace is case sensitive or not.
   */
  bool replaceFirst(QString search,QString replace,TxtCursor *from,TxtCursor *to,bool caseSensitive);

  /**
   * Continue finding a string <i>search</i> and replacing it with <i>replace</i> at the current cursor position and going forwards.
   * If the string was replaced, the replaced string is selected.<br>
   * Returns <i>true</i> if the string was found and replaced, else it returns <i>false</i>. If <i>true</i> is
   * returned, <i>from</i> to <i>to</i> points to the position, where the string was replaced. Else <i>from</i> and <i>to</i> are
   * undefined.<br>
   * <i>caseSensitive</i> sets, if the search/replace is case sensitive or not.
   */
  bool replaceNext(QString search,QString replace,TxtCursor *from,TxtCursor *to,bool caseSensitive);

  /**
   * Start finding a string <i>search</i> and replacing it with <i>replace</i> at the current cursor position and goes backwards.
   * If the string was replaced, the replaced string is selected.<br>
   * Returns <i>true</i> if the string was found and replaced, else it returns <i>false</i>. If <i>true</i> is
   * returned, <i>from</i> to <i>to</i> points to the position, where the string was replaced. Else <i>from</i> and <i>to</i> are
   * undefined.<br>
   * <i>caseSensitive</i> sets, if the search/replace is case sensitive or not.
   */
  bool replaceFirstRev(QString search,QString replace,TxtCursor *from,TxtCursor *to,bool caseSensitive);

  /**
   * Continue finding a string <i>search</i> and replacing it with <i>replace</i> at the current cursor position and going backwards.
   * If the string was replaced, the replaced string is selected.<br>
   * Returns <i>true</i> if the string was found and replaced, else it returns <i>false</i>. If <i>true</i> is
   * returned, <i>from</i> to <i>to</i> points to the position, where the string was replaced. Else <i>from</i> and <i>to</i> are
   * undefined.<br>
   * <i>caseSensitive</i> sets, if the search/replace is case sensitive or not.
   */
  bool replaceNextRev(QString search,QString replace,TxtCursor *from,TxtCursor *to,bool caseSensitive);

  /**
   * With this function you can set, that the whole text should be written in one color.
   */
  void enableDrawAllInOneColor(QColor c,bool _repaint=false)
    { allColor = c; allInOneColor = true; if (_repaint) repaint(false); }

  /**
   * Disable drawing the whole text in one color. Now the objectspecific colors are used again.
   */
  void disableDrawAllInOneColor(bool _repaint=false)
    { allInOneColor = false; if (_repaint) repaint(false); }

  /**
   * Enable or disable autofocus. Autofocus means, that the KTextObject automatically gets the focus,
   * if the mouse is over it.
   */
  void setAutoFocus(bool _auto)
    { autoFocus = _auto; }

  /**
   * Sets the font for the whole text.
   */
  void setFontToAll(QFont _font);

  /**
   * Sets the color for the whole text.
   */
  void setColorToAll(QColor _color);

  /**
   * Sets the horizontal alignment for the whole text.
   */
  void setHorzAlignToAll(TxtParagraph::HorzAlign _align);

  /**
   * Set the depth for the current paragraph.
   */
  void setDepth(int d)
    { paragraphList.at(txtCursor->positionParagraph())->setDepth(d); recalc(); repaint(true); }

  /**
   * Increase the depth for the current paragraph.
   */
  void incDepth() {
    paragraphList.at(txtCursor->positionParagraph())->setDepth(paragraphList.at(txtCursor->positionParagraph())->getDepth() + 1 < 16 ?
							       paragraphList.at(txtCursor->positionParagraph())->getDepth() + 1 : 15);
    recalc(); repaint(true);
  }

  /**
   * Decrease the depth for the current paragraph.
   */
  void decDepth() {
    paragraphList.at(txtCursor->positionParagraph())->setDepth(paragraphList.at(txtCursor->positionParagraph())->getDepth() - 1 >= 0 ?
							       paragraphList.at(txtCursor->positionParagraph())->getDepth() - 1 : 0);
    recalc(); repaint(true);
  }

  /**
   * Returns line spacing of the current paragraph.
   */
  int getLineSpacing();

  /**
   * Returns distance before the current paragraph.
   */
  int getDistBefore();

  /**
   * Returns distance after the current paragraph.
   */
  int getDistAfter();

  /**
   * Set line spacing for the current paragraph.
   */
  void setLineSpacing(int l);

  /**
   * Set distance before the current paragraph.
   */
  void setDistBefore(int d);

  /**
   * Set distance after the current paragraph.
   */
  void setDistAfter(int d);

  /**
   * Set line spacing for all paragraphs.
   */
  void setAllLineSpacing(int l);

  /**
   * Set distance before all paragraphs.
   */
  void setAllDistBefore(int d);

  /**
   * Set distance after all paragraphs.
   */
  void setAllDistAfter(int d);

  void extendContents2Height();
  
signals:

  /**
   * If the current font is changed, this signal is sent.
   */
  void fontChanged(QFont*);

  /**
   * If the current color is changed, this signal is sent.
   */
  void colorChanged(QColor*);

  /**
   * If the current horizontal alignment is changed, this signal is sent.
   */
  void horzAlignChanged(TxtParagraph::HorzAlign);

  void giveMeFocus();

protected:

  //**************** types *****************
  enum TagType {HTML,HEAD,BODY,FONT,BOLD,ITALIC,UNDERLINE,
		PARAGRAPH,BREAK,UNSORTLIST,ENUMLIST,PLAIN_TEXT,UNKNOWN_TAG};
  enum TagState {BEGIN,END,COMMENT};
  enum AttribType {FACE,CHAR,START,BEFORE,AFTER,TYPE,SIZE,COLOR,ALIGN,B,I,U,UNKNOWN_ATTRIB};
  enum Operators {PLUS,MINUS,ASSIGN};
  enum ParseState {KEY,OPERATOR,VALUE};

  struct Attrib
  {
    AttribType key;
    Operators op;
    QString value;
  };

  typedef QList<Attrib> AttribList;

  struct ParsedTag
  {
    TagType type;
    TagState state;
    AttribList attribs;
    QString additional;
  };

  enum CursorDirection {C_LEFT,C_RIGHT,C_UP,C_DOWN};

  //***************** methodes ****************

  void paintCell(class QPainter*,int,int);
  void paintEvent(QPaintEvent *);
  void focusInEvent(QFocusEvent*);
  void focusOutEvent(QFocusEvent*);
  void leaveEvent(QEvent*);
  void enterEvent(QEvent*);

  int cellWidth(int);
  int cellHeight(int);

  int totalWidth();
  int totalHeight();

  void resizeEvent(QResizeEvent*);

  void keyPressEvent(QKeyEvent*);

  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);

  void recalc(bool breakAllLines=true);

  void splitParagraph();
  void joinParagraphs(unsigned int,unsigned int);

  bool kbackspace();
  bool kdelete(bool _recalc=true);

  bool insertChar(QChar);

  bool sameEffects(TxtObj *to)
    {return (currFont == to->font() && currColor == to->color());}

  bool makeCursorVisible();

  TxtCursor getCursorPos(int,int,bool&,bool set=false,bool redraw=false);

  QString toHexString(QColor);

  ParsedTag parseTag(QString);

  QColor hexStringToQColor(QString);

  bool isValid(QString);

  QString simplify(QString);

  bool selectionInObj(int,int,int);
  bool selectFull(int,int,int,int&,int&);
  void redrawSelection(TxtCursor,TxtCursor);

  void changeAttribs();
  void changeHorzAlign(TxtParagraph::HorzAlign,int);
  void _setHorzAlign(TxtParagraph::HorzAlign,int);

  void createRBMenu();

  void selectText(TxtCursor*,CursorDirection);

  int getParagNum(TxtParagraph *_parag);
  int getLeftIndent();
  int getLeftIndent(int _parag);

protected slots:
  void clipCut() {cutRegion();}
  void clipCopy() {copyRegion(true);}
  void clipPaste() {paste();}

protected:
  //*********** variables ***********

  TxtCursor *txtCursor;
  TxtCursor searchIndexFrom,searchIndexTo;
  bool sCursor;
  bool drawSelection;
  bool autoFocus;

  ObjType obType;

  EnumListType objEnumListType;
  UnsortListType objUnsortListType;

  QList<TxtParagraph> paragraphList;

  TxtParagraph *paragraphPtr;

  QList<CellWidthHeight> cellWidths;
  QList<CellWidthHeight> cellHeights;

  CellWidthHeight *cwhPtr;

  unsigned int objRow,objCol;

  unsigned int xstart;
  unsigned int ystart;

  int drawLine;
  int drawParagraph;
  bool drawBelow;
  bool cursorChanged;
  bool mousePressed;
  bool drawPic;

  QFont currFont;
  QColor currColor;

  TxtParagraph *para1;
  TxtParagraph *para2;
  TxtParagraph *para3;
  TxtLine *lin;
  TxtObj *obj;
  CellWidthHeight *wh;
  TxtParagraph *paragraphPtr2;
  TxtLine *linePtr;
  TxtObj *objPtr;
  QPicture pic;
  TxtCursor startCursor,stopCursor;
  QColor selectionColor;
  bool doRepaints;

  QList<int> changedParagraphs;

  int linebreak_width;
  int _width;

  bool composerMode;
  QColor _quoted_color,_normal_color;
  QFont _quoted_font,_normal_font;

  bool _modified;

  QPopupMenu *rbMenu;
  QWidget *_parent;

  QList<TxtParagraph::RegExpMode> regExpList;
  bool regexpMode;

  QList<AutoReplace> autoReplace;

  QColor allColor;
  bool allInOneColor;

  int CB_CUT,CB_COPY,CB_PASTE;

  //**************** constants ******************
  // HTML stuff
  static const char open_tag = '<';
  static const char close_tag = '>';
  static const char end_tag = '/';
  static const char comment_tag = '!';

  static const char operator_assign = '=';
  static const char operator_plus = '+';
  static const char operator_minus = '-';
  static const char space = ' ';

  static const char tag_html[] = "HTML";
  static const char tag_head[] = "HEAD";
  static const char tag_body[] = "BODY";
  static const char tag_font[] = "FONT";
  static const char tag_bold[] = "B";
  static const char tag_italic[] = "I";
  static const char tag_underline[] = "U";
  static const char tag_paragraph[] = "P";
  static const char tag_break[] = "BR";
  static const char tag_h1[] = "H1";
  static const char tag_h2[] = "H2";
  static const char tag_h3[] = "H3";
  static const char tag_h4[] = "H4";
  static const char tag_h5[] = "H5";
  static const char tag_h6[] = "H6";
  static const char tag_plain[] = "PLAIN";
  static const char tag_enumlist[] = "ENUM_LIST";
  static const char tag_unsortlist[] = "UNSORT_LIST";

  static const char attrib_face[] = "FACE";
  static const char attrib_char[] = "CHAR";
  static const char attrib_start[] = "START";
  static const char attrib_before[] = "BEFORE";
  static const char attrib_after[] = "AFTER";
  static const char attrib_type[] = "TYPE";
  static const char attrib_size[] = "SIZE";
  static const char attrib_color[] = "COLOR";
  static const char attrib_align[] = "ALIGN";
  static const char attrib_bold[] = "BOLD";
  static const char attrib_italic[] = "ITALIC";
  static const char attrib_underline[] = "UNDERLINE";

  static const char attrib_value_left[] = "LEFT";
  static const char attrib_value_center[] = "CENTER";
  static const char attrib_value_right[] = "RIGHT";

};
#endif //KTEXTOBJECT_H



