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
//warning("Utopiaaa");
  /*
   * I'll remove this code to use formula->getFont()
   */
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
    pen->drawRect(x+familySize.x(),y-5,10,10);
  else {
/*
* I MUST to implement a generalFont
*/
    pen->setFont(font);
    pen->drawText(x+familySize.x(),y+offsetY,content); 
    // if( active )
    //          m_pDoc->setCursor(QRect(x+fm.width(content,m_pDoc->getPos()),y-oyu,2,oyu+oyd));
  }
if(beActive)
  pen->setPen(blue);
  myArea=globalSize;
  myArea.moveBy(x,y);
/*  globalArea=globalSize;
  globalArea.moveBy(x,y);*/
#ifdef RECT
 // pen->drawRect(myArea); 
  QRect  localArea;
  localArea=localSize;
  localArea.moveBy(x,y);  
  pen->drawRect(localArea); 
  localArea=familySize;
  localArea.moveBy(x,y);  
  pen->drawRect(localArea); 

#endif
  drawIndexes(pen,resolution);

if(beActive)
  pen->setPen(black);
  if(next!=0L) next->draw(drawPoint+QPoint(localSize.width(),0),resolution);
}

void TextElement::checkSize()
{
//warning("T %p",this);
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
