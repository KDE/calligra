/*
  PrefixedElement.cc 
  Project KOffice/KFormula
  
  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/

//#define RECT
#include <qrect.h>
#include "BasicElement.h" 
#include "PrefixedElement.h" 
#include "formuladef.h"
#include "kformula_doc.h"

PrefixedElement::PrefixedElement(KFormulaDocument *Formula,
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
    child.resize(childrenNumber);
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
	pen->setPen(red);

  
    /*  
	Draw Prefixed!!
    */
    int ofs=(numericFont/32)+1; 
    unit = 0;
    int symT=0;   //Top & Bottmo of
    int symB=0;   // symbol (integral,sum...)
    if( content[1]=='S') 
     {
      symT=familySize.top();
      symB=familySize.bottom(); 
      unit = (familySize.height()-2*ofs)/8;
     } 
    if( content[1]=='F')
     {
     
      unit = atoi(content.mid(2,3));
     symB= unit*4;
      symT=- unit*4;
     }
    if (unit< 4) unit =4;
    warning("Unit=%i ",unit);
    if (unit <6)ofs=1;

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
    /*
      Draw child[0], it must exist
    */    
    if( beActive )
	pen->setPen(blue);
   //I must add MaxLimitWidth
    child[0]->draw(QPoint(x+familySize.x()+unit*2+ofs+1,y),resolution);
    myArea=globalSize;;
    myArea.moveBy(x,y);
    // globalArea=
    // globalArea.moveBy(x,y);
#ifdef RECT
    pen->drawRect(myArea);   
    // pen->drawRect(globalArea); 
#endif
    drawIndexes(pen,resolution);
    if( beActive )
	pen->setPen(black);
    if(next!=0L) next->draw(drawPoint+QPoint(localSize.width(),0),resolution);
  
  
}

void PrefixedElement::checkSize()
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
    int unit=0;
    if( content[1]=='S')
     unit = (familySize.height())/8 ;
    if( content[1]=='F')
     unit = atoi(content.mid(2,3));
     if (unit< 4) unit =4;
    warning("Unit=%i ",unit);
    
    familySize.setTop(familySize.top()-1-(numericFont/32));
    familySize.setLeft(familySize.left()-1-(numericFont/32)-unit*2);  
  
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

int PrefixedElement::takeAsciiFromKeyb(int)
{
    return 2;
}
int PrefixedElement::takeActionFromKeyb(int)
{
    return -1;
}

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
