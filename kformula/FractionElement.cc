/*
  FractionElement.cc
  Project KOffice/KFormula

  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/

//#define RECT
#include <qrect.h>
#include <stdlib.h>
#include "formula_container.h"
#include "BasicElement.h"
#include "FractionElement.h"
#include "formuladef.h"


FractionElement::FractionElement(KFormulaContainer *Formula,
				 BasicElement *Prev,
				 int Relation,
				 BasicElement *Next,
				 QString Content) :
    BasicElement(Formula,Prev,Relation,Next), content(Content)
{
    /*
      Central alligned fraction
    */
    content="FMC5";
    childrenNumber=2;
    minChildren=2;
    child.resize(childrenNumber);
    child[0]=0L;
    child[1]=0L;
    relation=Relation;

}

FractionElement::~FractionElement()
{
}

void FractionElement::draw(QPoint drawPoint,int resolution)
{

    QPainter *pen;
    pen=formula->painter();
    //QRect globalArea;
    int x,y;
    x=drawPoint.x();
    y=drawPoint.y();
    //int space=atoi(content.right(content.length()-3);
    int space=content.right(content.length()-3).toInt();
    //warning("Sapce %i",space);
    space+=numericFont/24;


    if( beActive )
	pen->setPen(Qt::red);


    int ofs=(numericFont/24);
    //warning("Array");

    if (content[0]=='F') {
	QPointArray points(5);
	points.setPoint(1,x+familySize().x()+1,y+offsetY-ofs/2);
	points.setPoint(2,x+familySize().right()-2,y+offsetY-ofs/2);
	points.setPoint(3,x+familySize().right()-2,y+offsetY+ofs/2);
	points.setPoint(4,x+familySize().x()+1,y+offsetY+ofs/2);
	pen->setBrush(pen->pen().color());
	pen->drawPolygon(points,FALSE,1,9);
    }

    if( beActive )
	pen->setPen(Qt::blue);
    int y0=-child[0]->getSize().bottom()-space/2;
    int y1=-child[1]->getSize().top()+space/2;
    int x0=0,x1=0;
    y0+=offsetY;
    y1+=offsetY;
    if(offsetX>0) x0=offsetX;
    if(offsetX<0) x1=-offsetX;

    child[0]->draw(QPoint(x+x0+3,y+y0),resolution);
    child[1]->draw(QPoint(x+x1+3,y+y1),resolution);
    setMyArea(globalSize());
    myArea().moveBy(x,y);

#ifdef RECT
    pen->drawRect(myArea());
    // pen->drawRect(globalArea);
#endif

    drawIndexes(pen,resolution);
    if( beActive )
	pen->setPen(Qt::black);
    if(next!=0L) next->draw(drawPoint+QPoint(localSize().width(),0),resolution);


}

void FractionElement::checkSize()
{
    //warning("R %p",this);
    QRect nextDimension;

    if (next!=0L)
	{
	    next->checkSize();
	    nextDimension=next->getSize();
	}
    child[0]->checkSize();
    child[1]->checkSize();
    //int space=atoi(content.right(content.length()-3));
    int space=content.right(content.length()-3).toInt();
    //warning("Space %i",space);
    space+=numericFont/24;
    QRect child0Size=child[0]->getSize();
    QRect child1Size=child[1]->getSize();
    if(content[1]=='U')
	offsetY=child0Size.bottom()+space/2;
    if(content[1]=='D')
	offsetY=child1Size.top()-space/2;
    if(content[1]=='M')
	offsetY=0;
    offsetX=child1Size.width()-child0Size.width();
    if(content[2]=='L')
	offsetX=0;
    if(content[2]=='C')
	offsetX/=2;
    if(content[2]=='R')
        {
	//warning("R");
        }
    child0Size.moveBy(0,-space/2-child0Size.bottom());
    child1Size.moveBy(0,space/2-child1Size.top());
    setFamilySize(child0Size.unite(child1Size));

    familySize().moveBy(0,offsetY);
    familySize().setLeft(familySize().left()-3);
    familySize().setRight(familySize().right()+3);
    setLocalSize(familySize());
    checkIndexesSize();  //This will change localSize() adding Indexes Size
    familySize().moveBy(-localSize().left(),0);
    localSize().moveBy(-localSize().left(),0);
    setGlobalSize(localSize());
    nextDimension.moveBy(localSize().width(),0);
    setGlobalSize(globalSize().unite(nextDimension));
}

/*int FractionElement::takeAsciiFromKeyb(int)
{
    return 2;
}
*/

/*int FractionElement::takeActionFromKeyb(int)
{
    return -1;
}*/

void FractionElement::setNumericFont(int value)
{
    numericFont=value;
    /*
      We can use differnt pixmap:
      numericFont < 20   tinyFraction.xpm
      20 < numericFont < 50 normalFraction.xpm
      numericFont < 20   bigFraction.xpm
    */
}




