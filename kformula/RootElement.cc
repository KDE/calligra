/*
  TextElement.cc
  Project KOffice/KFormula

  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/

//#define RECT
#include <qrect.h>
#include "BasicElement.h"
#include "RootElement.h"
#include "formuladef.h"
#include "formula_container.h"

RootElement::RootElement(KFormulaContainer *Formula,
			 BasicElement *Prev,
			 int Relation,
			 BasicElement *Next,
			 QString Content) :
    BasicElement(Formula,Prev,Relation,Next,Content)
{
    /*
      Stuff to load pixmap (if need)
    */
    usePixmap=FALSE;
    childrenNumber=1;
    minChildren=1;
    child.resize(childrenNumber);
    child[0]=0L;
    relation=Relation;
}

RootElement::~RootElement()
{
}

void RootElement::draw(QPoint drawPoint,int resolution)
{

    QPainter *pen=formula->painter();
    //QRect globalArea;
    int x,y,unit; //unit is familySize.height/4,used to draw proportional roots
    x=drawPoint.x();
    y=drawPoint.y();
    unit=familySize.height()/3;
     if( beActive )
	pen->setPen(Qt::red);

    /*
	Draw root!!
    */
    int ofs=(numericFont/24);
    if (ofs<1) ofs=1;
    /*
      I think I'll change this part of code using a Fill function
      Because painting on a printer I think I may have problems.
    */
    /*
      //  int ofs;
      for(ofs=0;ofs<rootFont+1;ofs++){
      pen->drawLine(x+familySize.x()+ofs,y+familySize.y()+unit+1,
      x+familySize.x()+unit+ofs,y+familySize.bottom()+3+ofs);//familySize.y is < 0
      pen->drawLine(x+familySize.x()+unit+ofs,y+familySize.y()+3+ofs,
      x+familySize.x()+unit+ofs,y+familySize.bottom()+ofs);
      pen->drawLine(x+familySize.x()+unit+ofs,y+familySize.y()+3+ofs,
      x+familySize.right()+rootFont+1,y+familySize.y()+3+ofs);
      } */
      
      QColor elementColor(pen->pen().color());
      pen->setBrush(elementColor);
     
     
      pen->setPen(QPen(elementColor,ofs*2));
      pen->drawLine(x+familySize.x()+unit/3,y+familySize.y()+unit+2,
      x+familySize.x()+unit/2+unit/3,y+familySize.bottom()+1);//familySize.y is < 0

      pen->setPen(QPen(elementColor,ofs));

      pen->drawLine(x+familySize.x()+unit+unit/3,y+familySize.y()+3,
      x+familySize.x()+unit/2+unit/3,y+familySize.bottom());

      pen->drawLine(x+familySize.x()+unit+unit/3,y+familySize.y()+3,
      x+familySize.right()+1,y+familySize.y()+3);
      	
      pen->drawLine(x+familySize.x()+unit/3,y+familySize.y()+unit+1,
             x+familySize.x(),y+familySize.y()+unit+1+unit/2);
      
      
/*    warning("Array");
    QPointArray points(10);
    points.setPoint(1,x+familySize.x(),y+familySize.y()+unit);
    points.setPoint(2,x+familySize.x()+unit,y+familySize.bottom()+1);
    points.setPoint(3,x+familySize.x()+unit+ofs,y+familySize.bottom()+1+ofs);
    points.setPoint(4,x+familySize.x()+unit+ofs,y+familySize.y()+1+ofs);
    points.setPoint(5,x+familySize.right()+ofs-2,y+familySize.y()+1+ofs);
    points.setPoint(6,x+familySize.right()+ofs-2,y+familySize.y()+1);
    points.setPoint(7,x+familySize.x()+unit,y+familySize.y()+1);
    points.setPoint(8,x+familySize.x()+unit,y+familySize.bottom()+1-2*ofs);
    // points.setPoint(9,x+familySize.x()+unit,y+familySize.bottom()+1);
    points.setPoint(9,x+familySize.x()+ofs,y+familySize.y()+unit);
    pen->setBrush(pen->pen().color());
    pen->drawPolygon(points,FALSE,1,9);
*/    /*
      Draw child[0], it must exist
    */
    if( beActive )
	pen->setPen(Qt::blue);
    child[0]->draw(QPoint(x+familySize.x()+unit+unit/3+2+ofs+1,y),resolution);
    myArea=globalSize;;
    myArea.moveBy(x,y);
    // globalArea=
    // globalArea.moveBy(x,y);
#ifdef RECT
    pen->drawRect(myArea);
    // pen->drawRect(globalArea);
#endif
    if(index[0]!=0L)
	index[0]->draw(drawPoint+
		       familySize.topLeft()-
		       index[0]->getSize().bottomRight()
		       +QPoint(unit/2+unit/3,unit),
		       resolution);

    drawIndexes(pen,resolution);
    if( beActive )
	pen->setPen(Qt::black);
    if(next!=0L) next->draw(drawPoint+QPoint(localSize.width(),0),resolution);


}

void RootElement::checkSize()
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
    familySize.setTop(familySize.top()-8-(numericFont/24));
    familySize.setLeft(familySize.left()-8-(numericFont/24)-(familySize.height()/2));

    /*
    localSize is
    child[0]+lines or pixmap
    here put the code to calc it
    unit=familySize.height()/4;
  */

    localSize=familySize;
    checkIndexesSize();  //This will change localSize adding Indexes Size
    familySize.moveBy(-localSize.left(),0);
    localSize.moveBy(-localSize.left(),0);
    globalSize=localSize;
    nextDimension.moveBy(localSize.width(),0);
    globalSize=globalSize.unite(nextDimension);
    //warning("end");
}
/*
int RootElement::takeAsciiFromKeyb(int)
{
    return 2;
}
*/
/*int RootElement::takeActionFromKeyb(int)
{
    return 0;
}*/

void RootElement::setNumericFont(int value)
{
    numericFont=value;
    /*
      We can use differnt pixmap:
      numericFont < 20   tinyRoot.xpm
      20 < numericFont < 50 normalRoot.xpm
      numericFont < 20   bigRoot.xpm
    */
}

void RootElement::drawIndexes(QPainter *,int resolution)
{
    /*
      Index[0] is in draw() must be lower...
    */
    QPoint dp = myArea.topLeft() - globalSize.topLeft();
    if(index[1]!=0L)
	index[1]->draw(dp+familySize.bottomLeft() -
		       index[1]->getSize().topRight(),resolution);
    if(index[2]!=0L)
	index[2]->draw(dp+familySize.topRight() -
		       index[2]->getSize().bottomLeft(),resolution);
    if(index[3]!=0L)
	index[3]->draw(dp+familySize.bottomRight() -
		       index[3]->getSize().topLeft(),resolution);
}



