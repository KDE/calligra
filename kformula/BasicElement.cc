/*
  BasicElement.cc 
  Project KOffice/KFormula
  
  Author: Andrea Rizzi <rizzi@kde.org>
  License:GPL
*/
 
//#define RECT

#include <qrect.h> 
#include "BasicElement.h" 
#include "formuladef.h"
#include "kformula_doc.h"

BasicElement::BasicElement(KFormulaDocument *Formula,
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
    if(prev!=NULL) {
	numericFont=prev->getNumericFont();
	//warning("Font OK");
    } else
	numericFont=32;

    childrenNumber=0;    
    index[0]=0L;
    index[1]=0L;
    index[2]=0L;
    index[3]=0L;
    beActive=FALSE;
}

BasicElement::~BasicElement()
{
    /*ToDo:
     * Link prev with next & remove itself
     */
}

BasicElement *BasicElement::isInside(QPoint point)
{
    int i;
    if(myArea.contains(point)) {

	BasicElement *aValue=0L;
	if(next!=0)
	    if((aValue=next->isInside(point))!=0L) 
		return aValue;
	
	for(i=0;i<4;i++) 
	    if(index[i]!=0)
		if((aValue=index[i]->isInside(point))!=0L) 
		    return aValue;
	
	for(i=0;i<childrenNumber;i++)
	    {
		if(child[i]!=0) 
		    {
			warning("Child %i",i);
			if((aValue=child[i]->isInside(point))!=0L) 
			    return aValue;
		    }
	    }
	return this;

    } else 
	return 0L;
}

void BasicElement::draw(QPoint drawPoint,int resolution)
{
    QPainter *pen = formula->painter();
    //  QRect globalArea;
    int x = drawPoint.x();
    int y = drawPoint.y();
    if( beActive )
	pen->setPen(red);
    pen->drawRect(x+familySize.x(),y-5,10,10);
  
    myArea=globalSize;
    myArea.moveBy(x,y);
#ifdef RECT
    pen->drawRect(myArea);   
#endif
    if(beActive)
	pen->setPen(blue);
    drawIndexes(pen,resolution);
    if(beActive)
	pen->setPen( black);
    if(next!=0L) next->draw(drawPoint+QPoint(localSize.width(),0),resolution);
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

void BasicElement::checkIndexesSize()
{ 
    QRect indexDimension;
    QPoint vectorT;
    int i;
    for(i=0;i<4;i++) 
	if (index[i]!=0L) index[i]->checkSize();
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
    if((level & FN_ELEMENT)>0) { 
	if ((level & FN_REDUCE)>0)
	    {
		if((level & FN_BYNUM)>0) 
		    setNumericFont(numericFont-(level & 255));
		else  { 
		    int den=(level & 15);
		    int num=((level>>4) & 15);
		    setNumericFont((numericFont*num)/den);
		}
	    } else {
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

int BasicElement::takeActionFromKeyb(int)
{
    return  0;
}

void  BasicElement::substituteElement(BasicElement *clone)
{
    int i;
    clone->setContent(content);
    clone->setNext(next);
    clone->setPrev(prev);
    for(i=0;i<4;i++) {
	clone->setIndex(index[i],i);
	if(index[i]!=0L) index[i]->setPrev(clone);
    }
    /*  clone->setIndex(index[1],1);
	clone->setIndex(index[2],2);
	clone->setIndex(index[3],3);
    */
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
    warning("Substituted %p with %p,  waiting to be deleted",this,clone); 
}

int BasicElement::takeAsciiFromKeyb(char ch) 
{
    if(typeid(this) == typeid(BasicElement))     //This function may be used by RootElement
	//  content.insert(content.length(),ch);
	return FCOM_TEXTCLONE; //  Ask to be cloned into text & deleted
    else return 1;
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
    prev=element;
    element->setRelation(relation);
    element->setNext(this);
    relation=-1;
}

void  BasicElement::save(int) 
{ 
}

void  BasicElement::load(int) 
{ 
}
