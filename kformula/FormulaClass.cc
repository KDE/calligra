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
if(p==0L)
 p=new QPainter();
 thePainter=p; 
     
warning("General Settings");
 theFont.setFamily( "utopia" );
 theFont.setPointSize(32);
 theFont.setWeight( QFont::Normal );
 theFont.setItalic( FALSE ); 
 theColor=black;
warning("General Font OK");
 theActiveElement = 0;
}

void FormulaClass::setActiveElement(BasicElement* c)
 { 
  if(theActiveElement!=0L) theActiveElement->setActive(FALSE);
  theActiveElement=c;
  if(c!=0L)theActiveElement->setActive(TRUE);    
 }

