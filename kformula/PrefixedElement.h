#ifndef  _PREFIXED_ELEMENT_H_
#define  _PREFIXED_ELEMENT_H_

/*
 PrefixedElement.h 
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

#include <qpainter.h>

class BasicElement;

class PrefixedElement : public BasicElement
{
 public:
    /*
     * Normal constructor, Get font from prev element
     */
    PrefixedElement(KFormulaContainer *Formula,BasicElement *Prev=0,
		int Relation=-1,BasicElement *Next=0,
		QString Content="");
       
    /*
     * Link Next & Prev removing itself
     */
    virtual   ~PrefixedElement();
     
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
     * Again, in  the future....
     */
    /*   virtual void save(int file);
	 virtual void load(int file);
    */

    QString getContent() const {return content;}  
    void setContent(QString a) {content=a.copy();}  
    QString getContent() { return content;}
    
//protected:
private:
   
    /*
     *Note: Content meaning
     * 
     * content[0] 'S'=sum 'I'=integral 'P'=I don't know english name ;( (product)
     *                    'C'=closed (?) integral  
     * content[1] 'F'=Fixed size 'S'=AutoScale Size
     * content[2..4] Fixed Size Value
     */
    QString content;
    
     /*
     * If usePixmap is True we need a...
     */    
    
    
    QPixmap *PrefixedPixmap;
  
    bool usePixmap;
  
     
   
};

#endif
