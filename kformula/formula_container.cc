#include "formula_container.h"
#include "BasicElement.h"

BasicElement *KFormulaContainer::currentElement() const 
{ 
    return eList.current()->element; 
}

void KFormulaContainer::setFirstElement(BasicElement* c)
{
    if (c)
	theFirstElement = c;
    /*else
	//warning("Try to set first element to 0L");*/
}

void KFormulaContainer::addElement(BasicElement *e, int pos)
{
    PosType *p = new PosType;
    p->element = e;
    p->pos = pos;
    eList.append(p);
}

KFormulaContainer::KFormulaContainer() 
{
    eList.clear();
    eList.setAutoDelete(true); 
}

//#include "formula_container.moc"
