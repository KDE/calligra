/****************************************************************************
** $Id$
**
** Implementation of QwViewport class
**
** Author  : Warwick Allison (warwick@cs.uq.oz.au)
** Created : 950524
**
** Copyright (C) 1995,1996 by Warwick Allison.
**
*****************************************************************************/

#include <kdebug.h>
// #include "../config.h"
#include <qwidget.h>
#include <qscrbar.h>
#include <qobjcoll.h>
#include "QwViewport.h"

/*!
\class QwViewport QwViewport.h
\brief Version 1.2<p> The QwViewport widget provides a scrolling area with on-demand scrollbars

The contents of the viewport can be any widget, which itself may then contain
other widgets, just like any other QWidget.
*/


/*!
Construct a QwViewport.  A single child should then be attached
to the portHole() of the created QwViewport.

\sa portHole().
*/
QwViewport::QwViewport(QWidget *parent, const char *name, WFlags f) :
	QWidget(parent,name,f),
	hbar(QScrollBar::Horizontal,this,"horizontal"),
	vbar(QScrollBar::Vertical,this,"vertical"),
	porthole(this,"porthole")
{
	connect(&hbar, SIGNAL(valueChanged(int)),
		this, SLOT(hslide(int)));
	connect(&vbar, SIGNAL(valueChanged(int)),
		this, SLOT(vslide(int)));
	visibleScrollBars = true;
}

/*!
The QWidget in the scrolling area.  
*/
QWidget* QwViewport::viewedWidget() const
{
	const QObjectList *l = porthole.children();

	if (l) {
		QObjectListIt iter(*l);
		QObject *ch = iter.current();;
		if (ch && ch->isWidgetType()) {
			return (QWidget*)ch;
		}
	}

	return 0;
}

// This variable allows ensureVisible to move the viewed widget then
// update both the sliders.  Otherwise, updating the sliders would
// cause two image scrolls, creating ugly flashing.
//
bool QwViewport::signal_choke=false;

void QwViewport::hslide(int pos)
{
	if (!signal_choke && viewedWidget()) {
		viewedWidget()->move(-pos,viewedWidget()->y());
	}
}

void QwViewport::vslide(int pos)
{
	if (!signal_choke && viewedWidget()) {
		viewedWidget()->move(viewedWidget()->x(),-pos);
	}
}

/*!
Call this if properties change dynamically that would require resizing
the scrollbar.
*/
void QwViewport::resizeScrollBars()
{
  int w=width();
  int h=height();

  if (viewedWidget()) {
    int portw,porth;

    bool needh, needv;

    if (visibleScrollBars) {
      needh = w<viewedWidget()->width();
      needv = h<viewedWidget()->height();
      
      if (needh && h-scrollBarWidth()<viewedWidget()->height())
	needv=true;
      if (needv && w-scrollBarWidth()<viewedWidget()->width())
	needh=true;
    }
    else
      needh = needv = false;

    if (needh) {
      hbar.show();
      porth=h-scrollBarWidth();
    } else {
      hslide(0);
      hbar.hide();
      porth=h;
    }
    
    if (needv) {
      vbar.show();
      portw=w-scrollBarWidth();
    } else {
      vslide(0);
      vbar.hide();
      portw=w;
    }
    
    if (needv) {
      vbar.setRange(0,viewedWidget()->height()-porth);
      vbar.setSteps(1,porth);
    }
    if (needh) {
      hbar.setRange(0,viewedWidget()->width()-portw);
      hbar.setSteps(1,portw);
    }
    
    int top,bottom;
    
    if (needh) {
      int right=((needv && emptyCorner())
		 || alwaysEmptyCorner())
	? w-scrollBarWidth() : w;
      if (scrollBarOnTop()) {
	hbar.setGeometry(0,h-scrollBarWidth(),right,scrollBarWidth());
	top=scrollBarWidth();
	bottom=h;
      } else {
	hbar.setGeometry(0,h-scrollBarWidth(),right,scrollBarWidth());
	top=0;
	bottom=h-scrollBarWidth();
      }
    } else {
      top=0;
      bottom=h;
    }
    if (needv) {
      if (scrollBarOnLeft()) {
	vbar.setGeometry(0,top,scrollBarWidth(),bottom);
	porthole.setGeometry(scrollBarWidth(),top,w-scrollBarWidth(),bottom);
      } else {
	vbar.setGeometry(w-scrollBarWidth(),top,scrollBarWidth(),bottom);
	porthole.setGeometry(0,top,w-scrollBarWidth(),bottom);
      }
    } else {
      porthole.setGeometry(0,top,w,bottom);
    }
  } else {
    hbar.hide();
    vbar.hide();
    porthole.setGeometry(0,0,w,h);
  }
}

void QwViewport::showScrollBars () {
  visibleScrollBars = true;
  resizeScrollBars ();
}

void QwViewport::hideScrollBars () {
  visibleScrollBars = false;
  resizeScrollBars ();
}

/*!
An override - ensures scrollbars are correct size upon showing.
*/
void QwViewport::show()
{
	resizeScrollBars();
	QWidget::show();
}

/*!
An override - ensures scrollbars are correct size upon resize.
*/
void QwViewport::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	resizeScrollBars();
}

/*!
Override this to adjust width of scrollbars.
Default returns 16.
*/
int QwViewport::scrollBarWidth() const
{
	return 16;
}

/*!
Overriding this to determine which on side the vertical scrollbar appears.
Default returns false (always on left).
*/
bool QwViewport::scrollBarOnLeft() const
{
	return false;
}

/*!
Overriding this to determine which on side the horizontal scrollbar appears.
Default returns false (always on bottom).
*/
bool QwViewport::scrollBarOnTop() const
{
	return false;
}

/*!
Overriding this to determine whether the corner between the
scrollbars is empty when both are present.  Otherwise, the horizontal
scrollbar will use that space.

Default returns true (empty when both appear).
*/
bool QwViewport::emptyCorner() const
{
	return true;
}

/*!
Overriding this to determine whether the corner between the
scrollbars is \e always empty.  Otherwise, if one scrollbar is
present, it will use that space; if two, effect is determined
by emptyCorner().

Default returns false.
*/
bool QwViewport::alwaysEmptyCorner() const
{
	return false;
}

// Inlines:

/*!
\fn QWidget* QwViewport::portHole()
Returns the QWidget that is the scrolling area.  Attach a single
child to this (give \c yourviewport.portHole() as the parent when
creating the child widget).  The child should be resized
before attaching it. ??do what if it resizes??
*/

/*!
\fn QScrollBar& QwViewport::horizontalScrollBar()

 The component horizontal scrollbar.  It is made available to allow
 accelerators, autoscrolling, etc., and to allow changing
 of arrow scrollrates: bar->setSteps(rate, bar->pageStep()).

 It should not be otherwise manipulated.
*/

/*!
\fn QScrollBar& QwViewport::verticalScrollBar()

 The component vertical scrollbar.  It is made available to allow
 accelerators, autoscrolling, etc., and to allow changing
 of arrow scrollrates: bar->setSteps(rate, bar->pageStep()).

 It should not be otherwise manipulated.
*/

/*!
 Move such that (x,y) is visible and with at least the given
 pixel margins (if possible, otherwise, centered).
*/
void QwViewport::ensureVisible(int x, int y, int xmargin, int ymargin)
{
	// Algorithm taken from my WAX++ original

	int pw=portHole()->width();
	int ph=portHole()->height();

	QWidget* child=viewedWidget();
	if (!child) return;

	int cx=child->x();
	int cy=child->y();
	int cw=child->width();
	int ch=child->height();

	// int mbw=0; // XXX border width - is this a problem in Qt?

	if (pw < xmargin*2) xmargin=pw/2;
	if (ph < ymargin*2) ymargin=ph/2;

	if (cw <= pw) { xmargin=0; cx=0; }
	if (ch <= ph) { ymargin=0; cy=0; }

	if (x < -cx+xmargin) {
		cx = -x+pw-xmargin;
	} else if (x >= -cx+pw-xmargin) {
		cx = -x+xmargin;
	}

	if (y < -cy+ymargin) {
		cy = -y+ph-ymargin;
	} else if (y >= -cy+ph-ymargin) {
		cy = -y+ymargin;
	}

	if (cx > 0) cx=0;
	else if (cx < pw-cw && cw>pw) cx=pw-cw;
	if (cy > 0) cy=0;
	else if (cy < ph-ch && ch>ph) cy=ph-ch;

	// Choke signal handling while we update BOTH sliders.
	signal_choke=true;
	child->move(cx,cy);
	vbar.setValue(-cy);
	hbar.setValue(-cx);
	resizeScrollBars();
	signal_choke=false;
}

/*!
 Completely centered (except at edges of playfield)
*/
void QwViewport::centerOn(int x, int y)
{
	ensureVisible(x,y,32000,32000);
}

/*!
 Margins as fraction of visible area.
   0.0 = Allow (x,y) to be on edge of visible area.
   0.5 = Ensure (x,y) is in middle 50% of visible area.
   1.0 = CenterOn(x,y).
*/
void QwViewport::centralize(int x, int y, float xmargin, float ymargin)
{
	int pw=portHole()->width();
	int ph=portHole()->height();
	ensureVisible(x,y,int(xmargin/2.0*pw+0.5),int(ymargin/2.0*ph+0.5));
}

#include "QwViewport.moc"
