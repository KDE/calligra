/*
  MatrixElement.cc
  Project KOffice/KFormula

  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/

//#define RECT
#include <qrect.h>
#include <stdlib.h>
#include "BasicElement.h"
#include "MatrixElement.h"
#include "formuladef.h"
#include "formula_container.h"

MatrixElement::MatrixElement(KFormulaContainer *Formula,
			     BasicElement *Prev,
			     int Relation,
			     BasicElement *Next,
			     QString Content) :
    BasicElement(Formula,Prev,Relation,Next), content(Content)
{
    /*
      Central alligned noborder 3x3 Matrix
    */
    /*  content="MMC003003001006NNNNNN";
	childrenNumber=12;
	child.resize(childrenNumber);
	childPoint.resize(childrenNumber);
    */}

MatrixElement::~MatrixElement()
{
}

void MatrixElement::setChildrenNumber(int n)
{
    childrenNumber=n;
    minChildren=n;
    child.resize(childrenNumber);
    childPoint.resize(childrenNumber);
    hby.resize(childrenNumber);  //It's too much, but they are 2 only bytes..
}

void MatrixElement::draw(QPoint drawPoint,int resolution)
{

    QPainter *pen;
    pen=formula->painter();
    //QRect globalArea;
    int x,y;
    x=drawPoint.x()+familySize().x();
    y=drawPoint.y();
    int rows=content.mid(3,3).toInt();
    int cols=content.mid(6,3).toInt();
    if( beActive )
	pen->setPen(Qt::red);
    /*
      Here draw borders
    */
    int ofs=numericFont/24;
    int r,c;
    int minX;

    for(c=0;c<cols;c++)
      {
	minX=32000;

	for(r=0;r<rows;r++)
	    {
		child[c+r*cols]->draw(QPoint(x+3,y)+childPoint[c+r*cols],resolution);
		if(childPoint[c+r*cols].x()<minX) minX=childPoint[c+r*cols].x();
	     if(c==0)
              if(r<rows-1) {
      	       if(content[15]=='L')
                {
                 int vspace=content.mid(12,3).toInt()+ofs;
		QPointArray points(4);
	        points.setPoint(0,x,y+hby[r]+childPoint[c+r*cols].y()+vspace/2-ofs+ofs/2);
		points.setPoint(1,x,y+hby[r]+childPoint[c+r*cols].y()+vspace/2+ofs/2);
		points.setPoint(2,x+familySize().width(),
				  y+hby[r]+childPoint[c+r*cols].y()+vspace/2+ofs/2);
	        points.setPoint(3,x+familySize().width(),
				  y+hby[r]+childPoint[c+r*cols].y()+vspace/2-ofs+ofs/2);
		pen->setBrush(pen->pen().color());
		pen->drawPolygon(points,FALSE,0,4);

		}
	       if(content[15]=='D')
                {
                 int vspace=content.mid(12,3).toInt();
		QPointArray points(4);
	        points.setPoint(0,x,y+hby[r]+childPoint[c+r*cols].y()+vspace/2-1);
		points.setPoint(1,x,y+hby[r]+childPoint[c+r*cols].y()+vspace/2+ofs-1);
		points.setPoint(2,x+familySize().width(),
				  y+hby[r]+childPoint[c+r*cols].y()+vspace/2-1);
	        points.setPoint(3,x+familySize().width(),
				  y+hby[r]+childPoint[c+r*cols].y()+vspace/2+ofs-1);
		pen->setBrush(pen->pen().color());
		pen->drawPolygon(points,FALSE,0,4);

	        points.setPoint(0,x,y+hby[r]+childPoint[c+r*cols].y()+vspace/2+1+2*ofs);
		points.setPoint(1,x,y+hby[r]+childPoint[c+r*cols].y()+vspace/2+3*ofs+1);
		points.setPoint(2,x+familySize().width(),
				  y+hby[r]+childPoint[c+r*cols].y()+vspace/2+1+2*ofs);
	        points.setPoint(3,x+familySize().width(),
				  y+hby[r]+childPoint[c+r*cols].y()+vspace/2+3*ofs+1);
		pen->setBrush(pen->pen().color());
		pen->drawPolygon(points,FALSE,0,4);
		}

	       }
	    }

	 if(c>0) {
          if(content[16]=='L')
 	   {
            int hspace=content.mid(12,3).toInt()+ofs;
	    QPointArray points(4);
	    points.setPoint(0,minX+x+3-hspace/2-ofs+ofs/2,y+familySize().y());
	    points.setPoint(1,minX+x+3-hspace/2-ofs+ofs/2,y+familySize().bottom());
	    points.setPoint(2,minX+x+3-hspace/2+ofs/2,y+familySize().bottom());
	    points.setPoint(3,minX+x+3-hspace/2+ofs/2,y+familySize().y());
	    pen->setBrush(pen->pen().color());
	    pen->drawPolygon(points,FALSE,0,4);
	  }
	  if(content[16]=='D')
 	   {
            int hspace=content.mid(12,3).toInt();
	    QPointArray points(4);
	    points.setPoint(0,minX+x+3-hspace/2-ofs+1,y+familySize().y());
	    points.setPoint(1,minX+x+3-hspace/2-ofs+1,y+familySize().bottom());
	    points.setPoint(2,minX+x+3-hspace/2+1,y+familySize().bottom());
	    points.setPoint(3,minX+x+3-hspace/2+1,y+familySize().y());
	    pen->setBrush(pen->pen().color());
	    pen->drawPolygon(points,FALSE,0,4);
	    points.setPoint(0,minX+x+3-hspace/2-3*ofs-1,y+familySize().y());
	    points.setPoint(1,minX+x+3-hspace/2-3*ofs-1,y+familySize().bottom());
	    points.setPoint(2,minX+x+3-hspace/2-2*ofs-1,y+familySize().bottom());
	    points.setPoint(3,minX+x+3-hspace/2-2*ofs-1,y+familySize().y());
	    pen->setBrush(pen->pen().color());
	    pen->drawPolygon(points,FALSE,0,4);

	   }
	  }
      }
    if( beActive )
	pen->setPen(Qt::blue);

    x=drawPoint.x();

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

void MatrixElement::checkSize()
{
    //warning("M %p",this);
    QRect nextDimension;
    int rows=content.mid(3,3).toInt(); //Number of rows
    int cols=content.mid(6,3).toInt(); //Number of columns
    int midr=content.mid(9,3).toInt(); //Mid row
    int space=content.mid(12,3).toInt(); //Space between elments
//    warning("Rows:%i Cols:%i MidR:%i Space:%i",rows,cols,midr,space);

    if (next!=0L)
	{
	    next->checkSize();
	    nextDimension=next->getSize();
	}

    for(int chi=0;chi<rows*cols;chi++)
	child[chi]->checkSize();           //check size of every child

    int vspace = space, hspace=space;               //real spaces (change if there are borders)
    int ofs=numericFont/24;
    if (ofs<1) ofs=1;
    if(content[1]=='C')
        midr=0;
    if(content[16]=='L')
        hspace+=ofs;
    if(content[15]=='L')
        vspace+=ofs;
    if(content[16]=='D')
        hspace+=3*ofs;
    if(content[15]=='D')
        vspace+=3*ofs;

    int correction=0;
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
		}
            hby[r]=sizeR.bottom();
	    if(r<=midr)
		{
		    downy=-sizeC.bottom()+sizeR.top()-vspace;
		    if (r==0) downy=sizeR.top();
		    sizeC.moveBy(0,downy);
		    for(c=0;c<cols;c++)
			{
			    e=r*cols+c;
			    childPoint[e]=QPoint(0,0);
			}
		    for(c=0;c<(cols*r);c++)
			{
			//    warning("N:%i MoveUp by:%i",c,downy);
			    childPoint[c]+=QPoint(0,downy);
			}
		}

	    if(r==midr)
		{

		    //warning("MIDROW");
		    if(content[1]=='U')
			correction=-sizeR.top();
		    if(content[1]=='C')
			correction=-sizeR.top();
		    if(content[1]=='D')
			correction=-sizeR.bottom();

		}

	    if(r>midr)
		{
		    downy=sizeC.bottom()-sizeR.top()+vspace;
		    sizeR.moveBy(0,downy);
		    for(c=0;c<cols;c++)
			{
			    e=r*cols+c;
			    childPoint[e]=QPoint(0,downy);
			}
		}
	    sizeC=sizeC.unite(sizeR);
	}

    setFamilySize(sizeC);
    // familySize().setBottom(familySize().bottom()+vspace);
    familySize().moveBy(0,correction);
    int topBorderCorr=0;
    if(content[17]=='L')
       topBorderCorr=ofs+1+space;
    if(content[17]=='D')
       topBorderCorr=3*ofs+2+space;
    //correction+=topBorderCorr;
    familySize().setTop(familySize().top()-topBorderCorr);

    for(c=0;c<cols*rows;c++)
	childPoint[c]+=QPoint(0,correction);

    if(content[1]=='C')
	correction=-familySize().height()/2;
    familySize().moveBy(0,correction);

    for(c=0;c<cols*rows;c++)
	childPoint[c]+=QPoint(0,correction);

/*
And now columns!!
*/
    int right,x;
    sizeR.setRect(0,0,1,1);
    if(content[19]=='L')
     sizeR.setRect(0,0,ofs+space+1,1);
    if(content[19]=='D')
     sizeR.setRect(0,0,3*ofs+space+2,1);


    for(c=0;c<cols;c++) {
	sizeC.setRect(0,0,0,0);
	for(r=0;r<rows;r++) {
	    e=c+r*cols;
	    sizeE=child[e]->getSize();
	    sizeC=sizeC.unite(sizeE);
	}
	right=sizeR.width();
//	warning("C-X:%i,Y:%i,W:%i,E:%i",sizeC.x(),sizeC.y(),sizeC.height(),sizeC.width());
	sizeC.moveBy(hspace+right,0);
//	warning("C+X:%i,Y:%i,W:%i,E:%i",sizeC.x(),sizeC.y(),sizeC.height(),sizeC.width());
	for(r=0;r<rows;r++) {
	    e=c+r*cols;
	    x=sizeC.width()-child[e]->getSize().width();
	    if (content[2]=='C')
		x/=2;
	    if (content[2]=='L')
		x=0;
	    childPoint[e]=QPoint(x+right,childPoint[e].y());
	}
//	warning("R-X:%i,Y:%i,W:%i,E:%i",sizeR.x(),sizeR.y(),sizeR.height(),sizeR.width());
	sizeR=sizeR.unite(sizeC);
//	warning("R+X:%i,Y:%i,W:%i,E:%i",sizeR.x(),sizeR.y(),sizeR.height(),sizeR.width());
    }
    sizeR.setRight(sizeR.right()-hspace);
    setFamilySize(familySize().unite(sizeR));

    //Add Margins!!!

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

/* int MatrixElement::takeAsciiFromKeyb(int)
{
    return 2;
} */
/*int MatrixElement::takeActionFromKeyb(int)
{
    return 0;
}*/

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




