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

#include "box.h"
#include "kformula.h"
#include <stdio.h>
#include <ctype.h>

template class QList<box>;

using namespace Box;

//----------------------CONSTRUCTORS AND DESTRUCTORS------------
//most things they initialize get changed before they are used anyway.

box::box()
{
  b1 = b2 = parent = NULL;
  relx = rely = 0;
}

box::box(int setNum)
{
  type = TEXT;
  b1 = b2 = NULL;
  text.sprintf("%d", setNum);
  fontsize = DEFAULT_FONT_SIZE;
  relx = rely = 0;
  dirty = 1;
  parent = NULL;
}

box::box(QString setText)
{
  type = TEXT;
  b1 = b2 = NULL;
  text = setText;
  fontsize = DEFAULT_FONT_SIZE;
  relx = rely = 0;
  dirty = 1;
  parent = NULL;
}

box::box(int setType, box * setB1, box * setB2)
{
  type = setType;
  b1 = setB1;
  b2 = setB2;
  if(!IS_REFERENCE(setType)) {
    if(b1 != NULL) b1->parent = this;
    if(b2 != NULL) b2->parent = this;
  }
  else {
    if(b1 != NULL) {
      b1->refParents.append(this);
    }
  }
  fontsize = DEFAULT_FONT_SIZE;
  relx = rely = 0;
  dirty = 1;
  parent = NULL;
}

box::~box()
{
  if(!IS_REFERENCE(type)) {
    if(b1 != NULL) b1->parent = NULL;
    if(b2 != NULL) b2->parent = NULL;
  }
  else {
    if(b1 != NULL) b1->refParents.removeRef(this);
  }
  b1 = b2 = NULL;
  if(parent) {
    if(parent->b1 == this) parent->b1 = NULL;
    if(parent->b2 == this) parent->b2 = NULL;
    parent->makeDirty();
  }

  for(unsigned int i = 0; i < refParents.count(); ++i) {
    refParents.at(i)->b1 = NULL;
    refParents.at(i)->makeDirty();
  }
}

//--------------------------------MAKE DIRTY----------------------
//sets dirty to one and calls makeDirty on its parent
void box::makeDirty()
{
  if(dirty) return;
  dirty = 1;

  if(parent) parent->makeDirty();
  //if ( b1 ) b1->makeDirty();
  //if ( b2 ) b2->makeDirty();
  unsigned int i;

  for(i = 0; i < refParents.count(); ++i) {
    refParents.at(i)->makeDirty();
  }
}

//--------------------------------SET TEXT-----------------------
void box::setText(QString newText)
{
  text = newText;
  makeDirty();
}

//--------------------------------OFFSET-------------------------------
//shifts everything in the box--usually for determining position
//within relative to adjacent boxes.
void box::offset(int xoffs, int yoffs)
{
  relx += xoffs;
  rely += yoffs;
  b1x += xoffs;
  b1y += yoffs;
  b2x += xoffs;
  b2y += yoffs;
  rect.moveBy(xoffs, yoffs);
}

//-------------------------------CALCULATE-------------------------
//determine the size of the rectangle by first calculating the size
//of the children and then arranging them.
//Making it work was not easy, but making it readable is
//far harder.
//Now the 0 of the bounding rect represents the "midline" to which
//everything is valigned-- Andrea Rizzi's idea
void box::calculate(QPainter &p, int setFontsize, QFont *f_, QColor *, QColor *)
{
  if(!dirty) return;

  relx = rely = 0;
  b1x = b1y = b2x = b2y = 0;
  if(setFontsize == -1) setFontsize = p.font().pointSize();
  fontsize = setFontsize;
  if ( f_ )
      fontsize = f_->pointSize();
  if(fontsize < MIN_FONT_SIZE) fontsize = MIN_FONT_SIZE;

  QRect tmp1, tmp2; //temporary variables
  int i;
  QFont f; //temporary to preserve the current font of the painter.

  f = p.font();
  if ( f_ )
      f = *f_;

  lastFont = f;
  lastFont.setPointSize(fontsize);
  p.setFont(lastFont);
  QFontInfo font_info_tmp(lastFont);
  fontsize = font_info_tmp.pointSize();

  QFontMetrics fm = p.fontMetrics();
  if ( f_ )
      fm = QFontMetrics( *f_ );

  if(IS_REFERENCE(type)) {
    b1->calculate(p, fontsize);
    rect = b1->getRect();
  }
  else
  switch(type)
    {
    case TEXT:
      if(text.length() == 0) { //empty box will have a little square drawn
	// the size of + is the size of the square.
	rect = fm.boundingRect("+");
	rect.setRect(-SPACE, rect.y(), SPACE * 2, rect.height());
	rect.moveBy(0, -rect.center().y());

	//operators where the left box is optional don't get a little
	//square drawn.  These are the -, the parentheses, the square
	//root, and the absolute value.
	if(parent && (KFormula::delim() +
		      QChar(int(SQRT))).contains(QChar(parent->type)) &&
	   parent->b1 == this) {
	  rect.setX(0);
	  rect.setWidth(1);
	}
	if(parent && (KFormula::intext() + QChar(int(CAT))).
	   contains(QChar(parent->type))) {
	  rect.setX(0);
	  rect.setWidth(1);
	}
	
	break;
      }
      //if the box is not empty:
      if(text[text.length() - 1].isSpace()) {
	// we have a space at the end so count it!
	QString temptext = text;

	temptext.append(".");

	rect = fm.boundingRect(temptext);

	rect.setRight(rect.right() - fm.boundingRect(".").width() - 1);
      }
      else {
	rect = fm.boundingRect(text);
      }
      rect.moveBy(0, -fm.boundingRect("+").center().y());
      break;

    case SYMBOL:
      rect = symbolRect(p, (SymbolType)(text[0].unicode()), fontsize);
      break;

      //all the operators which just get drawn along with the text:
      //just shift the offsets
    case PLUS:
    case MINUS:
    case EQUAL:
    case MORE:
    case LESS:
      b1->calculate(p, fontsize);
      b2->calculate(p, fontsize);
      rect = b1->getRect();
      tmp1 = fm.boundingRect( QChar( type ) );
      relx += rect.right() + 1; //where the operator will be drawn
      rely += -fm.boundingRect("+").center().y();
      if(rect.width() == 1) relx -= tmp1.left() - rect.left();
      tmp1.moveBy(relx, rely);
      rect = rect.unite(tmp1);
      b2x += -b2->getRect().left() + rect.right() + SPACE - 1;
      tmp1 = b2->getRect();
      tmp1.moveBy(b2x, 0);
      if(tmp1.width() > 1) rect = rect.unite(tmp1);

      break;

      //easy:
    case CAT:
      b1->calculate(p, fontsize);
      b2->calculate(p, fontsize);
      rect = b1->getRect();
      b2x += -b2->getRect().left() + rect.right() + SPACE;
      tmp1 = b2->getRect();
      tmp1.moveBy(b2x, 0);
      rect = rect.unite(tmp1);
      break;

      //basically same thing as +-=<> but since it will be a dot,
      //we make room differently
#define DOTSIZE (fontsize / 5)
    case TIMES:
      b1->calculate(p, fontsize);
      b2->calculate(p, fontsize);
      rect = b1->getRect();

      tmp1 = QRect(1, -DOTSIZE / 2, DOTSIZE + 1, DOTSIZE);
      relx += rect.right() + SPACE + 1; //where the dot will be drawn.
      tmp1.moveBy(relx, rely);
      rect = rect.unite(tmp1);
      b2x += -b2->getRect().left() + rect.right() + SPACE;
      tmp1 = b2->getRect();
      tmp1.moveBy(b2x, 0);
      rect = rect.unite(tmp1);
      break;

    case SLASH:
      b1->calculate(p, fontsize);
      b2->calculate(p, fontsize);
      tmp1 = b1->getRect();
      tmp2 = b2->getRect();

      relx += tmp1.right() + SPACE;
      b2x += -tmp2.left() + tmp1.right() + SPACE * 3 +
	QMAX(tmp1.height(), tmp2.height()) / 2;
      tmp2.moveBy(b2x, 0);

      rect = tmp1.unite(tmp2);

      rect.setRect(rect.x(), rect.y() - SPACE,
		   rect.width(), rect.height() + SPACE * 2);

      break;

      //for the bar--yes, I know it's unreadable
    case DIVIDE:
      b1->calculate(p, fontsize * 19 / 20);
      b2->calculate(p, fontsize * 19 / 20);
      rect = b1->getRect();
      tmp1 = b2->getRect();
      rely += rect.bottom() + SPACE; //where the bar will be drawn
      tmp2 = tmp1;
      tmp1.moveBy(0, rect.bottom() - tmp1.top() + SPACE * 3);
      b2x += rect.center().x() - tmp1.center().x();
      b2y += rect.bottom() - tmp2.top() + SPACE * 3;
      tmp2.moveBy(b2x, b2y);
      rect = rect.unite(tmp2);
      rect.setRect(rect.x() - SPACE * 2, rect.y(),
		   rect.width() + SPACE * 4, rect.height());
      //the next line sets the position of the bar to the vertical center.
      offset(0, -rely - 2);
      break;

      // for the superscript
    case POWER:
    case SUB:
    case LSUB:
    case LSUP:
      if(type == POWER || type == SUB) {
	b1->calculate(p, fontsize);
	b2->calculate(p, fontsize * 3 / 4);
      }
      else {
	b2->calculate(p, fontsize);
	b1->calculate(p, fontsize * 3 / 4);
      }

      {
	box *tmp = b1;
	if(type == LSUB || type == LSUP) tmp = b2;

	while(tmp->type == POWER || tmp->type == SUB ||
	      tmp->type == LSUB || tmp->type == LSUP ||
	      tmp->type == ABOVE || tmp->type == BELOW) {

	  if((tmp->type == POWER || tmp->type == SUB ||
	      tmp->type == ABOVE || tmp->type == BELOW) &&
	     tmp->b1 != NULL && tmp->type != type) {
	    tmp = tmp->b1;
	    continue;
	  }

	  if((tmp->type == LSUB || tmp->type == LSUP) &&
	     tmp->b2 != NULL && tmp->type != type) {
	    tmp = tmp->b2;
	    continue;
	  }

	  break;
	}

	rect = tmp->getRect();
      }

      if(type == POWER) {
	tmp1 = b2->getRect();
	b2x += -tmp1.left() + rect.right() + SPACE;
	b2y += rect.top() - tmp1.bottom() + QMIN(SPACE, rect.height() / 3);
	tmp1.moveBy(b2x, b2y);
      }

      if(type == SUB) {
	tmp1 = b2->getRect();
	b2x += -tmp1.left() + rect.right() + SPACE;
	b2y += rect.bottom() - tmp1.top() - QMIN(SPACE, rect.height() / 3);
	tmp1.moveBy(b2x, b2y);
      }

      if(type == LSUP) {
	tmp1 = b1->getRect();
	b1x += rect.left() - tmp1.right() - SPACE;
	b1y += rect.top() - tmp1.bottom() + QMIN(SPACE, rect.height() / 3);
	tmp1.moveBy(b1x, b1y);
      }

      if(type == LSUB) {
	tmp1 = b1->getRect();
	b1x += rect.left() - tmp1.right() - SPACE;
	b1y += rect.bottom() - tmp1.top() - QMIN(SPACE, rect.height() / 3);
	tmp1.moveBy(b1x, b1y);
      }

      rect = rect.unite(tmp1);

      if(type == LSUP || type == LSUB)
	rect = rect.unite(b2->getRect());
      else
	rect = rect.unite(b1->getRect());

      break;

    case ABOVE: // the smaller one above the normal one
      b1->calculate(p, fontsize);
      b2->calculate(p, fontsize * 3 / 4);
      rect = b1->getRect();
      tmp1 = b2->getRect();
      b2y += -tmp1.bottom() + rect.top() - SPACE;
      b2x += -tmp1.center().x() + rect.center().x();
      tmp1.moveBy(b2x, b2y);
      rect = rect.unite(tmp1);
      rect.setRect(rect.x() - SPACE / 2, rect.y(),
		   rect.width() + SPACE, rect.height());
      break;

    case BELOW: // the smaller one below the normal one
      b1->calculate(p, fontsize);
      b2->calculate(p, fontsize * 3 / 4);
      rect = b1->getRect();
      tmp1 = b2->getRect();
      b2y += -tmp1.top() + rect.bottom() + SPACE;
      b2x += -tmp1.center().x() + rect.center().x();
      tmp1.moveBy(b2x, b2y);
      rect = rect.unite(tmp1);
      rect.setRect(rect.x() - SPACE / 2, rect.y(),
		   rect.width() + SPACE, rect.height());
      break;

      //we must make room for the radical.
    case SQRT:
      b2->calculate(p, fontsize);
      rect = b2->getRect();
      i = QMIN((SPACE + rect.height()) / 2, SPACE * 8);
      rect.setRect(rect.x() - i - SPACE * 2, rect.y() - SPACE,
		   rect.width() + i + SPACE * 3,
		   rect.height() + SPACE * 2);
      if(b1 != NULL) { //for nth root
	b1->calculate(p, fontsize * 3 / 5); //real small font
	tmp1 = b1->getRect();
	b1x += -tmp1.right() + rect.x() + SPACE + i / 3;
	b1y += -tmp1.bottom() + rect.center().y() - SPACE;
	tmp1.moveBy(b1x, b1y);
	rect = rect.unite(tmp1);
      }
      break;

    case PAREN:
      b2->calculate(p, fontsize);
      rect = b2->getRect();
      rect.setRect(rect.x(), QMIN(rect.top(), -rect.bottom()),
		   rect.width(), QMAX(-rect.top(), rect.bottom()) * 2);
      b2x += QMAX(fontsize / 4 + rect.height() / 10, SPACE);
      rect.setRect(rect.x() - SPACE / 2, rect.y() - b2x * 3 / 4,
		   rect.width() + b2x * 2 + SPACE,
		   rect.height() + b2x * 3 / 2);

      break;

    case ABS:
      b2->calculate(p, fontsize);
      rect = b2->getRect();
      rect.setRect(rect.x(), QMIN(rect.top(), -rect.bottom()),
		   rect.width(), QMAX(-rect.top(), rect.bottom()) * 2);
      b2x += SPACE + 2;
      rect.setRect(rect.x(), rect.y() - b2x,
		   rect.width() + b2x * 2, rect.height() + b2x * 2);

      break;

    case BRACKET:
      b2->calculate(p, fontsize);
      rect = b2->getRect();
      if(rect.width() < SPACE * 2 + 2) {
	QPoint c = rect.center();

	rect.setWidth(SPACE * 2 + 2);
	rect.moveCenter(c);
      }
	
      rect.setRect(rect.x(), QMIN(rect.top(), -rect.bottom()),
		   rect.width(), QMAX(-rect.top(), rect.bottom()) * 2);
      b2x += SPACE + 2;
      rect.setRect(rect.x(), rect.y() - b2x,
		   rect.width() + b2x * 2, rect.height() + b2x * 2);

      break;

    }

  //make sure we are at least as tall as a +
  if(rect.height() < fm.boundingRect("+").height())
    rect.setTop(rect.bottom() - fm.boundingRect("+").height());

  dirty = 0;

  p.setFont(f);

  return;
}

//-----------------------------GET LAST RECT-------------------------
//returns the last rectangle where this box was drawn
QRect box::getLastRect()
{
  QRect tmp = rect;
  tmp.moveBy(offsx - 1, offsy - 1);
  tmp.setWidth(tmp.width() + 2);
  tmp.setHeight(tmp.height() + 2);
  return tmp;
}

//--------------------------------DRAW------------------------------
//first draws its children and then itself.
//x and y are relative coordnates.
void box::draw(QPainter &p, int x, int y, QFont *f_, QColor *bc, QColor *fc)
{

  if(dirty) calculate(p); //just in case
  offsx = x;
  offsy = y;

  QFont f;
  f = p.font();
  if ( f_ )
      f = *f_;
  QPen oldPen = p.pen();
  //if ( fc )
  //    oldPen = QPen( *fc );

  p.setFont(lastFont);
  if ( fc )
      p.setPen( *fc );

  QFontMetrics fm = p.fontMetrics();

  QRect tmp;
  int i;
  QString tmpstr; //a one-char string for operators
  tmpstr[1] = 0;

#if 0  // draw bounding boxes?
  p.setPen(QColor(255, 0, 0));
  QRect tmpxx = rect;
  tmpxx.moveBy(x, y);
  p.drawRect(tmpxx);
  p.setPen(QColor(0, 0, 0));
#endif

  //Draw the children:
  if(b1 != NULL) b1->draw(p, x + b1x, y + b1y, f_, bc, fc );
  if(b2 != NULL) b2->draw(p, x + b2x, y + b2y, f_, bc, fc );

  if(IS_REFERENCE(type)) { //nothing
  }
  else
  switch(type) {
  case TEXT:
    if(text.length() == 0) { //empty
      //left operands parentheses and square roots as well as
      //all operands of intext operators are not displayed as boxes
      if(parent && (KFormula::delim() +
		    QChar(int(SQRT))).contains(QChar(parent->type)) &&
	 parent->b1 == this) break;
      if(parent && (KFormula::intext() + QChar(int(CAT))).
	 contains(QChar(parent->type))) break;

      p.drawRect(x - SPACE, y + rect.center().y() - SPACE,
		 SPACE * 2, SPACE * 2);
      break;
    }

    //if not empty, draw the text (offset to the baseline).
    p.drawText(x, y - fm.boundingRect("+").center().y(), text);
    break;

  case SYMBOL:
    p.setBrush( p.pen().color() );
    drawSymbol(p, (SymbolType)(text[0].unicode()), fontsize, x, y);
    p.setBrush(Qt::NoBrush);
    break;

  case PLUS:
  case MINUS:
  case EQUAL:
  case MORE:
  case LESS:
    tmpstr[0] = type;
    p.drawText(x + relx, y + rely, tmpstr); //just draw the operator
    break;

  case CAT: //children are already drawn.
    break;

  case TIMES: //just draw the filled dot
    p.setBrush( p.pen().color() );
    p.drawEllipse(x + relx, y + rely - DOTSIZE / 2, DOTSIZE, DOTSIZE);
    p.setBrush( Qt::NoBrush );
    break;

  case SLASH:
    if(fontsize >= (DEFAULT_FONT_SIZE + MIN_FONT_SIZE) / 2)
      p.setPen(QPen(p.pen().color(), 2));
    else
      p.setPen(QPen(p.pen().color(), 1));  // mainly for the printer

    p.drawLine(x + relx, rect.bottom() + y,
	       x + relx + rect.height() / 2, rect.top() + y);

    p.setPen(oldPen);

    break;

  case DIVIDE: //draw the bar whose thickness depends on fontsize
    if(fontsize >= (DEFAULT_FONT_SIZE + MIN_FONT_SIZE) / 2)
      p.setPen(QPen(p.pen().color(), 2));
    else
      p.setPen(QPen(p.pen().color(), 1));  // mainly for the printer

    p.drawLine(rect.left() + x, y + rely + 2,
	       rect.right() + x, y + rely + 2);
    p.setPen(oldPen);
    break;

  case POWER: //already children drawn
  case SUB:
  case LSUB:
  case LSUP:
  case ABOVE:
  case BELOW:
    break;

  case SQRT:
    tmp = b2->getRect();
    tmp.moveBy(b2x, b2y);
    i = QMIN((SPACE + tmp.height()) / 2, SPACE * 8);

    p.setPen(QPen(p.pen().color(), 1));

    //The overline:
    p.drawLine(x + tmp.left() - SPACE, tmp.top() + y - SPACE,
	       rect.right() + x, tmp.top() + y - SPACE);

    //The rising diagonal ("/"):
    p.drawLine(x + tmp.left() - SPACE, tmp.top() + y - SPACE,
	       x + tmp.left() + 1 - SPACE - i * 2 / 3, rect.bottom() + y);

    p.setPen(QPen(p.pen().color(), 2));

    //The falling diagonal ("\"):
    p.drawLine(x + tmp.left() - SPACE - i * 2 / 3, rect.bottom() + y,
           x + tmp.left() - SPACE - i, tmp.center().y() + y);

    p.setPen(QPen(p.pen().color(), 1));

    //The little tail:
    p.drawLine(x + tmp.left() - SPACE - i, tmp.center().y() + y,
	       x + tmp.left() - SPACE * 2 - i,
	       tmp.center().y() + y + SPACE / 2);

    p.setPen(oldPen);

    break;

  case PAREN: //the parentheses are ellipse arcs.
    p.setPen(QPen(p.pen().color(), 2));

    p.drawArc(SPACE / 2 + rect.left() + x, rect.top() + y,
	      b2x * 2, rect.height(),
	      100 * 16, 160 * 16);

    p.drawArc(rect.right() - b2x * 2 + x, rect.top() + y,
	      b2x * 2, rect.height(), -80 * 16, 160 * 16);

    p.setPen(oldPen);
    break;

  case ABS:
    if(fontsize >= (DEFAULT_FONT_SIZE + MIN_FONT_SIZE) / 2)
      p.setPen(QPen(p.pen().color(), 2));
    else
      p.setPen(QPen(p.pen().color(), 1));

    p.drawLine(rect.left() + x + 2, rect.top() + y,
	       rect.left() + x + 2, rect.bottom() + y);

    p.drawLine(rect.right() + x - 2, rect.top() + y,
	       rect.right() + x - 2, rect.bottom() + y);

    p.setPen(oldPen);
    break;

  case BRACKET:
    if(fontsize >= (DEFAULT_FONT_SIZE + MIN_FONT_SIZE) / 2)
      { p.setPen(QPen(p.pen().color(), 2)); i = 1; }
    else
      { p.setPen(QPen(p.pen().color(), 1)); i = 0; }

    //i is whether to offset the top line one pixel lower

    p.drawLine(rect.left() + x + 2, rect.top() + y,
	       rect.left() + x + 2, rect.bottom() + y);

    p.drawLine(rect.left() + x + 2 + SPACE + 1, rect.top() + y + i,
	       rect.left() + x + 2, rect.top() + y + i);
    p.drawLine(rect.left() + x + 2 + SPACE + 1, rect.bottom() + y - 1,
	       rect.left() + x + 2, rect.bottom() + y - 1);

    p.drawLine(rect.right() + x - 2, rect.top() + y,
	       rect.right() + x - 2, rect.bottom() + y);

    p.drawLine(rect.right() + x - 2 - SPACE - 1, rect.top() + y + i,
	       rect.right() + x - 2, rect.top() + y + i);
    p.drawLine(rect.right() + x - 2 - SPACE - 1, rect.bottom() + y - 1,
	       rect.right() + x - 2, rect.bottom() + y - 1);

    p.setPen(oldPen);
    break;

  }

  p.setFont(f);

}

//-------------------------GET CURSOR POS-----------------------------
//given a charinfo, this calls itself on its children
//until one matches the i.where.  Then it uses i.posinbox
//to figure out where the cursor should be drawn (and how tall
//it should be).
QRect box::getCursorPos(charinfo i, int x, int y)
{
  QRect tmp;

  QFontMetrics fm(lastFont);

  //if i.where is not this, try the children.
  if(i.where != this) {
    if(b1 != NULL) tmp = b1->getCursorPos(i, x + b1x, y + b1y);
    if(!tmp.isEmpty()) return tmp;
    if(b2 != NULL) tmp = b2->getCursorPos(i, x + b2x, y + b2y);
    return tmp; //tmp is empty if all children returned empty
                //or if the children don't exist.
  }

  //set the height of the rectangle proportional to the font height:
  //and offset to the baseline
  tmp.setRect(x, -fm.height() * 2 / 3 + y -
	      fm.boundingRect("+").center().y(), 1,
	      fm.height() * 3 / 4);

  switch(type) {
  case TEXT: {//just the position in the text.
    //The assert should be returned when the posinstr bug is fixed.
    ASSERT(i.posinbox <= (int)text.length());
    if(i.posinbox > (int)text.length()) i.posinbox = (int)text.length();
    if(text.length() < 1) {
      tmp.setX(rect.center().x() + x - 1);
      break;
    }

    //to make sure the spaces at the end are computed, we append a . and
    //then subtract off its width (also tweak it a few pixels since
    //there is a space between the last char and the dot).
    QString temptext( text.left( i.posinbox ) );
    temptext.append(".");

    if(i.posinbox == 0) tmp.setX(rect.left() + x - 1);
    else tmp.setX(fm.boundingRect(temptext).right() + x - 1 -
		  fm.boundingRect(".").width());


    break;

  }

  case PLUS:
  case SLASH:
  case MINUS:
  case TIMES:
  case MORE:
  case LESS:
  case EQUAL: //for these, the cursor can be to the left or right
              //of the operator.
    if(i.posinbox == 0) tmp.setX(relx + x - 1);
    else {
      if(b2 != NULL) tmp.setX(x + b2x + b2->rect.x() + 1);
      else  tmp.setX(x + rect.right() + 1);
    }
    break;

  default: //for everything else, the cursor is either to the left
           //of the whole box or to the right of the whole box.
    if(i.posinbox == 0) tmp.setX(rect.x() + x - 1);
    else tmp.setX(x + rect.right() + 1);
    break;
  }

  tmp.setWidth(1); //make sure it's a thin cursor

  return tmp;
}

//----------------------------static SYMBOL RECT--------------------
//returns the bounding rectangle for a symbol
QRect box::symbolRect(QPainter &p, SymbolType s, int size)
{
  if(s - SYMBOL_ABOVE >= 32) { // character in symbol font
    QFont f = p.font();
    f.setFamily("symbol");
    p.setFont(f);

    QFontMetrics fm(p.fontMetrics());

    QRect r = fm.boundingRect(QString(QChar(s - SYMBOL_ABOVE)));

    r.moveBy(0, - fm.boundingRect('+').center().y());

    return r;
  }


  switch(s) {
  case INTEGRAL:
    size = size * 4 / 3;
    return QRect(-size / 24, -size, size / 2 + size / 12, size * 2);
    break;

  case SUM:
    size = size * 6 / 5;
    return QRect(0, -size / 2, size, size);
    break;

  case PRODUCT:
    size = size * 6 / 5;
    return QRect(-size / 16, -size / 2 - size / 16,
		 size + size / 8, size + size / 8);
    break;

  case ARROW:
    return QRect(-size / 32, -size / 4 - size / 16,
		 size + size / 16, size / 2 + size / 8);
    break;
  }

  return QRect();
}

//----------------------------static DRAW SYMBOL---------------------
void box::drawSymbol(QPainter &p, SymbolType s, int size, int x, int y)
{
  if(s - SYMBOL_ABOVE >= 32) { // char from symbol font
    QFont f = p.font();
    f.setFamily("symbol");
    p.setFont(f);
    QFontMetrics fm(p.fontMetrics());

    p.drawText(x, y - fm.boundingRect('+').center().y() , QString(QChar(s - SYMBOL_ABOVE)));
    return;
  }

  QPointArray a;

  switch(s) {
  case INTEGRAL:
    size = size * 4 / 3;
    a.setPoints(12,
		size / 4 - size / 16, 0,           // 1
		size / 4 + size / 16, -size + size / 16,       // 2
		size / 2 - size / 16, -size,        // 3
		size / 2 + size / 24, -size + size / 8,        // 4
		size / 2 - size / 16, -size + size / 16, // 5
		size / 4 + size / 12, -size + size / 8, // 6
		size / 4 + size / 16, 0,           // 7
		size / 4 - size / 16, size - size / 16,       // 8
		size / 16, size,        // 9
		-size / 24, size - size / 8,        // 10
		size / 16, size - size / 16, // 11
		size / 4 - size / 12, size - size / 8); // 12
		
    break;

  case SUM:
    size = size * 6 / 5;
    a.setPoints(10,
		size, -size / 2, // 1
		0, -size / 2, // 2
		size / 2, 0, // 3
		0, size / 2, // 4
		size, size / 2, // 5
		size, size / 2 - size / 16, // 6
		size / 8, size / 2 - size / 16, // 7
		size / 2 + size / 16, 0, // 8
		size / 8, -size / 2 + size / 16, // 9
		size, -size / 2 + size / 16); // 10

    break;

  case PRODUCT:
    size = size * 6 / 5;
    p.setPen(QPen(p.pen().color(), size / 8));
    p.drawLine(x, y - size / 2, x + size, y - size / 2);
    p.drawLine(x, y + size / 2, x + size / 2 - size / 8,
	       y + size / 2);
    p.drawLine(x + size / 2 + size / 8, y + size / 2,
	       x + size, y + size / 2);
    p.setPen(QPen(p.pen().color(), size / 6));
    p.drawLine(x + size / 4 - size / 16, y - size / 2,
	       x + size / 4 - size / 16, y + size / 2);
    p.drawLine(x + size - size / 4 + size / 16, y - size / 2,
	       x + size - size / 4 + size / 16, y + size / 2);
    p.setPen(QPen(p.pen().color(), 1));

    return; // don't draw a polygon

    break;

  case ARROW:
    p.setPen(QPen(p.pen().color(), size / 8));
    p.drawLine(x, y, x + size, y);
    p.drawLine(x + size - size / 16, y,
	       x + size - size / 16 - size / 4, y - size / 4);
    p.drawLine(x + size - size / 16, y,
	       x + size - size / 16 - size / 4, y + size / 4);
    p.setPen(QPen(p.pen().color(), 1));

    return; //don't draw a polygon

    break;
  }

  a.translate(x, y);

  p.drawPolygon(a);

  return;
}







