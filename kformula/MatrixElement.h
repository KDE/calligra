/*
 MatrixElement.h 
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

#ifndef  _MATRIX_ELEMENT_H_
#define  _MATRIX_ELEMENT_H_
class MatrixElement : public BasicElement
 {
 public:
   /*
    * Normal constructor, Get font from prev element
    */
   MatrixElement(KFormulaDocument *Formula,BasicElement *Prev=NULL,int Relation=-1,BasicElement *Next=NULL,
                QString Content="");
       
   /*
    * Link Next & Prev removing itself
    */
   virtual   ~MatrixElement();
     
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
    
   virtual void setChildrenNumber(int n);  
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
   
   virtual int type() {return EL_MATRIX; }   

   /*
    * In  the future....
    */
/*   virtual void save(int file);
   virtual void load(int file);
*/
protected:
        
  /*
   * Note for "content" meaning:
   * 3..5:cols
   * 6..8:rows
   * 9..11:mid row
   * 12..14:space
   * 1: M=midline-midline,U=midline-up,D=midline-down,C=midline-HalfMatrix
   * 2: C,L,R Horiz align
   * 15,16,17,18,19,20=Horiz,Vert,top,bottom,left,right border: L=Single Line
   */
  QArray<QPoint> childPoint;
  
  
 };
#endif
