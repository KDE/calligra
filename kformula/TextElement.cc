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
#include "formula_container.h"

TextElement::TextElement(KFormulaContainer *Formula,
			 BasicElement *Prev,
			 int Relation,
			 BasicElement *Next,
			 QString Content) :
    BasicElement(Formula,Prev,Relation,Next,Content)
{
    // warning("A new text is born.. ");
    font=0L;
    //position=content.length();
    //warning("creation %i",position);

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

QRect TextElement::getCursor(int atPos) 
{
    QPoint dp = myArea.topLeft()-globalSize.topLeft();
    
    if(atPos>0)
	{
	    atPos--;
	    QFont formulaFont;
	    if(font!=0L)
		formulaFont=(*font);
	    else
		{
		    formulaFont=formula->generalFont();
		    formulaFont.setPointSize(numericFont);
		}

	    QFontMetrics fm(formulaFont);
	    return (QRect(dp.x()+familySize.x()+fm.width(content,atPos),
			  dp.y()+familySize.top()-1,
			  5,familySize.height()+2));

	} 
    else 
	{
	    if(atPos==0)
		return (QRect(dp.x()+familySize.x()-3,dp.y()-7,5,14));
	    else
		return (QRect(dp.x()+localSize.width()+2,dp.y()-8,5,16));	
	}
 

    return QRect(0,0,0,0);
}

void TextElement::makeList(bool active) 
{
    formula->addElement(this, 0);
    warning("append");
    beActive=0;
    
    for(int i=0;i<2;i++)
	if(index[i]!=0) {
	    warning("call for index%d %p",i,index[i]);
	    index[i]->makeList(active);
	}
    warning("index OK");
    
    for(unsigned int i=1;i<=content.length()+1;i++)
	formula->addElement(this, i);

    for(int i=2;i<4;i++)
	if(index[i]!=0) {
	    warning("call for index%d %p",i,index[i]);
	    index[i]->makeList(active);
	}

    /*p = new PosType;
      p->element=this;
      p->pos=-1;
      formula->eList.append(p);
    */
    if(next!=0)
	next->makeList(active);
    
}

