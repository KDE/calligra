#ifndef _BASIC_ELEMENT_H_
#define _BASIC_ELEMENT_H_

/*
 BasicElement.h
 Project KOffice/KFormula


 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/
// #include <koStream.h>

#include <qpainter.h>

#include <typeinfo>
#include <iostream>

#include "formuladef.h"
#include <qdom.h>

class BasicElement;
struct PosType
{
    BasicElement *element;
    int pos;
};

/*
 * BasicElement is the mother of all XxxxElement
 * It show a simple SQUARE 
 *
 */
class KFormulaContainer;

class BasicElement
{
 public:
    /*
     * Normal constructor, Get font from prev element
     */
    BasicElement(KFormulaContainer *Formula,
		 BasicElement *Prev=0,
		 int Relation=-1,
		 BasicElement *Next=0,
		 QString Content="");
  
    /*
     * Link Next & Prev removing itself
     */
    virtual   ~BasicElement();

    /*
     * each derived class must implement its own Draw()
     * "prev" is responsable for x,y
     *
     */
    virtual void draw(QPoint drawPoint,int resolution = 72);
  
    /*
     * Must be called by draw() 
     * RootElement may need to rewrite it.
     */ 
    virtual void drawIndexes(QPainter *pen,int resolution);
  
    /*
     * each derived class must implement its own CheckSize()
     * void because autostore size in Data
     */ 
    virtual void checkSize();
  
    /*
     * Check if needed children exists.
     *
     */
    virtual void check();
     

    /*
     * Must be called by draw() 
     * RootElement may need to rewrite it.
     */ 
    virtual void checkIndexesSize();

  
    /*
     * Scale Font size
     */
    void scaleNumericFont(int level); 

    /*
     * Set Font size
     */
    virtual void setNumericFont(int newValue); 
  
    /*  
     * Change pointer of next element
     */
    void setNext(BasicElement *newNext);

    /*  
     * Change poniter of previous element
     */
    void setPrev(BasicElement *newPrev);
  
    /*
     * Sets the beActive flag status
     *
     */
    void setActive(bool ac) { beActive=ac; }
  
    /*
     * Useful for Mouse click
     */
    virtual BasicElement *isInside(QPoint point);
  

    virtual QRect getCursor(int atPos);
  
  
    /*
     * Return element font
     */
    int getNumericFont() const { return numericFont; }
  
    /*
     * Return globalsize (see globalSize)
     */
    QRect getSize() const { return globalSize; }

    /*
     * Return next
     */
    BasicElement * getNext() const { return next; }
    /*
     * Return prev
     */
    BasicElement * getPrev() const { return prev; }

    /*
     * Return index
     */
    BasicElement * getIndex(int ind) const { return index[ind]; }

    /*
     * Return child
     */
    BasicElement * getChild(int chi) const { if(chi<childrenNumber) return child[chi]; else return 0L; }

    /*
     * Return the default color of element
     */ 
    QColor *getColor() const { return defaultColor; }
  
    QString getContent() const {return content;}  
    /*
     * Various Set-GetFunction
     */  
    void setContent(QString a) {content=a.copy();}  
    QString getContent() { return content;}
    void setIndex(BasicElement *e,int i) {index[i]=e; }    
    void setChild(BasicElement *e,int i) {child[i]=e; }
    void setColor(QColor *c) {defaultColor=c; }       
    void setRelation(int r) {relation=r; }       
    
    /*
     * Update the eList of kformuladoc
     * 
     */
      virtual void makeList(bool active=0);

    /*
     * change type
     * clone MUST be a derived class of BasicElement 
     * Note:every link with children is lost
     * Use this function only to change from BasicElement
     */
    void substituteElement(BasicElement *clone);
   
    /*
     * insert an element between this element and prev
     */
    void insertElement(BasicElement *element);    
  
    /*
     * delete this element, its children & index 
     */
    void deleteElement();
  
    /*
     *  At the moment they do nothing.
     *  
     */
    //    virtual void save(ostream& out);
    QDomElement save( QDomDocument& doc ) const;
    virtual void load(istream& in);

 protected:
    /*
     * I know nothing about the future of this member
     */
    KFormulaContainer *formula;
  
    /*
     * realtionship with prev
     * -1=I'm its Next
     * 0..3=Index 0..3
     * 4..n=child[n-4] 
     */
    int relation;

    /*
     * Next & previous Elements
     */
    BasicElement *next;
    BasicElement *prev;

    /*
     * Index printed near the corners of element 
     * Those aren't integral or Sum limits 
     * But left_up may be root index
     * 0=left_up,1=left_down,2=right_up,3=right_down
     */
    BasicElement  *index[4]; 
  
    /*
     * each dervied class could add children 
     * Sum,integral,matrix,fraction,root,delimiter,decoration need children
     * Plaintext,TextSymbol,operators do not need children 	
     * adding a:
     *	BasicElement *child[n]
     */
    QArray<BasicElement *> child;
    
    /*
     * The number of children.
     */
    int childrenNumber;

    /*
     * The number of children.
     */
    int minChildren;

    /*
     * real data: text,symbol code,delimiter code,matrix rows&cols
     */
    QString content;    
  
    /*
     * Size of (family+next) & next elements
     */
    QRect globalSize;   //y=0  base line
  
    /*
     * Size of family & indexes
     * This name isn't very good.I'm sorry.
     */
    QRect localSize;   
  
    /*
     * Size of Family:father & children  (without indexes )
     */
    QRect familySize;
  
    /*
     * Area where I am painted (last time)
     * localSize+drawPoint
     */
    QRect myArea;    

    /*
     * Font
     *
     QFont font;
    */
    /*
     * numericFont correspond to
     * font.pointSize for text,
     * a Scale value for symbols & decoration 
     * the number of line of a drawed(not pixmap) Root, Integral or Bracket
     */
    int numericFont;
  
    /*
     * When editing defaultColor may be overiden
     * if defaultColor is 0 it use  formula->defaultColor
     */
    QColor *defaultColor;
  
    /*
     * True if element is current (edited) element
     */
    bool beActive;
   /*
    * Cursor Internal position
    * In TextElement it is the position of the cursor into the contentstring
    * pos==0  Before element
    * pos!=0  After.
    */

};

#endif
