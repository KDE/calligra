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

#define POINT_TO_MM(px) (int((float)px/2.83465))
#define MM_TO_POINT(mm) (int((float)mm*2.83465))

/******************************************************************/
/* Class: KoRuler                                                 */
/******************************************************************/

/*================================================================*/
KoRuler::KoRuler(QWidget *_parent,QWidget *_canvas,Orientation _orientation,
		 KoPageLayout _layout,int _flags)
  : QFrame(_parent), buffer(width(),height())
{
  setFrameStyle(Box | Raised);

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
  whileMovingBorderLeft = whileMovingBorderRight = whileMovingBorderTop = whileMovingBorderBottom = false;

  QString pixdir = kapp->kde_datadir();
  pmFirst.load(pixdir + "/koffice/pics/koRulerFirst.xpm");
  pmLeft.load(pixdir + "/koffice/pics/koRulerLeft.xpm");
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
  buffer.fill(backgroundColor());

  QPainter p;
  p.begin(&buffer);

  int dist;
  int j = 0;
  int pw = static_cast<int>(layout.ptWidth);
  QString str;
  QFont font = QFont("helvetica",8);
  QFontMetrics fm(font);

  p.setPen(QPen(black,1,SolidLine));
  p.setBrush(white);

  QRect r; 
  if (!whileMovingBorderLeft)
    r.setLeft(-diffx + layout.ptLeft);
  else
    r.setLeft(oldMx);
  r.setTop(0);
  if (!whileMovingBorderRight)
    r.setRight(-diffx + pw - layout.ptRight);
  else
    r.setRight(oldMx);
  r.setBottom(height());

  p.drawRect(r);

  p.setPen(QPen(black,1,SolidLine));
  p.setFont(font);
  dist = static_cast<int>((1000 * _MM_TO_POINT) / 100);

  for (int i = 0;i <= layout.ptWidth;i += dist)
    {    
      str.sprintf("%d",j++);
      p.drawText(i - diffx - fm.width(str) / 2,(height() - fm.height()) / 2,fm.width(str),height(),AlignLeft | AlignTop,str);
    }

  for (int i = dist / 2;i <= layout.ptWidth;i += dist)
    p.drawLine(i - diffx,5,i - diffx,height() - 5);

  for (int i = dist / 4;i <= layout.ptWidth;i += dist / 2)
    p.drawLine(i - diffx,7,i - diffx,height() - 7);

  p.setPen(QPen(black));
  //p.drawLine(pw - diffx - 1,1,pw - diffx - 1,height() - 1);
  p.drawLine(pw - diffx + 1,1,pw - diffx + 1,height() - 1);
  p.setPen(QPen(white));
  p.drawLine(pw - diffx,1,pw - diffx,height() - 1);

  p.setPen(QPen(black));
  //p.drawLine(-diffx - 2,1,-diffx - 2,height() - 1);
  p.drawLine(-diffx,1,-diffx,height() - 1);
  p.setPen(QPen(white));
  p.drawLine(-diffx - 1,1,-diffx - 1,height() - 1);

  if (flags & F_INDENTS)
    {
      p.drawPixmap(i_first - pmFirst.size().width() / 2 + r.left(),2,pmFirst);
      p.drawPixmap(i_left - pmLeft.size().width() / 2 + r.left(),
		   height() - pmLeft.size().height() - 2,pmLeft);
    }

  if (action == A_NONE && showMPos)
    {
      p.setPen(QPen(black,1,SolidLine));
      p.drawLine(mposX,1,mposX,height() - 1);
    }
  hasToDelete = false;

  p.end();
  _painter->drawPixmap(0,0,buffer);
}

/*================================================================*/
void KoRuler::drawVertical(QPainter *_painter)
{
  buffer.fill(backgroundColor());

  QPainter p;
  p.begin(&buffer);

  int dist;
  int j = 0;
  int ph = static_cast<int>(layout.ptHeight);
  QString str;
  QFont font = QFont("helvetica",8);
  QFontMetrics fm(font);

  p.setPen(QPen(black,1,SolidLine));
  p.setBrush(white);

  QRect r;

  if (!whileMovingBorderTop)
    r.setTop(-diffy + layout.ptTop);
  else
    r.setTop(oldMy);
  r.setLeft(0);
  if (!whileMovingBorderBottom)
    r.setBottom(-diffy + ph - layout.ptBottom);
  else
    r.setBottom(oldMy);
  r.setRight(width());

  p.drawRect(r);

  p.setPen(QPen(black,1,SolidLine));
  p.setFont(font);
  dist = static_cast<int>((1000 * _MM_TO_POINT) / 100);

  for (int i = 0;i <= layout.ptHeight;i += dist)
    {    
      str.sprintf("%d",j++);
      p.drawText((width() - fm.width(str)) / 2,i - diffy - fm.height() / 2,width(),fm.height(),AlignLeft | AlignTop,str);
    }

  for (int i = dist / 2;i <= layout.ptHeight;i += dist)
    p.drawLine(5,i - diffy,width() - 5,i - diffy);

  for (int i = dist / 4;i <= layout.ptHeight;i += dist / 2)
    p.drawLine(7,i - diffy,width() - 7,i - diffy);

  p.setPen(QPen(black));
  //p.drawLine(1,ph - diffy - 1,width() - 1,ph - diffy - 1);
  p.drawLine(1,ph - diffy + 1,width() - 1,ph - diffy + 1);
  p.setPen(QPen(white));
  p.drawLine(1,ph - diffy,width() - 1,ph - diffy);

  p.setPen(QPen(black));
  //p.drawLine(1,-diffy - 2,width() - 1,-diffy - 2);
  p.drawLine(1,-diffy,width() - 1,-diffy);
  p.setPen(QPen(white));
  p.drawLine(1,-diffy - 1,width() - 1,-diffy - 1);

  if (action == A_NONE && showMPos)
    {
      p.setPen(QPen(black,1,SolidLine));
      p.drawLine(1,mposY,width() - 1,mposY);
    }
  hasToDelete = false;

  p.end();
  _painter->drawPixmap(0,0,buffer);
}

/*================================================================*/
void KoRuler::mousePressEvent(QMouseEvent *e)
{
  oldMx = e->x();
  oldMy = e->y();
  mousePressed = true;

  if (action == A_BR_RIGHT || action == A_BR_LEFT)
    {
      if (action == A_BR_RIGHT)
	whileMovingBorderRight = true;
      else
	whileMovingBorderLeft = true;

      if (canvas)
	{
	  QPainter p;
	  p.begin(canvas);
	  p.setRasterOp(NotROP);
	  p.setPen(QPen(black,1,SolidLine));
	  p.drawLine(oldMx,0,oldMx,canvas->height());
	  p.end();
	}

      repaint(false);
    }
  else if (action == A_BR_TOP || action == A_BR_BOTTOM)
    {
      if (action == A_BR_TOP)
	whileMovingBorderTop = true;
      else
	whileMovingBorderBottom = true;

      if (canvas)
	{
	  QPainter p;
	  p.begin(canvas);
	  p.setRasterOp(NotROP);
	  p.setPen(QPen(black,1,SolidLine));
	  p.drawLine(0,oldMy,canvas->width(),oldMy);
	  p.end();
	}

      repaint(false);
    }
  else if (action == A_FIRST_INDENT || action == A_LEFT_INDENT)
    {
      if (canvas)
	{
	  QPainter p;
	  p.begin(canvas);
	  p.setRasterOp(NotROP);
	  p.setPen(QPen(black,1,SolidLine));
	  p.drawLine(oldMx,0,oldMx,canvas->height());
	  p.end();
	}
    }
}

/*================================================================*/
void KoRuler::mouseReleaseEvent(QMouseEvent *e)
{
  mousePressed = false;

  if (action == A_BR_RIGHT || action == A_BR_LEFT)
    {
      whileMovingBorderRight = false;
      whileMovingBorderLeft = false;

      if (canvas)
	{
	  QPainter p;
	  p.begin(canvas);
	  p.setRasterOp(NotROP);
	  p.setPen(QPen(black,1,SolidLine));
	  p.drawLine(oldMx,0,oldMx,canvas->height());
	  p.end();
	}

      repaint(false);
      emit newPageLayout(layout);
    }
  else if (action == A_BR_TOP || action == A_BR_BOTTOM)
    {
      whileMovingBorderTop = false;
      whileMovingBorderBottom = false;

      if (canvas)
	{
	  QPainter p;
	  p.begin(canvas);
	  p.setRasterOp(NotROP);
	  p.setPen(QPen(black,1,SolidLine));
	  p.drawLine(0,oldMy,canvas->width(),oldMy);
	  p.end();
	}

      repaint(false);
      emit newPageLayout(layout);
    }
  else if (action == A_FIRST_INDENT)
    {
      if (canvas)
	{
	  QPainter p;
	  p.begin(canvas);
	  p.setRasterOp(NotROP);
	  p.setPen(QPen(black,1,SolidLine));
	  p.drawLine(oldMx,0,oldMx,canvas->height());
	  p.end();
	}
      
      repaint(false);
      emit newFirstIndent(POINT_TO_MM(i_first));
    }
  else if (action == A_LEFT_INDENT)
    {
      if (canvas)
	{
	  QPainter p;
	  p.begin(canvas);
	  p.setRasterOp(NotROP);
	  p.setPen(QPen(black,1,SolidLine));
	  p.drawLine(oldMx,0,oldMx,canvas->height());
	  p.end();
	}
      
      repaint(false);
      int _tmp = i_first;
      emit newLeftIndent(POINT_TO_MM(i_left));
      i_first = _tmp;
      emit newFirstIndent(POINT_TO_MM(i_first));
    }
}

/*================================================================*/
void KoRuler::mouseMoveEvent(QMouseEvent *e)
{
  hasToDelete = false;

  int pw = static_cast<int>(layout.ptWidth);
  int ph = static_cast<int>(layout.ptHeight);
  int left = static_cast<int>(layout.ptLeft);
  left -= diffx;
  int top = static_cast<int>(layout.ptTop);
  top -= diffy;
  int right = static_cast<int>(layout.ptRight);
  right = pw - right - diffx;
  int bottom = static_cast<int>(layout.ptBottom);
  bottom = ph - bottom - diffy;
  int ip_left = i_left;
  int ip_first = i_first;

  int mx = e->x();
  int my = e->y();

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

	    if (flags & F_INDENTS)
	      {
		if (mx > left + ip_first - 5 && mx < left + ip_first + 5 &&
		    my >= 2 && my <= pmFirst.size().height() + 2)
		  {
		    setCursor(ArrowCursor);
		    action = A_FIRST_INDENT;
		  }
		else if (mx > left + ip_left - 5 && mx < left + ip_left + 5 &&
			 my >=  height() - pmLeft.size().height() - 2 && my <= height() - 2)
		  {
		    setCursor(ArrowCursor);
		    action = A_LEFT_INDENT;
		  }
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
		      layout.left = (static_cast<float>(mx + diffx) * _POINT_TO_MM * 100) / 100;
		      layout.ptLeft = mx + diffx;
		      oldMx = e->x();
		      oldMy = e->y();
		      repaint(false);
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
		      layout.right = (static_cast<float>(pw - (mx + diffx)) * _POINT_TO_MM * 100) / 100;
		      layout.ptRight = pw - (mx + diffx);
		      oldMx = e->x();
		      oldMy = e->y();
		      repaint(false);
		    }
		} break;
	      case A_FIRST_INDENT:
		{
		  if (canvas)
		    {
		      QPainter p;
		      p.begin(canvas);
		      p.setRasterOp(NotROP);
		      p.setPen(QPen(black,1,SolidLine));
		      if (mx - left >= 0)
			{
			  p.drawLine(oldMx,0,oldMx,canvas->height());
			  p.drawLine(mx,0,mx,canvas->height());
			}
		      else 
			{
			  p.end();
			  return;
			}
		      p.end();
		      i_first = mx - left;
		      if (i_first < 0) i_first = 0;
		      oldMx = e->x();
		      oldMy = e->y();
		      repaint(false);
		    }
		} break;
	      case A_LEFT_INDENT:
		{
		  if (canvas)
		    {
		      QPainter p;
		      p.begin(canvas);
		      p.setRasterOp(NotROP);
		      p.setPen(QPen(black,1,SolidLine));
		      if (mx - left >= 0)
			{
			  p.drawLine(oldMx,0,oldMx,canvas->height());
			  p.drawLine(mx,0,mx,canvas->height());
			}
		      else 
			{
			  p.end();
			  return;
			}
		      p.end();
		      int oldLeft = i_left;
		      i_left = mx - left;
		      if (i_left < 0) i_left = 0;
		      else i_first += i_left - oldLeft;
		      oldMx = e->x();
		      oldMy = e->y();
		      repaint(false);
		    }
		} break;
	      default: break;
	      }
	  }
      } break;
    case VERTICAL:
      {
	if (!mousePressed)
	  {
	    setCursor(ArrowCursor);
	    action = A_NONE;
	    if (my > top - 5 && my < top + 5)
	      {
		setCursor(sizeVerCursor);
		action = A_BR_TOP;
	      }
	    else if (my > bottom - 5 && my < bottom + 5)
	      {
		setCursor(sizeVerCursor);
		action = A_BR_BOTTOM;
	      }
	  }
	else
	  {
	    switch (action)
	      {
	      case A_BR_TOP:
		{
		  if (canvas)
		    {
		      QPainter p;
		      p.begin(canvas);
		      p.setRasterOp(NotROP);
		      p.setPen(QPen(black,1,SolidLine));
		      p.drawLine(0,oldMy,canvas->width(),oldMy);
		      p.drawLine(0,my,canvas->width(),my);
		      p.end();
		      layout.top = (static_cast<float>(my + diffy) * _POINT_TO_MM * 100) / 100;
		      layout.ptTop = my + diffy;
		      oldMx = e->x();
		      oldMy = e->y();
		      repaint(false);
		    }
		} break;
	      case A_BR_BOTTOM:
		{
		  if (canvas)
		    {
		      QPainter p;
		      p.begin(canvas);
		      p.setRasterOp(NotROP);
		      p.setPen(QPen(black,1,SolidLine));
		      p.drawLine(0,oldMy,canvas->width(),oldMy);
		      p.drawLine(0,my,canvas->width(),my);
		      p.end();
		      layout.bottom = (static_cast<float>(ph - (my + diffy)) * _POINT_TO_MM * 100) / 100;
		      layout.ptBottom = ph - (my + diffy);
		      oldMx = e->x();
		      oldMy = e->y();
		      repaint(false);
		    }
		} break;
	      default: break;
	      }
	  }
      } break;
    }

  oldMx = e->x();
  oldMy = e->y();
}

/*================================================================*/
void KoRuler::resizeEvent(QResizeEvent *e)
{
  QFrame::resizeEvent(e);
  buffer.resize(size());
}
