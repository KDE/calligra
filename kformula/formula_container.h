#ifndef _kformula_container_h
#define _kformula_container_h

#include <qlist.h>
#include <qfont.h>
#include <qcolor.h>
struct PosType;
class BasicElement;

class KFormulaContainer //: public QObject
{
//    Q_OBJECT
	
 public:
    
    KFormulaContainer();

    void setPainter(QPainter* p) { thePainter=p; }
    QPainter *painter() const { return thePainter; } 

    void setFirstElement(BasicElement* c);

    QFont generalFont() const { return theFont; }
    void setFont(const QFont& f) { theFont=f;}
    
    QColor generalColor() const { return theColor; }
    void setColor(const QColor& c) { theColor=c;}

    BasicElement *currentElement() const;

    void addElement(BasicElement *e, int pos = -1);

 protected:
    QPainter *thePainter;
    QFont theFont;
    QColor theColor;

    BasicElement *theFirstElement;
    
    QList<PosType> eList;
};

#endif
