#ifndef  _TEXT_ELEMENT_H_
#define  _TEXT_ELEMENT_H_

/*
 TextElement.h 
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

#include <qpainter.h>

#include "BasicElement.h"

class TextElement : public BasicElement
{
public:
   /*
    * Normal constructor, Get font from prev element
    */
     TextElement(KFormulaContainer *Formula,
		 BasicElement *Prev=0,
		 int Relation=-1,
		 BasicElement *Next=0,
		 QString Content="");

   /*
    * Link Next & Prev removing itself
    */
   virtual   ~TextElement();

   /*
    * This is very useful for TextElement.
    * thanks to this function we can store in only 1 element
    * long( >1) string and add element in it in a very simple
    * way:Splitting it!
    *
    * "abcdefg"
    *     ^\__i want to insert here
    * Split!
    * "abc" "defg"
    * insert a block
    * "abc"#"defg"
    *
    * if pos=-1 it splits to the current cursor position
    */
   void split(int pos);

   /*
    * each derived class must implement its own Draw()
    * "prev" is responsable for x,y
    *
    */
   virtual void draw(QPoint drawPoint, int resolution);

   /*
    * each derived class must implement its own CheckSize()
    * void because autostore size in Data
    */
   virtual void checkSize();

   /*
    * Change font size
    */
   virtual void setNumericFont(int newValue);
   virtual void makeList(bool active=0);

   /*
    *
    *
    */
    virtual QRect getCursor(int atPos);

   /*
    * Change "font"
    * Rewrite This, it's better to pass a QFont not a QFont*
    */
   void changeFontFamily(QString family);

   void changePropertieFont(signed char _bold=-1,signed char _underline=-1,signed char _italic=-1);
   bool isBold() {return font.bold();  }
   bool isItalic() {return font.italic();  }
   bool isUnderline() {return font.underline();  }
   QString textFontFamily() const;

   void setPosition(int _pos) { position=_pos;}
   /*
    * Again, in  the future....
    */
   /*   virtual void save(int file);
	virtual void load(int file);
   */

    QString getContent() const {return text;}  
    void setContent(QString a) {text=a.copy();}  
    QString getContent() { return text;}
    
//protected:
private:

    QString text;

   /*
    * Normal text need it
    * in normal text it is the StrikeOutPosition
    */
   int offsetY;

   /*
    * Font
    */
   QFont font;

   /*
    * Cursor Internal position
    */
   uint position;

 };
#endif
