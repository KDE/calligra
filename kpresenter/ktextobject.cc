/******************************************************************/
/* KTextObject - (c) by Reginald Stadlbauer 1998                  */
/* Version: 0.0.2                                                 */
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
  _xpos = 0;
}

/*======================= defualt constructor =====================*/
TxtCursor::TxtCursor()
{
  // init the object
  absPos = 0;
  paragraph = 0;
  inParagraph = 0;
  line = 0;
  inLine = 0;
  objMaxPos = 0;
  txtObj = 0;
  _xpos = 0;
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
		      break;
		    }
		  l2 += linePtr->lineLength();
		}
	      break;
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
  objColor = red;
  objFont = QFont("utopia",20);
  objFont.setBold(true);
  objVertAlign = NORMAL;
  objText = "";
  _origSize = 12;
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
  objText.insert(0,text);
  _origSize = 10;
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
  // init the object
  objList.setAutoDelete(true);
 
  QFont _f = QFont("utopia",20);
  _f.setBold(true);

  if (init)
    append(" ",_f,red,TxtObj::NORMAL,TxtObj::SEPARATOR);

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
//       TxtObj *obj1 = new TxtObj();
//       TxtObj *obj2 = new TxtObj();
      
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
//       TxtObj *obj1 = new TxtObj();
//       TxtObj *obj2 = new TxtObj();
      
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
    append(new TxtObj(*l->itemAt(i)));
  
  return *this;
}

/*======================== operator += ===========================*/
TxtLine& TxtLine::operator+=(TxtLine *l)
{
  unsigned int i;

  for (i = 0;i < l->items();i++)
    append(new TxtObj(*l->itemAt(i)));
 
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
      obj1->append((const char*)itemAt(obj)->text().left(relPos));
      obj1->setFont(itemAt(obj)->font());
      obj1->setColor(itemAt(obj)->color());
      obj1->setType(itemAt(obj)->type());
      obj1->setVertAlign(itemAt(obj)->vertAlign());

      // insert the text after the object into the second object
      obj2->append((const char*)itemAt(obj)->text().right(itemAt(obj)->textLength()-relPos));
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
//   lin = new TxtLine();
//   lin->operator=(l);
  lineList.insert(i,l);
}

/*====================== append a line ===========================*/
void TxtParagraph::append(TxtLine *l)
{
//   lin = new TxtLine();
//   lin->operator=(l);
  lineList.append(l);
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

  lineList.at(lineList.count()-1)->append((const char*)to->text(),to->font(),
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
  //debug("break lines begin");
  //debug(" concat lines begin");
  // concat all lines to one
  linePtr = toOneLine();
  //debug(" concat lines end");

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
	  if (w + widthToNextSep(i) <= wid) // || obj->type() == TxtObj::SEPARATOR)
	    {
	      line->append(obj);
	      w += obj->width();
	    }
 
	  // if the object doesn't fit
	  else
 	    {
	      if (line->items() > 0) lineList.append(line);
	      line = new TxtLine();
	      w = 0;
	      
	      // insert all objects until the next separator
	      for (j = i;j < linePtr->items() && linePtr->itemAt(j)->type() != TxtObj::SEPARATOR;j++)
		{
		  obj = linePtr->itemAt(j);
		  line->append(obj);
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
  //debug("break lines end");
}

/*===================== break lines ==============================*/
void TxtParagraph::break_Lines(unsigned int wid)
{
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
	  if (w + charsToNextSep(i) <= wid) // || obj->type() == TxtObj::SEPARATOR)
	    {
	      line->append(obj);
	      w += obj->textLength();
	    }
 
	  // if the object doesn't fit
	  else
 	    {
	      if (line->items() > 0) lineList.append(line);
	      line = new TxtLine();
	      w = 0;
	      
	      // insert all objects until the next separator
	      for (j = i;j < linePtr->items() && linePtr->itemAt(j)->type() != TxtObj::SEPARATOR;j++)
		{
		  obj = linePtr->itemAt(j);
		  line->append(obj);
		  w += obj->textLength();
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

/*================== do composer mode ============================*/
void TxtParagraph::doComposerMode(QColor quoted_color,QFont quoted_font,QColor normal_color,QFont normal_font)
{
  QString str;
  bool quoted;
  int i;

  if (!lineList.isEmpty())
    {
      for (linePtr = lineList.first();linePtr != 0;linePtr = lineList.next())
	{
	  quoted = false;
	  if (linePtr->items() == 0) break;
	  
	  for (i = 0;i < linePtr->items();i++)
	    {
	      obj = linePtr->itemAt(i);
	      
	      if (obj->type() == TxtObj::SEPARATOR) continue;

	      str = obj->text().simplifyWhiteSpace();
	      if (str.length() == 0) continue; 

	      if (str.left(1) == ">" || str.left(1) == ":" || str.left(1) == "|")
		{
		  quoted = true;
		  break;
		}
	      else break;
	    }     

	  for (i = 0;i < linePtr->items();i++)
	    {
	      if (quoted)
		{
		  linePtr->itemAt(i)->setFont(quoted_font);
		  linePtr->itemAt(i)->setColor(quoted_color);
		}
	      else
		{
		  linePtr->itemAt(i)->setFont(normal_font);
		  linePtr->itemAt(i)->setColor(normal_color);
		}
	    }
	}
    }
}

/*============= with from an obj to next separator ===============*/
unsigned int TxtParagraph::widthToNextSep(unsigned int pos)
{
  unsigned int i,w = 0;
  
  for (i = pos;i < linePtr->items() && linePtr->itemAt(i)->type() != TxtObj::SEPARATOR;i++)
    w += linePtr->itemAt(i)->width();

  return w;
}

/*============= chars from an obj to next separator ==============*/
unsigned int TxtParagraph::charsToNextSep(unsigned int pos)
{
  unsigned int i,w = 0;
  
  for (i = pos;i < linePtr->items() && linePtr->itemAt(i)->type() != TxtObj::SEPARATOR;i++)
    w += linePtr->itemAt(i)->textLength();

  return w;
}

/******************************************************************/
/* class KTextObject - KTextObject                                */
/******************************************************************/

/*====================== constructor =============================*/
KTextObject::KTextObject(QWidget *parent=0,const char *name=0,ObjType ot=PLAIN,
			 unsigned int c=0,unsigned int r=0,int __width=0)
  : QTableView(parent,name)
{
  // init the objects
  doRepaints = true;

  composerMode = false;
  linebreak_width = __width;
  _width = 0;
  
  setFocusPolicy(QWidget::StrongFocus);
  setBackgroundColor(white);

  objEnumListType.type = NUMBER;
  objEnumListType.before = "";
  objEnumListType.after = ".";
  objEnumListType.start = 1;
  objEnumListType.font = QFont("times",12);
  objEnumListType.color = black;

  objUnsortListType.font = QFont("symbol",20);
  objUnsortListType.color = red;
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
  
  startCursor.setKTextObject((KTextObject*)this);
  stopCursor.setKTextObject((KTextObject*)this);

  setFocusPolicy(StrongFocus);

  drawLine = -1;
  drawParagraph = -1;
  drawBelow = false;

  currFont = QFont("utopia",20);
  currFont.setBold(true);
  currColor = red;

  setMouseTracking(true);
  mousePressed = false;
  drawPic = false;
  ystart = 0;
  drawSelection = false;

  selectionColor.operator=(QColor(0,255,255));
  
  TxtParagraph *para;
  para = new TxtParagraph(true);
  paragraphList.append(para);

  CellWidthHeight *wh = new CellWidthHeight;
  wh->wh = width();
  cellWidths.append(wh);
  setNumCols(1);

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
QPicture* KTextObject::getPic(int _x,int _y,int _w,int _h,bool presMode=false,int from=-1,int to=-1)
{
  QPainter p;

  drawPic = true;
  p.begin(&pic);
  p.setClipping(true);
  p.setClipRect(_x,_y,_w,_h);
  ystart = 0;
  
  from = min(from,to);
  to = max(from,to);

  if (paragraphList.count() == 1 && paragraphAt(0)->lines() == 1 && 
      paragraphAt(0)->lineAt(0)->items() == 1 && !presMode)
    {
      QFont _font("utopia",20);
      _font.setBold(true);
      p.setFont(_font);
      p.setPen(red);
      p.drawText(0,0,_w,_h,AlignLeft,i18n("Doubleclick to edit"));
    }
  else
    {
      for (unsigned i = 0;i < paragraphs();i++)
	{
	  if (from == -1 && to == -1 || from <= i && to >= i)
	    paintCell(&p,i,0);
	  ystart += cellHeights.at(i)->wh;
	}
    }

  p.end();
  ystart = 0;
  drawPic = false;
  return &pic;
}

/*====================== zoom text ===============================*/
void KTextObject::zoom(float _fakt)
{
  TxtObj *txtObj;
  TxtLine *txtLine;
  TxtParagraph *txtParagraph;
  unsigned int i,j,k;
  QFont font;

  for (i = 0;i < paragraphs();i++)
    {
      txtParagraph = paragraphAt(i);

      for (j = 0;j < txtParagraph->lines();j++)
	{
	  txtLine = txtParagraph->lineAt(j);

	  for (k = 0;k < txtLine->items();k++)
	    {
	      txtObj = txtLine->itemAt(k);
	      font.operator=(txtObj->font());
	      txtObj->setOrigSize(txtObj->font().pointSize());
	      font.setPointSize(((int)((float)font.pointSize() * _fakt)));
	      txtObj->setFont(font);
	    }
	}
    }
  
  font.operator=(objEnumListType.font);
  objEnumListType.ofont.operator=(font);
  font.setPointSize(((int)((float)font.pointSize() * _fakt)));
  objEnumListType.font.operator=(font);

  font.operator=(objUnsortListType.font);
  objUnsortListType.ofont.operator=(font);
  font.setPointSize(((int)((float)font.pointSize() * _fakt)));
  objUnsortListType.font.operator=(font);

  recalc();
  repaint(false);
}

/*==================== zoom to original size =====================*/
void KTextObject::zoomOrig()
{
  TxtObj *txtObj;
  TxtLine *txtLine;
  TxtParagraph *txtParagraph;
  unsigned int i,j,k;
  QFont font;

  for (i = 0;i < paragraphs();i++)
    {
      txtParagraph = paragraphAt(i);

      for (j = 0;j < txtParagraph->lines();j++)
	{
	  txtLine = txtParagraph->lineAt(j);

	  for (k = 0;k < txtLine->items();k++)
	    {
	      txtObj = txtLine->itemAt(k);
	      font.operator=(txtObj->font());
	      font.setPointSize(txtObj->origSize());
	      txtObj->setFont(font);
	    }
	}
    }

  objEnumListType.font.operator=(objEnumListType.ofont);
  objUnsortListType.font.operator=(objUnsortListType.ofont);

  recalc();
  repaint(false);
}

/*========================= add paragraph ========================*/
TxtParagraph* KTextObject::addParagraph()
{
  TxtParagraph *para;
  para = new TxtParagraph(false);
  paragraphList.append(para);

  wh = new CellWidthHeight;
  wh->wh = 0;
  cellHeights.append(wh);
  setNumRows(numRows()+1);

  return paragraphList.at(paragraphList.count()-1);
}

/*======================== clear =================================*/
void KTextObject::clear(bool init=true)
{
  if (init)
    {
      paragraphList.clear();
      cellWidths.clear();
      cellHeights.clear();
      
      TxtParagraph *para;
      para = new TxtParagraph(true);
      paragraphList.append(para);
      
      CellWidthHeight *wh = new CellWidthHeight;
      wh->wh = width();
      cellWidths.append(wh);
      setNumCols(1);
      
      wh = new CellWidthHeight;
      wh->wh = 0;
      cellHeights.append(wh);
      setNumRows(1);
      
      delete txtCursor;
      txtCursor = new TxtCursor((KTextObject*)this);
      
      recalc();
      repaint(false);
      updateTableSize();
      updateScrollBars();
    }
  else
    {
      paragraphList.clear();
      cellWidths.clear();
      cellHeights.clear();
      setNumRows(0);
      setNumCols(1);
      
      CellWidthHeight *wh = new CellWidthHeight;
      wh->wh = width();
      cellWidths.append(wh);
      setNumCols(1);
    }
}

/*==================== return ascii text =========================*/
QString KTextObject::toASCII(bool linebreak=true)
{
  QString str;

  TxtObj *txtObj;
  TxtLine *txtLine;
  TxtParagraph *txtParagraph;
  unsigned int i,j,k;
  QString chr;
  QString space = "                  ";

  for (i = 0;i < paragraphs();i++)
    {
      txtParagraph = paragraphAt(i);

      // TODO: Alignment

      switch (objType())
	{
	case PLAIN: break;
	case UNSORT_LIST: str += "- "; break;
	case ENUM_LIST:
	  {
	    if (objEnumListType.type == NUMBER)
	      chr.sprintf("%s%d%s ",(const char*)objEnumListType.before,i+objEnumListType.start,
		      (const char*)objEnumListType.after);
	    else
	      chr.sprintf("%s%c%s ",(const char*)objEnumListType.before,i+objEnumListType.start,
		      (const char*)objEnumListType.after);	      
	    str += chr;
	  } break;
	}

      for (j = 0;j < txtParagraph->lines();j++)
	{
	  txtLine = txtParagraph->lineAt(j);
	  if (j > 0)
	    {
	      switch (objType())
		{
		case PLAIN: break;
		case UNSORT_LIST: str += "  "; break;
		case ENUM_LIST: str += space.left(chr.length()); break;
		}
	    }

	  for (k = 0;k < txtLine->items();k++)
	    {
	      txtObj = txtLine->itemAt(k);
	      
	      str += txtObj->text();
	    }

	  if (linebreak) str += "\n";
	}
      if (!linebreak) str += "\n";
    }

  return str;
}

/*==================== return html text ==========================*/
QString KTextObject::toHTML(bool clean=false,bool onlyBody=false)
{
  QString str,str2;

  TxtObj *txtObj;
  TxtLine *txtLine;
  TxtParagraph *txtParagraph;
  unsigned int i,j,k;
  QFont font;
  QColor color;

  // head
  if (!onlyBody)
    {
      str = "<HTML>\n";
      str += "<HEAD>\n";
      str += "<TITLE></TITLE>\n";
      str += "</HEAD>\n";
    }

  // body
  if (!onlyBody)
    str += "<BODY>\n";

  if (!clean) str += "<!UNCLEAN FONTSIZES>\n";
  else str += "<!CLEAN FONTSIZES>\n";

  switch (objType())
    {
    case PLAIN: str += "<!PLAIN>\n";
      break;
    case UNSORT_LIST: 
      {
	font = objUnsortListType.font;
	color = objUnsortListType.color;

	str2.sprintf("%d",objUnsortListType.chr);
	
	str += "<!UNSORT_LIST ";
	str += "CHAR=";
	str += str2;
	str += " FACE=\"";
	str += font.family();
	str += "\" SIZE=";

	str2.sprintf("%d",font.pointSize());
	str += str2;
	
	str += " COLOR=\"";
	str += toHexString(color) + "\" ";
	if (font.bold()) str += "BOLD=1 ";
	else str += "BOLD=0 ";
	if (font.italic()) str += "ITALIC=1";
	else str += "ITALIC=0 ";
	if (font.underline()) str += "UNDERLINE=1>\n";
	else str += "UNDERLINE=0>\n";
	
	str += "<UL>\n";
      }
      break;
    case ENUM_LIST:
      {
	font = objEnumListType.font;
	color = objEnumListType.color;

	str2.sprintf("%d",objEnumListType.start);
	
	str += "<!ENUM_LIST ";
	str += "START=";
	str += str2;

	str2.sprintf("%d",objEnumListType.type);

	str += " TYPE=";
	str += str2;
	str += " BEFORE=\"";
	str += objEnumListType.before;
	str += "\" AFTER=\"";
	str += objEnumListType.after;
	str += "\" FACE=\"";
	str += font.family();
	str += "\" SIZE=";

	str2.sprintf("%d",font.pointSize());
	str += str2;
	
	str += " COLOR=\"";
	str += toHexString(color) + "\" ";
	if (font.bold()) str += "BOLD=1 ";
	else str += "BOLD=0 ";
	if (font.italic()) str += "ITALIC=1";
	else str += "ITALIC=0 ";
	if (font.underline()) str += "UNDERLINE=1>\n";
	else str += "UNDERLINE=0>\n";
	
	str += "<OL>\n";
       } break;
    }

  for (i = 0;i < paragraphs();i++)
    {
      txtParagraph = paragraphAt(i);

      switch (txtParagraph->horzAlign())
	{
	case TxtParagraph::LEFT:
	  str += "<P ALIGN=LEFT>\n"; 
	  break;
	case TxtParagraph::CENTER:
	  str += "<P ALIGN=CENTER>\n"; 
	  break;
	case TxtParagraph::RIGHT:
	  str += "<P ALIGN=RIGHT>\n"; 
	  break;
	case TxtParagraph::BLOCK:
	  str += "<P ALIGN=LEFT>\n"; 
	  break;
	}

      switch (objType())
	{
	case UNSORT_LIST:
	  str += "<LI>\n";
	  break;
	case ENUM_LIST:
	  str += "<LI>\n";
	  break;
	}

      for (j = 0;j < txtParagraph->lines();j++)
	{
	  txtLine = txtParagraph->lineAt(j);

	  for (k = 0;k < txtLine->items();k++)
	    {
	      txtObj = txtLine->itemAt(k);
	      if (k == 0 || font.operator!=(txtObj->font()) || color.operator!=(txtObj->color()))
		{
		  if (k > 0)
		    {
		      if (font.underline()) str += "</U>";
		      if (font.italic()) str += "</I>";
		      if (font.bold()) str += "</B>";
		      str += "</FONT>";
		    }

		  font = txtObj->font();
		  color = txtObj->color();
	      
		  if (!clean)
		    str2.sprintf("%d",font.pointSize());
		  else
		    str2.sprintf("%d",(int)((float)font.pointSize() / 6.0));

		  str += "<FONT FACE=\"";
		  str += font.family();
		  str += "\" SIZE=";
		  str += str2;
		  str += " COLOR=\"";
		  str += toHexString(color) + "\">";
		  if (font.bold()) str += "<B>";
		  if (font.italic()) str += "<I>";
		  if (font.underline()) str += "<U>";
		}
	      
	      QString *str3 = new QString();
	      str3->append(txtObj->text());
	      str3->replace(QRegExp("\x20"),"&nbsp;");
	      str += *str3;
	      delete str3;
	      
	    }
	  
	  if (font.underline()) str += "</U>";
	  if (font.italic()) str += "</I>";
	  if (font.bold()) str += "</B>";
	  str += "</FONT>";
	  str += "\n";

	}
    }

  switch (objType())
    {
    case UNSORT_LIST: str += "</UL>\n";
      break;
    case ENUM_LIST: str += "</OL>\n";
      break;
    }

  // foot
  if (!onlyBody)
    {
      str += "</BODY>\n";
      str += "</HTML>\n";
    }

  return str;
}

/*======================= save ASCII text ========================*/
void KTextObject::saveASCII(QString filename,bool linebreak=true)
{
  FILE *file;

  file = fopen((const char*)filename,"w");
  fprintf(file,(const char*)toASCII(linebreak));
  fclose(file);
}
/*======================= save HTML text =========================*/
void KTextObject::saveHTML(QString filename,bool clean=false)
{
  FILE *file;

  file = fopen((const char*)filename,"w");
  fprintf(file,(const char*)toHTML(clean));
  fclose(file);
}

/*======================== add text ==============================*/
void KTextObject::addText(QString text,QFont font,QColor color,
			  bool newParagraph=false,TxtParagraph::HorzAlign align=TxtParagraph::LEFT,bool _recalc=true,bool htmlMode=false)
{
  TxtObj *txtObj;
  TxtParagraph *txtParagraph;
  int i;
  int br = -1,sp = -1;

  if (htmlMode)
    text = text.replace(QRegExp("\x26nbsp\x3b")," ");

  if (newParagraph || paragraphs() < 1)
    {
      txtParagraph = addParagraph();
      txtParagraph->setHorzAlign(align);
    }
  else
    {
      txtParagraph = paragraphAt(paragraphs()-1);
      if (txtParagraph->lineAt(txtParagraph->lines()-1)->itemAt(txtParagraph->lineAt(txtParagraph->lines()-1)->items()-1)->type() == TxtObj::SEPARATOR)
 	txtParagraph->lineAt(txtParagraph->lines()-1)->deleteItem(txtParagraph->lineAt(txtParagraph->lines()-1)->items()-1);
    }

  txtObj = new TxtObj();
  txtObj->setFont(font);
  txtObj->setColor(color);
  txtObj->setType(TxtObj::TEXT);

  while (true)
    {
      sp = text.find(QRegExp("[\x20\t]"));
      br = text.find("\n");
      
      if (br == -1 && sp == -1) break;
      else
	{
	  if (br == -1 || (sp != -1 && sp < br))
	    {
	      if (isValid(text.left(sp)))
		{
		  if (!txtParagraph)
		    {
		      txtParagraph = addParagraph();
		      txtParagraph->setHorzAlign(align);
		    }
		  txtObj->append(text.left(sp));
		  txtParagraph->append(txtObj);
		} 
	      if (text.mid(sp,1) == " ")
		txtObj = new TxtObj(" ",font,color,TxtObj::NORMAL,TxtObj::SEPARATOR);
	      else
		txtObj = new TxtObj("    ",font,color,TxtObj::NORMAL,TxtObj::SEPARATOR);
	      
	      if (!txtParagraph)
		{
		  txtParagraph = addParagraph();
		  txtParagraph->setHorzAlign(align);
		}
	      txtParagraph->append(txtObj);
	      
	      txtObj = new TxtObj();
	      txtObj->setFont(font);
	      txtObj->setColor(color);
	      txtObj->setType(TxtObj::TEXT);

	      text.remove(0,sp+1);
	    }
	  
	  else if (sp == -1 || (br != -1 && br < sp))
	    {
	      if (isValid(text.left(br)))
		{
		  txtObj->append(text.left(br));
		  if (!txtParagraph)
		    {
		      txtParagraph = addParagraph();
		      txtParagraph->setHorzAlign(align);
		    }
		  txtParagraph->append(txtObj);
		} 
	      
	      txtObj = new TxtObj();
	      txtObj->setFont(font);
	      txtObj->setColor(color);
	      txtObj->setType(TxtObj::TEXT);

	      txtParagraph = 0;
	      
	      text.remove(0,br+1);
	    }
	}
    }

  if (text.length() > 0 && isValid(text))
    {
      if (!txtParagraph)
	{
	  txtParagraph = addParagraph();
	  txtParagraph->setHorzAlign(align);
	}
      txtObj->append(text.left(sp));
      txtParagraph->append(txtObj);
    } 
  
  if (!txtParagraph)
    {
      txtParagraph = addParagraph();
      txtParagraph->setHorzAlign(align);
    }

  txtObj = new TxtObj(" ",font,color,TxtObj::NORMAL,TxtObj::SEPARATOR);
  txtParagraph->append(txtObj);

  if (_recalc)
    {
      recalc();
      repaint(true);
      updateTableSize();
    }
}

/*======================= parse HTML =============================*/
void KTextObject::parseHTML(QString text)
{
  clear();

  text = text.simplifyWhiteSpace();

  int i;
  ParsedTag parsedTag;
  QFont font,oldFont;
  QColor color,oldColor;
  TxtParagraph::HorzAlign align = TxtParagraph::LEFT;
  bool newPara = false;
  Attrib *attribs;
  int tag_begin = -1,tag_end = -1;
  int font_fakt = 6;

  paragraphList.remove((unsigned int)0);

  while (true)
    {
      tag_begin = text.find("<");
      tag_end = text.find(">");
      
      if (tag_begin == -1 && tag_end == -1) break;
      else if (tag_begin < tag_end)
	{
	  if (isValid(text.left(tag_begin)))
	    {
	      addText(text.left(tag_begin),font,color,newPara,align,false,true);
	      newPara = false;
	    }

	  parsedTag = parseTag(text.mid(tag_begin + 1,tag_end - tag_begin - 1));

	  text.remove(0,tag_end + 1);

	  if (parsedTag.state == COMMENT)
	    {
	      parsedTag.additional = parsedTag.additional.simplifyWhiteSpace();
	
	      if (parsedTag.additional == "CLEAN FONTSIZES")
		font_fakt = 6;
	      else if (parsedTag.additional == "UNCLEAN FONTSIZES")
		font_fakt = 1;
	      else if (parsedTag.type == PLAIN_TEXT)
		setObjType(PLAIN);
	      else if (parsedTag.type == ENUMLIST)
		{
		  setObjType(ENUM_LIST);
		  if (!parsedTag.attribs.isEmpty())
		  {
		    objEnumListType.font.setBold(false);
		    objEnumListType.font.setUnderline(false);
		    objEnumListType.font.setItalic(false);
		    for (attribs = parsedTag.attribs.first();attribs != 0;attribs = parsedTag.attribs.next())
		      {
			if (attribs->key == START)
			  objEnumListType.start = atoi(attribs->value);
			else if (attribs->key == TYPE)
			  objEnumListType.type = atoi(attribs->value);
			else if (attribs->key == BEFORE)
			  objEnumListType.before = attribs->value;
			else if (attribs->key == AFTER)
			  objEnumListType.after = attribs->value;
			else if (attribs->key == FACE)
			  objEnumListType.font.setFamily(attribs->value.lower());
			else if (attribs->key == SIZE)
			  objEnumListType.font.setPointSize(atoi(attribs->value));
			else if (attribs->key == COLOR)
			  objEnumListType.color = hexStringToQColor(attribs->value);
			else if (attribs->key == B && atoi(attribs->value) == 1)
			  objEnumListType.font.setBold(true);
			else if (attribs->key == I && atoi(attribs->value) == 1)
			  objEnumListType.font.setItalic(true);
			else if (attribs->key == U && atoi(attribs->value) == 1)
			  objEnumListType.font.setUnderline(true);
		      }
		  }
		}
	      else if (parsedTag.type == UNSORTLIST)
		{
		  setObjType(UNSORT_LIST);
		  if (!parsedTag.attribs.isEmpty())
		  {
		    objUnsortListType.font.setBold(false);
		    objUnsortListType.font.setUnderline(false);
		    objUnsortListType.font.setItalic(false);
		    for (attribs = parsedTag.attribs.first();attribs != 0;attribs = parsedTag.attribs.next())
		      {
			if (attribs->key == CHAR)
			  objUnsortListType.chr = atoi(attribs->value);
			else if (attribs->key == FACE)
			  objUnsortListType.font.setFamily(attribs->value.lower());
			else if (attribs->key == SIZE)
			  objUnsortListType.font.setPointSize(atoi(attribs->value));
			else if (attribs->key == COLOR)
			  objUnsortListType.color = hexStringToQColor(attribs->value);
			else if (attribs->key == B && atoi(attribs->value) == 1)
			  objUnsortListType.font.setBold(true);
			else if (attribs->key == I && atoi(attribs->value) == 1)
			  objUnsortListType.font.setItalic(true);
			else if (attribs->key == U && atoi(attribs->value) == 1)
			  objUnsortListType.font.setUnderline(true);
		      } 
		  }
		}
	      continue;
	    }

	  switch (parsedTag.type)
	    {
	    case PARAGRAPH:
	      {
		newPara = true;
		if (!parsedTag.attribs.isEmpty() && parsedTag.state == BEGIN)
		  {
		    for (attribs = parsedTag.attribs.first();attribs != 0;attribs = parsedTag.attribs.next())
		      {
			if (attribs->key == ALIGN)
			  {
			    if (attribs->value == attrib_value_left)
			      align = TxtParagraph::LEFT;
			    else if (attribs->value == attrib_value_center)
			      align = TxtParagraph::CENTER;
			    else if (attribs->value == attrib_value_right)
			      align = TxtParagraph::RIGHT;
			  }
		      }
		  }
	      } break;
	    case FONT:
	      {
		if (parsedTag.state == BEGIN)
		  {
		    font.setBold(oldFont.bold());
		    font.setItalic(oldFont.italic());
		    font.setUnderline(oldFont.underline());
		    oldFont.operator=(font);
		    oldColor.operator=(color);
		    
		    if (!parsedTag.attribs.isEmpty())
		      {
			for (attribs = parsedTag.attribs.first();attribs != 0;attribs = parsedTag.attribs.next())
			  {
			    if (attribs->key == FACE)
			      font.setFamily(attribs->value.lower());
			    else if (attribs->key == SIZE)
				// ******* TODO: fonts with size +X, -X *********
			      font.setPointSize(atoi(attribs->value) * font_fakt);
			    else if (attribs->key == COLOR)
			      color = hexStringToQColor(attribs->value);
			  }
		      }
		  }
		else if (parsedTag.state == END)
		  {
		    oldFont.setBold(font.bold());
		    oldFont.setItalic(font.italic());
		    oldFont.setUnderline(font.underline());
		    font.operator=(oldFont);
		    color.operator=(oldColor);
		  }
	      } break;
	    case BOLD:
	      {
		if (parsedTag.state == BEGIN)
		  font.setBold(true);
		else if (parsedTag.state == END)
		  font.setBold(false);
	      } break;
	    case ITALIC:
	      {
		if (parsedTag.state == BEGIN)
		  font.setItalic(true);
		else if (parsedTag.state == END)
		  font.setItalic(false);
	      } break;
	    case UNDERLINE:
	      {
		if (parsedTag.state == BEGIN)
		  font.setUnderline(true);
		else if (parsedTag.state == END)
		  font.setUnderline(false);
	      } break;
	    case BREAK:
	      newPara = true;
	      break;
	    }
	}
    }	  

  recalc();
  repaint(false);
  updateTableSize();
} 

/*===================== open ascii file ==========================*/
void KTextObject::openASCII(QString filename)
{
  setCursor(waitCursor);
  doRepaints = false;

  QFile f(filename);
  QTextStream t(&f);
  QString s = "",tmp;  

  if (f.open(IO_ReadOnly))
    {
      while (!t.eof())
	{
	  
	  tmp = t.readLine();
	  if (!isValid(tmp)) tmp = " ";
	  tmp += "\n";
	  s.append(tmp);
	}
      f.close();   
    }
  
  addText(s,kapp->generalFont,kapp->windowTextColor,false,TxtParagraph::LEFT,false);
  
  doRepaints = true;

  recalc();
  repaint(false);
  updateTableSize();
  setCursor(ibeamCursor);
}

/*===================== open html file ===========================*/
void KTextObject::openHTML(QString filename)
{
  setCursor(waitCursor);
  doRepaints = false;

  QFile f(filename);
  QTextStream t(&f);
  QString s = "";  

  if (f.open(IO_ReadOnly))
    {
      while (!t.eof())
	{
	  s.append(t.readLine());
	  s.append("\n");
	}
      f.close();   
    }
  
  parseHTML(s);

  doRepaints = true;
  repaint(false);
  setCursor(ibeamCursor);
}

/*==================== set linebreak =============================*/
void KTextObject::setLineBreak(int _width)
{
  linebreak_width = _width;
  recalc();
  repaint(false);
}

/*====================== enable composer mode ====================*/
void KTextObject::enableComposerMode(QColor quoted_color,QFont quoted_font,QColor normal_color,QFont normal_font)
{
  _quoted_color = quoted_color;
  _quoted_font = quoted_font;
  _normal_color = normal_color;
  _normal_font = normal_font;
  composerMode = true;
  recalc(true);
  repaint(false);
}

/*====================== paint cell ==============================*/
void KTextObject::paintCell(class QPainter* painter,int row,int)
{
  if (!drawPic && !rowIsVisible(row)) return;

  //debug("Redraw Pargraph: %d",row);

  paragraphPtr2 = 0;
  paragraphPtr = 0;
  int x = xstart,y = ystart;
  int w = cellWidth(0),h = cellHeight(row);
  unsigned int i = 0,j = 0,chars = 0,len = 0;
  bool drawCursor = false,cursorDrawn = false;
  QPoint c1,c2;
  unsigned int scrBar = 0,wid = 0;
  char chr[11];
  int ry;
  QPixmap *pix;
  QPainter *p;

  // get pointer to the paragraph, which should be drwan
  paragraphPtr = paragraphList.at(row);
  
  // if the paragraph exists, draw it
  if (paragraphPtr)
    {      
      
      if (drawPic) p = painter;
      else p = new QPainter();

	 
      // object type
      switch (obType)
	{
	case PLAIN: case TABLE: break;
	case ENUM_LIST:
	  {
	    if (!drawPic)
	      {
		pix = new QPixmap(xstart,paragraphPtr->height());
		pix->fill(backgroundColor());
		p->begin(pix);
	      }

	    if (objEnumListType.type == NUMBER)
	      sprintf(chr,"%s%d%s",(const char*)objEnumListType.before,row+objEnumListType.start,
		      (const char*)objEnumListType.after);
	    else
	      sprintf(chr,"%s%c%s",(const char*)objEnumListType.before,row+objEnumListType.start,
		      (const char*)objEnumListType.after);	      
	    p->setFont(objEnumListType.font);
	    p->setPen(objEnumListType.color);
	    p->drawText(0,(!drawPic ? 0 : y) + paragraphPtr->lineAt(0)->ascent()-p->fontMetrics().ascent(),
			xstart,p->fontMetrics().height(),
			AlignRight,chr);	    

	    if (!drawPic)
	      {
		p->end();
		painter->drawPixmap(0,y,*pix);
	      }
	  } break;
	case UNSORT_LIST:
	  {
	    if (!drawPic)
	      {
		pix = new QPixmap(xstart,paragraphPtr->height());
		pix->fill(backgroundColor());
		p->begin(pix);
	      }

	    p->setFont(objUnsortListType.font);
	    p->setPen(objUnsortListType.color);
	    sprintf(chr,"%c",objUnsortListType.chr);
	    p->drawText(0,(!drawPic ? 0 : y) + paragraphPtr->lineAt(0)->ascent()-p->fontMetrics().ascent(),
			xstart,p->fontMetrics().height(),AlignRight,chr);

	    if (!drawPic)
	      {
		p->end();
		painter->drawPixmap(0,y,*pix);
	      }
	  } break;
	}

      // draw lines
      for (i = 0;i < paragraphPtr->lines();i++)
	{
	  linePtr = paragraphPtr->lineAt(i);
	  x = !drawPic ? 0 : xstart;

	  // check, if the line should be drawn
	  if (drawLine == -1 || drawParagraph == -1 ||
	      !drawBelow && drawLine == i && drawParagraph == row ||
	      drawBelow && (row > drawParagraph || row == drawParagraph && i >= drawLine))
	    {

	      if (!drawPic)
		{
		  if (w > 0 && linePtr->height() > 0)
		    {
		      pix = new QPixmap(cellWidth(0) - xstart,linePtr->height());
		      pix->fill(backgroundColor());
		      p->begin(pix);
		    }
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

		  // check, if cursor should be drawn - if yes calculate it
		  if (drawCursor = !drawPic && !cursorDrawn && showCursor() && txtCursor->positionParagraph() == row && 
		      txtCursor->positionLine() == i && txtCursor->positionInLine() >= chars && 
		      txtCursor->positionInLine() <= chars+len)
		    {
		      c1.setX(x + p->fontMetrics().width(objPtr->text().left(txtCursor->positionInLine()-chars)));
		      c1.setY(!drawPic? 0 : y);
		      c2.setX(c1.x());
		      c2.setY((!drawPic ? 0 : y) + linePtr->height());
		    }
		  
		  // draw Text
		  p->setPen(objPtr->color());
		  p->drawText(x,(!drawPic ? 0 : y) + linePtr->ascent()-objPtr->ascent(),w,linePtr->height(),AlignLeft,objPtr->text());

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
		      txtCursor->setXPos(c1.x());
		      rowYPos(row,&ry);
		      txtCursor->setYPos(c1.y() + ry);
		      txtCursor->setHeight(c2.y() - c1.y());
		      p->setPen(QPen(black,1,SolidLine));
		      if (p->font().italic()) c1.setX(c1.x() + (int)(((float)linePtr->height() / 3.732)));
		      p->drawLine(c1,c2);
		      cursorDrawn = true;
		    }

		  // draw selection
		  if (selectionInObj(row,i,j))
		    {
		      RasterOp ro = p->rasterOp();
		      p->setRasterOp(NotROP);
		      int sx,sw;
		      bool select_full = selectFull(row,i,j,sx,sw);
		      if (select_full)
			p->fillRect(x,(!drawPic ? 0 : y),objPtr->width(),linePtr->height(),black);
		      else
			p->fillRect(x + sx,(!drawPic ? 0 : y),sw,linePtr->height(),black);
		      p->setRasterOp(ro);
		    }

		  x += objPtr->width();
		  chars += len;
		}

	      if (!drawPic && p->isActive())
		{
		  p->end();
		  painter->drawPixmap(xstart,y,*pix);
		}
	    }

	  // calculate coordinates for the next line
	  y += linePtr->height();
	  x = xstart;
	  chars = 0;
	}
      if (!drawPic)
	{ 
	  delete p;
	  delete pix;
	}
    }
}

/*===================== paint event ==============================*/
void KTextObject::paintEvent(QPaintEvent *e)
{
  QPainter p;

  //debug("REPAINT EVENT - doRepaints = %d",doRepaints);

  // call superclass methode
  if (doRepaints)
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
  setAutoUpdate(false);
  resize(e->size());
  recalc();
  QTableView::resizeEvent(e);
  setAutoUpdate(true);
}

/*====================== key press event =========================*/
void KTextObject::keyPressEvent(QKeyEvent* e)
{
  //debug("keyPressEvent begin");
  if (showCursor())
    {
      unsigned int i;
      bool drawAbove = false;
      changedParagraphs.clear();
      bool drawFullPara = false;

      if (drawSelection)
	{
	  drawSelection = false;
	  redrawSelection(startCursor,stopCursor);
	}

      // always update the maximal cursor position when a key is pressed
      //txtCursor->setMaxPosition(textLength());
      
      TxtCursor *oldCursor = new TxtCursor((KTextObject*)this);
      oldCursor->setPositionAbs(txtCursor->positionAbs());
      
      // react on the pressed key
      switch (e->key())
	{
	case Key_Right: { txtCursor->charForward(); cursorChanged = true; } break;
	case Key_Left: { txtCursor->charBackward(); cursorChanged = true; } break;
	case Key_Up: { txtCursor->lineUp(); cursorChanged = true; } break;
	case Key_Down: { txtCursor->lineDown(); cursorChanged = true;} break;
	case Key_Return: case Key_Enter:
	  {
	    splitParagraph();
	    drawBelow = true;
	    drawAbove = true;
	    cursorChanged = true;
	  } break;
	case Key_Backspace:
	  {
	    if (kbackspace())
	      drawBelow = true;
	    else
	      {
		drawBelow = false;
		drawFullPara = true;
	      }
	    cursorChanged = true;
	  } break;
	case Key_Delete:
	  {
	    if (kdelete())
	      drawBelow = true;
	    else
	      {
		drawBelow = false;
		drawFullPara = true;
	      }
	    cursorChanged = true;
	  } break;
	default:
	  {
	    if (e->ascii() && e->ascii() > 31)
	      {
		if (insertChar(e->ascii()))
		  drawBelow = true;
		else
		  {
		    drawBelow = false;
		    drawFullPara = true;
		  }
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
	  if (drawFullPara) drawLine = -1;
	  updateCell(txtCursor->positionParagraph(),0,false);
	  drawLine = -1;
	  drawParagraph = -1;
	}
      
      // else redraw both lines
      else
	{
	  drawParagraph = txtCursor->positionParagraph();
	  drawLine = txtCursor->positionLine();
	  if (drawFullPara) drawLine = -1;
	  updateCell(txtCursor->positionParagraph(),0,false);
	  drawParagraph = oldCursor->positionParagraph();
	  drawLine = oldCursor->positionLine();
	  if (drawFullPara) drawLine = -1;
	  updateCell(oldCursor->positionParagraph(),0,false);
	  drawLine = -1;
	  drawParagraph = -1;
	}
      
      // if a line and everything below should be drawn
      if (drawBelow)
	{
	  oldCursor->calcPos();
	  TxtCursor *minCursor = txtCursor->minCursor(oldCursor);
	  drawParagraph = minCursor->positionParagraph();
	  drawLine = minCursor->positionLine();
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

      if (tableFlags() & Tbl_vScrollBar || tableFlags() & Tbl_hScrollBar) makeCursorVisible();

      changedParagraphs.clear();
      delete oldCursor;
    }
  //debug("keyPressEvent end");
}

/*====================== mouse press event ========================*/
void KTextObject::mousePressEvent(QMouseEvent *e)
{
  mousePressed = true;
  drawSelection = false;

  redrawSelection(startCursor,stopCursor);

  startCursor = getCursorPos(e->x(),e->y(),true,true);
  stopCursor = getCursorPos(e->x(),e->y(),true,true);
}

/*====================== mouse release event ======================*/
void KTextObject::mouseReleaseEvent(QMouseEvent *e)
{
  mousePressed = false;

  TxtCursor cursor = getCursorPos(e->x(),e->y(),true,false);
  if (cursor.positionAbs() > startCursor.positionAbs())
    stopCursor = cursor;
  else
    startCursor = cursor;
  
  if (stopCursor.positionAbs() != startCursor.positionAbs())
    drawSelection = true;

  redrawSelection(startCursor,stopCursor);
}

/*====================== mouse move event =========================*/
void KTextObject::mouseMoveEvent(QMouseEvent *e)
{
  if (mousePressed)
    {
      TxtCursor _startCursor = startCursor;
      TxtCursor _stopCursor = stopCursor;

      TxtCursor cursor = getCursorPos(e->x(),e->y(),true,false);
      if (cursor.positionAbs() > startCursor.positionAbs())
	stopCursor = cursor;
      else
	startCursor = cursor;

      TxtCursor c1 = *startCursor.minCursor(&stopCursor);
      TxtCursor c2 = *startCursor.maxCursor(&stopCursor);

      startCursor = c1;
      stopCursor = c2;

      if (stopCursor.positionAbs() != startCursor.positionAbs())
	drawSelection = true;

      redrawSelection(startCursor,stopCursor);

      if (_startCursor.positionAbs() < startCursor.positionAbs())
	redrawSelection(_startCursor,startCursor);

      if (_stopCursor.positionAbs() > stopCursor.positionAbs())
	redrawSelection(stopCursor,_stopCursor);
    }
}

/*=================== recalcualte everything =====================*/
void KTextObject::recalc(bool breakAllLines=true)
{
  switch (obType)
    {
    case PLAIN: xstart = 0; break;
    case ENUM_LIST:
      {
	QFontMetrics fm(objEnumListType.font);
	char chr[12];
	sprintf(chr,"%s99 %s",(const char*)objEnumListType.before,(const char*)objEnumListType.after);
	xstart = fm.width(chr);
      } break;
    case UNSORT_LIST:
      {
	QFontMetrics fm(objUnsortListType.font);
	xstart = fm.width(objUnsortListType.chr);
      } break;
    }
  
  cellWidths.at(0)->wh = width()-xstart;
  if (tableFlags() & Tbl_vScrollBar) cellWidths.at(0)->wh -= verticalScrollBar()->width(); 
  
  bool breakLines = true;
  if (!breakAllLines)
    breakLines = changedParagraphs.count() > 0 ? true : false;
  else breakLines = breakAllLines;

  for (paragraphPtr = paragraphList.first();paragraphPtr != 0;paragraphPtr = paragraphList.next())
    {
      // break the lines, and resize the cell
      if (breakAllLines || breakLines && changedParagraphs.containsRef((int*)paragraphList.at()))
	{
	  if (linebreak_width < 1)
	    {
	      paragraphPtr->breakLines(cellWidth(0));
	      cellWidths.at(0)->wh = width()-xstart;
	      if (tableFlags() & Tbl_vScrollBar) cellWidths.at(0)->wh -= verticalScrollBar()->width(); 
	    }	 
	  else
	    {
	      paragraphPtr->break_Lines(linebreak_width);
	      _width = max(_width,paragraphPtr->width());
	      cellWidths.at(0)->wh = _width;
	    }

	  if (composerMode)
	    paragraphPtr->doComposerMode(_quoted_color,_quoted_font,_normal_color,_normal_font);
	}

      cellHeights.at(paragraphList.at())->wh = paragraphPtr->height();
    }

  // calculate the new cursorposition
  txtCursor->calcPos();
  changedParagraphs.clear();
}

/*====================== split paragraph =========================*/
void KTextObject::splitParagraph()
{
  para1 = new TxtParagraph();
  para2 = new TxtParagraph();
  para3 = new TxtParagraph();
  unsigned int i;
  unsigned int para = txtCursor->positionParagraph();
  unsigned int line = txtCursor->positionLine();
  int p1 = para+1,p2 = para+2;
  obj = 0;
  TxtParagraph::HorzAlign ha;
 
  changedParagraphs.append((int*)para);

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

	  delete para2;
	  delete para3;
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
	  paragraphList.take(para);

	  // insert new paragraphs
	  if (para3->lines() > 0 && para3->lineAt(0)->items() > 0)
	    paragraphList.insert(para,para3);
	  if (para2->lines() > 0 && para2->lineAt(0)->items() > 0)
	    paragraphList.insert(para,para2);
	  if (para1->lines() > 0 && para1->lineAt(0)->items() > 0)
	    paragraphList.insert(para,para1);
	  
	  changedParagraphs.append((int*)p1);
	  changedParagraphs.append((int*)p2);

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
	  changedParagraphs.append((int*)p1);
	  delete para2;
	  delete para3;
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
 	  paragraphList.take(para);

	  // insert new paragraphs
	  para1->setHorzAlign(ha);
	  para2->setHorzAlign(ha);
	  para3->setHorzAlign(ha);
 	  paragraphList.insert(para,para3);
 	  paragraphList.insert(para,para2);
  	  paragraphList.insert(para,para1);
	  changedParagraphs.append((int*)p1);
	  changedParagraphs.append((int*)p2);

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
      paragraphList.take(para);

      // insert the two new paragraphs
      para1->setHorzAlign(ha);
      para2->setHorzAlign(ha);
      if (para2->lines() > 0 && para2->lineAt(0)->items() > 0)
	paragraphList.insert(para,para2);
      if (para1->lines() > 0 && para1->lineAt(0)->items() > 0)
	paragraphList.insert(para,para1);
      changedParagraphs.append((int*)p1);
      delete para3;

      // if the remembered object (type) is not a separator -> move cursor one position forward
      if (obj && obj->type() != TxtObj::SEPARATOR) txtCursor->charForward();
    }
  
  // insert the new row into the table
  wh = new CellWidthHeight;
  wh->wh = 0;
  cellHeights.append(wh);
  setNumRows(numRows()+1);
  
  // recalculate everything
  recalc(false);
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
bool KTextObject::kbackspace()
{
  unsigned int i;
  unsigned int para = txtCursor->positionParagraph();
  unsigned int line = txtCursor->positionLine();

  changedParagraphs.append((int*)para);
  int tmp = para-1;
  changedParagraphs.append((int*)tmp);
  tmp = para+1;
  changedParagraphs.append((int*)tmp);
 
  int _h = cellHeight(para);

  // if the cursor is at the first position of a paragraph -> this and the upper paragraph have to be joined
  if (txtCursor->positionLine() == 0 && txtCursor->positionInLine() == 0 && para > 0)
    {
      joinParagraphs(para,para-1);
      _h = -1;
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
      
      // delete the char in front of the cursor
      if (lin->getInObj(inLine) != -1) lin->backspaceChar(inLine);
      else if (lin->getAfterObj(inLine) != -1) lin->backspaceLastChar(lin->getAfterObj(inLine));
      
      // if the paragraph is empty now, delete it
      if (lin->lineLength() == 0) 
	{
	  paragraphList.remove(para);
	  cellHeights.remove(para);
	  setNumRows(numRows()-1);
	  _h = -1;
	}
    }

  // move cursor backward
  txtCursor->setMaxPosition(textLength());
  txtCursor->charBackward();
      
  // recalculate everything
  recalc(false);

  if (_h != cellHeight(para) || _h == -1) return true;
  return false;
}

/*========================= key delete ===========================*/
bool KTextObject::kdelete()
{
  unsigned int i;
  unsigned int para = txtCursor->positionParagraph();
  unsigned int line = txtCursor->positionLine();

  changedParagraphs.append((int*)para);
  int tmp = para-1;
  changedParagraphs.append((int*)tmp);
  tmp = para+1;
  changedParagraphs.append((int*)tmp);

  int _h = cellHeight(para);

  // if the cursor is at the last position of a paragraph -> this and the lower paragraph have to be joined
  if (txtCursor->positionLine() == paragraphAt(para)->lines()-1 && 
      txtCursor->positionInLine() == paragraphAt(para)->lineAt(line)->lineLength()-1)
    {
      if (para < paragraphs()-1) joinParagraphs(para,para+1);
      _h = -1;
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
	  _h = -1;
	}
    }

  txtCursor->setMaxPosition(textLength());
      
  // recalculate everything
  recalc(false);

  if (_h != cellHeight(para) || _h == -1) return true;
  return false;
}

/*========================= insert char ==========================*/
bool KTextObject::insertChar(char c)
{
  unsigned int i,objPos,w = 0;
  unsigned int para = txtCursor->positionParagraph();
  QString str;
  str = "";
  str.insert(0,c);

  int _h = cellHeight(para);

  lin = paragraphAt(para)->toOneLine();
  paragraphAt(para)->append(lin);
  txtCursor->calcPos();
  unsigned int inLine = txtCursor->positionInLine();
  lin = paragraphAt(para)->lineAt(0);
  
  changedParagraphs.append((int*)para);

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
	    lin->insert(objPos,obj);
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
	      lin->insert(objPos,obj);
	    }
	}
      } break;
    }

  txtCursor->setMaxPosition(textLength());
  txtCursor->charForward();

  // recalculate everything
  recalc(false);

  if (_h != cellHeight(para)) return true;
  return false;
}

/*====================== make cursor  vsisble ====================*/
void KTextObject::makeCursorVisible()
{
  bool _update = false;

  if (height() - 16 <= txtCursor->ypos() + txtCursor->height())
    {
      setYOffset((yOffset() + txtCursor->ypos() < totalHeight() - height() + 16 ? 
		  yOffset() + txtCursor->ypos() : totalHeight() - height() + 16));
      _update = true;
    }

  if (!_update && txtCursor->ypos() <= 0)
    {
      setYOffset(yOffset() - txtCursor->height());
      _update = true;
    }

  if (xOffset() + width() - 16 < txtCursor->xpos() || xOffset() > txtCursor->xpos())
    {
      setXOffset(txtCursor->xpos());
      _update = true;
    }

  if (_update)
    {
      updateTableSize();
      updateScrollBars();
    }
}

/*====================== set cursor psoition ====================*/
TxtCursor KTextObject::getCursorPos(int _x,int _y,bool set=false,bool redraw=false)
{
  unsigned int x = (unsigned int)_x + xOffset();
  unsigned int y = (unsigned int)_y + yOffset(),i,h = 0,w = 0,para,line,objPos,pos,absPos = 0;;
  TxtCursor tmpCursor((KTextObject*)this);

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
  
  if (set)
    {
      tmpCursor.setPositionAbs(txtCursor->positionAbs());
      
      txtCursor->setPositionAbs(absPos);
      
      if (redraw)
	{
	  drawLine = tmpCursor.positionLine();
	  drawParagraph = tmpCursor.positionParagraph();
	  updateCell(drawParagraph,0,false);
	  drawLine = txtCursor->positionLine();
	  drawParagraph = txtCursor->positionParagraph();
	  updateCell(drawParagraph,0,false);
	}

      drawParagraph = -1;
      drawLine = -1;
    }

  tmpCursor.setMaxPosition(txtCursor->maxPosition());
  tmpCursor.setPositionAbs(absPos);

  return tmpCursor;
}

/*====================== convert color to hex-string ============*/
QString KTextObject::toHexString(QColor c)
{
  int r,g,b;
  QString str;

  c.rgb(&r,&g,&b);

  str.sprintf("#%2X%2X%2X",r,g,b);

  return str;
}

/*====================== parse HTML tag =========================*/
KTextObject::ParsedTag KTextObject::parseTag(QString tag)
{
  ParsedTag parsedTag;
  int i;
  AttribList attribList;
  Attrib *attrib;
  QString str;
  char c[1];
  
  tag = tag.simplifyWhiteSpace();
  tag = tag.upper();
  tag += " ";
  sprintf(c,"%s",(const char*)tag.mid(0,1));

  if (c[0] == end_tag) parsedTag.state = END;
  else if (c[0] == comment_tag) parsedTag.state = COMMENT;
  else parsedTag.state = BEGIN;

  if (parsedTag.state != BEGIN)
    tag.remove(0,1);

  if (parsedTag.state == COMMENT)
    {
      parsedTag.additional = "";
      parsedTag.additional.append(tag);
      parsedTag.type = UNKNOWN_TAG;
    }

  if (tag.find(' ') != -1)
    str = tag.left(tag.find(' '));
  else
    str = tag;

  tag.remove(0,str.length());
  if (tag.length() > 0 && tag.mid(0,1) == " ")
    tag.remove(0,1);

  if (str == tag_html)
    parsedTag.type = HTML;
  else if (str == tag_head)
    parsedTag.type = HEAD;
  else if (str == tag_body)
    parsedTag.type = BODY;
  else if (str == tag_font)
    parsedTag.type = FONT;
  else if (str == tag_bold)
    parsedTag.type = BOLD;
  else if (str == tag_italic)
    parsedTag.type = ITALIC;
  else if (str == tag_underline)
    parsedTag.type = UNDERLINE;
  else if (str == tag_paragraph)
    parsedTag.type = PARAGRAPH;
  else if (str == tag_enumlist)
    parsedTag.type = ENUMLIST;
  else if (str == tag_unsortlist)
    parsedTag.type = UNSORTLIST;
  else if (str == tag_plain)
    parsedTag.type = PLAIN_TEXT;
  else if (str == tag_break || str == tag_h1 || str == tag_h2 || str == tag_h3 || str == tag_h4 || str == tag_h5 || str == tag_h6)
    parsedTag.type = BREAK;
  else
    parsedTag.type = UNKNOWN_TAG;
  
  str = "";

  if ((parsedTag.state == BEGIN && (parsedTag.type == FONT || parsedTag.type == PARAGRAPH)) ||
      (parsedTag.state == COMMENT && (parsedTag.type == UNSORTLIST || parsedTag.type == ENUMLIST)))
    {
      tag = tag.simplifyWhiteSpace();

      i = -1;
      while (true)
	{
	  i = tag.find(" = ");
	  if (i != -1)
	    tag.replace(i,3,"=");
	  else
	    {
	      i = tag.find("= ");
	      if (i != -1)
		tag.replace(i,2,"=");
	      else
		{
		  i = tag.find(" =");
		    if (i != -1)
		      tag.replace(i,2,"=");
		    else break;
		}
	    }
	}

      while (true)
	{
	  i = tag.find(QRegExp("[\x3d\x2d\x2b]"));
	  if (i != -1)
	    {
	      attrib = new Attrib;
	      attrib->value = "";
	      tag += " ";
	      
	      str = tag.left(i);
	      str = str.simplifyWhiteSpace();

	      if (str == attrib_face)
		attrib->key = FACE;
	      else if (str == attrib_char)
		attrib->key = CHAR;
	      else if (str == attrib_start)
		attrib->key = START;
	      else if (str == attrib_before)
		attrib->key = BEFORE;
	      else if (str == attrib_after)
		attrib->key = AFTER;
	      else if (str == attrib_type)
		attrib->key = TYPE;
	      else if (str == attrib_size)
		attrib->key = SIZE;
	      else if (str == attrib_color)
		attrib->key = COLOR;
	      else if (str == attrib_align)
		attrib->key = ALIGN;
	      else if (str == attrib_bold)
		attrib->key = B;
	      else if (str == attrib_underline)
		attrib->key = U;
	      else if (str == attrib_italic)
		attrib->key = I;
	      else
		attrib->key = UNKNOWN_ATTRIB;
	      
	      tag.remove(0,i);
	      
	      if (tag.left(1)[0] == operator_plus)
		attrib->op = PLUS;
	      else if (tag.left(1)[0] == operator_minus)
		attrib->op = MINUS;
	      else if (tag.left(1)[0] == operator_assign)
		attrib->op = ASSIGN;
	      
	      tag.remove(0,1);

	      if (tag.left(1) == "\"")
		{
		  tag.remove(0,1);
		  i = tag.find("\"");
		}
	      else
		i = tag.find(" ");


	      if (i != -1)
		{
		  attrib->value = tag.left(i);
		  tag.remove(0,i+1);
		  tag.simplifyWhiteSpace();
		  attribList.append(attrib);
		  
		  if (tag.length() == 0)
		    break;
		}
	      else
		break;
	    }
	  else 
	    break;
	}
    }

  parsedTag.attribs = attribList;
  return parsedTag;
}

/*====================== convert hex-string to color ============*/
QColor KTextObject::hexStringToQColor(QString str)
{
  int r,g,b;
  
  QColor c(str);

  return c;
}

/*============ checks, if a string contains chars ===============*/
bool KTextObject::isValid(QString str)
{
  if (str.length() == 0) return false;

  str = str.simplifyWhiteSpace();
  if (str.length() == 0) return false;

  for (int i = 0;i < str.length();i++)
    if (str.mid(i,1) != " ") return true;

  return false;
}

/*=================== simplify string ===========================*/
QString KTextObject::simplify(QString text)
{
  // should be rewritten
  bool hasSpace = (text.right(1) == " ");
  text = text.simplifyWhiteSpace();
  if (hasSpace) text += " ";

  return text;
}

/*============== check if selection is in a object ==============*/
bool KTextObject::selectionInObj(int para,int line,int item)
{
  if (drawSelection && !drawPic)
    {
      bool start = false,stop = false;

      if (startCursor.positionParagraph() < para || startCursor.positionParagraph() == para 
	  && startCursor.positionLine() < line) start = true;
      else if (startCursor.positionParagraph() == para && startCursor.positionLine() == line)
	{
	  int startInLine = startCursor.positionInLine();
	  int startItem;
	  
	  startItem = paragraphAt(para)->lineAt(line)->getInObj(startInLine);
	  if (startItem == -1)
	    {
	      startItem = paragraphAt(para)->lineAt(line)->getBeforeObj(startInLine);
	      if (startItem != -1);
	      else startItem = paragraphAt(para)->lineAt(line)->getAfterObj(startInLine);
	    }

	  if (startItem <= item) start = true;
	}

      if (stopCursor.positionParagraph() > para || stopCursor.positionParagraph() == para 
	  && stopCursor.positionLine() > line) stop = true;
      else if (stopCursor.positionParagraph() == para && stopCursor.positionLine() == line)
	{
	  int stopInLine = stopCursor.positionInLine();
	  int stopItem;
	  
	  stopItem = paragraphAt(para)->lineAt(line)->getInObj(stopInLine);
	  if (stopItem == -1)
	    {
	      stopItem = paragraphAt(para)->lineAt(line)->getBeforeObj(stopInLine);
	      if (stopItem != -1) stopItem--;
	      else stopItem = paragraphAt(para)->lineAt(line)->getAfterObj(stopInLine);
	    }

	  if (stopItem >= item) stop = true;
	}

      return (start && stop);
    }
  else return false;
}

/*===================== check if select full ====================*/
bool KTextObject::selectFull(int para,int line,int item,int &sx,int &sw)
{
  sx = 0;
  sw = paragraphAt(para)->lineAt(line)->itemAt(item)->width();
  bool select_full = true;
  int i,wid;

  if (startCursor.positionParagraph() == para && startCursor.positionLine() == line)
    {
      int startInLine = startCursor.positionInLine();
      int startItem = paragraphAt(para)->lineAt(line)->getInObj(startInLine);
      if (startItem != -1)
	{
	  wid = 0;
	  linePtr =  paragraphAt(para)->lineAt(line);
	  for (i = 0;i < item;i++)
	    {
	      wid += linePtr->itemAt(i)->textLength();
	      if (wid > startInLine) break;
	    }

	  if (wid < startInLine)
	    {
	      wid = startInLine - wid;
	      QFontMetrics fm(linePtr->itemAt(item)->font());
	      sx = fm.width(linePtr->itemAt(item)->text().left(wid));
	      sw -= sx;
	      select_full = false;
	    }
	}
    }

  if (stopCursor.positionParagraph() == para && stopCursor.positionLine() == line)
    {
      wid = 0;
      int stopInLine = stopCursor.positionInLine();
      int stopItem = paragraphAt(para)->lineAt(line)->getInObj(stopInLine);
      if (stopItem != -1)
	{
	  linePtr =  paragraphAt(para)->lineAt(line);
	  for (i = 0;i < item;i++)
	    {
	      wid += linePtr->itemAt(i)->textLength();
	      if (wid > stopInLine) break;
	    }

	  if (wid < stopInLine)
	    {
	      wid = stopInLine - wid;
	      QFontMetrics fm(linePtr->itemAt(item)->font());
	      sw = fm.width(linePtr->itemAt(item)->text().left(wid));
	      if (sx != 0) sw -= sx;
	      select_full = false;
	    }
	}
    }

  return select_full;
}

/*====================== redraw selection =======================*/
void KTextObject::redrawSelection(TxtCursor _startCursor,TxtCursor _stopCursor)
{
  // should be rewritten - this is too slow
  drawLine = -1;
  drawParagraph = -1;
  for (int i = _startCursor.positionParagraph(); i <= _stopCursor.positionParagraph();i++)
    updateCell(i,0,false);
}
