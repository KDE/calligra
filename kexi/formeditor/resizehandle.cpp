/* This file is part of the KDE libraries
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>

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
#include <kglobal.h>
#include <kiconloader.h>
#include <kaction.h>
#include <klineedit.h>

#include <qptrlist.h>
#include <qsize.h>
#include <qpainter.h>
#include <qpen.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <kurlrequester.h>

#include <qobjectlist.h>

#include "resizehandle.h"
#include "widgetcontainer.h"

namespace KFormEditor {

	ResizeHandle::ResizeHandle(QWidget *parent,QWidget *buddy, HandlePos pos):QWidget(parent)
	{
		m_dragging=false;
		setBackgroundColor(black);
		setFixedWidth(6);
		setFixedHeight(6);
		m_pos=pos;
		m_buddy=buddy;
		buddy->installEventFilter(this);
		updatePos();
		show();
	}

	void ResizeHandle::updatePos()
	{
		switch (m_pos)
		{
			case TopLeft: 		move(m_buddy->x(),m_buddy->y());
						setCursor(QCursor(SizeFDiagCursor));
						break;
			case TopCenter: 	move(m_buddy->x()+m_buddy->width()/2-3,m_buddy->y());
						setCursor(QCursor(SizeVerCursor));
						break;
			case TopRight: 		move(m_buddy->x()+m_buddy->width()-6,m_buddy->y());
						setCursor(QCursor(SizeBDiagCursor));
						break;
			case LeftCenter: 	move(m_buddy->x(),m_buddy->y()+m_buddy->height()/2-3);
						setCursor(QCursor(SizeHorCursor));
						break;
			case RightCenter: 	move(m_buddy->x()+m_buddy->width()-6,m_buddy->y()+m_buddy->height()/2-3);
						setCursor(QCursor(SizeHorCursor));
						break;
			case BottomLeft: 	move(m_buddy->x(),m_buddy->y()+m_buddy->height()-6);
						setCursor(QCursor(SizeBDiagCursor));
						break;
			case BottomCenter: 	move(m_buddy->x()+m_buddy->width()/2-3,m_buddy->y()+m_buddy->height()-6);
						setCursor(QCursor(SizeVerCursor));
						break;
			case BottomRight:	move(m_buddy->x()+m_buddy->width()-6,m_buddy->y()+m_buddy->height()-6);
					  	setCursor(QCursor(SizeFDiagCursor));
						break;

		}

	}


	bool ResizeHandle::eventFilter(QObject *obj, QEvent *ev)
	{
		if ((ev->type()==QEvent::Move) || (ev->type()==QEvent::Resize))
		{
			QTimer::singleShot(0,this,SLOT(updatePos()));
		}
		return false;
	}

	void ResizeHandle::mousePressEvent(QMouseEvent *ev)
	{
		m_dragging=true;
		m_x=ev->x();
		m_y=ev->y();
	}

	void ResizeHandle::mouseMoveEvent(QMouseEvent *ev)
	{
		int m_dotSpacing=WidgetContainer::dotSpacing();
		if (!m_dragging) return;

		int tmpx=m_buddy->x();
		int tmpy=m_buddy->y();
		int tmpw=m_buddy->width();
		int tmph=m_buddy->height();
		int dummyx=ev->x()-m_x;
		int dummyy=ev->y()-m_y;
	        dummyy = (((float)dummyy)/((float)m_dotSpacing)+0.5);
        	dummyy*=m_dotSpacing;
	        dummyx = (((float)dummyx)/((float)m_dotSpacing)+0.5);
        	dummyx*=m_dotSpacing;

		switch (m_pos)
		{
			case TopRight:
					tmpw=tmpw+dummyx;
					tmpy=tmpy+dummyy;
					tmph=tmph-dummyy;
				break;
			case RightCenter:
					tmpw=tmpw+dummyx;
				break;
			case BottomRight:
					tmpw=tmpw+dummyx;
					tmph=tmph+dummyy;
				break;
			case TopCenter:
					tmpy=tmpy+dummyy;
					tmph=tmph-dummyy;
				break;
			case BottomCenter:
					tmph=tmph+dummyy;
				break;
			case TopLeft:
					tmpx=tmpx+dummyx;
					tmpw=tmpw-dummyx;
					tmpy=tmpy+dummyy;
					tmph=tmph-dummyy;
				break;
			case LeftCenter:
					tmpx=tmpx+dummyx;
					tmpw=tmpw-dummyx;
				break;
			case BottomLeft:
					tmpx=tmpx+dummyx;
					tmpw=tmpw-dummyx;
					tmph=tmph+dummyy;
				break;
		}

		if ( (tmpx!=m_buddy->x()) || (tmpy!=m_buddy->y()) )
			m_buddy->move(tmpx,tmpy);

		if ( (tmpw!=m_buddy->width()) || (tmph!=m_buddy->height()) )
			m_buddy->resize(tmpw,tmph);

	}

	void ResizeHandle::mouseReleaseEvent(QMouseEvent *ev)
	{
		m_dragging=false;
	}



	ResizeHandle::~ResizeHandle()
	{
	}

	ResizeHandleSet::ResizeHandleSet(QWidget *modify):QObject(modify->parentWidget()),m_widget(modify)
	{
		QWidget *parent=modify->parentWidget();
		handles[0]=new ResizeHandle(parent,modify,ResizeHandle::TopLeft);
		handles[1]=new ResizeHandle(parent,modify,ResizeHandle::TopCenter);
		handles[2]=new ResizeHandle(parent,modify,ResizeHandle::TopRight);
		handles[3]=new ResizeHandle(parent,modify,ResizeHandle::LeftCenter);
		handles[4]=new ResizeHandle(parent,modify,ResizeHandle::RightCenter);
		handles[5]=new ResizeHandle(parent,modify,ResizeHandle::BottomLeft);
		handles[6]=new ResizeHandle(parent,modify,ResizeHandle::BottomCenter);
		handles[7]=new ResizeHandle(parent,modify,ResizeHandle::BottomRight);
	}

	ResizeHandleSet::~ResizeHandleSet()
	{
		for (int i=0;i<8;i++) delete handles[i];
	}

}

#include "resizehandle.moc"
