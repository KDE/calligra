/*
 FormulaClass.cc
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/
#include "FormulaClass.h"
#include "BasicElement.h"
FormulaClass::FormulaClass(QPainter *p=0L) 
{ 
 thePainter=p; 
}

void FormulaClass::setActiveElement(BasicElement* c)
 { 
  if(theActiveElement!=0L) theActiveElement->setActive(FALSE);
  theActiveElement=c;
  if(c!=0L)theActiveElement->setActive(TRUE);    
 }

