/******************************************************************/
/* KTextObject - (c) by Reginald Stadlbauer 1998                  */
/* Version: 0.0.1                                                 */
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

#include <qkeycode.h>
#include <qtablevw.h>
#include <qcolor.h>
#include <qfont.h>
#include <qpainter.h>
#include <qwidget.h>
#include <qstring.h>
#include <qlist.h>
#include <qrect.h>
#include <qpoint.h>
#include <qfontmet.h>
#include <qevent.h>
#include <qcursor.h>
#include <qpicture.h>
#include <qscrbar.h>

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
class TxtCursor
{
public:

  // constructor - destructor 
  TxtCursor(KTextObject*);
  ~TxtCursor() {};

  // move the cursor
  void charForward();
  void charBackward();
  void lineUp();
  void lineDown();
  void wordForward();
  void wordBackward();
  void lineForward();
  void lineBackward();
  void paragraphForward();
  void paragraphBackward();
  void pos1();
  void end();

  // set the cursor before a certain character in the text 
  void setPositionAbs(unsigned int);

  // set the corser before a certain character in a certain paragraph
  void setPositionParagraph(unsigned int,unsigned int);

  // set the corser before a certain character in a certain paragraph in a certain line
  void setPositionLine(unsigned int,unsigned int,unsigned int);

  // get the cursorposition relative to the whole text
  unsigned int positionAbs() {return absPos;}
  
  // get the paragraph, in which the cursor is
  unsigned int positionParagraph() {return paragraph;}

  // get the cursorposition relative to the paragraph
  unsigned int positionInParagraph() {return inParagraph;}

  // get the line in the paragraph, in which the the cursor is
  unsigned int positionLine() {return line;}

  // get the cursorposition relative to the line in the paragraph
  unsigned int positionInLine() {return inLine;} 

  // set and get maximal position
  void setMaxPosition(unsigned int m) {objMaxPos = m;}
  unsigned int maxPosition() {return objMaxPos;}
  
  // calculate the position of the cursor
  void calcPos();

  // get minimum/maximum of two cursors
  TxtCursor* minCursor(TxtCursor*);
  TxtCursor* maxCursor(TxtCursor*);

protected:

  //*********** variables ***********

  // absolute position of the cursor
  unsigned int absPos;
  
  // paragraph, in which the cursor is, and the position in ths paragraph
  unsigned int paragraph;
  unsigned int inParagraph;

  // line of the paragraph, in which the cursor is, and position in the line 
  unsigned int line;
  unsigned int inLine;

  // maximal position
  unsigned int objMaxPos;

  // pointer to the textobject, in which this cursor is used
  KTextObject *txtObj;

  // internal
  TxtParagraph *paragraphPtr;
  TxtLine *linePtr;
  TxtObj *objPtr;
};

/******************************************************************/
/* class TxtObj - Text Object                                     */
/******************************************************************/
class TxtObj
{
public:

  // type of the textobject
  enum ObjType {TEXT,SEPARATOR};
  
  // vertical alignment
  enum VertAlign {SUBSCRIPT,NORMAL,SUPERSCRIPT};

  // constructor - destructor
  TxtObj();
  TxtObj(const char* t,QFont f,QColor,VertAlign,ObjType ot=TEXT);
  ~TxtObj() {};

  // set and get type, font, color and vertical alignment of the object
  void setType(ObjType ot) {objType = ot;}
  ObjType type() {return objType;}
  void setFont(QFont f) {objFont = f;}
  QFont font() {return objFont;}
  void setColor(QColor c) {objColor = c;}
  QColor color() {return objColor;}
  void setVertAlign(VertAlign va) {objVertAlign = va;}
  VertAlign vertAlign() {return objVertAlign;}

  // insert/append a char or string into the object
  void insert(unsigned int index,const char* text) {objText.insert(index,qstrdup(text));}
  void insert(unsigned int index,char c) {objText.insert(index,qstrdup(&c));}
  void append(const char* text) {objText.append(qstrdup(text));}
  void append(char c) {objText.append(qstrdup(&c));}

  // delete c char
  void deleteChar(unsigned int i) {objText.remove(i,1);}

  // get text and length of text
  QString text() {return objText;}
  unsigned int textLength() {return objText.length();}

  // returns width, height, ascent and descent of the object
  unsigned int width();
  unsigned int height();
  unsigned int ascent();
  unsigned int descent();

  // get position in obj
  int getPos(unsigned int); 

protected:

  //*********** variables ***********

  // type, font, color and vertical alignment of the object
  ObjType objType;
  QColor objColor;
  QFont objFont;
  VertAlign objVertAlign;

  // text of the object
  QString objText;

};

/******************************************************************/
/* class TxtLine - Text Line                                      */
/******************************************************************/
class TxtLine
{
public:

  // constructor - destructor
  TxtLine(bool init = false);
  ~TxtLine() {};

  // insert/append a char/string/TxtObj into the line
  void insert(unsigned int,const char*,QFont,QColor,TxtObj::VertAlign);
  void insert(unsigned int,char,QFont,QColor,TxtObj::VertAlign);
  void insert(unsigned int i,TxtObj& to)
    {objList.insert(i,new TxtObj(to));}
  void append(const char* t,QFont f,QColor c,TxtObj::VertAlign va)
    {objList.append(new TxtObj(t,f,c,va));}
  void append(char t,QFont f,QColor c,TxtObj::VertAlign va)
    {objList.append(new TxtObj((char*)t,f,c,va));}
  void append(const char* t,QFont f,QColor c,TxtObj::VertAlign va,TxtObj::ObjType ot)
    {objList.append(new TxtObj(t,f,c,va,ot));}
  void append(char t,QFont f,QColor c,TxtObj::VertAlign va,TxtObj::ObjType ot)
    {objList.append(new TxtObj((char*)t,f,c,va,ot));}
  void append(TxtObj &to)
    {objList.append(new TxtObj(to));}

  // delete a char or region in the line
  void deleteChar(unsigned int);
  void deleteFirstChar(unsigned int);
  void backspaceChar(unsigned int);
  void backspaceLastChar(unsigned int);
  void deleteRegion(unsigned int,unsigned int);
  void deleteItem(unsigned int i) {objList.remove(i);}

  // change font, color and vertical alignment of a region 
  void changeRegion(unsigned int,unsigned int,QFont,QColor,TxtObj::VertAlign);

  // get the object at a certain position
  TxtObj *itemAt(unsigned int i) {return objList.at(i);}

  // returns number of items
  unsigned int items() {return objList.count();}

  // returns length of the line (chars)
  unsigned int lineLength();

  // returns width, height, ascent and descent of the line
  unsigned int width();
  unsigned int height();
  unsigned int ascent();
  unsigned int descent();

  // assignment functions
  TxtLine &operator=(TxtLine *l);
  TxtLine &operator+=(TxtLine *l);

  // clear
  void clear() {objList.clear();}

  // split an object in the line
  void splitObj(unsigned int);

  // get position in/before/after object
  int getInObj(unsigned int);
  int getBeforeObj(unsigned int);
  int getAfterObj(unsigned int);

protected:

  //*********** variables ***********

  // list of objects of the line
  QList<TxtObj> objList;

  // pointer to a textobject
  TxtObj *objPtr;

};

/******************************************************************/
/* class TxtParagraph - Text Paragraph                            */
/******************************************************************/
class TxtParagraph 
{
public:

  // horizontal alignment of the paragraph
  enum HorzAlign {LEFT,CENTER,RIGHT,BLOCK};
  
  // constructor - destructor
  TxtParagraph(bool init = false);
  ~TxtParagraph() {};

  // insert/append a char or string into the paragraph
  void insert(TxtCursor,const char*,QFont,QColor,TxtObj::VertAlign);
  void insert(TxtCursor,char,QFont,QColor,TxtObj::VertAlign);
  void append(const char*,QFont,QColor,TxtObj::VertAlign);
  void append(char,QFont,QColor,TxtObj::VertAlign);

  // insert/append a line
  void insert(unsigned int i,TxtLine *l);
  void append(TxtLine*);

  // insert/append an object
  void insert(unsigned int i,TxtObj*);
  void append(TxtObj*);

  // delete a char or a region in the paragraph
  void deleteChar(TxtCursor);
  void deleteRegion(TxtCursor,TxtCursor);

  // change font, color and vertical alignment of a region 
  void changeRegion(TxtCursor,TxtCursor,QFont,QColor,TxtObj::VertAlign);

  // get certain line 
  TxtLine *lineAt(unsigned int i) {return lineList.at(i);}

  // returns number of lines
  unsigned int lines() {return lineList.count();}

  // return textlength of the paragraph
  unsigned int paragraphLength();

  // returns width and height of the paragraph
  unsigned int width();
  unsigned int height();

  // set and get horizontal alignment of the paragraph
  void setHorzAlign(HorzAlign ha) {objHorzAlign = ha;}
  HorzAlign horzAlign() {return objHorzAlign;}

  // break lines in a certain width, returns needed needed rect
  QRect breakLines(unsigned int);

  // concate all lines and return the pointer to the resulting line
  TxtLine* toOneLine();

protected:

  // get the width from an object to the next separator
  unsigned int widthToNextSep(unsigned int);
  
  //*********** variables ***********

  // list of lines
  QList<TxtLine> lineList;

  // horizontal alignment
  HorzAlign objHorzAlign;

  // pointer to a line
  TxtLine *linePtr;
  TxtLine *lin;
  TxtLine *line;
  TxtObj *obj;
};

/******************************************************************/
/* class KTextObject - KTextObject                                */
/******************************************************************/
class KTextObject : public QTableView
{
  Q_OBJECT

public:

  // structur for type of unsorted lists
  struct UnsortListType
  {
    QFont font;
    QColor color;
    int chr;
  };

  // structure for enum list
  const int NUMBER = 1;
  const int ALPHABETH = 2;

  struct EnumListType
  {
    int type;
    QString before;
    QString after;
    int start;
    QFont font;
    QColor color;
  };

  // structure for cell width/height
  struct CellWidthHeight
  {
    unsigned int wh;
  };

  // type of the textobject
  enum ObjType {PLAIN,ENUM_LIST,UNSORT_LIST,TABLE};
 
  // constructor - destructor
  KTextObject(QWidget *parent=0,const char *name=0,ObjType ot=PLAIN,unsigned int c=0,unsigned int r=0);
  ~KTextObject() {};                                            

  // set and get object type
  void setObjType(ObjType);
  ObjType objType() {return obType;}

  // set and get the type of an enumerated list
  void setEnumListType(EnumListType t) {objEnumListType = t; repaint(true);}
  EnumListType enumListType() {return objEnumListType;}

  // set and get font, char and color of an unsorted list
  void setUnsortListType(UnsortListType t) {objUnsortListType = t; repaint(true);}
  UnsortListType unsortListType() {return objUnsortListType;}

  // set and get row/column
  void setRow(unsigned int r) {objRow = r;}
  unsigned int row() {return objRow;}
  void setCol(unsigned int c) {objCol = c;}
  unsigned int col() {return objCol;}

  // show or hide cursor
  void setShowCursor(bool s) {sCursor = s;}
  bool showCursor() {return sCursor;}

  // get length of the text
  unsigned int textLength();

  // get a certain paragraph
  TxtParagraph *paragraphAt(unsigned int i) {return paragraphList.at(i);}

  // get number of paragraphs
  unsigned int paragraphs() {return paragraphList.count();}

  // set and get current font/color
  void setFont(QFont f) {currFont = f;}
  QFont font() {return currFont;}
  void setColor(QColor c) {currColor = c;}
  QColor color() {return currColor;}

  // set and get horizontal alignment
  void setHorzAlign(TxtParagraph::HorzAlign,int p = -1);
  TxtParagraph::HorzAlign horzAlign(int p = -1);

  // show scrollbar
  void showScrollbar() {setTableFlags(Tbl_vScrollBar);}

  // get QPicture of the object
  QPicture* getPic(int,int,int,int);

  // resize
  void resize(int w,int h)
    {QTableView::resize(w,h); recalc();}
  void resize(QSize s)
    {resize(s.width(),s.height());}

  // zoom text
  void zoom(float);

  // add paragraph
  TxtParagraph* addParagraph();

  // clear
  void clear();

signals:

  // sent current effects
  void fontChanged(QFont*);
  void colorChanged(QColor*);
  void horzAlignChanged(TxtParagraph::HorzAlign);

protected:

  // paint the contents of a cell
  void paintCell(class QPainter*,int,int);
  void paintEvent(QPaintEvent *);
  
  // return width and height of a certain cell
  int cellWidth(int);
  int cellHeight(int);

  // return total width and height of the table
  int totalWidth();
  int totalHeight();
  
  // resize event
  void resizeEvent(QResizeEvent*);

  // key press event
  void keyPressEvent(QKeyEvent*);

  // mouse events
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);

  // recalcualate everything
  void recalc();

  // split and join paragraphs
  void splitParagraph();
  void joinParagraphs(unsigned int,unsigned int);

  // backspace/delete
  void kbackspace();
  void kdelete();
  
  // insert charcter
  void insertChar(char);

  // check, if effects are equal
  bool sameEffects(TxtObj *to)
    { return (currFont.operator==(to->font()) && currColor.operator==(to->color())); }

  void makeCursorVisible();
  
  TxtCursor* getCursorPos(int,int,bool set=false);

  //*********** variables ***********

  // text cursor and flag of it
  TxtCursor *txtCursor;
  bool sCursor;
  bool drawSelection;

  // object Type
  ObjType obType;

  // list types
  EnumListType objEnumListType;
  UnsortListType objUnsortListType;

  // list of paragraphs
  QList<TxtParagraph> paragraphList;

  // pointer to a paragraph
  TxtParagraph *paragraphPtr;

  // list of the cell-widths and cell-heights
  QList<CellWidthHeight> cellWidths;
  QList<CellWidthHeight> cellHeights;

  // pointer to cell width/height
  CellWidthHeight *cwhPtr;

  // row and col
  unsigned int objRow,objCol;

  // x position, where drawing should be started
  unsigned int xstart;
  unsigned int ystart;
  
  // line/paragraph which should be drawn, -1 means: draw all lines 
  int drawLine;
  int drawParagraph;
  bool drawBelow;
  bool cursorChanged;
  bool mousePressed;
  bool drawPic;

  // current font an color
  QFont currFont;
  QColor currColor;

  // internal
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
  TxtCursor *startCursor,*stopCursor;
  QColor selectionColor;

};
#endif //KTEXTOBJECT_H



