/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Autoform Editor                                                */
/* Version: 0.1.0alpha                                            */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: draw widget                                            */
/******************************************************************/

#include "drawWidget.h"
#include "drawWidget.moc"

/*================================================================*/
/* Class: DrawWidget                                              */
/*================================================================*/

/*====================== constructor =============================*/
DrawWidget::DrawWidget(QWidget *parent=0)
  : QWidget(parent)
{
  setBackgroundColor(white);
  setMouseTracking(true);
  drawRaster = true;
  dRastW = 0;
  dRastH = 0;
  xPoints = 100;
  yPoints = 100;
  relation = true;
  show();
  setMinimumSize(100,100);
  wid = 1;
}

/*======================= destrcutor =============================*/
DrawWidget::~DrawWidget()
{
}

/*====================== set relation ============================*/
void DrawWidget::setRelation(bool r)
{
  relation = r;
  resize(width(),height());

  aWidth = width() - 10;
  aHeight = height() - 10;
  if (relation)
    {
      if (aWidth > aHeight) aWidth = aHeight;
      if (aWidth < aHeight) aHeight = aWidth;
    }
  emit getPntArry(aWidth,aHeight);
  repaint(true);
}

/*======================= create pixmap ==========================*/
void DrawWidget::createPixmap(const char *fileName)
{
  QPixmap *pm = new QPixmap(32,32);
  QPainter *painter = new QPainter();
  QPointArray pntArray2(pntArry.size());
  unsigned int i,px,py,pw,pwOrig = 1;
  int aw = aWidth,ah = aHeight;

  aWidth = 32;
  aHeight = 32;
  emit getPntArry(aWidth,aHeight);  
  pm->fill(white);         
  painter->begin(pm);
  painter->setPen(QPen(black,1,SolidLine));
  for (i=0;i < pntArry.size();i++)
    {
      px = pntArry.at(i).x();
      py = pntArry.at(i).y();
      if (atrLs.at(i)->pwDiv > 0)
	{
	  pw = pwOrig / atrLs.at(i)->pwDiv;
	  if (px < (unsigned int)aWidth / 2) px += pw;
	  if (py < (unsigned int)aHeight / 2) py += pw;
	  if (px > (unsigned int)aWidth / 2) px -= pw;
	  if (py > (unsigned int)aHeight / 2) py -= pw;
	}
      pntArray2.setPoint(i,px,py);
    }
  painter->drawPolygon(pntArray2);
  painter->end();
  aWidth = aw;
  aHeight = ah;
  emit getPntArry(aWidth,aHeight);  
  pm->save(fileName,"XPM");
}

/*======================= resize event ===========================*/
void DrawWidget::resizeEvent(QResizeEvent *e)
{
  aWidth = e->size().width() - 10;
  aHeight = e->size().height() - 10;

  if (relation)
    {
      if (aWidth > aHeight) aWidth = aHeight;
      if (aWidth < aHeight) aHeight = aWidth;
    }
  emit getPntArry(aWidth,aHeight);
}

/*======================= pait event =============================*/
void DrawWidget::paintEvent(QPaintEvent*)
{
  QPainter *painter = new QPainter();
  QPointArray pntArray2(pntArry.size());
  unsigned int i,px,py,pw,pwOrig;

  painter->begin(this);
  painter->setPen(QPen(darkGray,1,SolidLine));
  painter->setBrush(white);
  painter->drawRect(5,5,aWidth,aHeight);
  painter->setPen(QPen(black,wid,SolidLine));
  painter->translate(5.0,5.0);
  pwOrig = painter->pen().width() + 3;
  for (i=0;i < pntArry.size();i++)
    {
      px = pntArry.at(i).x();
      py = pntArry.at(i).y();
      if (atrLs.at(i)->pwDiv > 0)
	{
	  pw = pwOrig / atrLs.at(i)->pwDiv;
	  if (px < (unsigned int)aWidth / 2) px += pw;
	  if (py < (unsigned int)aHeight / 2) py += pw;
	  if (px > (unsigned int)aWidth / 2) px -= pw;
	  if (py > (unsigned int)aHeight / 2) py -= pw;
	}
      pntArray2.setPoint(i,px,py);
    }
  painter->drawPolygon(pntArray2);
  painter->resetXForm();
  painter->end();
} 
