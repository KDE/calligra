#ifndef  _FRACTION_ELEMENT_H_
#define  _FRACTION_ELEMENT_H_

/*
 FractionElement.h 
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

#include <qpainter.h>

class BasicElement;

class FractionElement : public BasicElement
{
 public:
    /*
     * Normal constructor, Get font from prev element
     */
    FractionElement(KFormulaContainer *Formula,
		    BasicElement *Prev=0,
		    int Relation=-1,
		    BasicElement *Next=0,
		    QString Content="");
       
    /*
     * Link Next & Prev removing itself
     */
    virtual   ~FractionElement();
     
    /*
     * each derived class must implement its own Draw()
     * "prev" is responsable for x,y
     * 
     */
    virtual void draw(QPoint drawPoint,int resolution);

    /*
     * each derived class must implement its own CheckSize()
     * void because autostore size in Data
     */ 
    virtual void checkSize(); 
      
    /*
     * Change font size
     */
    virtual void setNumericFont(int newValue); 
      
    /*
     * usually call by keyPressEvent()
     * if input is delete,backspace,arrows,home,end....
     * return cursor position (-1 if no cursor is need)
     */
    
//    virtual int takeActionFromKeyb(int action);
    /*
     * do nothing
     */
//    virtual int takeAsciiFromKeyb(int action);
   
    /*
     * In  the future....
     */
    /*   virtual void save(int file);
	 virtual void load(int file);
    */
 protected:
        
    /*
     * Note for "content" meaning:
     * content[0]  'F'=with line 'V'= no-line
     * content[1](vertAlign) 'U'=up 'M'=mid 'L'=low
     * content[2]( HorizAlign) 'L'=left 'C'=center 'R'=right
     * content+3 space between num & den
     */
    int offsetY;
    int offsetX;   
};
#endif
