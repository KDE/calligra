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
// $Id$

#include "kformula.h"
#include "box.h"
#include "matrixbox.h"
#include "math.h"
#include <stdio.h>
#include <ctype.h>
#include <qregexp.h>
#include <kglobal.h>
#include <kstddirs.h>

using namespace Box;

template class QArray<box*>;
template class QDict<double>;

//initialize the static members:
QString *KFormula::SPECIAL = NULL;
QString *KFormula::DELIM = NULL;
QString *KFormula::INTEXT = NULL;
QString *KFormula::LOC = NULL;
QString *KFormula::BIGOP = NULL;
QString *KFormula::EVAL = NULL;

void KFormula::initStrings()
{
  if(SPECIAL) return;
  SPECIAL = new QString();
  DELIM = new QString();
  LOC = new QString();
  INTEXT = new QString();
  BIGOP = new QString();
  EVAL = new QString();

  *SPECIAL += (L_GROUP);
  *SPECIAL += (R_GROUP);
  *SPECIAL += (QChar(PLUS));
  *SPECIAL += (QChar(MINUS));
  *SPECIAL += (QChar(TIMES));
  *SPECIAL += (QChar(DIVIDE));
  *SPECIAL += (QChar(POWER));
  *SPECIAL += (QChar(SQRT));
  *SPECIAL += (QChar(ABS));
  *SPECIAL += (QChar(BRACKET));
  *SPECIAL += (QChar(SUB));
  *SPECIAL += (QChar(LSUB));
  *SPECIAL += (QChar(LSUP));
  *SPECIAL += (QChar(PAREN));
  *SPECIAL += (QChar(EQUAL));
  *SPECIAL += (QChar(MORE));
  *SPECIAL += (QChar(LESS));
  *SPECIAL += (QChar(CAT));
  *SPECIAL += (QChar(SLASH));
  *SPECIAL += (QChar(ABOVE));
  *SPECIAL += (QChar(BELOW));
  *SPECIAL += (QChar(L_GROUP));
  *SPECIAL += (QChar(R_GROUP));
  *SPECIAL += (QChar(MATRIX));
  *SPECIAL += (QChar(SEPARATOR));

  *INTEXT += (QChar(PLUS));
  *INTEXT += (QChar(MINUS));
  *INTEXT += (QChar(TIMES));
  *INTEXT += (QChar(EQUAL));
  *INTEXT += (QChar(MORE));
  *INTEXT += (QChar(LESS));
  *INTEXT += (QChar(SLASH));

  *LOC += (QChar(POWER));
  *LOC += (QChar(SUB));
  *LOC += (QChar(LSUB));
  *LOC += (QChar(LSUP));
  *LOC += (QChar(ABOVE));
  *LOC += (QChar(BELOW));

  *DELIM += (QChar(ABS));
  *DELIM += (QChar(PAREN));
  *DELIM += (QChar(BRACKET));

  *BIGOP += (QChar(INTEGRAL));
  *BIGOP += (QChar(SUM));
  *BIGOP += (QChar(PRODUCT));

  *EVAL += (QChar(PLUS));
  *EVAL += (QChar(MINUS));
  *EVAL += (QChar(TIMES));
  *EVAL += (QChar(DIVIDE));
  *EVAL += (QChar(SLASH));
  *EVAL += (QChar(POWER));
  *EVAL += (QChar(PAREN));
  *EVAL += (QChar(ABS));
  *EVAL += (QChar(SQRT));

}


//-----------------------------TO UGLY--------------------------
//static--converts string to ugly form
QString KFormula::toUgly(QString ugly)
{
  int i;

  if(ugly.isNull() || ugly.isEmpty()) return QString("");

  //look for roots
  i = ugly.find(QChar(SQRT));
  while(i != -1) {
    if(ugly[i - 2] == L_GROUP) { // we have a square root
      ugly.remove(i - 2, 3);
      ugly.insert(i - 2, "sqrt"); // {}@{...}  -->  sqrt{...}
    }
    else { // we have an nth root.  What to do?
      kdWarning() << "What do you want to do about nth roots?" << endl;
      //for now remove the root sign just to keep the conversion alive
      ugly.remove(i, 1);
    }

    i = ugly.find(QChar(SQRT), i);
  }

  //look for brackets
  i = ugly.find(QChar(BRACKET));
  while(i != -1) {
    i -= 2;
    ugly.remove(i, 3);  // {}[{...}  -->  {...}

    ugly[ findMatch(ugly, i) ] = ']';
    ugly[i] = '[';  // {...}  -->  [...]

    i = ugly.find(QChar(BRACKET), i + 1); // find next parentheses
  }

  //do all other replacements.
  QRegExp r;

  r = QString(L_GROUP) + R_GROUP + QChar(PAREN); //parentheses
  ugly.replace(r, "");  // {}({...} --> {...}

  r = QString(L_GROUP) + R_GROUP + QChar(ABS); // absolute value
  ugly.replace(r, "abs"); // {}|{...} --> abs{...}

  for(i = 0; i < (int)ugly.length(); i++) {
    if(ugly[i] == QChar(POWER)) ugly[i] = '^';
    else if(ugly[i] == QChar(SUB)) ugly[i] = '_';
    else if(ugly[i] == QChar(DIVIDE)) ugly[i] = '/';
    else if(ugly[i] == L_GROUP) ugly[i] = '(';
    else if(ugly[i] == R_GROUP) ugly[i] = ')';
  }

  return ugly;
}


//-----------------------------FROM UGLY--------------------------
//static--converts string from ugly form toparseable form
QString KFormula::fromUgly(QString ugly)
{
  int i;

  //search for absolute value:
  i = ugly.find("abs(", 0, FALSE); // case insensitive
  while(i != -1) {
    if( (i == 0 || !ugly[i - 1].isLetter()) ) { //we really have an abs
      int tmp = findMatch( ugly, i + 3);
      ugly.replace(i, 4, QString(L_GROUP) + R_GROUP + QChar(ABS) + L_GROUP); // abs( --> {}|{
      ugly[tmp] = R_GROUP;
    }

    i = ugly.find("abs(", i + 1, FALSE);
  }

  //search for square roots:
  i = ugly.find("sqrt(", 0, FALSE); // case insensitive
  while(i != -1) {
    if( (i == 0 || !ugly[i - 1].isLetter()) ) { //we really have an sqrt
      ugly[ findMatch( ugly, i + 4) ] = R_GROUP;
      ugly.replace(i, 5, QString(L_GROUP) + R_GROUP + QChar(SQRT) + L_GROUP); // abs( --> {}|{
    }

    i = ugly.find("sqrt(", i + 1, FALSE);
  }

  //search for brackets:
  for(i = 0; i < (int)ugly.length(); i++) {
    if(ugly[i] == '[') {
      ugly[ findMatch(ugly, i) ] = R_GROUP;
      ugly.remove(i, 1);
      ugly.insert(i, QString(L_GROUP) + R_GROUP + QChar(BRACKET) + L_GROUP);
      i += 4;
    }
  }

  //look for division:
  i = ugly.find(")/("); //if it doesn't have parentheses around it, it will be a slash not a fraction.
  while(i != -1) {
    ugly[ findMatch(ugly, i) ] = L_GROUP;
    ugly[i] = R_GROUP;
    ugly[i + 1] = QChar(DIVIDE);
    ugly[ findMatch(ugly, i + 2) ] = R_GROUP;
    ugly[i + 2] = L_GROUP;

    i = ugly.find(")/(", i + 1);
  }

  //the quest for power (and subscript):
  i = ugly.find("^("); // it will just remain a caret if it has no parentheses
  while(i != -1) {
    ugly[ findMatch(ugly, i + 1) ] = R_GROUP;
    ugly[i + 1] = L_GROUP;
    ugly[i] = QChar(POWER);

    i = ugly.find("^(", i + 1);
  }

  i = ugly.find("_("); // it will just remain an underscore if it has no parentheses
  while(i != -1) {
    ugly[ findMatch(ugly, i + 1) ] = R_GROUP;
    ugly[i + 1] = L_GROUP;
    ugly[i] = QChar(SUB);

    i = ugly.find("_(", i + 1);
  }

  //finally, take care of all the remaining parentheses:
  for(i = 0; i < (int)ugly.length(); i++) {
    if(ugly[i] == '(') {
      ugly[i] = L_GROUP;
      ugly.insert(i + 1, QString(R_GROUP) + QChar(PAREN) + L_GROUP);
      i += 3;
    }
    if(ugly[i] == ')') ugly[i] = R_GROUP;
  }

  //it's not ugly anymore--it's parseable!
  return ugly;

}


//-----------------------------FIND MATCH--------------------------
//static--finds the matching delimiter
int KFormula::findMatch(QString s, int pos)
{
  int level = 0;
  QChar lchar, rchar;

  if(s[pos] == L_GROUP || s[pos] == R_GROUP) { lchar = L_GROUP; rchar = R_GROUP; }
  else if(s[pos] == '(' || s[pos] == ')') { lchar = '('; rchar = ')'; }
  else if(s[pos] == '[' || s[pos] == ']') { lchar = '['; rchar = ']'; }
  else if(s[pos] == '{' || s[pos] == '}') { lchar = '{'; rchar = '}'; }

  if(pos < 0 || pos >= (int)s.length()) return -1;

  if(s[pos] == lchar) { // look for the matching R_GROUP to the right
    while(pos < (int)s.length()) {
      if(s[pos] == lchar) level++;
      if(s[pos] == rchar) level--;

      if(level == 0) return pos;

      pos++;
    }

    kdError() << "Mismatched delimeters.  String = " << s << endl;
    return -1;
  }

  if(s[pos] == rchar) { // find the L_GROUP to the left
    while(pos >= 0) {
      if(s[pos] == lchar) level--;
      if(s[pos] == rchar) level++;

      if(level == 0) return pos;

      pos--;
    }

    kdError() << "Mismatched delimeters.  String = " << s << endl;
    return -1;
  }

  kdError() << "Bad delimeters.  String = " << s << endl;
  return -1;
}


//This class stores and displays the formula

//---------------------CONSTRUCTORS AND DESTRUCTORS-----------------
KFormula::KFormula(bool r)
  : font( 0 ), backColor( 0 ), foreColor( 0 ), referenceFetcher(0)
{
  posx = posy = 0;
  restricted = r;
}

KFormula::KFormula(int x, int y, bool r)
  : font( 0 ), backColor( 0 ), foreColor( 0 ), referenceFetcher(0)
{
  posx = x;
  posy = y;
  restricted = r;
}

KFormula::~KFormula()
{
  while(boxes.size()) {
    delete boxes[boxes.size() - 1];
    boxes.resize(boxes.size() - 1);
  }
  if ( font )
      delete font;
  if ( backColor )
      delete backColor;
  if ( foreColor)
      delete foreColor;
  if ( referenceFetcher)
      delete referenceFetcher;
}

//---------------------------GET CURSOR POS-------------------------
//simply calls the function on the last (top level) box.
QRect KFormula::getCursorPos(charinfo i)
{
  QRect tmp = boxes[boxes.size() - 1]->getRect();
  return boxes[boxes.size() - 1]->getCursorPos(i, posx - tmp.center().x(),
				 posy - tmp.center().y());
}

//--------------------------------SIZE------------------------------
QSize KFormula::size() const
{
  if(boxes.size() == 0) return QSize(0, 0);

  return QSize(boxes[boxes.size() - 1]->getRect().width() + 8, // 8 is just some extra room
	       boxes[boxes.size() - 1]->getRect().height() + 8);
}

void KFormula::makeDirty()
{
    if ( boxes.size() == 0 )
	return;
    for ( unsigned int i = 0; i < boxes.size(); ++i )
	boxes[ i ]->makeDirty();
}

//-------------------REFETCH REFERENCES AND CALCULATE-----------------
void KFormula::refetchReferencesAndCalculate(QPainter &p)
{
  if(boxes.size() == 0) return;

  if(p.device()->devType() == QInternal::Printer) makeDirty();

  if(boxes[boxes.size() - 1]->dirty == false) return;

  unsigned int i;
  for(i = 0; i < boxes.size(); ++i) {
    if(!IS_REFERENCE(boxes[i]->getType())) continue;

    KFormula *k = referenceFetcher->getFormula(boxes[i]->getType() - REFERENCE_ABOVE);

    if(k->boxes[k->boxes.size() - 1] != boxes[i]->b1) {
      boxes[i]->b1 = k->boxes[k->boxes.size() - 1];
      boxes[i]->b1->refParents.append(boxes[i]);

      ASSERT(boxes[i]->dirty);
    }

    k->refetchReferencesAndCalculate(p);
  }

  boxes[boxes.size() - 1]->calculate(p, p.font().pointSize(),
				     getFont(),
				     getBackColor(),
				     getForeColor() );
}

//--------------------------------REDRAW----------------------------
//first call calculate, figure out the center, and draw the boxes
void KFormula::redraw(QPainter &p)
{

  if(boxes.size() == 0) return;

  refetchReferencesAndCalculate(p);

  QRect tmp = boxes[boxes.size() - 1]->getRect();
  boxes[boxes.size() - 1]->draw(p, posx - tmp.center().x(),
				posy - tmp.center().y(),
				getFont(),
				getBackColor(),
				getForeColor() );

  return;
}

//-------------------------------SET BOXES-------------------------
//duh
void KFormula::setBoxes(QArray<box *> newBoxes)
{
  while(boxes.size()) {
    delete boxes[boxes.size() - 1];
    boxes.resize(boxes.size() - 1);
  }
  boxes = newBoxes.copy();
}

//--------------------------------GET BOXES------------------------
//an even bigger DUH than the previous one.
QArray<box *> KFormula::getBoxes()
{
  return boxes.copy();
}

void KFormula::setPos(int x, int y)
{
  posx = x;
  posy = y;
}

//------------------------------EVALUATE--------------------------------
//default value of b is NULL
//if it's a text box, looks up variables vars and their values
//in vals and returns them.  otherwise, evaluates the children and
//does whatever is necessary to them.
double KFormula::evaluate(const QDict<double>& variables,
			  int *error, box *b)
{
  if(!restricted) return 0; // evaluate only if restricted

  int err_dummy;

  if(!error)
    error = &err_dummy;

  *error = NO_ERROR;

  if(!b) b = boxes[boxes.size() - 1];

  if(IS_REFERENCE(b->type)) { //reference--this is simple
    if(referenceFetcher == 0) {
      *error = UNDEFINED_REFERENCE;
      return 0;
    }
    return referenceFetcher->getValue(b->type - REFERENCE_ABOVE);
  }

  if(b->type == TEXT) {
    QString temptext = b->text.stripWhiteSpace();

    if(temptext.length() > 0) { //is it empty?
      double x;
      bool ok;

      x = temptext.toDouble(&ok);
      if(ok) return x; // we have a number

      double *v = variables.find(temptext);
      if(v != NULL) return *v;
      else { // variable not found
        *error = UNDEFINED_VARIABLE;
        return 0;
      }
    }
    if(!b->parent) {
      *error = EMPTY_BOX;
      return 0;
    }
    if(b->parent->type == SQRT && b->parent->b1 == b) return 2;
    if(b->parent->type == MINUS && b->parent->b1 == b) return 0;
    if(b->parent->type == PLUS && b->parent->b1 == b) return 0;
    if(delim().contains(QChar(b->parent->type)) && b->parent->b1 == b)
      return 0;

    if(b->parent->type == CAT) return 1; // cat is multiplication

    *error = EMPTY_BOX;
    return 0;
  }

  double b1 = 0, b2 = 0;
  int undefined_in_b1 = 0; // whether b1 had an undefined variable

  if(b->b1) b1 = evaluate(variables, error, b->b1);

  if(*error == UNDEFINED_VARIABLE && b->type == CAT) {
    *error = 0; // it may be a function!
    undefined_in_b1 = 1;
  }

  if(*error) return 0;

  if(b->b2) b2 = evaluate(variables, error, b->b2);

  if(*error) return 0;

  switch(b->type) {
  case PLUS:
    return b1 + b2;
    break;
  case MINUS:
    return b1 - b2;
    break;
  case TIMES:
    return b1 * b2;
    break;
  case SLASH:
  case DIVIDE:
    if(b2)
      return b1 / b2;
    else {
      *error = DIVISION_BY_ZERO;
      return 0;
    }
    break;
  case POWER:
    return pow(b1, b2);
    break;
  case SQRT:
    if(!b1) {
      *error = DIVISION_BY_ZERO;
      return 0;
    }

    if(b2 < 0) {
      *error = ROOT_OF_NEGATIVE;
      return 0;
    }

    return pow(b2, 1 / b1);

    break;
  case PAREN:
    return b2;
    break;
  case ABS:
    return fabs(b2);
    break;
  case CAT: // multiply variables or evaluate functions
    if(!undefined_in_b1) return b1 * b2;

    if(b->b1->type != TEXT) {
      *error = PARSE_ERROR;
      return 0;
    }

    QString fun = b->b1->text.stripWhiteSpace();

    if(fun == "sqrt") return sqrt(b2);
    if(fun == "log") return log(b2);
    if(fun == "exp") return exp(b2);
    if(fun == "floor") return floor(b2);
    if(fun == "ceil") return ceil(b2);
    if(fun == "abs") return fabs(b2);

    if(fun == "sin") return sin(b2);
    if(fun == "cos") return cos(b2);
    if(fun == "tan") return tan(b2);
    if(fun == "sinh") return sinh(b2);
    if(fun == "cosh") return cosh(b2);
    if(fun == "tanh") return tanh(b2);

    if(fun == "asin") return asin(b2);
    if(fun == "acos") return acos(b2);
    if(fun == "atan") return atan(b2);
    if(fun == "asinh") return asinh(b2);
    if(fun == "acosh") return acosh(b2);
    if(fun == "atanh") return atanh(b2);

    break;
  }

  return 0;
}

//INSERTED goes through the charinfo array and increments posinstr
//for all the characters after the one that was inserted.

#define INSERTED(xxi) { int tmpx; if(info) for(tmpx = 0; \
					       tmpx < (int)info->size(); \
					       tmpx++) \
  if((*info)[tmpx].posinstr >= (xxi)) (*info)[tmpx].posinstr++; \
}

// The parser works by first fully parenthesizing (with {}) the string
// and then removing the parentheses one by one in make_boxes,
// adding a box each time.  Not especially efficient or versatile
// but speed is not critical and this was easy to write.

void KFormula::parse(QString text, QArray<charinfo> *info)
{
  int i, j;

  if(info) { //initialize info

    info->resize(text.length() + 1);

    for(i = 0; i <= (int)text.length(); i++) {
      charinfo inf;

      inf.where = NULL;
      inf.posinbox = 0;
      inf.posinstr = i;
      inf.left = 0;

      info->at(i) = inf;
    }

    //make empty boxes and put cursor into them if necessary:
    for(i = 0; i < (int)text.length(); i++) {
      if(!special().contains(text[i]) || text[i] == L_GROUP ||
	 text[i] == R_GROUP || intext().contains(text[i])) continue;

      if(i == 0 || (text[i - 1] != R_GROUP &&
		    special().contains(text[i - 1]))) {
	text.insert(i, L_GROUP);
	INSERTED(i);
	text.insert(i + 1, R_GROUP);
	INSERTED(i + 2); //note! we don't move the current cursor over!
	continue;
      }

      if(i == (int)text.length() - 1 ||
	 (text[i + 1] != L_GROUP &&
	  special().contains(text[i + 1]))) {
	text.insert(i + 1, L_GROUP);
	INSERTED(i + 1);
	text.insert(i + 2, R_GROUP);
	INSERTED(i + 3); //note! we don't move the current cursor over!
	continue;
      }
    }

    for(i = 0; i < (int)info->size(); i++) {
      //decide whether to atach the cursor to the left or right character:

      if((*info)[i].posinstr &&
	 (!special().contains(text[(*info)[i].posinstr - 1]) ||
	 intext().contains(text[(*info)[i].posinstr - 1])))
	(*info)[i].posinstr--; //stick to a letter or an intext

      else if((*info)[i].posinstr < (int)text.length() &&
	      (!special().contains(text[(*info)[i].posinstr]) ||
	      intext().contains(text[(*info)[i].posinstr])))
	(*info)[i].left = 1; //stick to letter on the right

      else if((*info)[i].posinstr &&
	      text[(*info)[i].posinstr - 1] == R_GROUP)
	(*info)[i].posinstr--; //stick to an rgroup if possible

      else (*info)[i].left = 1;
    }
  }

  //delete the existing boxes
  while(boxes.size() > 0) {
    delete boxes[boxes.size() - 1];
    boxes.resize(boxes.size() - 1);
  }

  //make "unseen" braces into regular ones:
  for(i = 0; i < (int)text.length(); i++) {
    if(text[i] == QChar(L_BRACE_UNSEEN)) text[i] = L_GROUP;
    if(text[i] == QChar(R_BRACE_UNSEEN)) text[i] = R_GROUP;
  }

  if(restricted) {
    //isolate numbers from letters for evaluation:
    //and insert cats after spaces
    for(i = 0; i < (int)text.length() - 1; i++)
      {
	if(text[i].isDigit() && !special().contains(text[i + 1]) &&
	   text[i + 1].isLetter()) {
	  text.insert(i + 1, QChar(CAT));
	  INSERTED(i + 1);
	  i++;
	}
	else if(text[i] == ' ' && text[i + 1] != ' ') {
	  text.insert(i + 1, QChar(CAT));
	  INSERTED(i + 1);
	  i++;
	}
      }
  }

  //isolate all symbols and references from text:
  for(i = 0; i < (int)text.length(); i++)
    {
      if(text[i].unicode() >= SYMBOL_ABOVE || IS_REFERENCE(text[i].unicode())) {
	if(i > 0 && !special().contains(text[i - 1])) {
	  text.insert(i, QChar(CAT));
	  INSERTED(i);
	  i++;
	}
	if(i < (int)text.length() - 1 && !special().contains(text[i + 1])) {
	  i++;
	  text.insert(i, QChar(CAT));
	  INSERTED(i);
	}
      }
    }


  //search for implicit concatenation:
  //insert a CAT (#) symbol at every concatenation:
  //"a{b}/{c}d" -> "a#{b}/{c}#d".

  for(i = 0; i < ((int)text.length() - 1); i++)
    {
      j = i + 1;
      if(j > (int)text.length() - 1) continue;
      if(text[j] != L_GROUP && text[i] != R_GROUP) continue;
      if(text[i] == R_GROUP && text[j] == L_GROUP) {
	text.insert(j, QChar(CAT));
	INSERTED(j);
      }

      if(text[i].unicode() && text[j].unicode() &&
          special().contains(text[i]) && special().contains(text[j])) continue;
      text.insert(j, QChar(CAT));
      INSERTED(j);
    }

  //now parenthesize everything in reverse order (think about it).
  //After this, "x+y*z^{2}" becomes "{x}+{{y}*{{z}^{2}}}"

#define ADD_PAREN(s)                           \
for(i = (int)text.length() - 1; i >= 0; i--) { \
  if(!(s).contains(text[i])) continue;         \
  parenthesize(text, i, info);                 \
}

#define ADD_PAREN_REVERSE(s)                   \
for(i = 0; i < (int)text.length(); i++) {      \
  if(!(s).contains(text[i])) continue;         \
  parenthesize(text, i, info);                 \
}

  ADD_PAREN( QString(QChar((int)LESS)) + QChar(MORE) + QChar(EQUAL) )

  ADD_PAREN( QString(QChar((int)PLUS)) + QChar(MINUS) )

  ADD_PAREN( QString(QChar((int)SLASH)) + QChar(TIMES) )

  ADD_PAREN_REVERSE( QString(QChar((int)CAT)) ) // backwards for evaluation

  ADD_PAREN( QString(QChar((int)POWER)) + QChar(SUB) )

  ADD_PAREN_REVERSE( QString(QChar((int)LSUB)) + QChar(LSUP) )

  ADD_PAREN( delim() + QChar(SQRT) + QChar(DIVIDE) + QChar(ABOVE) + QChar(BELOW) )

  // now realize the left char/right char attachments
  if ( info )
    for(i = 0; i < (int)info->size(); i++) {
      if(!(*info)[i].left) (*info)[i].posinstr++;
    }

  //Now just make the boxes.
  makeBoxes(text, 0, (int)text.length(), info);

}

//---------------------------PARENTHESIZE--------------------------
//given a string and an operator in position i, this puts
//curly braces around the two groups near the operator.  e.g.:
// Before: "{x}+{y*3}".  Then parenthesize is called with
// i = 6.  After: "{x}+{{y}*{3}}"
void KFormula::parenthesize(QString &temp, int &i, QArray<charinfo> *info)
{
  int j;
  int level = 0;
  int need_paren = 0;

  if(i == 0 || temp[i - 1] != R_GROUP) need_paren = 1;

  //search for the left end of the left group
  for(j = i; j >= 0; j--) {
    if(temp[j] == R_GROUP) level++;
    if(temp[j] == L_GROUP) level--;
    if(level < 0) {
      j++;
      break;
    }

    if(j == 0) break;

    if(j < i && level == 0 && temp[j - 1] != L_GROUP) need_paren = 1;
  }

  //insert left brace of left group if necessary
  if(need_paren) {
    temp.insert(j, L_GROUP);
    INSERTED(j);
    i++;
  //insert the right brace of the left group
    temp.insert(i, R_GROUP);
    INSERTED(i); i++;
  }

  level = 0;

  if(i + 1 < (int)temp.length() && temp[i + 1] == L_GROUP)
    need_paren = 0;
  else need_paren = 1;

  //now search for the right end of the right group
  for(j = i + 1; j <= (int)temp.length(); j++) {
    if(j < (int)temp.length() && temp[j] == L_GROUP) level++;
    if(j < (int)temp.length() && temp[j] == R_GROUP) level--;

    if(level < 0 || j == (int)temp.length()) break;

    if(level == 0 && j > i + 1 && (j < (int)temp.length() - 1 &&
       temp[j + 1] != R_GROUP)) need_paren = 1;
  }

  if(need_paren) {
    //insert right brace of left group:
    temp.insert(j, R_GROUP);
    INSERTED(j);

    //insert the left brace of the right group:
    temp.insert(i + 1, L_GROUP);
    INSERTED(i + 1);
  }
}

//-----------------------------MAKE BOXES-----------------------------
//creates the boxes from a fully parenthesized str.
//calls itself recursively.
box * KFormula::makeBoxes(QString str, int offset,
			  int maxlen, QArray<charinfo> *info)
{
  int toplevel = -1; //the location of the toplevel operator
  int level = 0;
  int i;

  //when the posinstr bug is fixed, change the "maxlen + 1" in the info
  //comparison to "maxlen"

  if(maxlen <= 0) { // make empty box:
    boxes.resize(boxes.size() + 1);
    boxes[boxes.size() - 1] = new box("");
    if(info) { //update the charinfo for all characters in this box
      for(i = 0; i < (int)info->size(); i++) {
	if((*info)[i].posinstr >= offset &&
	   (*info)[i].posinstr - offset <= maxlen) {
	  (*info)[i].where = boxes[boxes.size() - 1];
	  (*info)[i].posinbox = 0;
	}
      }
    }

    return boxes[boxes.size() - 1];
  }

  if(str[0] != L_GROUP) { //we have a literal--make a
                                 //TEXT or SYMBOL or REFERENCE box:
    boxes.resize(boxes.size() + 1);
    if(IS_REFERENCE(str[0].unicode())) {
      ASSERT(referenceFetcher != NULL);

      KFormula *f = referenceFetcher->getFormula(str[0].unicode() - REFERENCE_ABOVE);

      ASSERT(f != NULL);

      boxes[boxes.size() - 1] =
	new box(str[0].unicode(), f->boxes[f->boxes.size() - 1]);
    }
    else
    if(str[0].unicode() < SYMBOL_ABOVE) {
      boxes[boxes.size() - 1] = new box(str.left(maxlen));
    }
    else {
      boxes[boxes.size() - 1] = new box(SYMBOL, NULL, NULL);
      boxes[boxes.size() - 1]->text[0] = str[0];
    }

    if(info) {
      for(i = 0; i < (int)info->size(); i++) {
	if((*info)[i].posinstr >= offset &&
	   (*info)[i].posinstr - offset <= maxlen) {
	  (*info)[i].where = boxes[boxes.size() - 1];
	  (*info)[i].posinbox = (*info)[i].posinstr - offset;
	    //	    (1 - (*info)[i].left);
	}
      }
    }

    return boxes[boxes.size() - 1];
  }

  //find toplevel:  in "{{x}*{y}}+{3}" toplevel = 9, the +
  for(i = 0; i < maxlen; i++) {
    if(str[i] == L_GROUP) level++;
    if(str[i] == R_GROUP) level--;

    if(level == 0 && i < maxlen - 1) {
      toplevel = i + 1;
      break;
    }
  }

  //if there is no toplevel operand, strip the outside curly braces.  e.g.:
  //"{{x}+{y}}" -> "{x}+{y}".  Done by recursively calling makeBoxes
  //on the appropriate substring.
  if(toplevel == -1) return makeBoxes(str.mid(1), offset + 1,
				      maxlen - 2, info);

  if(str[toplevel] == QChar(MATRIX)) { // we have a matrix!
    int w, h, level, startpos = 0;

    //it looks like this: {w&h}M{{a}&{b}&{x}&...&{y}}
    w = str[toplevel - 4].unicode();
    h = str[toplevel - 2].unicode();

    matrixbox *tmpbox;

    tmpbox = new matrixbox(w, h);

    level = 0;

    for(i = toplevel + 2; i < maxlen - 1; i++) { // now add the elements
      if(str[i] == L_GROUP) {
	if(level == 0) startpos = i; // we start a new element
	level++;
	continue;
      }
      if(str[i] == R_GROUP) {
	level--;
      }

      if(level == 0 && str[i] == QChar(SEPARATOR)) {
	// we add the element
	tmpbox->addElem(makeBoxes(str.mid(startpos + 1),
				  offset + startpos + 1, i - startpos - 2,
				  info));
      }
    }

    boxes.resize(boxes.size() + 1);
    boxes[boxes.size() - 1] = tmpbox;
  }
  else { //we don't have a matrix
    //this stores the returned pointer so we don't add it to the
    //boxes array until all the children have been added.
    box *tmpbox;

    tmpbox = new box(str[toplevel].unicode(), //that's the operator.
		     makeBoxes(str, offset, toplevel, info),
		     makeBoxes(str.mid(toplevel + 1), offset + toplevel + 1,
			       maxlen - toplevel - 1, info));

    boxes.resize(boxes.size() + 1);
    boxes[boxes.size() - 1] = tmpbox;
  }

  if(info) {
    for(i = 0; i < (int)info->size(); i++) {
      //if the character has been assigned nowhere else, assign it to
      //this box.
      if((*info)[i].where == NULL && (*info)[i].posinstr >= offset &&
	 (*info)[i].posinstr - offset <= maxlen) {
	(*info)[i].where = boxes[boxes.size() - 1];
	//for non-TEXT boxes, posinbox is 0 if the character is
	//to the left of the box and 1 if it is to the right.
	if((*info)[i].posinstr - offset > toplevel) (*info)[i].posinbox = 1;
	else (*info)[i].posinbox = 0;
      }
    }
  }

  return boxes[boxes.size() - 1];
}






