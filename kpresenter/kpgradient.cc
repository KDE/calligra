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
  int ncols = 4;
  int depth = QColor::numBitPlanes();
  QColor _color1,_color2;

  QPainter painter;
  painter.begin(&pixmap);

  switch (bcType)
    {
    case BCT_PLAIN:
      {
  	painter.setPen(NoPen);
  	painter.setBrush(color1);
  	painter.drawRect(QRect(0,0,pixmap.size().width(),pixmap.size().height()));
      } break;
    case BCT_GHORZ: case BCT_GVERT:
      {
	if (color1 == color2)
	  {
	    painter.setPen(NoPen);
	    painter.setBrush(color2);
	    painter.drawRect(QRect(0,0,pixmap.size().width(),pixmap.size().height()));
	    break;
	  }

	if (bcType == BCT_GHORZ)
	  {
	    _color1 = color2;
	    _color2 = color1;
	  }
	else
	  {
	    _color1 = color1;
	    _color2 = color2;
	  }

	QPixmap pmCrop;
	QColor cRow;
	int ySize;
	int rca, gca, bca;
	int rDiff, gDiff, bDiff;
	float rat;
	uint *p;
	uint rgbRow;
		
	if (bcType == BCT_GHORZ)
	  ySize = pixmap.size().height();
	else
	  ySize = pixmap.size().width();
    
	pmCrop.resize(30,ySize);
	QImage image(30,ySize,32);
    
	rca = _color1.red();
	gca = _color1.green();
	bca = _color1.blue();
	rDiff = _color2.red() - _color1.red();
	gDiff = _color2.green() - _color1.green();
	bDiff = _color2.blue() - _color1.blue();
    
	for (int y = ySize - 1;y > 0;y--) 
	  {
	    p = (unsigned int*)image.scanLine(ySize - y - 1);
	    rat = 1.0 * y / ySize;
	    
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
	    if(depth == 16) ncols = 32;
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
	
	int s;
	int sSize = 20;
	int sOffset = 5;
	
	if (bcType == BCT_GHORZ)
	  s = pixmap.size().width() / sSize + 1;
	else
	  s = pixmap.size().height() / sSize + 1;
	
	if (bcType == BCT_GHORZ)	
	  for(int i = 0;i < s;i++)
	    painter.drawPixmap(sSize*i,0,pmCrop,sOffset,0,sSize,ySize);
	else 
	  {
 	    QWMatrix matrix;
 	    matrix.translate(static_cast<float>(pixmap.size().width()),0.0);
 	    matrix.rotate(90.0);
 	    painter.setWorldMatrix(matrix);
	    for(int i = 0;i < s;i++)
	      painter.drawPixmap(sSize*i,0,pmCrop,sOffset,0,sSize,ySize);
 	    matrix.rotate(-90.0);
 	    matrix.translate(-static_cast<float>(pixmap.size().width()),0.0);
 	    painter.setWorldMatrix(matrix);
	  }
      } break;
    }
  painter.end();
}






