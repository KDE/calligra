/*
  BasicElement.cc
  Project KOffice/KFormula

  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/

//#define RECT
#include <qrect.h>


#include <kdebug.h>

#include "BasicElement.h"
#include "formuladef.h"
#include "formula_container.h"

BasicElement::BasicElement(KFormulaContainer *Formula,
			   BasicElement *Prev,
			   int Relation,
			   BasicElement *Next,
			   QString Content)
{
    formula=Formula;
    prev=Prev;
    relation=Relation;
    next=Next;
    content=Content;
    if(prev!=NULL)
        {
	  numericFont=prev->getNumericFont();
	  kdDebug(39001) <<"Font OK\n";
        }
    else
      numericFont=24;

    childrenNumber=0;
    minChildren=0;
    index[0]=0L;
    index[1]=0L;
    index[2]=0L;
    index[3]=0L;
    beActive=FALSE;
}

BasicElement::~BasicElement()
{
    /* See deleteElement to
     * Link prev with next & remove itself
     */

}

BasicElement *BasicElement::isInside(QPoint point)
{
    int i;
    if(myArea.contains(point))
        {

	  BasicElement *aValue=0L;
	  if(next!=0)
	    {
	      if((aValue=next->isInside(point))!=0L)
		return aValue;
	    }
	  for(i=0;i<4;i++)
	    if(index[i]!=0)
	      if((aValue=index[i]->isInside(point))!=0L)
		return aValue;

	  for(i=0;i<childrenNumber;i++)
	    {
	      if(child[i]!=0)
		{
		  //kdDebug(39001) << "Child "<<i<<endl;
		  if((aValue=child[i]->isInside(point))!=0L)
		    return aValue;
		}
	    }
	  return this;

        }
    else
      return 0L;
}

void BasicElement::draw(QPoint drawPoint,int resolution)
{
    QPainter *pen = formula->painter();
    //  QRect globalArea;
    int x = drawPoint.x();
    int y = drawPoint.y();
    if( beActive )
      pen->setPen(Qt::red);
    pen->setBrush(Qt::NoBrush);
    pen->drawRect(x+familySize.x(),y-5,10,10);
    
    myArea=globalSize;
    myArea.moveBy(x,y);
#ifdef RECT
    pen->drawRect(myArea);
#endif
    if(beActive)
      pen->setPen(Qt::blue);
    drawIndexes(pen,resolution);
    if(beActive)
      pen->setPen(Qt::black);
    if(next!=0L)
        next->draw(drawPoint+QPoint(localSize.width(),0),resolution);

}

void BasicElement::drawIndexes(QPainter *,int resolution)
{
    //draw point
    QPoint dp = myArea.topLeft()-globalSize.topLeft();

    if(index[0]!=0L)
	index[0]->draw(dp + familySize.topLeft() -
		       index[0]->getSize().bottomRight(),
		       resolution);
    if(index[1]!=0L)
	index[1]->draw(dp + familySize.bottomLeft() -
		       index[1]->getSize().topRight(),
		       resolution);
    if(index[2]!=0L)
	index[2]->draw(dp + familySize.topRight() -
		       index[2]->getSize().bottomLeft(),
		       resolution);
    if(index[3]!=0L)
	index[3]->draw(dp + familySize.bottomRight() -
		       index[3]->getSize().topLeft(),
		       resolution);
}

void BasicElement::checkSize()
{
    //warning("%p",this);
    //kdDebug(39001) <<"%p"<<this<<endl;
    QRect nextDimension;

    if (next!=0L)
      {
	next->checkSize();
	nextDimension=next->getSize();
      }
    localSize=QRect(0,-5,10,10);
    familySize=localSize;
    checkIndexesSize();  //This will change localSize adding Indexes Size
    familySize.moveBy(-localSize.left(),0);
    localSize.moveBy(-localSize.left(),0);
    globalSize=localSize;
    nextDimension.moveBy(localSize.width(),0);
    globalSize=globalSize.unite(nextDimension);

}

void BasicElement::check()
{
  int i=0;
  for(i=0;i<childrenNumber;i++)
    {
      if (child[i]==0L)
	{
	  if (i<minChildren)
	    child[i]=new BasicElement(formula,this,i+4);
	}
    }
}
void BasicElement::checkIndexesSize()
{
    QRect indexDimension;
    QPoint vectorT;
    int i;
    for(i=0;i<4;i++)
        {
        if (index[i]!=0L)
                index[i]->checkSize();
        }

    if(index[0]!=0L)
	{
	    indexDimension=index[0]->getSize();
	    vectorT=familySize.topLeft()-indexDimension.bottomRight();
	    indexDimension.moveBy(vectorT.x(),vectorT.y());
	    localSize=localSize.unite(indexDimension);
	}
    if(index[1]!=0L)
	{
	    indexDimension=index[1]->getSize();
	    vectorT=familySize.bottomLeft()-indexDimension.topRight();
	    indexDimension.moveBy(vectorT.x(),vectorT.y());
	    localSize=localSize.unite(indexDimension);
	}

    if(index[2]!=0L)
	{
	    indexDimension=index[2]->getSize();
	    vectorT=familySize.topRight()-indexDimension.bottomLeft();
	    indexDimension.moveBy(vectorT.x(),vectorT.y());
	    localSize=localSize.unite(indexDimension);
	}
    if(index[3]!=0L)
	{
	    indexDimension=index[3]->getSize();
	    vectorT=familySize.bottomRight()-indexDimension.topLeft();
	    indexDimension.moveBy(vectorT.x(),vectorT.y());
	    localSize=localSize.unite(indexDimension);
	}
}

void BasicElement::scaleNumericFont(int level)
{
    if((level & FN_ELEMENT)>0) 
      {
	if ((level & FN_REDUCE)>0)
	  {
	    if((level & FN_BYNUM)>0)
	      setNumericFont(numericFont-(level & 255));
	    else  
	      {
		int den=(level & 15);
		int num=((level>>4) & 15);
		setNumericFont((numericFont*num)/den);
	      }
	  } 
	else 
	  {
	    if((level & FN_BYNUM)>0)
	      setNumericFont(numericFont+(level & 255));
	    else
	      {
		int num=(level & 15);
		int den=((level>>4) & 15);
		setNumericFont((numericFont*num)/den);
	      }
	  }
      }
    int ps;
    if(level & FN_INDEXES)
	for(ps=0;ps<4;ps++)
	    if(index[ps]!=0L) index[ps]->scaleNumericFont(level|FN_ALL);

    if(level & FN_NEXT)
	if(next!=0) next->scaleNumericFont(level|FN_ALL);

    if(level & FN_CHILDREN)
	{
	    for(ps=0;ps<childrenNumber;ps++)
		if(child[ps]!=0L)child[ps]->scaleNumericFont(level|FN_ALL);
	}
    if(numericFont<FN_MIN) numericFont=FN_MIN;
    if(numericFont>FN_MAX) numericFont=FN_MAX;
}


void  BasicElement::setNext(BasicElement *newNext)
{
    next=newNext;
}

void  BasicElement::setPrev(BasicElement *newPrev)
{
    prev=newPrev;
}

void BasicElement::setNumericFont(int value)
{
    numericFont=value;
}

void  BasicElement::substituteElement(BasicElement *clone)
{
    int i;
    clone->setContent(content);
    clone->setNext(next);
    clone->setPrev(prev);
    for(i=0;i<4;i++) 
      {
	clone->setIndex(index[i],i);
	if(index[i]!=0L) 
	  index[i]->setPrev(clone);
      }
    clone->setNumericFont(numericFont);
    clone->setColor(defaultColor);
    clone->setRelation(relation);
    if(prev!=0L)
	{
	    if(relation<4)
		{
		    if(relation>=0)
			prev->setIndex(clone,relation);
		    else
			prev->setNext(clone);
		}
	    else
		prev->setChild(clone,relation-4);
	}
    else //I'm the first element!!
	formula->setFirstElement(clone);
    //kdDebug(39001) <<"Substituted "<<this<<"  waiting to be deleted"<<clone<<endl;
}



void  BasicElement::insertElement(BasicElement *element)
{
    element->setPrev(prev);
    if(prev!=0L)
	{
	    if(relation<4)
		{
		    if(relation>=0)
			prev->setIndex(element,relation);
		    else
			prev->setNext(element);
		}
	    else
		prev->setChild(element,relation-4);
	}
    else //I'm the first element!!
	formula->setFirstElement(element);


    prev=element;
    element->setRelation(relation);
    element->setNext(this);
    relation=-1;
}

void  BasicElement::deleteElement()
{
  kdDebug(39001) <<"deleteElement of -> "<<this<<"   prev "<<prev<<"    next "<<next<<endl;
  if(next!=0L)
    {
      next->setPrev(prev);
      next->setRelation(relation);
    }
  if(prev!=0L)
    {
      if(relation<4)
	{
	  if(relation>=0)
                        {
	        prev->setIndex(next,relation);
                        }
	  else
                        {
	        prev->setNext(next);
                        }
	}
      else
                {
                  prev->setChild(next,relation-4);
                }
    }

  else //I'm the first element.
    {
    if(next!=0L)
      formula->setFirstElement(next);
    else
      formula->setFirstElement(new BasicElement(formula));
    }
    int nc=0;
    while (nc<childrenNumber)
	{
	    if (child[nc]!=0L)
		{
		    //warning("I'm %p, I delete my child[%d]=%p i.e. %d of %d",this,nc,child[nc],nc+1,childrenNumber);
		    child[nc]->deleteElement();
		}
	    else
		nc++;
	}
    while (nc<4)
	{
	    if (index[nc]!=0L)
		index[nc]->deleteElement();
	    else
		nc++;
	}
    delete this;  // It is a good call ?


}

QDomElement BasicElement::save( QDomDocument& doc ) const
{
  QDomElement format2 = doc.createElement( "format2" );
  format2.setAttribute("type",childrenNumber);
  return format2;
  /*    out << "TYPE=" << -1 << " "
	<< "CONTENT=" << content.utf8().data() << " "
	<< "NUMERICFONT=" << numericFont << " "
	<< " >" << endl;

    for(int i=0;i<4;i++)
	if(index[i]!=0L)
	    {
		out << " <ELEM INDEX=" << i << " ";
		index[i]->save(out);
	    }

    for(int i=0;i<childrenNumber;i++)
	if(child[i]!=0L)
	    {
		out << " <ELEM CHILD=" << i << " ";
		child[i]->save(out);
	    }
    if(next!=0)
	{
	    out << " <ELEM NEXT ";
	    next->save(out);
	}
	out << "</ELEM>" << endl;*/
}
/*void  BasicElement::save(ostream& out)
{
    out << "TYPE=" << -1 << " "
	<< "CONTENT=" << content.utf8().data() << " "
	<< "NUMERICFONT=" << numericFont << " "
	<< " >" << endl;

    for(int i=0;i<4;i++)
	if(index[i]!=0L)
	    {
		out << " <ELEM INDEX=" << i << " ";
		index[i]->save(out);
	    }

    for(int i=0;i<childrenNumber;i++)
	if(child[i]!=0L)
	    {
		out << " <ELEM CHILD=" << i << " ";
		child[i]->save(out);
	    }
    if(next!=0)
	{
	    out << " <ELEM NEXT ";
	    next->save(out);
	}
    out << "</ELEM>" << endl;
    }*/

void  BasicElement::load(istream& )
{
}

void BasicElement::makeList(bool active)
{
//    warning("make list %p " ,this);
    bool basic;
    basic=(typeid(*this) == typeid(BasicElement));
    if(!basic)
	formula->addElement(this, 0);
//    warning("append");
    kdDebug(39001) <<"append\n";

    beActive=0;

    for(int i=0;i<2;i++)
	if(index[i]!=0) {
            kdDebug(39001) <<"call for index%d"<<i<<" %p"<<index[i]<<endl;
	    index[i]->makeList(active);
	}
    kdDebug(39001) <<"index OK\n";

    for(int i=0;i<childrenNumber;i++)
	if(child[i]!=0)
	    {
	//	warning("call for child%d %p",i,child[i]);
		child[i]->makeList(active);
	    }
    if(basic) 
	formula->addElement(this);

    kdDebug(39001) <<"children done\n";

    for(int i=2;i<4;i++)
	if(index[i]!=0) {
//	    warning("call for index%d %p",i,index[i]);
	    index[i]->makeList(active);
	}

    if(next!=0)
	next->makeList(active);
    else if(!basic)
	formula->addElement(this);
}

QRect BasicElement::getCursor(int atPos) 
{
    QPoint dp = myArea.topLeft()-globalSize.topLeft();
    if (typeid(*this) == typeid(BasicElement))
	return (QRect(dp.x()+familySize.x()+3,dp.y()-8,5,16));	
    else 
	{
	    if(atPos==0)
		return (QRect(dp.x()+localSize.x(),dp.y()-7,5,14));
	    else
		return (QRect(dp.x()+localSize.right(),dp.y()-8,5,16));	
	}


    return QRect(0,0,0,0);
}
