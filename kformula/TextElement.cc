/*
 TextElement.cc 
 Project KOffice/KFormula
 
 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

//#define RECT
/*#include <qstring.h>
#include <qpainter.h>
#include <qcolor.h>
*/
#include <qrect.h> 
/*#include <qfont.h>
included in ...*/
#include "BasicElement.h" 
#include "TextElement.h" 
#include "formuladef.h"

TextElement::TextElement(FormulaClass *Formula,BasicElement *Prev=0L,int Relation=-1,BasicElement *Next=0L,
			    QString Content="") : BasicElement(Formula,Prev,Relation,Next,Content)
{
warning("Utopiaaa");
     font.setFamily( "utopia" );
     font.setPointSize( numericFont );
     font.setWeight( QFont::Normal );
     font.setItalic( FALSE ); 
    position=content.length();
}

TextElement::~TextElement()
{
}
 
void TextElement::draw(QPoint drawPoint,int resolution=72)
{
  
  QPainter *pen;
  pen=formula->painter();
  int x,y;
  x=drawPoint.x();
  y=drawPoint.y();

  if( beActive )
    pen->setPen(red);
  if ( content == "" )
    pen->drawRect(x,y-5,10,10);
  else {
    pen->setFont(font);
    pen->drawText(x+familySize.x(),y+offsetY,content); 
    // if( active )
    //          m_pDoc->setCursor(QRect(x+fm.width(content,m_pDoc->getPos()),y-oyu,2,oyu+oyd));
  }
  pen->setPen(blue);
  myArea=globalSize;
  myArea.moveBy(x,y);
/*  globalArea=globalSize;
  globalArea.moveBy(x,y);*/
#ifdef RECT
 // pen->drawRect(myArea); 
  QRect  localArea;
  localArea=aSize;
  localArea.moveBy(x,y);  
  pen->drawRect(localArea); 
  localArea=familySize;
  localArea.moveBy(x,y);  
  pen->drawRect(localArea); 

#endif
  drawIndexes(pen,resolution);
  pen->setPen(black);
  if(next!=0L) next->draw(drawPoint+QPoint(aSize.width(),0),resolution);
}

void TextElement::checkSize()
{
  QFontMetrics fm(font);
  QRect nextDimension; 

  if (next!=0L)
    {
      next->checkSize();
      nextDimension=next->getSize();
    }
  
  offsetY=fm.strikeOutPos();
  familySize=fm.boundingRect(content);
  familySize.moveBy(0,offsetY);
  if(content == "") 
   {
     familySize.setRect(0,-5,10,10);
   }
  
  int offsetX;  // Need To adjust X value
  offsetX=familySize.left();
  
   if( offsetX<0) 
    familySize.moveBy(-offsetX,0);
      else {
       familySize.setLeft(0); 
       offsetX=0;
      }
  aSize=familySize;
  checkIndexesSize();  //This will change aSize adding Indexes Size
  familySize.moveBy(-aSize.left()-offsetX,0);
  aSize.moveBy(-aSize.left(),0);
  globalSize=aSize;
  nextDimension.moveBy(aSize.width(),0);
  globalSize=globalSize.unite(nextDimension);

}

int TextElement::takeAsciiFromKeyb(char ch) 
{
content.insert(content.length(),ch);
return 1;
}     

int TextElement::takeActionFromKeyb(int action)
{
return position;
}

void TextElement::setNumericFont(int value)
{
 numericFont=value;
 font.setPointSize( numericFont );
}
