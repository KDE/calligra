/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Gradients                                              */
/******************************************************************/

#include "kpgradient.h"
#include "kpgradient.moc"

/******************************************************************/
/* Class: KPGradient                                              */
/******************************************************************/

/*======================= constructor ============================*/
KPGradient::KPGradient(QColor _color1,QColor _color2,BCType _bcType,QSize _size)
  : QObject(), color1(_color1), color2(_color2), pixmap(_size), refCount(0)
{
  bcType = _bcType;
  paint();
}

/*====================== add reference ==========================*/
void KPGradient::addRef()
{
#ifdef SHOW_INFO
  debug("Refs of '(%d,%d,%d) (%d,%d,%d)': %d",color1.red(),color1.green(),color1.blue(),
	color2.red(),color2.green(),color2.blue(),++refCount);
#else
  ++refCount;
#endif
}

/*====================== remove reference =======================*/
bool KPGradient::removeRef()
{ 
#ifdef SHOW_INFO
  debug("Refs of '(%d,%d,%d) (%d,%d,%d)': %d",color1.red(),color1.green(),color1.blue(),
	color2.red(),color2.green(),color2.blue(),--refCount);
  return refCount == 0; 
#else
  return (--refCount == 0);
#endif
}

/*====================== paint ===================================*/
void KPGradient::paint()
{
  QPainter painter;

  switch (bcType)
    {
    case BCT_PLAIN:
      {
	painter.begin(&pixmap);

  	painter.setPen(NoPen);
  	painter.setBrush(color1);
  	painter.drawRect(QRect(0,0,pixmap.size().width(),pixmap.size().height()));

	painter.end();
      } break;
    case BCT_GHORZ:
      {
	painter.begin(&pixmap);

	if (color1 == color2)
	  {
	    painter.setPen(NoPen);
	    painter.setBrush(color2);
	    painter.drawRect(QRect(0,0,pixmap.size().width(),pixmap.size().height()));
	    break;
	  }

	QPixmap pmCrop;
	int ySize = pixmap.size().height();
	makeLineGradient(pmCrop,color2,color1,ySize);

	int s;
	int sSize = 20;
	int sOffset = 5;

	s = pixmap.size().width() / sSize + 1;

	for(int i = 0;i < s;i++)
	  painter.drawPixmap(sSize*i,0,pmCrop,sOffset,0,sSize,ySize);

	painter.end();
      } break;
    case BCT_GVERT:
      {
	painter.begin(&pixmap);

	if (color1 == color2)
	  {
	    painter.setPen(NoPen);
	    painter.setBrush(color2);
	    painter.drawRect(QRect(0,0,pixmap.size().width(),pixmap.size().height()));
	    break;
	  }

	QPixmap pmCrop;
	int ySize = pixmap.size().width();
	makeLineGradient(pmCrop,color1,color2,ySize);

	int s;
	int sSize = 20;
	int sOffset = 5;

	s = pixmap.size().height() / sSize + 1;

	QWMatrix matrix;
	matrix.translate(static_cast<float>(pixmap.size().width()),0.0);
	matrix.rotate(90.0);
	painter.setWorldMatrix(matrix);
	for(int i = 0;i < s;i++)
	  painter.drawPixmap(sSize*i,0,pmCrop,sOffset,0,sSize,ySize);

	painter.end();
      } break;
    case BCT_GDIAGONAL1: case BCT_GDIAGONAL2:
      {
	painter.begin(&pixmap);

	if (color1 == color2)
	  {
	    painter.setPen(NoPen);
	    painter.setBrush(color2);
	    painter.drawRect(QRect(0,0,pixmap.size().width(),pixmap.size().height()));
	    break;
	  }

	QPixmap pmCrop;
	int ySize = static_cast<int>(sqrt(pixmap.size().width() * pixmap.size().width() + 
					  pixmap.size().height() * pixmap.size().height()));
	painter.end();

	QPixmap pix(ySize,ySize);
	painter.begin(&pix);

	makeLineGradient(pmCrop,color2,color1,ySize);

	int s;
	int sSize = 20;
	int sOffset = 5;

	s = static_cast<int>(sqrt(pixmap.size().width() * pixmap.size().width() + 
				  pixmap.size().height() * pixmap.size().height())) / sSize + 1;
	
	QRect br = QRect(QRect(0,0,ySize,ySize));
	int pw = br.width();
	int ph = br.height();
	QRect rr = br;
	int yPos = -rr.y();
	int xPos = -rr.x();
	rr.moveTopLeft(QPoint(-rr.width() / 2,-rr.height() / 2));
      
	QWMatrix m,mtx,m2;

	if (bcType == BCT_GDIAGONAL1)
	  mtx.rotate(-45.0);
	else
	  mtx.rotate(45.0);

	m.translate(pw / 2,ph / 2);
	m2.translate(rr.left() + xPos,rr.top() + yPos);
	m = m2 * mtx * m;
      
	painter.setWorldMatrix(m);

	for(int i = 0;i < s;i++)
	  painter.drawPixmap(sSize * i,0,pmCrop,sOffset,0,sSize + 2,ySize);

	painter.end();

	bitBlt(&pixmap,0,0,&pix,(ySize - pixmap.size().width()) / 2,(ySize - pixmap.size().height()) / 2,pixmap.width(),pixmap.height());
      } break;
    case BCT_GCIRCLE: case BCT_GRECT:
      {
	painter.begin(&pixmap);

	if (color1 == color2)
	  {
	    painter.setPen(NoPen);
	    painter.setBrush(color2);
	    painter.drawRect(QRect(0,0,pixmap.size().width(),pixmap.size().height()));
	    break;
	  }
	
	int ySize = 0;
	ySize = max(pixmap.size().width(),pixmap.size().height());
	// I dont know why....
	ySize -= ySize / 4;

	QList<QColor> colorList;
	colorList.setAutoDelete(true);
	makeColorList(colorList,color2,color1,ySize);

	int s;
	s = ySize / (colorList.count());

	painter.setPen(NoPen);
	for(int i = 0;i < static_cast<int>(colorList.count());i++)
	  {
	    if (i * s - ySize / 4 - 20 < min(pixmap.size().width(),pixmap.size().height()) / 2)
	      {
		painter.setBrush(*colorList.at(i));
		if (bcType == BCT_GCIRCLE)
		  painter.drawEllipse(i * s - ySize / 4 - 20,i * s - ySize / 4 - 20,
				      pixmap.size().width() - 2 * i * s + ySize / 2 + 40,
				      pixmap.size().height() - 2 * i * s + ySize / 2 + 40);
		else
		  painter.drawRect(i * s - ySize / 4 - 20,i * s - ySize / 4 - 20,
				   pixmap.size().width() - 2 * i * s + ySize / 2 + 40,
				   pixmap.size().height() - 2 * i * s + ySize / 2 + 40);
	      }
	  }

	painter.end();
      } break;
    }
}

/*================================================================*/
void KPGradient::makeLineGradient(QPixmap &pmCrop,QColor _color1,QColor _color2,int _ySize)
{
  int ncols = 4;
  int depth = QColor::numBitPlanes();

  QColor cRow;
  int rca, gca, bca;
  int rDiff, gDiff, bDiff;
  float rat;
  unsigned int *p;
  unsigned int rgbRow;
		
  pmCrop.resize(30,_ySize);
  QImage image(30,_ySize,32);
    
  rca = _color1.red();
  gca = _color1.green();
  bca = _color1.blue();
  rDiff = _color2.red() - _color1.red();
  gDiff = _color2.green() - _color1.green();
  bDiff = _color2.blue() - _color1.blue();
  
  for (int y = _ySize - 1;y > 0;y--) 
    {
      p = (unsigned int*)image.scanLine(_ySize - y - 1);
      rat = 1.0 * y / _ySize;
      
      cRow.setRgb(rca + static_cast<int>(rDiff * rat),
		  gca + static_cast<int>(gDiff * rat), 
		  bca + static_cast<int>(bDiff * rat));
      
      rgbRow = cRow.rgb();
      
      for(int x = 0;x < 30;x++) 
	{
	  *p = rgbRow;
	  p++;
	}
    }
  
  if (depth <= 16)
    {
      if (depth == 16) ncols = 32;
      if (ncols < 2 || ncols > 256) ncols = 3;
      
      QColor *dPal = new QColor[ncols];
      for (int i = 0;i < ncols;i++) 
	{
	  dPal[i].setRgb(rca + rDiff * i / (ncols - 1),
			 gca + gDiff * i / (ncols - 1),
			 bca + bDiff * i / (ncols - 1));
	}
      
      kFSDither dither(dPal,ncols);
      QImage dImage = dither.dither(image);
      pmCrop.convertFromImage(dImage);
      
      delete [] dPal;	
      
    } 
  else 
    pmCrop.convertFromImage(image);
}

/*================================================================*/
void KPGradient::makeColorList(QList<QColor> &_colorList,QColor _color1,QColor _color2,int _ySize)
{

  QColor cRow;
  int rca, gca, bca;
  int rDiff, gDiff, bDiff;
  float rat;
		
  rca = _color1.red();
  gca = _color1.green();
  bca = _color1.blue();
  rDiff = _color2.red() - _color1.red();
  gDiff = _color2.green() - _color1.green();
  bDiff = _color2.blue() - _color1.blue();
  
  for (int y = _ySize - 1;y > 0;y--) 
    {
      rat = 1.0 * y / _ySize;
      
      cRow.setRgb(rca + static_cast<int>(rDiff * rat),
		  gca + static_cast<int>(gDiff * rat), 
		  bca + static_cast<int>(bDiff * rat));
      
      _colorList.append(new QColor(cRow));
    }
}
