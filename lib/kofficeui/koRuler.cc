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

/******************************************************************/
/* Class: KoRuler                                                 */
/******************************************************************/

/*================================================================*/
KoRuler::KoRuler(QWidget *_parent,QWidget *_canvas,Orientation _orientation,
		 KoPageLayout _layout,int _flags,KoTabChooser *_tabChooser = 0L)
  : QFrame(_parent), buffer(width(),height()), unit("mm")
{
  setFrameStyle(Box | Raised);
  tabChooser = _tabChooser;

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
  currTab = -1;

  tabList.setAutoDelete(false);
  frameStart = -1;

  allowUnits = true;
  setupMenu();
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
  
  if (unit == "inch")
    dist = static_cast<int>((1000 * _INCH_TO_POINT) / 1000);
  else if (unit == "pt")
    dist = 100;
  else
    dist = static_cast<int>((1000 * _MM_TO_POINT) / 100);

  for (unsigned int i = 0;i <= layout.ptWidth;i += dist)
    {    
      if (unit == "pt")
	str.sprintf("%d00",j++);
      else
	str.sprintf("%d",j++);
      p.drawText(i - diffx - fm.width(str) / 2,(height() - fm.height()) / 2,fm.width(str),height(),AlignLeft | AlignTop,str);
    }

  for (unsigned int i = dist / 2;i <= layout.ptWidth;i += dist)
    p.drawLine(i - diffx,5,i - diffx,height() - 5);

  for (unsigned int i = dist / 4;i <= layout.ptWidth;i += dist / 2)
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

  if (tabChooser && (flags & F_TABS) && !tabList.isEmpty())
    drawTabs(p);

  p.end();
  _painter->drawPixmap(0,0,buffer);
}

/*================================================================*/
void KoRuler::drawTabs(QPainter &_painter)
{
  KoTabulator *_tab = 0L;
  int ptPos = 0;

  _painter.setPen(QPen(black,2,SolidLine));

  for (unsigned int i = 0;i < tabList.count();i++)
    {
      _tab = tabList.at(i);
      ptPos = _tab->ptPos - diffx + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart);
      switch (_tab->type)
	{
	case T_LEFT:
	  {
	    ptPos -= 4;
	    _painter.drawLine(ptPos + 4,height() - 4,ptPos + 20 - 4,height() - 4);
	    _painter.drawLine(ptPos + 5,4,ptPos + 5,height() - 4);
	  } break;
	case T_CENTER:
	  {
	    ptPos -= 10;
	    _painter.drawLine(ptPos + 4,height() - 4,ptPos + 20 - 4,height() - 4);
	    _painter.drawLine(ptPos + 20 / 2,4,ptPos + 20 / 2,height() - 4);
	  } break;
	case T_RIGHT:
	  {
	    ptPos -= 16;
	    _painter.drawLine(ptPos + 4,height() - 4,ptPos + 20 - 4,height() - 4);
	    _painter.drawLine(ptPos + 20 - 5,4,ptPos + 20 - 5,height() - 4);
	  } break;
	case T_DEC_PNT:
	  {
	    ptPos -= 10;
	    _painter.drawLine(ptPos + 4,height() - 4,ptPos + 20 - 4,height() - 4);
	    _painter.drawLine(ptPos + 20 / 2,4,ptPos + 20 / 2,height() - 4);
	    _painter.fillRect(ptPos + 20 / 2 + 2,height() - 9,3,3,black);
	  } break;
	default: break;
	}
    }
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

  if (unit == "inch")
    dist = static_cast<int>((1000 * _INCH_TO_POINT) / 1000);
  else if (unit == "pt")
    dist = 100;
  else
    dist = static_cast<int>((1000 * _MM_TO_POINT) / 100);

  for (unsigned int i = 0;i <= layout.ptHeight;i += dist)
    {    
      if (unit == "pt")
	str.sprintf("%d00",j++);
      else
	str.sprintf("%d",j++);
      p.drawText((width() - fm.width(str)) / 2,i - diffy - fm.height() / 2,width(),fm.height(),AlignLeft | AlignTop,str);
    }

  for (unsigned int i = dist / 2;i <= layout.ptHeight;i += dist)
    p.drawLine(5,i - diffy,width() - 5,i - diffy);

  for (unsigned int i = dist / 4;i <= layout.ptHeight;i += dist / 2)
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

  if (e->button() == RightButton)
    {
      QPoint pnt(QCursor::pos());
      rb_menu->popup(pnt);
      action = A_NONE;
      mousePressed = false;
      return;
    }

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
  else if (action == A_TAB)
    {
      if (canvas)
	{
	  QPainter p;
	  p.begin(canvas);
	  p.setRasterOp(NotROP);
	  p.setPen(QPen(black,1,SolidLine));
	  p.drawLine(tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart),0,
		     tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart),canvas->height());
	  p.end();
	}
    }
  else if (tabChooser && (flags & F_TABS) && tabChooser->getCurrTabType() != 0)    
    {
      KoTabulator *_tab = new KoTabulator;
      switch (tabChooser->getCurrTabType())
	{
	case KoTabChooser::TAB_LEFT:
	  _tab->type = T_LEFT;
	  break;
	case KoTabChooser::TAB_CENTER:
	  _tab->type = T_CENTER;
	  break;
	case KoTabChooser::TAB_RIGHT:
	  _tab->type = T_RIGHT;
	  break;
	case KoTabChooser::TAB_DEC_PNT:
	  _tab->type = T_DEC_PNT;
	  break;
	default: break;
	}
      _tab->ptPos = e->x() + diffx - (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart);
      _tab->mmPos = cPOINT_TO_MM(_tab->ptPos);
      _tab->inchPos = cPOINT_TO_INCH(_tab->ptPos);

      tabList.append(_tab);
      emit tabListChanged(&tabList);
      repaint(false);
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
      emit newFirstIndent(i_first);
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
      emit newLeftIndent(i_left);
      i_first = _tmp;
      emit newFirstIndent(i_first);
    }
  else if (action == A_TAB)
    {
      if (canvas)
	{
	  QPainter p;
	  p.begin(canvas);
	  p.setRasterOp(NotROP);
	  p.setPen(QPen(black,1,SolidLine));
	  p.drawLine(tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart),0,
		     tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart),canvas->height());
	  p.end();
	}
      if (/*tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart) < layout.ptLeft || 
	  tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart) > layout.ptWidth - 
	  (layout.ptRight + layout.ptLeft) || */e->y() < -50 || e->y() > height() + 50)
	tabList.remove(currTab);

      emit tabListChanged(&tabList);
      repaint(false);
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
	    if (flags & F_TABS)
	      {
		int pos;
		currTab = -1;
		for (unsigned int i = 0;i < tabList.count();i++)
		  {
		    pos = tabList.at(i)->ptPos - diffx + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart);
		    if (mx > pos - 5 && mx < pos + 5)
		      {
			setCursor(sizeHorCursor);
			action = A_TAB;
			currTab = i;
			break;
		      }
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
		      layout.left = layout.mmLeft = cPOINT_TO_MM(mx + diffx);
		      layout.inchLeft = cPOINT_TO_INCH(mx + diffx);
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
		      layout.right = layout.mmRight = cPOINT_TO_MM(static_cast<int>(pw - (mx + diffx)));
		      layout.ptRight = pw - (mx + diffx);
		      layout.inchRight = cPOINT_TO_INCH(static_cast<int>(pw - (mx + diffx)));
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
	      case A_TAB:
		{
		  if (canvas)
		    {
		      QPainter p;
		      p.begin(canvas);
		      p.setRasterOp(NotROP);
		      p.setPen(QPen(black,1,SolidLine));
		      p.drawLine(tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart),0,
				 tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart),
				 canvas->height());
		      tabList.at(currTab)->ptPos += (e->x() - oldMx);
		      tabList.at(currTab)->mmPos = cPOINT_TO_MM(tabList.at(currTab)->ptPos);
		      tabList.at(currTab)->inchPos = cPOINT_TO_INCH(tabList.at(currTab)->ptPos);
		      p.drawLine(tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart),0,
				 tabList.at(currTab)->ptPos + (frameStart == -1 ? static_cast<int>(layout.ptLeft) : frameStart),
				 canvas->height());
		      p.end();
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
		      layout.top = layout.mmTop = cPOINT_TO_MM(my + diffy);
		      layout.ptTop = my + diffy;
		      layout.inchTop = cPOINT_TO_INCH(my + diffy);
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
		      layout.bottom = layout.mmBottom = cPOINT_TO_MM(static_cast<int>(ph - (my + diffy)));
		      layout.ptBottom = ph - (my + diffy);
		      layout.inchBottom = cPOINT_TO_INCH(static_cast<int>(ph - (my + diffy)));
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

/*================================================================*/
void KoRuler::mouseDoubleClickEvent(QMouseEvent*)
{
  if ((tabChooser && (flags & F_TABS) && tabChooser->getCurrTabType() != 0))
    {
      tabList.remove(tabList.count() - 1);
      emit tabListChanged(&tabList);
      repaint(false);
    }

  emit openPageLayoutDia(); 
}

/*================================================================*/
void KoRuler::setTabList(QList<KoTabulator>* _tabList)
{ 
  tabList.setAutoDelete(true);
  tabList.clear(); 
  tabList.setAutoDelete(false);
  for (unsigned int i = 0;i < _tabList->count();i++)
    {
      KoTabulator *t = new KoTabulator;
      t->type = _tabList->at(i)->type;
      t->mmPos = _tabList->at(i)->mmPos;
      t->inchPos = _tabList->at(i)->inchPos;
      t->ptPos = _tabList->at(i)->ptPos;
      tabList.append(t);
    }
  repaint(false); 
}

/*================================================================*/
unsigned int KoRuler::makeIntern(float _v)
{
  if (unit == "mm") return cMM_TO_POINT(_v);
  if (unit == "inch") return cINCH_TO_POINT(_v);
  return static_cast<unsigned int>(_v);
}

/*================================================================*/
void KoRuler::setupMenu()
{
  rb_menu = new QPopupMenu();
  CHECK_PTR(rb_menu);
  mMM = rb_menu->insertItem(i18n("Millimeters (mm)"),this,SLOT(rbMM()));
  mPT = rb_menu->insertItem(i18n("Points (pt)"),this,SLOT(rbPT()));
  mINCH = rb_menu->insertItem(i18n("Inches (inch)"),this,SLOT(rbINCH()));
  rb_menu->setCheckable(false);
  rb_menu->setItemChecked(mMM,true);
}

/*================================================================*/
void KoRuler::uncheckMenu()
{
  rb_menu->setItemChecked(mMM,false);
  rb_menu->setItemChecked(mPT,false);
  rb_menu->setItemChecked(mINCH,false);
}

/*================================================================*/
void KoRuler::setUnit(QString _unit) 
{ 
  unit = _unit; 
  uncheckMenu();
  
  if (unit == "mm")
    {
      rb_menu->setItemChecked(mMM,true);
      layout.unit = PG_MM;
    }
  else if (unit == "pt")
    {
      rb_menu->setItemChecked(mPT,true);
      layout.unit = PG_PT;
    }
  else if (unit == "inch")
    {
      rb_menu->setItemChecked(mINCH,true);
      layout.unit = PG_INCH;
    }
  repaint(false);
}

#include "koRuler.moc"
