/*
  MatrixElement.cc 
  Project KOffice/KFormula
  
  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/

#define RECT
#include <qrect.h> 
#include <stdlib.h>
#include "BasicElement.h" 
#include "MatrixElement.h" 
#include "formuladef.h"

MatrixElement::MatrixElement(FormulaClass *Formula,BasicElement *Prev=0L,int Relation=-1,BasicElement *Next=0L,
				 QString Content="") : BasicElement(Formula,Prev,Relation,Next,Content)
{
  /*
    Central alligned noborder 3x3 Matrix
  */
  content="003003001006MCNNNNNN";
  childrenNumber=9;
  child.resize(childrenNumber);
  childPoint.resize(childrenNumber);
}

MatrixElement::~MatrixElement()
{
}

void MatrixElement::draw(QPoint drawPoint,int resolution=72)
{
  
  QPainter *pen;
  pen=formula->painter();
  //QRect globalArea;
  int x,y; 
  x=drawPoint.x();
  y=drawPoint.y();
  int rows=atoi(content.mid(0,3));
  int cols=atoi(content.mid(3,3));
 /* warning("Sapce %i",space);
  space+=numericFont/32;
  */
  if( beActive )
    pen->setPen(red);
 /*
  Here drow borders
 */
 
  int ofs=(numericFont/32); 
  warning("Array");
  /*
if (content[0]=='F') {
  QPointArray points(5);  
  points.setPoint(1,x+familySize.x()+1,y+offsetY-ofs/2);
  points.setPoint(2,x+familySize.right()-2,y+offsetY-ofs/2);
  points.setPoint(3,x+familySize.right()-2,y+offsetY+ofs/2);
  points.setPoint(4,x+familySize.x()+1,y+offsetY+ofs/2);
  pen->setBrush(pen->pen().color());
  pen->drawPolygon(points,FALSE,1,9);
  }*/
  
  if( beActive )
    pen->setPen(blue);
  int r,c;
  for(r=0;r<rows;r++)
   for(c=0;c<cols;c++) {
     warning("R:%i C:%i N:%i",r,c,c+r*cols);
     child[c+r*cols]->draw(QPoint(x,y)+childPoint[c+r*cols],resolution);
}
  myArea=globalSize;
  myArea.moveBy(x,y);

#ifdef RECT
  pen->drawRect(myArea);   
  // pen->drawRect(globalArea); 
#endif
 
  drawIndexes(pen,resolution);
  if( beActive )
    pen->setPen(black);
  if(next!=0L) next->draw(drawPoint+QPoint(localSize.width(),0),resolution);
  
  
}

void MatrixElement::checkSize()
{
//warning("M %p",this);
QRect nextDimension; 
  int rows=atoi(content.mid(0,3)); //Number of rows
  int cols=atoi(content.mid(3,3)); //Number of columns
  int midr=atoi(content.mid(6,3)); //Mid row
  int space=atoi(content.mid(9,3)); //Space between elments
  warning("Rows:%i Cols:%i MidR:%i Space:%i",rows,cols,midr,space);

if (next!=0L)
{
  next->checkSize();
  nextDimension=next->getSize();
}

for(int chi=0;chi<rows*cols;chi++)
 child[chi]->checkSize();           //check size of every child
 
 int vspace=space,hspace=space;               //real spaces (change if there are borders)
 int ofs=numericFont/32;
 if(content[12]=='M') midr=0;
 if(content[14]=='L') hspace+=ofs;
 if(content[15]=='L') vspace+=ofs; 

 QRect sizeC;
 QRect sizeR;
 QRect sizeE;
 int r,c,e;
 int downy=0;  
 sizeC.setRect(0,0,1,1);
 for(r=0;r<rows;r++)
  {
   sizeR.setRect(0,0,1,1);
   for(c=0;c<cols;c++)
    {
     e=r*cols+c;
     sizeE=child[e]->getSize();
     sizeR=sizeR.unite(sizeE);
     warning("ROW-R:%i C:%i N:%i",r,c,c+r*cols);
    }
   
   if(r<=midr) 
    {
     warning("r%i<=midr%i",r,midr);
     downy=-sizeC.bottom()+sizeR.top()-vspace;
     sizeC.moveBy(0,downy);              
     for(c=0;c<cols;c++)
      {
        e=r*cols+c;
	childPoint[e]=QPoint(0,0);
      }
     for(c=0;c<(cols*r);c++)
      {
        warning("N:%i MoveUp by:%i",c,downy);
	childPoint[c]+=QPoint(0,downy);
      }                  
    } 
/*  
   if(r=mdir)
    {
     downy=sizeR.bottom();
    }
  */
   if(r>midr) 
    {
     warning("r%i>midr%i",r,midr);
     downy=sizeC.bottom()-sizeR.top()+vspace;
     sizeR.moveBy(0,downy);              
     for(c=0;c<cols;c++)
      {
        e=r*cols+c;
	warning("%i,%i:%i 0,%i",r,c,e,downy);
	childPoint[e]=QPoint(0,downy);
      }               
    } 
    sizeC=sizeC.unite(sizeR);
        
  }
  familySize=sizeC;
   sizeC.setRect(0,0,1,1);
/*
And now columns!!
*/ 
 int right,x;
 sizeR.setRect(0,0,1,1);
 for(c=0;c<cols;c++)
  {
   sizeC.setRect(0,0,0,0);
   for(r=0;r<rows;r++)
    {
     e=c+r*cols;
     sizeE=child[e]->getSize();
     sizeC=sizeC.unite(sizeE);
    }
    right=sizeR.width();
    warning("C-X:%i,Y:%i,W:%i,E:%i",sizeC.x(),sizeC.y(),sizeC.height(),sizeC.width());    
    sizeC.moveBy(hspace+right,0);
    warning("C+X:%i,Y:%i,W:%i,E:%i",sizeC.x(),sizeC.y(),sizeC.height(),sizeC.width());
   for(r=0;r<rows;r++)
    {
     e=c+r*cols;
     x=sizeC.width()-child[e]->getSize().width();
     if (content[13]=='C')
      x/=2;
     if (content[13]=='L')
      x=0;
     childPoint[e]=QPoint(x+right,childPoint[e].y());
    }
    warning("R-X:%i,Y:%i,W:%i,E:%i",sizeR.x(),sizeR.y(),sizeR.height(),sizeR.width());
  sizeR=sizeR.unite(sizeC);        
    warning("R+X:%i,Y:%i,W:%i,E:%i",sizeR.x(),sizeR.y(),sizeR.height(),sizeR.width());
  } 
    sizeR.setRight(sizeR.right()-hspace);  
    familySize=familySize.unite(sizeR);
          
    //Add Margins!!!
	  
 familySize.setLeft(familySize.left()-3);  
 familySize.setRight(familySize.right()+3);
  localSize=familySize;
  checkIndexesSize();  //This will change localSize adding Indexes Size
  familySize.moveBy(-localSize.left(),0);
  localSize.moveBy(-localSize.left(),0);
  globalSize=localSize;
  nextDimension.moveBy(localSize.width(),0);
  globalSize=globalSize.unite(nextDimension);
}

int MatrixElement::takeAsciiFromKeyb(int action)
{
  return 2;
}
int MatrixElement::takeActionFromKeyb(int action)
{
  return -1;
}

void MatrixElement::setNumericFont(int value)
{
  numericFont=value;
  /*
    We can use differnt pixmap:
    numericFont < 20   tinyMatrix.xpm
    20 < numericFont < 50 normalMatrix.xpm
    numericFont < 20   bigMatrix.xpm
  */
}




