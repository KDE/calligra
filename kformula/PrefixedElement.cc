/*
  PrefixedElement.cc
  Project KOffice/KFormula

  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/

//#define RECT

#include "BasicElement.h"
#include "PrefixedElement.h"
#include "formuladef.h"
#include "formula_container.h"
#include <stdlib.h>

#include <kdebug.h>

#include <qrect.h>

PrefixedElement::PrefixedElement(KFormulaContainer *Formula,
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
  childrenNumber=3;
  minChildren=1;
  child.resize(childrenNumber);
  child[0]=0L;
  child[1]=0L;
  child[2]=0L;
}

PrefixedElement::~PrefixedElement()
{
}

void PrefixedElement::draw(QPoint drawPoint,int resolution)
{

  QPainter *pen=formula->painter();
  //QRect globalArea;
  int x,y,unit=0; //unit is familySize.height/6,used to draw proportional Prefixeds
  x=drawPoint.x();
  y=drawPoint.y();
  if( beActive )
    pen->setPen(Qt::red);


  /*
    Draw Prefixed!!
  */
  int ofs=(numericFont/24)+1;
  unit = 0;
  int symT=0;   //Top & Bottom of
  int symB=0;   // symbol (integral,sum...)
  if( content[1]=='S')
    {
      symT=familySize.top();
      symB=familySize.bottom();
      unit = (familySize.height()-2*ofs)/8;
    }
  if( content[1]=='F')
    {
      unit = content.mid(2,3).toInt();
      symB= unit*4;
      symT=- unit*4;
    }
  if (unit< 4)
    unit =4;

  kdDebug(39001) <<"Unit= "<<unit<<endl;

  if (unit <6)
    ofs=1;

  if(content[0]=='I')  //Integral
    {
      QColor elementColor(pen->pen().color());
      pen->setBrush(elementColor);
      pen->drawChord(x+familySize.x()-ofs+unit+1,y+symT,
		     unit+2*ofs,(unit+ofs)*2,
		     0,180*16);
      pen->setBrush(pen->backgroundColor());
      pen->setPen(pen->backgroundColor());
      pen->drawChord(x+familySize.x()+unit+1,y+symT+ofs  ,
		     unit,unit*2+ofs,
		     0,180*16);
      pen->setPen(elementColor);

      pen->setBrush(elementColor);
      pen->drawChord(x+familySize.x()+1,y+symB-(unit+ofs)*2,
		     unit+ofs*2,(unit+ofs)*2,
		     180*16,180*16);
      pen->setBrush(pen->backgroundColor());
      pen->setPen(pen->backgroundColor());
      pen->drawChord(x+familySize.x()+ofs+1,y+symB-(unit+ofs)*2,
		     unit,unit*2+ofs,
		     180*16,180*16);
      pen->setPen(elementColor);

      QPointArray points(5);
      points.setPoint(1,x+familySize.x()+unit+ofs+1 ,y+symB-unit-ofs    );
      points.setPoint(2,x+familySize.x()+unit+ofs*2  ,y+symB-unit-ofs  );
      points.setPoint(3,x+familySize.x()+unit,y+symT+unit+ofs-1);
      points.setPoint(4,x+familySize.x()-ofs+unit+1,y+symT+unit+ofs-1);
      pen->setBrush(pen->pen().color());
      pen->drawPolygon(points,FALSE,1,4);
    }
    //area is too small
    //start to implement SUM
    else  if(content[0]=='S')  //Sum
    {
      QPointArray points(6);
      points.setPoint(1,x+familySize.x()+unit , y+familySize.y()  );
      points.setPoint(2,x+familySize.x() , y+familySize.y() );
      points.setPoint(3,x+familySize.x()+unit,y+familySize.y()+ (y+familySize.bottom()-(y+familySize.y()))/2);
      points.setPoint(4,x+familySize.x(),y+familySize.bottom());
      points.setPoint(5,x+familySize.x()+unit , y+familySize.bottom());
      pen->setBrush(pen->pen().color());
      pen->drawPolyline(points,1,5);
    }
    //start to implement Product

    else if(content[0]=='P')  //Product
    {
     QPointArray points(6);
      points.setPoint(1,x+familySize.x()+unit , y+familySize.y()  );
      points.setPoint(2,x+familySize.x() , y+familySize.y() );
      pen->setBrush(pen->pen().color());
      pen->drawPolyline(points,1,2);
      points.setPoint(1,x+familySize.x()+unit/4,y+familySize.y());
        points.setPoint(2,x+familySize.x()+unit/4/*+ofs*/,y+familySize.y());
	  points.setPoint(3,x+familySize.x()+unit/4/*+ofs*/,y+familySize.bottom());
	  points.setPoint(4,x+familySize.x()+unit/4,y+familySize.bottom());
                pen->setBrush(pen->pen().color());
	  pen->drawPolyline(points,1,4);
                        points.setPoint(1,x+familySize.x()+unit*3/4,y+familySize.y());
	  points.setPoint(2,x+familySize.x()+unit*3/4/*+ofs*/,y+familySize.y());
	  points.setPoint(3,x+familySize.x()+unit*3/4/*+ofs*/,y+familySize.bottom());
	  points.setPoint(4,x+familySize.x()+unit*3/4,y+familySize.bottom());
	  pen->setBrush(pen->pen().color());
	  pen->drawPolyline(points,1,4);

    }
  /*
    Draw child[0], it must exist
  */
  if( beActive )
    pen->setPen(Qt::blue);
  //I must add MaxLimitWidth
  //    child[0]->draw(QPoint(x+familySize.x()+unit*2+ofs+1,y),resolution);
  child[0]->draw(QPoint(x+familySize.right()-child[0]->getSize().width(),y),resolution);

  int y1=0,y2=0;
  if( content[1]=='S')
    {
      if(child[1]!=0)
	y1=y-child[1]->getSize().top()+familySize.top();
      if(child[2]!=0)
	y2=y-child[2]->getSize().bottom()+familySize.bottom();
    }
  if( content[1]=='F')
    {
      if(child[1]!=0)
	y1=y-child[1]->getSize().bottom()-unit*2;
      if(child[2]!=0)
	y2=y-child[2]->getSize().top()+unit*2;
    }

  if(child[1]!=0)
    child[1]->draw(QPoint(x+familySize.x()+unit*2+ofs+1,y1),resolution);

  if(child[2]!=0)
    child[2]->draw(QPoint(x+familySize.x()+unit*2+ofs+1,y2),resolution);

  myArea=globalSize;
  myArea.moveBy(x,y);
  // globalArea=
  // globalArea.moveBy(x,y);
#ifdef RECT
  pen->drawRect(myArea);
  // pen->drawRect(globalArea);
#endif
  drawIndexes(pen,resolution);
  if( beActive )
    pen->setPen(Qt::black);
  if(next!=0L)
    next->draw(drawPoint+QPoint(localSize.width(),0),resolution);
}

void PrefixedElement::checkSize()
{
  kdDebug(39001) <<"R "<<this<<endl;
  QRect nextDimension;
  QRect child1Size,child2Size;
  if (next!=0L)
    {
      next->checkSize();
      nextDimension=next->getSize();
    }

  child[0]->checkSize();
  familySize=child[0]->getSize();

  if (child[1]!=0)
    {
      //kdDebug(39001) <<"There is a CHild1:lowerLimit\n";
      child[1]->checkSize();
      child1Size=child[1]->getSize();
    }
  else
    child1Size.setRect(0,0,1,1);

  if (child[2]!=0)
    {
      kdDebug(39001) <<"There is a CHild2:lowerLimit\n";
      child[2]->checkSize();
      child2Size=child[2]->getSize();
    }
  else
    child2Size.setRect(0,0,1,1);

  int MaxX=0;
  if (child1Size.width()>child2Size.width())
    MaxX=child1Size.width();
  else
    MaxX=child2Size.width();
  familySize.moveBy(MaxX,0);
  child1Size.moveBy(0,familySize.top()-child1Size.bottom());
  child2Size.moveBy(0,familySize.bottom()-child2Size.top());
  familySize=familySize.unite(child1Size);
  familySize=familySize.unite(child2Size);

  int unit=0;
  if( content[1]=='S')
    unit = (familySize.height())/8 ;
  if( content[1]=='F')
    unit = content.mid(2,3).toInt();
  if (unit< 4)
    unit =4;
  kdDebug(39001) <<"Unit= "<<unit<<endl;

  int ofs=(numericFont/24);
  if (unit <6)ofs=1;
  familySize.setTop(familySize.top()-1-ofs);
  familySize.setLeft(familySize.left()-1-ofs-unit*2);

  if (familySize.height()< unit*8 )
    {
      if (familySize.top() >-unit*4)
	familySize.setTop(-unit*4 );
      if (familySize.bottom() < unit*4)
	familySize.setBottom( unit*4  );

    }

  localSize=familySize;
  checkIndexesSize();  //This will change localSize adding Indexes Size
  familySize.moveBy(-localSize.left(),0);
  localSize.moveBy(-localSize.left(),0);
  globalSize=localSize;
  nextDimension.moveBy(localSize.width(),0);
  globalSize=globalSize.unite(nextDimension);

}
/*
int PrefixedElement::takeAsciiFromKeyb(int)
{
    return 2;
} */

/*int PrefixedElement::takeActionFromKeyb(int)
{
    return -1;
}*/

void PrefixedElement::setNumericFont(int value)
{
    numericFont=value;
    /*
      We can use differnt pixmap:
      numericFont < 20   tinyPrefixed.xpm
      20 < numericFont < 50 normalPrefixed.xpm
      numericFont < 20   bigPrefixed.xpm
    */
}
