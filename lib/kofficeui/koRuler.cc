/******************************************************************/
/* KOffice Library - (c) by Reginald Stadlbauer 1998              */
/* Version: 1.0                                                   */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Ruler (header)                                         */
/******************************************************************/

#include "koRuler.h"
#include "koRuler.moc"

/******************************************************************/
/* Class: KoRuler                                                 */
/******************************************************************/

/*================================================================*/
KoRuler::KoRuler(QWidget *_parent,QWidget *_canvas,Orientation _orientation,
		 KoPageLayout _layout,int _flags)
  : QFrame(_parent)
{
  setFrameStyle(Box | Sunken);

  canvas = _canvas;
  orientation = _orientation;
  layout = _layout;
  flags = _flags;

  diffx = 0;
  diffy = 0;

  setMouseTracking(true);
  mousePressed = false;
  action = A_NONE;

  oldMx = 0;
  oldMy = 0;

  showMPos = false;
  mposX = 0;
  mposY = 0;
  hasToDelete = false;
}

/*================================================================*/
KoRuler::~KoRuler()
{
}

/*================================================================*/
void KoRuler::setMousePos(int mx,int my)
{
  if (!showMPos || (mx == mposX && my == mposY)) return;

  QPainter p;
  p.begin(this);
  p.setRasterOp(NotROP);
  p.setPen(QPen(black,1,SolidLine));

  if (orientation == HORIZONTAL)
    {
      if (hasToDelete)
	p.drawLine(mposX,1,mposX,height() - 1);
      p.drawLine(mx,1,mx,height() - 1);
      hasToDelete = true;
    }

  if (orientation == VERTICAL)
    {
      if (hasToDelete)
	p.drawLine(1,mposY,width() - 1,mposY);
      p.drawLine(1,my,width() - 1,my);
      hasToDelete = true;
    }

  p.end();

  mposX = mx; 
  mposY = my; 
}

/*================================================================*/
void KoRuler::drawHorizontal(QPainter *_painter)
{
  int dist;
  int j = 0;
  int wid = static_cast<int>(layout.width * 100) / 100;
  int pw = wid * static_cast<int>(_MM_TO_POINT * 100) / 100;
  QString str;
  QFont font = QFont("helvetica",8);
  QFontMetrics fm(font);

  _painter->setPen(QPen(black,1,SolidLine));
  _painter->setBrush(white);

  QRect r = QRect(-diffx + (layout.left * 100 * _MM_TO_POINT) / 100,0,pw - (layout.left * 100 * _MM_TO_POINT) / 100 -
		  (layout.right * 100 * _MM_TO_POINT) / 100,height());

  _painter->drawRect(r);

  _painter->setPen(QPen(black,1,SolidLine));
  _painter->setFont(font);
  dist = static_cast<int>((1000 * _MM_TO_POINT) / 100);

  for (int i = 0;i <= (layout.width * 100 * _MM_TO_POINT) / 100;i+= dist)
    {    
      str.sprintf("%d",j++);
      _painter->drawText(i - diffx - fm.width(str) / 2,(height() - fm.height()) / 2,fm.width(str),height(),AlignLeft | AlignTop,str);
    }

  for (int i = dist / 2;i <= (layout.width * 100 * _MM_TO_POINT) / 100;i+= dist)
    _painter->drawLine(i - diffx,5,i - diffx,height() - 5);

  for (int i = dist / 4;i <= (layout.width * 100 * _MM_TO_POINT) / 100;i+= dist / 2)
    _painter->drawLine(i - diffx,7,i - diffx,height() - 7);

  _painter->setPen(QPen(black));
  _painter->drawLine(pw - diffx - 1,1,pw - diffx - 1,height() - 1);
  _painter->drawLine(pw - diffx + 1,1,pw - diffx + 1,height() - 1);
  _painter->setPen(QPen(white));
  _painter->drawLine(pw - diffx,1,pw - diffx,height() - 1);

  if (action == A_NONE && showMPos)
    {
      _painter->setPen(QPen(black,1,SolidLine));
      _painter->drawLine(mposX,1,mposX,height() - 1);
    }
  hasToDelete = false;
}

/*================================================================*/
void KoRuler::drawVertical(QPainter *_painter)
{
  if (action == A_NONE && showMPos)
    {
      _painter->setPen(QPen(black,1,SolidLine));
      _painter->drawLine(1,mposY,width() - 1,mposY);
    }
  hasToDelete = false;
}

/*================================================================*/
void KoRuler::mousePressEvent(QMouseEvent *e)
{
  oldMx = e->x() - diffx;
  oldMy = e->y() - diffy;
  mousePressed = true;

  if (action == A_BR_RIGHT || action == A_BR_LEFT)
    {
      QPainter p;
      p.begin(canvas);
      p.setRasterOp(NotROP);
      p.setPen(QPen(black,1,SolidLine));
      p.drawLine(oldMx,0,oldMx,canvas->height());
      p.end();
    }
}

/*================================================================*/
void KoRuler::mouseReleaseEvent(QMouseEvent *e)
{
  mousePressed = false;

  if (action == A_BR_RIGHT || action == A_BR_LEFT)
    {
      QPainter p;
      p.begin(canvas);
      p.setRasterOp(NotROP);
      p.setPen(QPen(black,1,SolidLine));
      p.drawLine(oldMx,0,oldMx,canvas->height());
      p.end();

      emit newPageLayout(layout);
    }
}

/*================================================================*/
void KoRuler::mouseMoveEvent(QMouseEvent *e)
{
  hasToDelete = false;

  int wid = static_cast<int>(layout.width * 100) / 100;
  int pw = wid * static_cast<int>(_MM_TO_POINT * 100) / 100;
  int left = static_cast<int>(layout.left * _MM_TO_POINT * 100) / 100;
  left -= diffx;
  int right = static_cast<int>(layout.right * _MM_TO_POINT * 100) / 100;
  right -= diffx;
  right = pw - right;

  int mx = e->x() - diffx;
  int my = e->y() - diffy;

  switch (orientation)
    {
    case HORIZONTAL:
      {
	if (!mousePressed)
	  {
	    setCursor(ArrowCursor);
	    action = A_NONE;
	    if (mx > left - 5 && mx < left + 5)
	      {
		setCursor(sizeHorCursor);
		action = A_BR_LEFT;
	      }
	    else if (mx > right - 5 && mx < right + 5)
	      {
		setCursor(sizeHorCursor);
		action = A_BR_RIGHT;
	      }
	  }
	else
	  {
	    switch (action)
	      {
	      case A_BR_LEFT:
		{
		  if (canvas)
		    {
		      QPainter p;
		      p.begin(canvas);
		      p.setRasterOp(NotROP);
		      p.setPen(QPen(black,1,SolidLine));
		      p.drawLine(oldMx,0,oldMx,canvas->height());
		      p.drawLine(mx,0,mx,canvas->height());
		      p.end();
		      layout.left = (static_cast<float>(mx + 1) * _POINT_TO_MM * 100) / 100;
		      repaint(true);
		    }
		} break;
	      case A_BR_RIGHT:
		{
		  if (canvas)
		    {
		      QPainter p;
		      p.begin(canvas);
		      p.setRasterOp(NotROP);
		      p.setPen(QPen(black,1,SolidLine));
		      p.drawLine(oldMx,0,oldMx,canvas->height());
		      p.drawLine(mx,0,mx,canvas->height());
		      p.end();
		      layout.right = (static_cast<float>(pw - mx - 1) * _POINT_TO_MM * 100) / 100;
		      repaint(true);
		    }
		} break;
	      default: break;
	      }
	  }

      } break;
    case VERTICAL:
      {
      } break;
    }

  oldMx = e->x() - diffx;
  oldMy = e->y() - diffy;
}
