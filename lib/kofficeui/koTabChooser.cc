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
/* Module: Tabulator chooser (header)                             */
/******************************************************************/

#include "koTabChooser.h"

/******************************************************************/
/* Class: KoTabChooser                                            */
/******************************************************************/

/*================================================================*/
KoTabChooser::KoTabChooser(QWidget *parent,int _flags)
  : QFrame(parent,"")
{
  setFrameStyle(Box | Raised);
  flags = _flags;

  currType = 0;
  
  if (flags & TAB_DEC_PNT) currType = TAB_DEC_PNT;
  if (flags & TAB_CENTER) currType = TAB_CENTER;
  if (flags & TAB_RIGHT) currType = TAB_RIGHT;
  if (flags & TAB_LEFT) currType = TAB_LEFT;

  setupMenu();
}

/*================================================================*/
void KoTabChooser::mousePressEvent(QMouseEvent *e)
{
  if (currType == 0) return;

  switch (e->button())
    {
    case LeftButton: case MidButton:
      {
	switch (currType)
	  {
	  case TAB_LEFT:
	    {
	      if (flags & TAB_CENTER) currType = TAB_CENTER;
	      else if (flags & TAB_RIGHT) currType = TAB_RIGHT;
	      else if (flags & TAB_DEC_PNT) currType = TAB_DEC_PNT;
	    } break;
	  case TAB_RIGHT:
	    {
	      if (flags & TAB_DEC_PNT) currType = TAB_DEC_PNT;
	      else if (flags & TAB_LEFT) currType = TAB_LEFT;
	      else if (flags & TAB_CENTER) currType = TAB_CENTER;
	    } break;
	  case TAB_CENTER:
	    {
	      if (flags & TAB_RIGHT) currType = TAB_RIGHT;
	      else if (flags & TAB_DEC_PNT) currType = TAB_DEC_PNT;
	      else if (flags & TAB_LEFT) currType = TAB_LEFT;
	    } break;
	  case TAB_DEC_PNT:
	    {
	      if (flags & TAB_LEFT) currType = TAB_LEFT;
	      else if (flags & TAB_CENTER) currType = TAB_CENTER;
	      else if (flags & TAB_RIGHT) currType = TAB_RIGHT;
	    } break;
	  }
	repaint(true);
      } break;
    case RightButton:
      {
	QPoint pnt(QCursor::pos());

	rb_menu->setItemChecked(mLeft,false);
	rb_menu->setItemChecked(mCenter,false);
	rb_menu->setItemChecked(mRight,false);
	rb_menu->setItemChecked(mDecPoint,false);

	switch (currType)
	  {
	  case TAB_LEFT: rb_menu->setItemChecked(mLeft,true);
	    break;
	  case TAB_CENTER: rb_menu->setItemChecked(mCenter,true);
	    break;
	  case TAB_RIGHT: rb_menu->setItemChecked(mRight,true);
	    break;
	  case TAB_DEC_PNT: rb_menu->setItemChecked(mDecPoint,true);
	    break;
	  }

	rb_menu->popup(pnt);
      } break;
    }
}

/*================================================================*/
void KoTabChooser::drawContents(QPainter *painter)
{
  if (currType == 0) return;

  painter->setPen(QPen(black,2,SolidLine));

  switch (currType)
    {
    case TAB_LEFT:
      {
	painter->drawLine(4,height() - 4,width() - 4,height() - 4);
	painter->drawLine(5,4,5,height() - 4);
      } break;
    case TAB_CENTER:
      {
	painter->drawLine(4,height() - 4,width() - 4,height() - 4);
	painter->drawLine(width() / 2,4,width() / 2,height() - 4);
      } break;
    case TAB_RIGHT:
      {
	painter->drawLine(4,height() - 4,width() - 4,height() - 4);
	painter->drawLine(width() - 5,4,width() - 5,height() - 4);
      } break;
    case TAB_DEC_PNT:
      {
	painter->drawLine(4,height() - 4,width() - 4,height() - 4);
	painter->drawLine(width() / 2,4,width() / 2,height() - 4);
	painter->fillRect(width() / 2 + 2,height() - 9,3,3,black);
      } break;
    default: break;
    }
}

/*================================================================*/
void KoTabChooser::setupMenu()
{
  rb_menu = new QPopupMenu();
  CHECK_PTR(rb_menu);
  mLeft = rb_menu->insertItem(i18n("Tabulator &Left"),this,SLOT(rbLeft()));
  mCenter = rb_menu->insertItem(i18n("Tabulator &Center"),this,SLOT(rbCenter()));
  mRight = rb_menu->insertItem(i18n("Tabulator &Right"),this,SLOT(rbRight()));
  mDecPoint =  rb_menu->insertItem(i18n("Tabulator &Decimal Point"),this,SLOT(rbDecPoint()));
  rb_menu->setCheckable(false);
}
