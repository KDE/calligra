/* This file is part of the KDE libraries
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>
#include <klocale.h>

#include <qpainter.h>
#include <qcursor.h>

#include "form.h"
#include "resizehandle.h"

#define MINIMUM_WIDTH 10
#define MINIMUM_HEIGHT 10

namespace KFormDesigner {

ResizeHandle::ResizeHandle(ResizeHandleSet *set, HandlePos pos, bool editing)
 : QWidget(set->m_widget->parentWidget()), m_set(set)
{
	m_dragging = false;
	//m_editing = editing;
	if(editing)
		setBackgroundColor(blue);
	else
		setBackgroundColor(black);
	setFixedWidth(6);
	setFixedHeight(6);
	m_pos = pos;
	//m_buddy = buddy;
	//buddy->installEventFilter(this);
	m_set->m_widget->installEventFilter(this);

	updatePos();
	show();
}

void ResizeHandle::updatePos()
{
	switch (m_pos)
	{
		case TopLeft:
			move(m_set->m_widget->x() - 3, m_set->m_widget->y() - 3);
			setCursor(QCursor(SizeFDiagCursor));
			break;
		case TopCenter:
		 	move(m_set->m_widget->x() + m_set->m_widget->width()/2 - 3, m_set->m_widget->y() - 3);
			setCursor(QCursor(SizeVerCursor));
			break;
		case TopRight:
			move(m_set->m_widget->x() + m_set->m_widget->width() - 3, m_set->m_widget->y() - 3);
			setCursor(QCursor(SizeBDiagCursor));
			break;
		case LeftCenter:
			move(m_set->m_widget->x() - 3, m_set->m_widget->y() + m_set->m_widget->height()/2 - 3);
			setCursor(QCursor(SizeHorCursor));
			break;
		case RightCenter:
			move(m_set->m_widget->x() + m_set->m_widget->width() - 3, m_set->m_widget->y() + m_set->m_widget->height()/2 - 3);
			setCursor(QCursor(SizeHorCursor));
			break;
		case BottomLeft:
			move(m_set->m_widget->x() - 3, m_set->m_widget->y() + m_set->m_widget->height() - 3);
			setCursor(QCursor(SizeBDiagCursor));
			break;
		case BottomCenter:
			move(m_set->m_widget->x() + m_set->m_widget->width()/2 - 3, m_set->m_widget->y() + m_set->m_widget->height() - 3);
			setCursor(QCursor(SizeVerCursor));
			break;
		case BottomRight:
			move(m_set->m_widget->x() + m_set->m_widget->width() - 3, m_set->m_widget->y() + m_set->m_widget->height() - 3);
			setCursor(QCursor(SizeFDiagCursor));
			break;
	}

}


bool ResizeHandle::eventFilter(QObject *, QEvent *ev)
{
	if ((ev->type() == QEvent::Move) || (ev->type() == QEvent::Resize))
	{
		//QTimer::singleShot(0,this,SLOT(updatePos()));
		updatePos();
	}
	return false;
}

void ResizeHandle::mousePressEvent(QMouseEvent *ev)
{
	m_dragging = true;
	m_x = ev->x();
	m_y = ev->y();
}

void ResizeHandle::mouseMoveEvent(QMouseEvent *ev)
{
#ifndef Q_WS_WIN
		#warning FIXME
#endif
	//int m_dotSpacing = 10;
	int gridX = m_set->m_form->gridX();
	int gridY = m_set->m_form->gridY();

	if (!m_dragging) return;
	//if(m_editing)  return;

	int tmpx = m_set->m_widget->x();
	int tmpy = m_set->m_widget->y();
	int tmpw = m_set->m_widget->width();
	int tmph = m_set->m_widget->height();

	int dummyx = ev->x() - m_x;
	int dummyy = ev->y() - m_y;

	dummyy = (int) ( ((float)dummyy) / ((float)gridY) + 0.5 );
	dummyy *= gridY;
	dummyx = (int) ( ((float)dummyx) / ((float)gridX) + 0.5 );
	dummyx *= gridX;

	switch (m_pos)
	{
		case TopRight:
			tmpw += dummyx;
			tmpy += dummyy;
			tmph -= dummyy;
			break;
		case RightCenter:
			tmpw += dummyx;
			break;
		case BottomRight:
			tmpw += dummyx;
			tmph += dummyy;
			break;
		case TopCenter:
			tmpy += dummyy;
			tmph -= dummyy;
			break;
		case BottomCenter:
			tmph=tmph+dummyy;
			break;
		case TopLeft:
			tmpx += dummyx;
			tmpw -= dummyx;
			tmpy += dummyy;
			tmph -= dummyy;
			break;
		case LeftCenter:
			tmpx += dummyx;
			tmpw -= dummyx;
			break;
		case BottomLeft:
			tmpx += dummyx;
			tmpw -= dummyx;
			tmph += dummyy;
			break;
	}

	// Not move the top-left corner further than the bottom-right corner
	if(tmpx >= m_set->m_widget->x() + m_set->m_widget->width())
	{
		tmpx = m_set->m_widget->x() + m_set->m_widget->width() - MINIMUM_WIDTH;
		tmpw = MINIMUM_WIDTH;
	}

	if(tmpy >= m_set->m_widget->y() + m_set->m_widget->height())
	{
		tmpy = m_set->m_widget->y() + m_set->m_widget->height() - MINIMUM_HEIGHT;
		tmph = MINIMUM_HEIGHT;
	}

	// Do not resize a widget outside of parent boundaries
	if(tmpx < 0)
	{
		tmpw += tmpx;
		tmpx = 0;
	}
	else if(tmpx + tmpw > m_set->m_widget->parentWidget()->width())
		tmpw = m_set->m_widget->parentWidget()->width() - tmpx;

	if(tmpy < 0)
	{
		tmph += tmpy;
		tmpy = 0;
	}
	else if(tmpy + tmph > m_set->m_widget->parentWidget()->height())
		tmph = m_set->m_widget->parentWidget()->height() - tmpy;

	// Move the widget if necessary
	if ( (tmpx != m_set->m_widget->x()) || (tmpy != m_set->m_widget->y()) )
		m_set->m_widget->move(tmpx,tmpy);

	// Resize it
	if ( (tmpw != m_set->m_widget->width()) || (tmph != m_set->m_widget->height()) )
	{
		// Keep a QSize(10, 10) minimum size
		tmpw = (tmpw < MINIMUM_WIDTH) ? MINIMUM_WIDTH : tmpw;
		tmph = (tmph < MINIMUM_HEIGHT) ? MINIMUM_HEIGHT : tmph;
		m_set->m_widget->resize(tmpw,tmph);
	}

}

void ResizeHandle::mouseReleaseEvent(QMouseEvent *)
{
	m_dragging = false;
}

void ResizeHandle::paintEvent( QPaintEvent *ev )
{
	//draw XORed background

	/*QPainter p(this);
	p.setRasterOp(XorROP);
	p.fillRect(QRect(0, 0, 6, 6),white);
	bitBlt( this, QPoint(0,0), parentWidget(), rect(), XorROP);*/
}

ResizeHandle::~ResizeHandle()
{
}

ResizeHandleSet::ResizeHandleSet(QWidget *modify, Form *form, bool editing)
: QObject(modify->parentWidget()), /*m_widget(modify),*/ m_form(form)
{
	m_widget = 0;
	/*QWidget *parent = modify->parentWidget();

	handles[0] = new ResizeHandle( modify, ResizeHandle::TopLeft, editing);
	handles[1] = new ResizeHandle( modify, ResizeHandle::TopCenter, editing);
	handles[2] = new ResizeHandle( modify, ResizeHandle::TopRight, editing);
	handles[3] = new ResizeHandle( modify, ResizeHandle::LeftCenter, editing);
	handles[4] = new ResizeHandle( modify, ResizeHandle::RightCenter, editing);
	handles[5] = new ResizeHandle( modify, ResizeHandle::BottomLeft, editing);
	handles[6] = new ResizeHandle( modify, ResizeHandle::BottomCenter, editing);
	handles[7] = new ResizeHandle( modify, ResizeHandle::BottomRight, editing);*/
	setWidget(modify, editing);
}

ResizeHandleSet::~ResizeHandleSet()
{
	for (int i = 0; i < 8; i++)
		delete handles[i];
}

void
ResizeHandleSet::setWidget(QWidget *modify, bool editing)
{
	if(modify == m_widget)
		return;

	if(m_widget)
		for(int i = 0; i < 8; i++)
			delete handles[i];

	QWidget *parent = modify->parentWidget();
	m_widget = modify;

	handles[0] = new ResizeHandle(this, ResizeHandle::TopLeft, editing);
	handles[1] = new ResizeHandle(this, ResizeHandle::TopCenter, editing);
	handles[2] = new ResizeHandle(this, ResizeHandle::TopRight, editing);
	handles[3] = new ResizeHandle(this, ResizeHandle::LeftCenter, editing);
	handles[4] = new ResizeHandle(this, ResizeHandle::RightCenter, editing);
	handles[5] = new ResizeHandle(this, ResizeHandle::BottomLeft, editing);
	handles[6] = new ResizeHandle(this, ResizeHandle::BottomCenter, editing);
	handles[7] = new ResizeHandle(this, ResizeHandle::BottomRight, editing);
}

}

#include "resizehandle.moc"
