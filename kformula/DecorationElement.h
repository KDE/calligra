#ifndef  _DECORATION_ELEMENT_H_
#define  _DECORATION_ELEMENT_H_

/*
 DecorationElement.h 
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

#include <qpainter.h>

class BasicElement;

class DecorationElement : public BasicElement
{
 public:
    /*
     * Normal constructor, Get font from prev element
     */
    DecorationElement(KFormulaDoc *Formula, BasicElement *Prev=0,
		int Relation = -1, BasicElement *Next=0,
		QString Content = "");
       
    /*
     * Link Next & Prev removing itself
     */
    virtual  ~DecorationElement();
     
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
    
    virtual int takeActionFromKeyb(int action);
    /*
     * do nothing
     */
    virtual int takeAsciiFromKeyb(int action);
   
 
 protected:
   
    /*
     * If usePixmap is True we need a...
     */    
    QPixmap *symbPixmap;
  
    bool usePixmap;
/*
  "Content" meaning
   char[0..3] "High Decoration"
   char[4..7] "Low Decoration"
   char[8..11] "High Decoration Len. (in pixels) 0000=Auto"
   char[12..15] "Low Decoration Len. (in pixels) 0000=Auto"
  
   Decoration Names:
   XXXX	   = Nothing
   ASL0    = Arrow Right to Left single line
   ASR0    =    "   Left to Right   "     "
   ASD0    = Double Arrow            "    "
   ADL0    = Arrow Right to Left double line
   ADR0    =    "   Left to Right   "     "
   ADD0    = Double Arrow            "    "
   LINn    = Line ( 0 < n < 4 = prop. distance 0 : Nearer)
   LIDn    = Double Line  
   TILD    = Tilde
   ANGL    = ^ in Italian "Accento circonflesso" I need english name
   CIRC    =  Ellipse (if Len. auto it is a circ.)
   RECT    =  Rectangle 
   HRUP    =   |_____|
   HRDW    =    V-Flip the prevoius ;)

   .......
   
 */  
     
   
};

#endif
