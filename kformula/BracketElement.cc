/*
  BracketElement.cc 
  Project KOffice/KFormula
  
  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/

//#define RECT
#include <qrect.h> 
#include "BasicElement.h" 
#include "BracketElement.h" 
#include "formuladef.h"
#include "kformula_doc.h"

BracketElement::BracketElement(KFormulaDocument *Formula,
			       BasicElement *Prev,
			       int Relation,
			       BasicElement *Next,
			       QString Content) : BasicElement(Formula,Prev,Relation,Next,Content)
{
  /*
    Stuff to load pixmap (if need)
  */
  usePixmap=FALSE;
  childrenNumber=1;
  child.resize(childrenNumber);
}

BracketElement::~BracketElement()
{
}

void BracketElement::draw(QPoint drawPoint,int resolution)
{
  
  QPainter *pen=formula->painter();
  int x=drawPoint.x();
  int y=drawPoint.y();
  if( beActive )
    pen->setPen(red);
  int unit=familySize.height()/4;
  /*  
      Draw Bracket!!
  */
  int ofs=(numericFont/32); 
  if (ofs>0) ofs-=1;
  char ch=0;
  x+=1;
  for(int i=0;i<2;i++) {
    ch=content[i];
    if(i) x+=familySize.width()-unit-ofs-3;
    switch (ch)
     { 
      case '|':
    /*
     I could also use DrawRect
    */{
      QPointArray points(5);
      points.setPoint(1,x+familySize.x()+unit/2,y+familySize.y());
      points.setPoint(2,x+familySize.x()+unit/2+ofs,y+familySize.y());
      points.setPoint(3,x+familySize.x()+unit/2+ofs,y+familySize.bottom());
      points.setPoint(4,x+familySize.x()+unit/2,y+familySize.bottom());
      pen->setBrush(pen->pen().color());
      pen->drawPolygon(points,FALSE,1,4);
      }
      break;

      case '\\':{
      QPointArray points(5);
      points.setPoint(1,x+familySize.x(),y+familySize.y());
      points.setPoint(2,x+familySize.x()+ofs,y+familySize.y());
      points.setPoint(3,x+familySize.x()+unit+ofs,y+familySize.bottom());
      points.setPoint(4,x+familySize.x()+unit,y+familySize.bottom());
      pen->setBrush(pen->pen().color());
      pen->drawPolygon(points,FALSE,1,4);
      }
      break;
      case '/':
      {
      QPointArray points(5);
      points.setPoint(1,x+familySize.x()+unit,y+familySize.y());
      points.setPoint(2,x+familySize.x()+unit+ofs,y+familySize.y());
      points.setPoint(3,x+familySize.x()+ofs,y+familySize.bottom());
      points.setPoint(4,x+familySize.x(),y+familySize.bottom());
      pen->setBrush(pen->pen().color());
      pen->drawPolygon(points,FALSE,1,4);
      }
      break;
      case '[':
      {
      QPointArray points(10);
      points.setPoint(1,x+familySize.x(),y+familySize.y());
      points.setPoint(2,x+familySize.x()-1+unit+ofs,y+familySize.y());
      points.setPoint(3,x+familySize.x()-1+unit+ofs,y+familySize.y()+ofs);
      points.setPoint(4,x+familySize.x()+ofs,y+familySize.y()+ofs);
      points.setPoint(5,x+familySize.x()+ofs,y+familySize.bottom()-ofs);
      points.setPoint(6,x+familySize.x()-1+unit+ofs,y+familySize.bottom()-ofs);
      points.setPoint(7,x+familySize.x()-1+unit+ofs,y+familySize.bottom());
      points.setPoint(8,x+familySize.x(),y+familySize.bottom());
      pen->setBrush(pen->pen().color());
      pen->drawPolygon(points,FALSE,1,8);
      }
      break;
      case ']':
      {
      QPointArray points(10);
      points.setPoint(1,x+familySize.x(),y+familySize.y());
      points.setPoint(2,x+familySize.x()-1+unit+ofs,y+familySize.y());
      points.setPoint(3,x+familySize.x()-1+unit+ofs,y+familySize.bottom());
      points.setPoint(4,x+familySize.x(),y+familySize.bottom());
      points.setPoint(5,x+familySize.x(),y+familySize.bottom()-ofs);
      points.setPoint(6,x+familySize.x()-1+unit,y+familySize.bottom()-ofs);
      points.setPoint(7,x+familySize.x()-1+unit,y+familySize.y()+ofs);
      points.setPoint(8,x+familySize.x(),y+familySize.y()+ofs);
      pen->setBrush(pen->pen().color());
      pen->drawPolygon(points,FALSE,1,8);
      }
      break;
      case '(':
      {
      QColor elementColor(pen->pen().color());
      pen->setBrush(elementColor);
      pen->drawChord(x+familySize.x(),y+familySize.y(),
                      unit+ofs,familySize.height(),
		      90*16,180*16);
      pen->setBrush(pen->backgroundColor());
      pen->setPen(pen->backgroundColor());
      pen->drawChord(x+familySize.x()+ofs+1,y+familySize.y(),
                      unit,familySize.height(),
		      90*16,180*16);
      pen->setPen(elementColor);
    
      }
      break;
      case ')':
      {
      QColor elementColor(pen->pen().color());
      pen->setBrush(elementColor);
      pen->drawChord(x+familySize.x(),y+familySize.y(),
                      unit+ofs,familySize.height(),
		      270*16,180*16);
      pen->setBrush(pen->backgroundColor());
      pen->setPen(pen->backgroundColor());
      pen->drawChord(x+familySize.x()-1,y+familySize.y(),
                      unit,familySize.height(),
		      270*16,180*16);
      pen->setPen(elementColor);
    
      }
      break;


    }
  
  }
  /*
    Draw child[0], it must exist
  */    
  
  x=drawPoint.x();  //modified in delimiters draw
  
  if( beActive )
    pen->setPen(blue);
  child[0]->draw(QPoint(x+familySize.x()+unit+ofs+3,y),resolution);
  myArea=globalSize;;
  myArea.moveBy(x,y);
  // globalArea=
  // globalArea.moveBy(x,y);
#ifdef RECT
  pen->setBrush(green);
  pen->setBrush(NoBrush);
  pen->drawRect(myArea);   
  QRect area(localSize);
  area.moveBy(x,y);
  pen->drawRect(area); 
  pen->setBrush(SolidPattern);
#endif
  drawIndexes(pen,resolution);
  if( beActive )
    pen->setPen(black);
  if(next!=0L) next->draw(drawPoint+QPoint(localSize.width()+1,0),resolution);
  
  
}

void BracketElement::checkSize()
{
 //warning("R %p",this);
  QRect nextDimension; 
  
  if (next!=0L)
    {
      next->checkSize();
      nextDimension=next->getSize();
    }
  
  child[0]->checkSize();
  familySize=child[0]->getSize();

  if (familySize.height()<18)
    {
     int fmY=familySize.height();
     familySize.setTop(familySize.top()-(16-fmY)/2-1);
     familySize.setBottom(familySize.bottom()+(16-fmY)/2+1);
    }
  familySize.setLeft(familySize.left()-(numericFont/32)-(familySize.height()/4)-2);  
  familySize.setRight(familySize.right()+(numericFont/32)+(familySize.height()/4)+2);  

  
  localSize=familySize;
  checkIndexesSize();  //This will change localSize adding Indexes Size
  familySize.moveBy(-localSize.left(),0);
  localSize.moveBy(-localSize.left(),0);
  globalSize=localSize;
  nextDimension.moveBy(localSize.width(),0);
  globalSize=globalSize.unite(nextDimension);
  //warning("end");
}

int BracketElement::takeAsciiFromKeyb(int)
{
  return 2;
}
int BracketElement::takeActionFromKeyb(int)
{
  return -1;
}

void BracketElement::setNumericFont(int value)
{
  numericFont=value;
  /*
    We can use differnt pixmap:
    numericFont < 20   tinyRoot.xpm
    20 < numericFont < 50 normalRoot.xpm
    numericFont < 20   bigRoot.xpm
  */
}



