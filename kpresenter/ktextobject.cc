/******************************************************************/
/* KTextObject - (c) by Reginald Stadlbauer 1998                  */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KTextObject is under GNU GPL                                   */
/******************************************************************/
/* Module: Main Widget                                            */
/******************************************************************/

#include "ktextobject.h"
#include "ktextobject.moc"

/******************************************************************/
/* class TxtCursor - Text Cursor                                  */
/******************************************************************/

/*======================= constructor ============================*/
TxtCursor::TxtCursor(KTextObject *to)
{
  // init the object
  absPos = 0;
  paragraph = 0;
  inParagraph = 0;
  line = 0;
  inLine = 0;
  objMaxPos = 0;
  txtObj = to;
}

/*==================== cursor char forward =======================*/
void TxtCursor::charForward()
{
  if (absPos < objMaxPos-1)
    {
      absPos++;
      calcPos();
    }
}

/*=================== cursor char backward =======================*/
void TxtCursor::charBackward()
{
  if (absPos > 0)
    {
      absPos--;
      calcPos();
    }
}

/*====================== cursor line up ==========================*/
void TxtCursor::lineUp()
{
  if (absPos > 0 && txtObj)
    {
      // if the cursor is in the first line -> do nothing
      if (line == 0 && paragraph == 0) return;

      // else: if the upper line is in the upper paragraph and the line is at least as long as the current one
      // -> jump exactly one line up
      else if (line == 0 && paragraph != 0 &&
	       txtObj->paragraphAt(paragraph-1)->lineAt(txtObj->paragraphAt(paragraph-1)->lines()-1)->lineLength() >= inLine)
	absPos -= txtObj->paragraphAt(paragraph-1)->lineAt(txtObj->paragraphAt(paragraph-1)->lines()-1)->lineLength(); 

      // else: if the upper line is in the same paragraph and the line is at least as long as the current one
      // -> jump exactly one line up
      else if (line != 0 &&
	       txtObj->paragraphAt(paragraph)->lineAt(line-1)->lineLength() >= inLine)
	absPos -= txtObj->paragraphAt(paragraph)->lineAt(line-1)->lineLength(); 

      // else: if the upper line is shorter as the current one, jump to the end of the upper line
      // (here it's irrelevant, if the upper line is in the same paragraph, or not)
      else
	absPos -= inLine+1;

      // calculate the new position
      calcPos();
    }
}

/*==================== cursor line down ==========================*/
void TxtCursor::lineDown()
{
 if (absPos < objMaxPos-1 && txtObj)
    {
      
      // if the cursor is in the last line -> do nothing
      if (paragraph == txtObj->paragraphs()-1 && 
	  line == txtObj->paragraphAt(paragraph)->lines()-1)
	return;

      // else: if the below line is in the below paragraph and the line is at least as long as the current one
      // -> jump exactly one line down
      else if (paragraph != txtObj->paragraphs()-1 &&
	       line == txtObj->paragraphAt(paragraph)->lines()-1 &&
	       txtObj->paragraphAt(paragraph+1)->lineAt(0)->lineLength() >= inLine)
	absPos += txtObj->paragraphAt(paragraph)->lineAt(line)->lineLength(); 

      // else: if the below line is in the same paragraph and the line is at least as long as the current one
      // -> jump exactly one line down
      else if (line != txtObj->paragraphAt(paragraph)->lines()-1 &&
	       txtObj->paragraphAt(paragraph)->lineAt(line+1)->lineLength() >= inLine)
	absPos += txtObj->paragraphAt(paragraph)->lineAt(line)->lineLength(); 

      // else: if the below line is shorter than the current one, and the below line is one paragraph below
      // -> jump to the end of the below line
      else if (paragraph != txtObj->paragraphs()-1 &&
	       line == txtObj->paragraphAt(paragraph)->lines()-1)
	absPos += txtObj->paragraphAt(paragraph)->lineAt(line)->lineLength() - inLine + 
	  txtObj->paragraphAt(paragraph+1)->lineAt(0)->lineLength() - 1;

      // else: if the below line is shorter than the current one, and the below line is in the same paragraph
      // -> jump to the end of the below line
      else if (line != txtObj->paragraphAt(paragraph)->lines()-1)
	absPos += txtObj->paragraphAt(paragraph)->lineAt(line)->lineLength() - inLine + 
	  txtObj->paragraphAt(paragraph)->lineAt(line+1)->lineLength() - 1;
      
      // calculate the new position
      if (absPos > objMaxPos-1) absPos = objMaxPos-1;
      calcPos();
    }
}

/*==================== cursor word forward =======================*/
void TxtCursor::wordForward()
{
}

/*=================== cursor word backward =======================*/
void TxtCursor::wordBackward()
{
}

/*==================== cursor line forward =======================*/
void TxtCursor::lineForward()
{
}

/*=================== cursor line backward =======================*/
void TxtCursor::lineBackward()
{
}

/*================== cursor paragraph forward ====================*/
void TxtCursor::paragraphForward()
{
}

/*================= cursor paragraph backward ====================*/
void TxtCursor::paragraphBackward()
{
}

/*======================== cursor pos1 ===========================*/
void TxtCursor::pos1()
{
}

/*========================== cursor end ==========================*/
void TxtCursor::end()
{
}

/*=================== set absolute position ======================*/
void TxtCursor::setPositionAbs(unsigned int pos)
{
  absPos = pos;
  calcPos();
}

/*================== set position in a paragraph =================*/
void TxtCursor::setPositionParagraph(unsigned int paragraph,unsigned int pos)
{
}

/*================== set position in a line ======================*/
void TxtCursor::setPositionLine(unsigned int paragraph,unsigned int line,unsigned int pos)
{
} 

/*============== calculate the position of the cursor ============*/
void TxtCursor::calcPos()
{
  // if the textobject is valid
  if (txtObj)
    {
      unsigned int l1 = 0,l2 = 0,i,j;
      
      for (i = 0;i < txtObj->paragraphs();i++)
 	{
 	  paragraphPtr = txtObj->paragraphAt(i);

	  // if the cursor is in the current paragraph
	  if (absPos >= l1 && absPos < l1 + paragraphPtr->paragraphLength())
	    {

	      // calculate first values
	      paragraph = i;
	      inParagraph = absPos - l1;
	      for (j = 0;j < paragraphPtr->lines();j++)
		{
		  linePtr = paragraphPtr->lineAt(j);

		  // if the cursor is in the current line
		  if (inParagraph >= l2 && inParagraph < l2 + linePtr->lineLength())
		    {

		      // calculate other values
		      line = j;
		      inLine = inParagraph - l2;
		    }
		  l2 += linePtr->lineLength();
		}
	    }
 	  l1 += paragraphPtr->paragraphLength();
 	}
    }
}

/*============ get minimum of two cursors ========================*/
TxtCursor* TxtCursor::minCursor(TxtCursor *c)
{
  if (c->absPos < absPos) return c;
  else return this;
}

/*============ get maximum of two cursors ========================*/
TxtCursor* TxtCursor::maxCursor(TxtCursor *c)
{
  if (c->absPos > absPos) return c;
  else return this;
}

/******************************************************************/
/* class TxtObj - Text Object                                     */
/******************************************************************/

/*======================== constructor ===========================*/ 
TxtObj::TxtObj()
{
  // init the object
  objType = TEXT;
  objColor = black;
  objFont = QFont("times",12);
  objVertAlign = NORMAL;
  objText = "";
}

/*================= overloaded constructor =======================*/ 
TxtObj::TxtObj(const char *text,QFont f,QColor c,VertAlign va,ObjType ot=TEXT)
{
  // init the object
  objType = ot;
  objColor = c;
  objFont = f;
  objVertAlign = va;
  objText = "";
  objText.insert(0,qstrdup(text));
}

/*===================== width of the object ======================*/
unsigned int TxtObj::width()
{
  QFontMetrics fm(objFont);

  return fm.width((const char*)objText);
}

/*===================== height of the object =====================*/
unsigned int TxtObj::height()
{
  QFontMetrics fm(objFont);

  return fm.height();  
}

/*===================== ascent of the object =====================*/
unsigned int TxtObj::ascent()
{
  QFontMetrics fm(objFont);

  return fm.ascent();  
}

/*==================== descent of the object =====================*/
unsigned int TxtObj::descent()
{
  QFontMetrics fm(objFont);

  return fm.descent();  
}

/*======================= get psoition in object =================*/
int TxtObj::getPos(unsigned int pos)
{
  QFontMetrics fm(objFont);
  unsigned int w = 0;

  for (unsigned int i = 0;i < objText.length();i++)
    {
      if (pos >= w && pos <= w + fm.width((const char*)objText.mid(i,1)))
	return i;
      w += fm.width((const char*)objText.mid(i,1));
    }
  return -1;
}
/******************************************************************/
/* class TxtLine - Text Line                                      */
/******************************************************************/

/*==================== constructor ===============================*/
TxtLine::TxtLine(bool init = false)
{

  // *************** bool y has to be deleted

  // init the object
  objList.setAutoDelete(true);
  
  if (init)
    append(" ",QFont("times",12),black,TxtObj::NORMAL,TxtObj::SEPARATOR);

}

/*===================== insert a text ============================*/
void TxtLine::insert(unsigned int pos,const char *text,QFont f,QColor c,
		     TxtObj::VertAlign va)
{
}

/*===================== insert a char ============================*/
void TxtLine::insert(unsigned int pos,char text,QFont f,QColor c,
		     TxtObj::VertAlign va)
{
}

/*===================== delete character =========================*/
void TxtLine::deleteChar(unsigned int pos)
{
  int obj = getInObj(pos);

  if (obj != -1)
    {
      unsigned int relPos,i,w = 0;
      TxtObj *obj1 = new TxtObj();
      TxtObj *obj2 = new TxtObj();
      
      // get the cursor position relative to the object 
      for (i = 0;i < obj;i++)
	w += itemAt(i)->textLength();
      relPos = pos - w;
      
      itemAt(obj)->deleteChar(relPos);
      if (itemAt(obj)->textLength() == 0) objList.remove(obj);
    }
}

/*===================== delete first character ===================*/
void TxtLine::deleteFirstChar(unsigned int obj)
{
  if (itemAt(obj))
    {
      itemAt(obj)->deleteChar(0);
      if (itemAt(obj)->textLength() == 0) objList.remove(obj);
    }
}

/*===================== backspace char ===========================*/
void TxtLine::backspaceChar(unsigned int pos)
{
  int obj = getInObj(pos);

  if (obj != -1)
    {
      unsigned int relPos,i,w = 0;
      TxtObj *obj1 = new TxtObj();
      TxtObj *obj2 = new TxtObj();
      
      // get the cursor position relative to the object 
      for (i = 0;i < obj;i++)
	w += itemAt(i)->textLength();
      relPos = pos - w;
      
      itemAt(obj)->deleteChar(relPos-1);
      if (itemAt(obj)->textLength() == 0) objList.remove(obj);
    }
}

/*=================== backspace last char ========================*/
void TxtLine::backspaceLastChar(unsigned int obj)
{
  if (itemAt(obj))
    {
      itemAt(obj)->deleteChar(itemAt(obj)->textLength()-1);
      if (itemAt(obj)->textLength() == 0) objList.remove(obj);
    }
}

/*===================== delete region ============================*/
void TxtLine::deleteRegion(unsigned int from,unsigned int to)
{
}

/*================== change effects of a region ==================*/
void TxtLine::changeRegion(unsigned int from,unsigned int to,QFont f,QColor c,
			   TxtObj::VertAlign va)
{
}

/*=================== get length of the line =====================*/
unsigned int TxtLine::lineLength()
{
  unsigned int l = 0;
  
  for (objPtr = objList.first();objPtr != 0;objPtr = objList.next())
    l += objPtr->textLength();

  return l;
}

/*==================== width of the line =========================*/
unsigned int TxtLine::width()
{
  unsigned int w = 0;
  
  for (objPtr = objList.first();objPtr != 0;objPtr = objList.next())
    w += objPtr->width();

  return w;
}

/*==================== height of the line ========================*/
unsigned int TxtLine::height()
{
  unsigned int h = 0;
  
  for (objPtr = objList.first();objPtr != 0;objPtr = objList.next())
    h = max(h,objPtr->height());

  return h;
}

/*==================== ascent of the line ========================*/
unsigned int TxtLine::ascent()
{
  unsigned int a = 0;
  
  for (objPtr = objList.first();objPtr != 0;objPtr = objList.next())
    a = max(a,objPtr->ascent());

  return a;
}

/*==================== descent of the line =======================*/
unsigned int TxtLine::descent()
{
  unsigned int d = 0;
  
  for (objPtr = objList.first();objPtr != 0;objPtr = objList.next())
    d = max(d,objPtr->ascent());

  return d;
}

/*======================== operator = ============================*/
TxtLine& TxtLine::operator=(TxtLine *l)
{
  unsigned int i;
  clear();

  for (i = 0;i < l->items();i++)
    append(*l->itemAt(i));
  
  return *this;
}

/*======================== operator += ===========================*/
TxtLine& TxtLine::operator+=(TxtLine *l)
{
  unsigned int i;

  for (i = 0;i < l->items();i++)
    append(*l->itemAt(i));
 
  return *this;
}

/*======================= split an object ========================*/
void TxtLine::splitObj(unsigned int pos)
{
  int obj = getInObj(pos);

  if (obj != -1)
    {
      unsigned int relPos,i,w = 0;
      TxtObj *obj1 = new TxtObj();
      TxtObj *obj2 = new TxtObj();
      
      // get the cursor position relative to the object 
      for (i = 0;i < obj;i++)
	w += itemAt(i)->textLength();
      relPos = pos - w;

      // insert the text in front of the cursor into the first object
      obj1->append(qstrdup((const char*)itemAt(obj)->text().left(relPos)));
      obj1->setFont(itemAt(obj)->font());
      obj1->setColor(itemAt(obj)->color());
      obj1->setType(itemAt(obj)->type());
      obj1->setVertAlign(itemAt(obj)->vertAlign());

      // insert the text after the object into the second object
      obj2->append(qstrdup((const char*)itemAt(obj)->text().right(itemAt(obj)->textLength()-relPos)));
      obj2->setFont(itemAt(obj)->font());
      obj2->setColor(itemAt(obj)->color());
      obj2->setType(itemAt(obj)->type());
      obj2->setVertAlign(itemAt(obj)->vertAlign());

      // remove original object
      objList.remove(obj);

      // insert the two new objects
      objList.insert(obj,obj2);
      objList.insert(obj,obj1);
    }
}

/*=============== get position in an object ======================*/
int TxtLine::getInObj(unsigned int pos)
{
  unsigned int i,obj = 0,objNum = 0;

  // avoid segfaults :-)
  if (pos > lineLength() || pos == 0) return -1;  

  for (i = 0;i <= pos;i++)
    {

      // if we are at the end of an object
      if (i == obj+objList.at(objNum)->textLength())
	{
	  obj = i;
	  objNum++;
	  if (pos == i) return -1;
	}

      // if we are in an object and we are at the given position -> return object number
      else if (pos == i) return (int)objNum;
    } 

  // avoid compiler warnings :-)
  return -1;
}

/*============== get position before an object ===================*/
int TxtLine::getBeforeObj(unsigned int pos)
{
  unsigned int i,obj = 0,objNum = 0;

  // avoid segfaults :-)
  if (pos > lineLength()) return -1;  

  // if pos at the beginning
  if (pos == 0) return 0;

  for (i = 0;i <= pos;i++)
    {

      // if we are at the end of an object == beginning of next object
      if (i == obj+objList.at(objNum)->textLength())
	{
	  obj += objList.at(objNum)->textLength();
	  objNum++;
	  if (pos == i && i < lineLength()-1) return objNum;
	}
    }

  return -1;
}

/*=============== get position after an object ===================*/
int TxtLine::getAfterObj(unsigned int pos)
{
  unsigned int i,obj = 0,objNum = 0;

  // avoid segfaults :-)
  if (pos > lineLength()) return -1;  

  for (i = 0;i <= pos;i++)
    {

      // if we are at the end of an object 
      if (i == obj+objList.at(objNum)->textLength())
	{
	  obj += objList.at(objNum)->textLength();
	  if (pos == i && i > 0) return objNum;
	  objNum++;
	}
    }

  return -1;
}

/******************************************************************/
/* class TxtParagraph - Text Paragraph                            */
/******************************************************************/

/*===================== constructor ==============================*/
TxtParagraph::TxtParagraph(bool init = false)
{
  // init the objects
  lineList.setAutoDelete(true);
  objHorzAlign = LEFT;

  if (init)
    {
      TxtLine *l = new TxtLine(init);
      lineList.append(l);
    }
}

/*======================== insert a text =========================*/
void TxtParagraph::insert(TxtCursor pos,const char* text,QFont f,
			  QColor c,TxtObj::VertAlign va)
{
}

/*======================== insert a char =========================*/
void TxtParagraph::insert(TxtCursor pos,char text,QFont f,
			  QColor c,TxtObj::VertAlign va)
{
}

/*======================= append a text ==========================*/
void TxtParagraph::append(const char* text,QFont f,QColor c,
			  TxtObj::VertAlign va)
{
}

/*====================== append a char ===========================*/
void TxtParagraph::append(char text,QFont f,QColor c,TxtObj::VertAlign va)
{
}

/*====================== insert a line ===========================*/
void TxtParagraph::insert(unsigned int i,TxtLine *l)
{
  lin = new TxtLine();
  lin->operator=(l);
  lineList.insert(i,lin);
}

/*====================== append a line ===========================*/
void TxtParagraph::append(TxtLine *l)
{
  lin = new TxtLine();
  lin->operator=(l);
  lineList.append(lin);
}

/*======================= append an object =======================*/
void TxtParagraph::insert(unsigned int i,TxtObj*)
{
}

/*====================== append an object ========================*/
void TxtParagraph::append(TxtObj* to)
{
  if (lineList.isEmpty()) 
    {
      lin = new TxtLine();
      lineList.append(lin);
    }

  lineList.at(lineList.count()-1)->append(qstrdup((const char*)to->text()),to->font(),
					  to->color(),to->vertAlign(),to->type());
}

/*===================== delete character =========================*/
void TxtParagraph::deleteChar(TxtCursor pos)
{
}

/*===================== delete region ============================*/
void TxtParagraph::deleteRegion(TxtCursor from,TxtCursor to)
{
}

/*================== change effects of a region ==================*/
void TxtParagraph::changeRegion(TxtCursor from,TxtCursor to,QFont f,
				QColor c,TxtObj::VertAlign va)
{
}

/*============= return textlength of the paragraph ===============*/
unsigned int TxtParagraph::paragraphLength()
{
  unsigned int l = 0;
  
  for (linePtr = lineList.first();linePtr != 0;linePtr = lineList.next())
    l += linePtr->lineLength();

  return l;
}

/*==================== width of the paragraph ====================*/
unsigned int TxtParagraph::width()
{
  unsigned int w = 0;

  for (linePtr = lineList.first();linePtr != 0;linePtr = lineList.next())
    w = max(w,linePtr->width());

  return w;
}

/*==================== height of the paragraph ===================*/
unsigned int TxtParagraph::height()
{
  unsigned int h = 0;

  for (linePtr = lineList.first();linePtr != 0;linePtr = lineList.next())
    h += linePtr->height();

  return h;
}

/*======================= break lines ============================*/
QRect TxtParagraph::breakLines(unsigned int wid)
{
  // concat all lines to one
  linePtr = toOneLine();

  line = new TxtLine();
  unsigned int i,j,w = 0;
  
  // if a line exists
  if (linePtr)
    {
      for (i = 0;i < linePtr->items(); i++)
 	{
	  obj = linePtr->itemAt(i);

	  // if the object fits into the line or is a separator 
	  // (separators have always to be appended, never insert a separator as first char in a line -> looks ugly!) 
	  if (w + widthToNextSep(i) <= wid || obj->type() == TxtObj::SEPARATOR)
	    {
	      line->append(*obj);
	      w += obj->width();
	    }
 
	  // if the object doesn't fit
	  else
 	    {
	      lineList.append(line);
	      line = new TxtLine();
	      w = 0;
	      
	      // insert all objects until the next separator
	      for (j = i;j < linePtr->items() && linePtr->itemAt(j)->type() != TxtObj::SEPARATOR;j++)
		{
		  obj = linePtr->itemAt(j);
		  line->append(*obj);
		  w += obj->width();
		}
	      i = j-1;
	    }
 	}

      // don't forget the last line!
      if (line->items() > 0) lineList.append(line);

      for (linePtr = lineList.first();linePtr != 0;linePtr = lineList.next())
	if (linePtr->items() > 0 && linePtr->itemAt(linePtr->items()-1)->type() != TxtObj::SEPARATOR)
	  linePtr->append(" ",linePtr->itemAt(linePtr->items()-1)->font(),linePtr->itemAt(linePtr->items()-1)->color(),
			  TxtObj::NORMAL,TxtObj::SEPARATOR);
    }
}

/*==================== concat all lines to one line ==============*/
TxtLine* TxtParagraph::toOneLine()
{
  unsigned int i,j;
  linePtr = new TxtLine();

  if (!lineList.isEmpty())
    {
      // concat all lines to one line
      if (lines() == 1)
	linePtr->operator=(lineAt(0));
      else
	for (i = 0;i < lines();i++)
	  linePtr->operator+=(lineAt(i));
      
      // clear the list and return the pointer to the resulting line 
      lineList.clear();
      return linePtr;
    }

  return 0;
}

/*============= with from an obj to next separator ===============*/
unsigned int TxtParagraph::widthToNextSep(unsigned int pos)
{
  unsigned int i,w = 0;
  
  
  for (i = pos;i < linePtr->items() && linePtr->itemAt(i)->type() != TxtObj::SEPARATOR;i++)
    w += linePtr->itemAt(i)->width();

  return w;
}

/******************************************************************/
/* class KTextObject - KTextObject                                */
/******************************************************************/

/*====================== constructor =============================*/
KTextObject::KTextObject(QWidget *parent=0,const char *name=0,ObjType ot=PLAIN,
			 unsigned int c=0,unsigned int r=0)
  : QTableView(parent,name)
{
  // init the objects
  setFocusPolicy(QWidget::StrongFocus);
  setBackgroundColor(white);

  objEnumListType.type = NUMBER;
  objEnumListType.before = "";
  objEnumListType.after = ".";
  objEnumListType.start = 1;
  objEnumListType.font = QFont("times",12);
  objEnumListType.color = black;

  objUnsortListType.font = QFont("times",12);
  objUnsortListType.color = black;
  objUnsortListType.chr = '-';
  paragraphList.setAutoDelete(true);

  cellWidths.setAutoDelete(true);
  cellHeights.setAutoDelete(true);
  objRow = r;
  objCol = c;

  sCursor = true;
  txtCursor = new TxtCursor((KTextObject*)this);
  cursorChanged = true;
  setCursor(ibeamCursor);

  setFocusPolicy(StrongFocus);

  drawLine = -1;
  drawParagraph = -1;
  drawBelow = false;

  currFont = QFont("times",12);
  currColor = black;

  setMouseTracking(true);
  mousePressed = false;
  drawPic = false;
  ystart = 0;
  drawSelection = false;

  selectionColor = lightGray;
  
  TxtParagraph *para;
  para = new TxtParagraph(true);
  paragraphList.append(para);

  CellWidthHeight *wh = new CellWidthHeight;
  wh->wh = width();
  cellWidths.append(wh);
  setNumCols(numCols()+1);

  wh = new CellWidthHeight;
  wh->wh = 0;
  cellHeights.append(wh);
  setNumRows(1);

  setShowCursor(true);

  setObjType(ot);
}

/*===================== set objecttype ===========================*/
void KTextObject::setObjType(ObjType ot)
{
  obType = ot;
  recalc();
  repaint(true);
}

/*================= get length of the text =======================*/
unsigned int KTextObject::textLength()
{
  unsigned int l = 0;
  
  for (paragraphPtr = paragraphList.first();paragraphPtr != 0;paragraphPtr = paragraphList.next())
    l += paragraphPtr->paragraphLength();

  return l;
}

/*==================== set horizontal alignemnt ==================*/
void KTextObject::setHorzAlign(TxtParagraph::HorzAlign ha,int p = -1)
{
  if (p == -1) p = txtCursor->positionParagraph();

  paragraphAt(p)->setHorzAlign(ha);
  updateCell(p,0,false);
}

/*==================== get horizontal alignemnt ==================*/
TxtParagraph::HorzAlign KTextObject::horzAlign(int p = -1)
{
  if (p == -1) p = txtCursor->positionParagraph();

  return paragraphAt(p)->horzAlign();
}

/*=================== get QPicture of the obj ====================*/
QPicture* KTextObject::getPic(int _x,int _y,int _w,int _h)
{
  QPainter p;

  drawPic = true;
  p.begin(&pic);
  p.setClipping(true);
  p.setClipRect(_x,_y,_w,_h);
  ystart = 0;
  for (unsigned i = 0;i < paragraphs();i++)
    {
      paintCell(&p,i,0);
      ystart += cellHeights.at(i)->wh;
    }
  p.end();
  ystart = 0;
  drawPic = false;
  return &pic;
}

/*====================== paint cell ==============================*/
void KTextObject::paintCell(class QPainter* p,int row,int)
{
  paragraphPtr2 = 0;
  paragraphPtr = 0;
  int x = xstart,y = ystart;
  int w = cellWidth(0),h = cellHeight(row);
  unsigned int i = 0,j = 0,chars = 0,len = 0;
  bool drawCursor = false;
  QPoint c1,c2;
  unsigned int scrBar = 0,wid = 0,selX,selW;
  if (tableFlags() & Tbl_vScrollBar) scrBar = 16;
  char chr[11];
  int selStart = -1,selStop = -1;

  // get pointer to the paragraph, which should be drwan
  paragraphPtr = paragraphList.at(row);
  
  // if the paragraph exists, draw it
  if (paragraphPtr)
    {      

      if (!drawPic)
	{
	  p->setPen(NoPen);
	  p->setBrush(backgroundColor());
	  p->drawRect(0,0,xstart,h);
	}
	 
      // object type
      switch (obType)
	{
	case PLAIN: case TABLE: break;
	case ENUM_LIST:
	  {
	    if (objEnumListType.type == NUMBER)
	      sprintf(chr,"%s%d%s ",(const char*)objEnumListType.before,row+objEnumListType.start,
		      (const char*)objEnumListType.after);
	    else
	      sprintf(chr,"%s%c%s ",(const char*)objEnumListType.before,row+objEnumListType.start,
		      (const char*)objEnumListType.after);	      
	    p->setFont(objEnumListType.font);
	    p->setPen(objEnumListType.color);
	    p->drawText(0,y+paragraphPtr->lineAt(0)->ascent()-p->fontMetrics().ascent(),
			xstart,p->fontMetrics().height(),
			AlignRight,(const char*)&chr);	    
	  } break;
	case UNSORT_LIST:
	  {
	    p->setFont(objUnsortListType.font);
	    p->setPen(objUnsortListType.color);
	    sprintf(chr,"%c ",objUnsortListType.chr);
	    p->drawText(0,y+paragraphPtr->lineAt(0)->ascent()-p->fontMetrics().ascent(),
			xstart,p->fontMetrics().height(),
			AlignRight,(const char*)&chr);
	  } break;
	}

      // draw lines
      for (i = 0;i < paragraphPtr->lines();i++)
	{
	  linePtr = paragraphPtr->lineAt(i);
	  x = xstart;

	  // check, if the line should be drawn
	  if (drawLine == -1 && drawParagraph == -1 ||
	      !drawBelow && drawLine == i && drawParagraph == row ||
	      drawBelow && (row > drawParagraph || row == drawParagraph && i >= drawLine))
	    {
	      // erase the line
	      if (!drawPic)
		{
		  p->setPen(NoPen);
		  p->setBrush(backgroundColor());
		  p->drawRect(x,y,w,linePtr->height());
		}

	      // alignment
	      switch (paragraphPtr->horzAlign())
		{
		case TxtParagraph::LEFT: w -= scrBar; break;
		case TxtParagraph::CENTER: x += (w - linePtr->width()) / 2 - scrBar / 2; break;
		case TxtParagraph::RIGHT: x += w - linePtr->width() - scrBar; break;
		}

	      // draw all objects of the line
	      wid = 0;
	      for (j = 0;j < linePtr->items();wid += linePtr->itemAt(j)->textLength(),j++)
		{
		  objPtr = linePtr->itemAt(j);
		  len = objPtr->textLength();
		  p->setFont(objPtr->font());
		  
		  // check, if a selection should be drawn
// 		  selStart = -1;
// 		  selStop = -1;
//  		  if ((mousePressed || drawSelection) && showCursor())
//  		    {
		      
// 		      // get start position
//  		      if (startCursor->positionParagraph() == row)
// 			{
// 			  if (startCursor->positionLine() == i)
// 			    {
// 			      unsigned int inLine = startCursor->positionInLine();
// 			      if (linePtr->getBeforeObj(inLine) == j || inLine <= wid)
// 				selStart = 0;
// 			      else if (linePtr->getInObj(inLine) == j)
// 				selStart = inLine - wid;
// 			      else selStart = -1;
// 			    }
// 			  else if (startCursor->positionLine() < i)
// 			    {
// 			      selStart = 0;
// 			    }
// 			  else selStart = -1;
// 			}
// 		      else if (startCursor->positionParagraph() < row)
// 			selStart = 0;
// 		      else selStart = -1;

// 		      if (selStart != -1)
// 			{
// 			  // get end position
// 			  if (stopCursor->positionParagraph() == row)
// 			    {
// 			      if (stopCursor->positionLine() == i)
// 				{
// 				  unsigned int inLine = stopCursor->positionInLine();
// 				  if (linePtr->getAfterObj(inLine) == j || inLine >= wid + linePtr->itemAt(j)->textLength())
// 				    selStop = wid + linePtr->itemAt(j)->textLength();
// 				  else if (linePtr->getInObj(inLine) == j)
// 				    selStop = inLine - wid;
// 				  else selStop = -1;
// 				}
// 			      else if (startCursor->positionLine() > i)
// 				{
// 				  unsigned int inLine = stopCursor->positionInLine();
// 				  selStop = inLine - wid;
// 				}
// 			      else selStop = -1;
// 			    }
// 			  else if (startCursor->positionParagraph() > row)
// 			    {
// 			      unsigned int inLine = stopCursor->positionInLine();
// 			      selStop = inLine - wid;
// 			    }
// 			  else selStop = -1;
			  
// 			  // if selection should really be drawn
// 			  if (selStop != -1 && selStart < selStop)
// 			    {
// 			      unsigned int inLine = stopCursor->positionInLine();
// 			      selX = x;
// 			      if (selStart > 0)
// 				selX += p->fontMetrics().width(objPtr->text().left(selStart));
// 			      if (selStop == inLine - wid)
// 				selW = objPtr->width() - selX;
// 			      else
// 				selW = objPtr->width() - selX -
// 				  p->fontMetrics().width(objPtr->text().right(objPtr->textLength() - selStop));
// 			      p->setPen(QPen(selectionColor));
// 			      p->setBrush(QBrush(selectionColor));
// 			      p->drawRect(selX,y,selW,linePtr->height());
// 			    }
// 			}
// 		    }

		  // check, if cursor should be drawn - if yes calculate it
		  if (drawCursor = showCursor() && txtCursor->positionParagraph() == row && 
		      txtCursor->positionLine() == i && txtCursor->positionInLine() >= chars && 
		      txtCursor->positionInLine() <= chars+len)
		    {
		      c1.setX(x + p->fontMetrics().width(objPtr->text().left(txtCursor->positionInLine()-chars)));
		      c1.setY(y);
		      c2.setX(c1.x());
		      c2.setY(y + linePtr->height());
		    }
		  
		  // draw Text
		  p->setPen(objPtr->color());
		  p->drawText(x,y+linePtr->ascent()-objPtr->ascent(),w,linePtr->height(),AlignLeft,objPtr->text());
		  
		  // draw Cursor
		  if (drawCursor) 
		    {
		      if (cursorChanged)
			{
			  emit fontChanged((QFont*)&p->font());
			  emit colorChanged((QColor*)&p->pen().color());
			  emit horzAlignChanged(paragraphPtr->horzAlign());
			  cursorChanged = false;
			  currFont = p->font();
			  currColor = p->pen().color();
			}
		      p->setPen(QPen(black,1,SolidLine));
		      p->drawLine(c1,c2);
		    }
		  x += objPtr->width();
		  chars += len;
		}
	    }

	  // calculate coordinates for the next line
	  y += linePtr->height();
	  x = xstart;
	  chars = 0;
	}
    }
}

/*===================== paint event ==============================*/
void KTextObject::paintEvent(QPaintEvent *e)
{
  QPainter p;

  // call superclass methode
  QTableView::paintEvent(e);

  // erase unused space
  if (totalHeight() < height() && !drawPic)
    {
      p.begin(this);
      p.setPen(NoPen);
      p.setBrush(backgroundColor());
      p.drawRect(0,totalHeight(),width(),height()-totalHeight());
      p.end();
    }
}

/*====================== return cell width =======================*/
int KTextObject::cellWidth(int i)
{
  if (i < numCols())
    return cellWidths.at(i)->wh;
  else return 0;
}

/*====================== return cell height ======================*/
int KTextObject::cellHeight(int i)
{
  if (i < numRows())
    return cellHeights.at(i)->wh;
  else return 0;
}

/*====================== return total width ======================*/
int KTextObject::totalWidth()
{
  int w = 0;
  unsigned int i;

  for (i = 0,cwhPtr = cellWidths.first();cwhPtr != 0;cwhPtr = cellWidths.next(),i++)
    w += cellWidth(i);
  
  return w;
}

/*====================== return total height =====================*/
int KTextObject::totalHeight()
{
  int h = 0;
  unsigned int i;

  for (i = 0,cwhPtr = cellHeights.first();cwhPtr != 0;cwhPtr = cellHeights.next(),i++)
    h += cellHeight(i);
  
  return h;
}

/*====================== resize event ============================*/
void KTextObject::resizeEvent(QResizeEvent* e)
{
  resize(e->size());
  recalc();
  QTableView::resizeEvent(e);
}

/*====================== key press event =========================*/
void KTextObject::keyPressEvent(QKeyEvent* e)
{
  if (showCursor())
    {
      unsigned int i;
      bool drawAbove = false;
      
      // ***************************
      // ** redraw needed here!!!!!!
      // ***************************
      drawSelection = false;

      // always update the maximal cursor position when a key is pressed
      txtCursor->setMaxPosition(textLength());
      
      TxtCursor *oldCursor = new TxtCursor((KTextObject*)this);
      oldCursor->setPositionAbs(txtCursor->positionAbs());
      
      // react on the pressed key
      switch (e->key())
	{
	case Key_Right: { txtCursor->charForward(); cursorChanged = true; } break;
	case Key_Left: { txtCursor->charBackward(); cursorChanged = true; } break;
	case Key_Up: { txtCursor->lineUp(); cursorChanged = true; } break;
	case Key_Down: { txtCursor->lineDown(); cursorChanged = true; } break;
	case Key_Return: case Key_Enter:
	  {
	    splitParagraph();
	    drawBelow = true;
	    drawAbove = true;
	    cursorChanged = true;
	  } break;
	case Key_Backspace:
	  {
	    kbackspace();
	    drawBelow = true;
	    cursorChanged = true;
	  } break;
	case Key_Delete:
	  {
	    kdelete();
	    drawBelow = true;
	    cursorChanged = true;
	  } break;
	default:
	  {
	    if (e->ascii() && e->ascii() > 31)
	      {
		insertChar(e->ascii());
		drawBelow = true;
		cursorChanged = true;
	      }
	  };
	}

      // if only one line has changed, redraw it
      if (oldCursor->positionParagraph() == txtCursor->positionParagraph() &&
	  oldCursor->positionLine() == txtCursor->positionLine())
	{
	  drawParagraph = txtCursor->positionParagraph();
	  drawLine = txtCursor->positionLine();
	  updateCell(txtCursor->positionParagraph(),0,false);
	  drawLine = -1;
	  drawParagraph = -1;
	}
      
      // else redraw both lines
      else
	{
	  drawParagraph = txtCursor->positionParagraph();
	  drawLine = txtCursor->positionLine();
	  updateCell(txtCursor->positionParagraph(),0,false);
	  drawParagraph = oldCursor->positionParagraph();
	  drawLine = oldCursor->positionLine();
	  updateCell(oldCursor->positionParagraph(),0,false);
	  drawLine = -1;
	  drawParagraph = -1;
	}
      
      // if a line and everything below should be drawn
      if (drawBelow)
	{
	  oldCursor->calcPos();
	  oldCursor = txtCursor->minCursor(oldCursor);
	  drawParagraph = oldCursor->positionParagraph();
	  drawLine = oldCursor->positionLine();
	  for (i = drawParagraph;i < paragraphs();i++)
	    updateCell(i,0,false);
	  i = drawParagraph;
	  drawLine = -1;
	  drawParagraph = -1;
	  drawBelow = false;
	}
      
      if (drawAbove)
	{
	  drawLine = -1;
	  drawParagraph = -1;
	  updateCell(i,0,false);
	}

      if (tableFlags() & Tbl_vScrollBar) makeCursorVisible();
    }
}

/*====================== mouse press event ========================*/
void KTextObject::mousePressEvent(QMouseEvent *e)
{
  mousePressed = true;
  drawSelection = false;
  startCursor = getCursorPos(e->x(),e->y(),true);

  // ugly - but for testing ok
  //repaint(true);
}

/*====================== mouse release event ======================*/
void KTextObject::mouseReleaseEvent(QMouseEvent *e)
{
  mousePressed = false;
  stopCursor = getCursorPos(e->x(),e->y(),true);

  if (stopCursor->positionAbs() != startCursor->positionAbs())
    drawSelection = true;

  // ugly - but for testing ok
  //repaint(true);
}

/*====================== mouse move event =========================*/
void KTextObject::mouseMoveEvent(QMouseEvent *e)
{
  if (mousePressed)
    {
      stopCursor = getCursorPos(e->x(),e->y(),true);
      TxtCursor *c1 = startCursor->maxCursor(stopCursor);
      TxtCursor *c2 = startCursor->minCursor(stopCursor);
      startCursor = c1;
      stopCursor = c2;

      // ugly - but for testing ok
      //repaint(true);
    }
}

/*=================== recalcualte everything =====================*/
void KTextObject::recalc()
{
  switch (obType)
    {
    case PLAIN: xstart = 0; break;
    case ENUM_LIST:
      {
	QFontMetrics fm(objEnumListType.font);
	char chr[12];
	sprintf(chr,"%s99 %s ",(const char*)objEnumListType.before,(const char*)objEnumListType.after);
	xstart = fm.width(chr);
      } break;
    case UNSORT_LIST:
      {
	QFontMetrics fm(objUnsortListType.font);
	xstart = fm.width(objUnsortListType.chr) + fm.width(" ");
      } break;
    }
  
  cellWidths.at(0)->wh = width()-xstart;
  if (tableFlags() & Tbl_vScrollBar) cellWidths.at(0)->wh -= verticalScrollBar()->width(); 
  
  for (paragraphPtr = paragraphList.first();paragraphPtr != 0;paragraphPtr = paragraphList.next())
    {
      // break the lines, and resize the cell
      paragraphPtr->breakLines(cellWidth(0));
      cellWidths.at(0)->wh = width()-xstart;
      cellHeights.at(paragraphList.at())->wh = paragraphPtr->height();
    }

  // calculate the new cursorposition
  txtCursor->calcPos();
}

/*====================== split paragraph =========================*/
void KTextObject::splitParagraph()
{
  para1 = new TxtParagraph();
  para2 = new TxtParagraph();
  para3 = new TxtParagraph();
  lin;
  unsigned int i;
  unsigned int para = txtCursor->positionParagraph();
  unsigned int line = txtCursor->positionLine();
  obj = 0;
 TxtParagraph::HorzAlign ha;
 
  // if the cursor is at the beginning of a line
  if (txtCursor->positionInLine() == 0)
    {

      // if the cursor is in the first line -> just append an empty paragraph below
      if (line == 0)
	{
	  lin = new TxtLine();
	  lin->append(" ",currFont,currColor,TxtObj::NORMAL,TxtObj::SEPARATOR);
	  para1->append(lin);
	  paragraphList.insert(para,para1);
	}

      // if the cursor is in another line
      else 
	{

	  // 1. paragraph: text before the line, in which the cursor is
 	  for (i = 0;i < line;i++)
	    para1->append(paragraphAt(para)->lineAt(i));
	  
	  // 2. paragraph: empty (new) pragraph
	  lin = new TxtLine();
	  lin->append("  ",currFont,currColor,TxtObj::NORMAL,TxtObj::SEPARATOR);
	  para2->append(lin);

	  // 3. paragraph: text after the line in which the cursor is
 	  for (i = line;i < paragraphAt(para)->lines();i++)
	    para3->append(paragraphAt(para)->lineAt(i));

	  // remove original paragraph
	  paragraphList.remove(para);

	  // insert new paragraphs
	  paragraphList.insert(para,para3);
	  paragraphList.insert(para,para2);
 	  paragraphList.insert(para,para1);

	  // insert the new row into the table
 	  wh = new CellWidthHeight;
 	  wh->wh = 0;
 	  cellHeights.append(wh);
 	  setNumRows(numRows()+1);
	}

      // move cursor forward
      txtCursor->setMaxPosition(textLength());
      txtCursor->charForward();
    }

  // if the cursor is at the end of a line
  else if (txtCursor->positionInLine() == paragraphAt(para)->lineAt(txtCursor->positionLine())->lineLength()-1)
    {
      // if the cursor is in the last line, just insert an empty paragraph below
      if (line == paragraphAt(para)->lines()-1)
	{
 	  lin = new TxtLine();
 	  lin->append("  ",currFont,currColor,TxtObj::NORMAL,TxtObj::SEPARATOR);
	  para1->setHorzAlign(paragraphAt(para)->horzAlign());
 	  para1->append(lin);
 	  paragraphList.insert(para+1,para1);
	}

      // if the cursor is in another line
      else 
	{

	  // 1. paragraph: text before the line, in which the cursor is, and text of the line in which the cursor is
  	  for (i = 0;i <= line;i++)
 	    para1->append(paragraphAt(para)->lineAt(i));
	  
	  // 2. paragraph: empty (new) pragraph
 	  lin = new TxtLine();
 	  lin->append("  ",currFont,currColor,TxtObj::NORMAL,TxtObj::SEPARATOR);
 	  para2->append(lin);

	  // 3. paragraph: text after the line in which the cursor is
  	  for (i = line+1;i < paragraphAt(para)->lines();i++)
 	    para3->append(paragraphAt(para)->lineAt(i));

	  // remove original paragraph
	  ha = paragraphAt(para)->horzAlign();
 	  paragraphList.remove(para);

	  // insert new paragraphs
	  para1->setHorzAlign(ha);
	  para2->setHorzAlign(ha);
	  para3->setHorzAlign(ha);
 	  paragraphList.insert(para,para3);
 	  paragraphList.insert(para,para2);
  	  paragraphList.insert(para,para1);

 	  // insert the new row into the table
  	  wh = new CellWidthHeight;
  	  wh->wh = 0;
  	  cellHeights.append(wh);
  	  setNumRows(numRows()+1);
	}

      // move cursor forward
      txtCursor->setMaxPosition(textLength());
      txtCursor->charForward();
    }

  // if the cursor is anywhere else in the line
  else
    {

      // create two empty paragraphs
      lin = new TxtLine();
      para1->append(lin);
      lin = new TxtLine();
      para2->append(lin);

      // concat the paragraph to one line and recalc the cursorposition
      lin = paragraphAt(para)->toOneLine();
      paragraphAt(para)->append(lin);
      txtCursor->calcPos();
      unsigned int inLine = txtCursor->positionInLine();

      // if the cursor is in an object, split it
      if (lin->getInObj(inLine) != -1) lin->splitObj(inLine);
      
      // remeber the object (type) of the object, which is in front of the cursor
      if (lin->getAfterObj(inLine) != -1) 
	obj = lin->itemAt(lin->getAfterObj(inLine));

      // insert the obejcts, which are in front of the cursor, into the first paragraph
      for (i = 0;i <= lin->getAfterObj(inLine);i++)
	para1->append(lin->itemAt(i));
      
      // insert the objects, which are after the cursor, into the secont paragraph
      for (i = lin->getBeforeObj(inLine);i < lin->items();i++)
	para2->append(lin->itemAt(i));
      
      // remove the original paragraph
      ha = paragraphAt(para)->horzAlign();
      paragraphList.remove(para);

      // insert the two new paragraphs
      para1->setHorzAlign(ha);
      para2->setHorzAlign(ha);
      paragraphList.insert(para,para2);
      paragraphList.insert(para,para1);
      
      // if the remembered object (type) is not a separator -> move cursor one position forward
      if (obj && obj->type() != TxtObj::SEPARATOR) txtCursor->charForward();
    }
  
  // insert the new row into the table
  wh = new CellWidthHeight;
  wh->wh = 0;
  cellHeights.append(wh);
  setNumRows(numRows()+1);
  
  // recalculate everything
  recalc();
}

/*======================= join paragraph =========================*/
void KTextObject::joinParagraphs(unsigned int p1,unsigned int p2)
{
  unsigned int para1n = min(p1,p2);
  unsigned int para2n = max(p1,p2);
  lin = new TxtLine();
  para1 = new TxtParagraph();
  para1->append(lin);
  
  lin = paragraphAt(para1n)->toOneLine();
  linePtr = paragraphAt(para2n)->toOneLine();
  lin->deleteItem(lin->items()-1);
  lin->operator+=(linePtr);

  para1->setHorzAlign(paragraphAt(para1n)->horzAlign());

  paragraphList.remove(para2n);
  paragraphList.remove(para1n);

  para1->append(lin);
  paragraphList.insert(para1n,para1);

  cellHeights.remove(para2n);
  setNumRows(numRows()-1);
}

/*======================= key backspace ==========================*/
void KTextObject::kbackspace()
{
  unsigned int i;
  unsigned int para = txtCursor->positionParagraph();
  unsigned int line = txtCursor->positionLine();

  // if the cursor is at the first position of a paragraph -> this and the upper paragraph have to be joined
  if (txtCursor->positionLine() == 0 && txtCursor->positionInLine() == 0 && para > 0)
    joinParagraphs(para,para-1);
   
  // if the cursor is elsewhere
  else
    {
      // concat the paragraph to one line and recalc the cursorposition
      lin = paragraphAt(para)->toOneLine();
      paragraphAt(para)->append(lin);
      txtCursor->calcPos();
      unsigned int inLine = txtCursor->positionInLine();
      lin = paragraphAt(para)->lineAt(0);
      
      // delete the char in front of the cursor
      if (lin->getInObj(inLine) != -1) lin->backspaceChar(inLine);
      else if (lin->getAfterObj(inLine) != -1) lin->backspaceLastChar(lin->getAfterObj(inLine));
      
      // if the paragraph is empty now, delete it
      if (lin->lineLength() == 0) 
	{
	  paragraphList.remove(para);
	  cellHeights.remove(para);
	  setNumRows(numRows()-1);
	}
    }

  // move cursor backward
  txtCursor->setMaxPosition(textLength());
  txtCursor->charBackward();
      
  // recalculate everything
  recalc();
}

/*========================= key delete ===========================*/
void KTextObject::kdelete()
{
  unsigned int i;
  unsigned int para = txtCursor->positionParagraph();
  unsigned int line = txtCursor->positionLine();

  // if the cursor is at the last position of a paragraph -> this and the lower paragraph have to be joined
  if (txtCursor->positionLine() == paragraphAt(para)->lines()-1 && 
      txtCursor->positionInLine() == paragraphAt(para)->lineAt(line)->lineLength()-1)
    {
      if (para < paragraphs()-1) joinParagraphs(para,para+1);
    }

  // if the cursor is elsewhere
  else
    {
      // concat the paragraph to one line and recalc the cursorposition
      lin = paragraphAt(para)->toOneLine();
      paragraphAt(para)->append(lin);
      txtCursor->calcPos();
      unsigned int inLine = txtCursor->positionInLine();
      lin = paragraphAt(para)->lineAt(0);
      
      // delete the char behind the cursor
      if (lin->getInObj(inLine) != -1) lin->deleteChar(inLine);
      else if (lin->getBeforeObj(inLine) != -1) lin->deleteFirstChar(lin->getBeforeObj(inLine));
      
      // if the paragraph is empty now, delete it
      if (lin->lineLength() == 0) 
	{
	  paragraphList.remove(para);
	  cellHeights.remove(para);
	  setNumRows(numRows()-1);
	}
    }

  txtCursor->setMaxPosition(textLength());
      
  // recalculate everything
  recalc();
}

/*========================= insert char ==========================*/
void KTextObject::insertChar(char c)
{
  unsigned int i,objPos,w = 0;
  unsigned int para = txtCursor->positionParagraph();
  QString str;
  str = "";
  str.insert(0,c);

  lin = paragraphAt(para)->toOneLine();
  paragraphAt(para)->append(lin);
  txtCursor->calcPos();
  unsigned int inLine = txtCursor->positionInLine();
  lin = paragraphAt(para)->lineAt(0);
  
  switch (c)
    {
      
      // separator
    case ' ':
      {
	if (lin->getInObj(inLine) != -1) lin->splitObj(inLine);
	if (lin->getAfterObj(inLine) != -1)
	  objPos = lin->getAfterObj(inLine)+1;
	else
	  objPos = lin->getBeforeObj(inLine);
	if (objPos != -1)
	  {
	    obj = new TxtObj(" ",currFont,currColor,TxtObj::NORMAL,TxtObj::SEPARATOR);
	    lin->insert(objPos,*obj);
	  }
      } break;

      // other chars
    default:
      {
	if (lin->getInObj(inLine) != -1)
	  {
	    if (sameEffects(lin->itemAt(lin->getInObj(inLine))))
	      {
		objPos = lin->getInObj(inLine);
		for (i = 0;i < objPos;i++)
		  w += lin->itemAt(i)->textLength();
		lin->itemAt(objPos)->insert(inLine-w,(const char*)str);
		break;
	      }
	    else
	      lin->splitObj(inLine);
	  }
	if (lin->getAfterObj(inLine) != -1 &&
	    lin->itemAt(lin->getAfterObj(inLine))->type() != TxtObj::SEPARATOR && sameEffects(lin->itemAt(lin->getAfterObj(inLine))))
	  {
	    objPos = lin->getAfterObj(inLine);
	    lin->itemAt(objPos)->append((const char*)str);
	  }
	else if (lin->getBeforeObj(inLine) != -1 &&
	    lin->itemAt(lin->getBeforeObj(inLine))->type() != TxtObj::SEPARATOR && sameEffects(lin->itemAt(lin->getBeforeObj(inLine))))
	  {
	    objPos = lin->getBeforeObj(inLine);
	    lin->itemAt(objPos)->insert(0,(const char*)str);
	  }
	else
	{
	  if (lin->getAfterObj(inLine) != -1)
	    objPos = lin->getAfterObj(inLine)+1;
	  else
	    objPos = lin->getBeforeObj(inLine);
	  if (objPos != -1)
	    {
	      obj = new TxtObj((const char*)str,currFont,currColor,TxtObj::NORMAL,TxtObj::TEXT);
	      lin->insert(objPos,*obj);
	    }
	}
      } break;
    }

  txtCursor->setMaxPosition(textLength());
  txtCursor->charForward();

  // recalculate everything
  recalc();
}

/*====================== make cursor  vsisble ====================*/
void KTextObject::makeCursorVisible()
{
  unsigned int para = txtCursor->positionParagraph();
  unsigned int line = txtCursor->positionLine();
  unsigned int i,h = 0;

  for (i = 0;i < para;i++)
    h += paragraphAt(i)->height();

  for (i = 0;i < line;i++)
    h += paragraphAt(para)->lineAt(i)->height();

  if (!(h >= yOffset() && h+paragraphAt(para)->lineAt(line)->height() <= yOffset()+viewHeight()))
    {
//       setYOffset(h);
//       updateTableSize();
//       updateScrollBars();
      setOffset(0,h,true);
    }
}

/*====================== set cursor psoition ====================*/
TxtCursor* KTextObject::getCursorPos(int _x,int _y,bool set=false)
{
  unsigned int x = (unsigned int)_x;
  unsigned int y = (unsigned int)_y,i,h = 0,w = 0,para,line,objPos,pos,absPos = 0;;
      
  para = paragraphs() - 1;
  for (i = 0;i < paragraphs();i++)
    {
      if (y >= h && y <= h + paragraphAt(i)->height())
	{
	  para = i;
	  break;
	}
      h += paragraphAt(i)->height();
    }
  
  line = paragraphAt(para)->lines() - 1;
  for (i = 0;i < paragraphAt(para)->lines();i++)
    {
      if (y >= h && y <= h + paragraphAt(para)->lineAt(i)->height())
	{
	  line = i;
	  break;
	}
      h += paragraphAt(para)->lineAt(i)->height();
    }
  
  paragraphPtr = paragraphAt(para);
  linePtr = paragraphPtr->lineAt(line);
  unsigned int scrBar = 0;
  if (tableFlags() & Tbl_vScrollBar) scrBar = 16;
  x -= xstart;
  switch (paragraphPtr->horzAlign())
    {
    case TxtParagraph::LEFT: break;
    case TxtParagraph::CENTER: x -= (cellWidth(0) - linePtr->width()) / 2 - scrBar / 2; break;
    case TxtParagraph::RIGHT: x -= cellWidth(0) - linePtr->width() - scrBar; break;
    }
  
  objPos = paragraphAt(para)->lineAt(line)->items() - 1;
  for (i = 0;i < paragraphAt(para)->lineAt(line)->items();i++)
    {
      if (x >= w && x <= w + paragraphAt(para)->lineAt(line)->itemAt(i)->width())
	{
	  objPos = i;
	  break;
	}
      w += paragraphAt(para)->lineAt(line)->itemAt(i)->width();
    }
  
  pos = paragraphAt(para)->lineAt(line)->itemAt(objPos)->getPos(x - w);
  if (pos == -1) pos = paragraphAt(para)->lineAt(line)->itemAt(objPos)->textLength()-1;
  else pos++;
  
  
  for (i = 0;i < para;i++)
    absPos += paragraphAt(i)->paragraphLength();
  for (i = 0;i < line;i++)
    absPos += paragraphAt(para)->lineAt(i)->lineLength();
  for (i = 0;i < objPos;i++)
    absPos += paragraphAt(para)->lineAt(line)->itemAt(i)->textLength();
  absPos += pos;
  
  TxtCursor *_cursor = new TxtCursor((KTextObject*)this);
  _cursor->setPositionAbs(absPos);

  if (set)
    {
      TxtCursor *oldCursor = new TxtCursor((KTextObject*)this);
      oldCursor->setPositionAbs(txtCursor->positionAbs());
      
      txtCursor->setPositionAbs(absPos);
      
      drawLine = oldCursor->positionLine();
      drawParagraph = oldCursor->positionParagraph();
      updateCell(drawParagraph,0,false);
      drawLine = txtCursor->positionLine();
      drawParagraph = txtCursor->positionParagraph();
      updateCell(drawParagraph,0,false);
      drawParagraph = -1;
      drawLine = -1;
    }

  return _cursor;
}

