/*
 FormulaClass.h
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

/*
 * Fonts enalarge/reduce constants
 */
#include <qpainter.h>
#ifndef _FORMULA_CLASS_H_
#define _FORMULA_CLASS_H_

class BasicElement;

class FormulaClass 
{
public:
 FormulaClass(QPainter *p=0L);
 void setPainter(QPainter* p) { thePainter=p; }
 void setActiveElement(BasicElement* c); 
 void setCursor(QRect r) { theCursor=r; }
 void setFont(QFont f) { theFont=f;}
 void setColor(QColor c) { theColor=c;}
 void setFirstElement(BasicElement* c);
 QPainter * painter() { return thePainter; } 
 BasicElement *activeElement() {return theActiveElement; } 
 QRect cursor() {return theCursor;}
 QFont generalFont() {return theFont; }
 QColor generalColor() {return theColor; }

protected:
// QPoint theClickPoint;
 QPainter *thePainter;
 BasicElement *theActiveElement;
 BasicElement *theFirstElement;
 QRect theCursor;
 QFont theFont;
 QColor theColor;
};

#endif		     
