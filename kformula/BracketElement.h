#ifndef  _BRACKET_ELEMENT_H_
#define  _BRACKET_ELEMENT_H_

/*
 BracketElement.h 
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

#include <qpoint.h>
#include <qstring.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qrect.h> 
#include <qfont.h>
#include "BasicElement.h"

class BracketElement : public BasicElement
{
 public:
    /*
     * Normal constructor, Get font from prev element
     */
    BracketElement(KFormulaDoc *Formula,
		   BasicElement *Prev=NULL,
		   int Relation=-1,
		   BasicElement *Next=NULL,
		   QString Content="");
       
    /*
     * Default destructor
     */
    virtual   ~BracketElement();
     
    /*
     * each derived class must implement its own Draw()
     * "prev" is responsable for x,y
     * 
     */
    virtual void draw(QPoint drawPoint,int resolution=72);

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
     * Scale font of my child
     */
    //   virtual  void scaleChildrenFont(int level) { child[0]->scaleNumericFont(level); }  
    
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
   
    /*
     * Again, in  the future....
     */
    /*   virtual void save(int file);
	 virtual void load(int file);
    */
 protected:
   
    /*
     * If usePixmap is True we need a...
     */    
    QPixmap *rootPixmap;
  
    bool usePixmap;
  
     
   
};

#endif
