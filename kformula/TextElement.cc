/*
 TextElement.cc
 Project KOffice/KFormula

 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

//#define RECT
#include <qrect.h>
#include <qkeycode.h>
#include "BasicElement.h"
#include "TextElement.h"
#include "formuladef.h"
#include "kformula_doc.h"

TextElement::TextElement(KFormulaDoc *Formula,
			 BasicElement *Prev,
			 int Relation,
			 BasicElement *Next,
			 QString Content) :
    BasicElement(Formula,Prev,Relation,Next,Content)
{
    // warning("A new text is born.. ");
    font=0L;
    position=content.length();
    warning("creation %i",position);

}

TextElement::~TextElement()
{
}

void TextElement::draw(QPoint drawPoint,int resolution)
{
    QPainter *pen = formula->painter();

    int x=drawPoint.x();
    int y=drawPoint.y();

    if( beActive )
	pen->setPen(Qt::red);
    if ( content.isEmpty() )
	pen->drawRect(x+familySize.x(),y-5,5,10);
    else {

	//warning("Font for text");
	QFont formulaFont;
	if(font!=0L)
	    formulaFont=(*font);
	else
	    {
		formulaFont=formula->generalFont();
		formulaFont.setPointSize(numericFont);
	    }
	pen->setFont(formulaFont);
	pen->drawText(x+familySize.x(),y+offsetY,content);
	if( beActive ) {
	    QFontMetrics fm(formulaFont);
	    formula->setCursor(QRect(x+familySize.x()+fm.width(content,position),
				     y+familySize.top()-1,
				     3,familySize.height()+2));
	}
	
    }
    if(beActive)
	pen->setPen(Qt::blue);
    myArea=globalSize;
    myArea.moveBy(x,y);
#ifdef RECT
    /*QRect  localArea;
      localArea=localSize;
      localArea.moveBy(x,y);
      pen->drawRect(localArea);
      localArea=familySize;
      localArea.moveBy(x,y);
      pen->drawRect(localArea);
    */
    pen->setBrush(green);
    pen->setBrush(NoBrush);
    pen->drawRect(myArea);
    QRect area(localSize);
    area.moveBy(x,y);
    pen->drawRect(area);
    pen->setBrush(SolidPattern);
#endif
    drawIndexes(pen,resolution);

    if(beActive)
	pen->setPen(Qt::black);
    if(next!=0L) next->draw(drawPoint+QPoint(localSize.width(),0),resolution);
}

void TextElement::checkSize()
{
    //  warning("T %p",this);
    QFont formulaFont;
    if(font!=0L)
	formulaFont=(*font);
    else {
	formulaFont=formula->generalFont();
	formulaFont.setPointSize(numericFont);
    }

    QFontMetrics fm(formulaFont);
    QRect nextDimension;

    if (next!=0L) {
	next->checkSize();
	nextDimension=next->getSize();
    }

    offsetY=fm.strikeOutPos();
    familySize=fm.boundingRect(content);
    familySize.moveBy(0,offsetY);
    if(content.isEmpty())
	familySize.setRect(0,-5,10,10);

    int offsetX;  // Need To adjust X value
    offsetX=familySize.left();

    if( offsetX<0)
	familySize.moveBy(-offsetX,0);
    else {
	familySize.setLeft(0);
	offsetX=0;
    }
    localSize=familySize;
    checkIndexesSize();  //This will change localSize adding Indexes Size
    familySize.moveBy(-localSize.left()-offsetX,0);
    localSize.moveBy(-localSize.left(),0);
    globalSize=localSize;
    nextDimension.moveBy(localSize.width(),0);
    globalSize=globalSize.unite(nextDimension);
    //warning("End");
}

int TextElement::takeAsciiFromKeyb(char ch)
{
    warning("...pos:%i",position);
    content.insert(position,ch);
    position++;
    warning("ins %i",position);
    return 1;
}

int TextElement::takeActionFromKeyb(int action)
{
    if(action==Qt::Key_Backspace)
	{
	    if(position > 0) {
		position--;
		content.remove(position,1);
	    }
	}
    else
     if(action==Qt::Key_Delete)
	    content.remove(position,1);   //Change this....
      else
       if(action==Qt::Key_Left)
            position--;	



    if(position > content.length())
	position = content.length();
    if(position < 0 )
      {
      	position = content.length();
        return -1;
      }
    return position;
}

void TextElement::changeFontFamily(QString family)
{

    if (family=="")
	{
	    if (font!=0L) delete font;	
	    font=0L;
	}
    else
	{
	    if (font==0L) font = new QFont;
	    font->setFamily(family);
	    font->setPointSize(numericFont);
	}
}

void TextElement::setNumericFont(int value)
{
    numericFont=value;
    if(font!=0L) font->setPointSize( numericFont );
}

void TextElement::split(int pos)
{
 if(pos==-1)
  pos=position;
 TextElement *FirstHalf = new TextElement(formula);
 insertElement(FirstHalf);
 FirstHalf->setContent(content.left(pos));
 FirstHalf->setNumericFont(numericFont);
 if (font !=0L)
  FirstHalf->changeFontFamily(font->family());

 setContent(content.right(content.length()-pos));
 warning(content);
 position=content.length();

}
