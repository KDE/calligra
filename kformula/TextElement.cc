/*
 TextElement.cc
 Project KOffice/KFormula

 Author: Andrea Rizzi <rizzi@kde.org>
 License:GPL
*/

//#define RECT

#include <qrect.h>
#include <qkeycode.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kcharsets.h>
#include <klocale.h>

#include "BasicElement.h"
#include "TextElement.h"
#include "formuladef.h"
#include "formula_container.h"

TextElement::TextElement(KFormulaContainer *Formula,
			 BasicElement *Prev,
			 int Relation,
			 BasicElement *Next,
			 QString Text) :
    BasicElement(Formula,Prev,Relation,Next), text(Text)
{
  kdDebug(39001) <<"A new text is born..\n";
  QFont tmpfont = KGlobalSettings::generalFont();
  tmpfont.setPointSize( getNumericFont());
  KGlobal::charsets()->setQFont(tmpfont, KGlobal::locale()->charset());
  font = tmpfont;
  relation=Relation;
  //position=content.length();
  //kdDebug(39001) <<"creation %i"<<position<<endl;

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
  if ( text.isEmpty() )
    pen->drawRect(x+familySize().x(),y-5,5,10);
  else
    {
      //kdDebug(39001) <<"Font for text\n";
      pen->setFont(font);
      pen->drawText(x+familySize().x(),y+offsetY,text);
    }
  if(beActive)
    pen->setPen(Qt::blue);
  setMyArea(globalSize());
  myArea().moveBy(x,y);
#ifdef RECT
  /*QRect  localArea;
    localArea=localSize();
    localArea.moveBy(x,y);
    pen->drawRect(localArea);
    localArea=familySize();
    localArea.moveBy(x,y);
    pen->drawRect(localArea);
  */
  pen->setBrush(green);
  pen->setBrush(NoBrush);
  pen->drawRect(myArea());
  QRect area(localSize());
  area.moveBy(x,y);
  pen->drawRect(area);
  pen->setBrush(SolidPattern);
#endif
  drawIndexes(pen,resolution);
  
  if(beActive)
    pen->setPen(Qt::black);
  if(next!=0L) 
    next->draw(drawPoint+QPoint(localSize().width(),0),resolution);
}

void TextElement::checkSize()
{
  //kdDebug(39001) <<"T "<<this<<endl;
  
  QFontMetrics fm(font);
  QRect nextDimension;
  
  if (next!=0L) {
    next->checkSize();
    nextDimension=next->getSize();
  }

  offsetY=fm.strikeOutPos();
  setFamilySize(fm.boundingRect(text));
  familySize().moveBy(0,offsetY);
  if(text.isEmpty())
    familySize().setRect(0,-5,10,10);
  
  int offsetX;  // Need To adjust X value
  offsetX=familySize().left();
  
  if( offsetX<0)
    familySize().moveBy(-offsetX,0);
  else {
    familySize().setLeft(0);
    offsetX=0;
  }
  setLocalSize(familySize());
  checkIndexesSize();  //This will change localSize() adding Indexes Size
  familySize().moveBy(-localSize().left()-offsetX,0);
  localSize().moveBy(-localSize().left(),0);
  setGlobalSize(localSize());
  nextDimension.moveBy(localSize().width(),0);
  setGlobalSize(globalSize().unite(nextDimension));
  kdDebug(39001) <<"End\n";
}

void TextElement::changeFontFamily(QString family)
{
  font.setFamily(family);
  font.setPointSize(numericFont);
}

QString TextElement::textFontFamily() const
{
  return font.family();
}

void TextElement::changePropertieFont(signed char _bold,signed char _underline,signed char _italic)
{
  if(_bold>=0)
    font.setBold((bool)_bold);
  if(_italic>=0)
    font.setItalic((bool)_italic);
  if(_underline>=0)
    font.setUnderline((bool)_underline);
  font.setPointSize(numericFont);
}

void TextElement::setNumericFont(int value)
{
  numericFont=value;
  font.setPointSize( numericFont );
}

void TextElement::split(int pos)
{
  if(pos==-1)
    pos=position;
  kdDebug(39001) <<"position : "<<position<<endl;
  //position is not updating when you move cursor
  //fix it
  TextElement *FirstHalf = new TextElement(formula);
  insertElement(FirstHalf);
  FirstHalf->setContent(text.left(pos));
  FirstHalf->setNumericFont(numericFont);
  
  FirstHalf->changeFontFamily(font.family());
  FirstHalf->changePropertieFont(font.bold(),font.underline(),font.italic());
  
  setContent(text.right(text.length()-pos));
  kdDebug(39001) <<text<<endl;
  
  position=text.length();
  
}

QRect TextElement::getCursor(int atPos)
{
  QPoint dp = myArea().topLeft()-globalSize().topLeft();
  
  if(atPos>0)
    {
      atPos--;
      QFontMetrics fm(font);
      return (QRect(dp.x()+familySize().x()+fm.width(text,atPos),
		    dp.y()+familySize().top()-1,
		    5,familySize().height()+2));
      
    }
  else
    {
      if(atPos==0)
	return (QRect(dp.x()+familySize().x()-3,dp.y()-7,5,14));
      else
	return (QRect(dp.x()+localSize().width()+2,dp.y()-8,5,16));
    }
  
  
  return QRect(0,0,0,0);
}

void TextElement::makeList(bool active)
{
  formula->addElement(this, 0);
  
  kdDebug(39001) <<"append\n";
  beActive=0;
  
  for(int i=0;i<2;i++)
    if(index[i]!=0)
      {
	kdDebug(39001) <<"call for index "<<i<<"  "<<index[i]<<endl;
	index[i]->makeList(active);
      }
  kdDebug(39001) <<"index OK\n";
  
  for(unsigned int i=1;i<=text.length()+1;i++)
    formula->addElement(this, i);
  
  for(int i=2;i<4;i++)
    if(index[i]!=0)
      {
	kdDebug(39001) <<"call for index "<<i<<" "<<index[i]<<endl;
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

