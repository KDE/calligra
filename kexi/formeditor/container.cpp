/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpainter.h>
#include <qpixmap.h>
#include <qcursor.h>

#include <kdebug.h>

#include "container.h"
//#include "resizehandle.h"
#include "widgetlibrary.h"
#include "objecttree.h"

namespace KFormDesigner {

Container::Container(Container *toplevel, QWidget *container, QObject *parent, const char *name)
:QObject(parent, name)
{
	m_container = container;
	m_gridX = 10;
	m_gridY = 10;

	m_prepare = false;
	m_moving = 0;
	m_resizeHandles = 0;
	m_lib = 0;
	m_selected = 0;
	m_tree = 0;
	m_toplevel = toplevel;

	container->installEventFilter(this);

	if(toplevel)
	{
		connect(toplevel, SIGNAL(modeChanged(bool)), this, SLOT(setEditingMode(bool)));
		connect(toplevel, SIGNAL(selectionChanged(QWidget*)), this, SLOT(slotSelectionChanged(QWidget *)));
		connect(toplevel, SIGNAL(insertStop()), this, SLOT(stopInsert()));

		connect(toplevel, SIGNAL(prepareInsert(WidgetLibrary *, const QString &)), this,
		 SLOT(slotPrepareInsert(WidgetLibrary *, const QString &)));
	}
}

bool
Container::eventFilter(QObject *s, QEvent *e)
{
	switch(e->type())
	{
		case QEvent::MouseButtonPress:
		{
			kdDebug() << "QEvent::MouseButtonPress sender object = " << s->name() << endl;
			kdDebug() << "QEvent::MouseButtonPress this          = " << this->name() << endl;

			m_moving = static_cast<QWidget*>(s);
			if(m_moving->parent()->inherits("QWidgetStack"))
			{
				m_moving = m_moving->parentWidget()->parentWidget();
			}

			if(m_toplevel)
				m_toplevel->setSelectionChanged(m_moving);
			else
				setSelectionChanged(0);

			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			m_grab = QPoint(mev->x(), mev->y());

			if(s == m_container && m_prepare)
			{
				int tmpx,tmpy;
				tmpx = ((float)mev->x()/((float)m_gridX)+0.5);
				tmpx*=m_gridX;
				tmpy = ((float)mev->y()/((float)m_gridY)+0.5);
				tmpy*=m_gridX;

				m_insertBegin = QPoint(tmpx, tmpy);
				return true;
			}

			return true;
		}
		case QEvent::MouseButtonRelease:
		{
			if(m_prepare)
			{
				if(!m_lib)
					return true;

				char *name = "";
				if(tree())
					name = tree()->genName(m_insertClass).latin1();
				else
					name = m_insertClass.latin1();
				QWidget *w = m_lib->createWidget(m_insertClass, m_container, name, this);

				if(m_toplevel)
					m_toplevel->stopInsert();
				else
					stopInsert();

				if(!w)
					return true;

				addWidget(w, m_insertRect);
				if(tree())
					tree()->addChild(new ObjectTreeItem(m_insertClass, name));
			}
			return true; // eat
		}
		case QEvent::MouseMove:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(s == m_container && m_prepare)
			{
				int tmpx,tmpy;
				tmpx = ((float)mev->x()/((float)m_gridX)+0.5);
				tmpx*=m_gridX;
				tmpy = ((float)mev->y()/((float)m_gridY)+0.5);
				tmpy*=m_gridX;

				m_insertRect = QRect(m_insertBegin, QPoint(tmpx, tmpy));
				updateBackground();
				return true;
			}
			if(mev->state() & Qt::LeftButton && m_toplevel)
			{
				int tmpx = (((m_moving->x()+mev->x()-m_grab.x())+m_gridX/2)/m_gridX)*m_gridX;
				int tmpy = (((m_moving->y()+mev->y()-m_grab.y())+m_gridY/2)/m_gridY)*m_gridY;
				if((tmpx!=m_moving->x()) ||(tmpy!=m_moving->y()))
					m_moving->move(tmpx,tmpy);
			}
			return true; // eat
		}
		case QEvent::Resize:
			if(s==m_container)
				updateBackground();
			return false;

		case QEvent::MouseButtonDblClick:
		case QEvent::Enter:
		case QEvent::Leave:
		case QEvent::FocusIn:
		case QEvent::FocusOut:
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
			return true; // eat them

		default:
			return false; // let the widget do the rest ...
	}
	return false;
}

void
Container::addWidget(QWidget *w, QRect r)
{
	w->setGeometry(r);
	w->show();
}

void
Container::updateBackground()
{
	QPainter *p = new QPainter();
	m_dotBg = QPixmap(m_container->size());
	p->begin(&m_dotBg, this);

	QColor c = m_container->paletteBackgroundColor();
	p->setPen(QPen(c));
	QBrush bg(c);
	p->setBrush(bg);
	p->drawRect(0, 0, m_container->width(), m_container->height());
	QPen dots(black, 1);
	p->setPen(dots);
	int cols = m_container->width() / m_gridX;
	int rows = m_container->height() / m_gridY;

	for(int rowcursor = 1; rowcursor < rows; ++rowcursor)
	{
		for(int colcursor = 1; colcursor < cols; ++colcursor)
		{
			p->drawPoint(colcursor *m_gridX, rowcursor *m_gridY);
		}
	}

	if(m_prepare)
	{
		p->setBrush(QBrush::NoBrush);
		p->setPen(QPen(black, 2));
		p->drawRect(m_insertRect);
	}

	p->end();
	m_container->setPaletteBackgroundPixmap(m_dotBg);
}

void
Container::setSelectionChanged(QWidget *w)
{
	slotSelectionChanged(w);
	emit selectionChanged(w);
}

void
Container::setEditingMode(bool)
{
}

void
Container::slotSelectionChanged(QWidget *w)
{
	kdDebug() << "slotSelectionChanged " << (void *)w << " == " << (void *)m_moving << endl;

	if(m_resizeHandles)
	{
		delete m_resizeHandles;
		m_resizeHandles = 0;
	}
	m_selected = 0;

	if(w && w == m_moving)
	{
		m_resizeHandles = new ResizeHandleSet(w);
		m_selected = w;
	}
}

void
Container::emitPrepareInsert(WidgetLibrary *l, const QString &classname)
{
	slotPrepareInsert(l, classname);
	emit prepareInsert(l, classname);
}

void
Container::slotPrepareInsert(WidgetLibrary *l, const QString &classname)
{
//	emit insertRequested(f, classname);
	kdDebug() << "Container::insertWidget(this=" << m_container->name() << ","<< l << "," << classname << ")" << endl;
	m_container->setCursor(QCursor(CrossCursor));
	m_prepare = true;

	m_lib = l;
	m_insertClass = classname;

}

void
Container::stopInsert()
{
	if(!m_toplevel)
		emit insertStop();

	m_prepare = false;
	updateBackground();
	m_container->setCursor(QCursor(ArrowCursor));
}

Container*
Container::toplevel()
{
	if(m_toplevel)
		return m_toplevel;
	else
		return this;
}

ObjectTree*
Container::tree()
{
	if(m_toplevel)
		return m_toplevel->tree();

	return m_tree;
}

Container::~Container()
{
}

}

#include "container.moc"
