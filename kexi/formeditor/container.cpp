/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
#include <qrect.h>
#include <qcursor.h>
#include <qobjectlist.h>

#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include "container.h"
#include "widgetlibrary.h"
#include "objecttree.h"
#include "form.h"
#include "formmanager.h"

namespace KFormDesigner {

Container::Container(Container *toplevel, QWidget *container, QObject *parent, const char *name)
:QObject(parent, name)
{
	m_container = container;

	m_moving = 0;
	m_selected = 0;
	m_tree = 0;
	m_form = 0;
	m_toplevel = toplevel;

	container->installEventFilter(this);

	if(toplevel)
	{

		connect(toplevel, SIGNAL(modeChanged(bool)), this, SLOT(setEditingMode(bool)));

		Container *pc = static_cast<Container *>(parent);

		m_form = toplevel->form();

		ObjectTreeItem *it = new ObjectTreeItem(widget()->className(), widget()->name(), widget(), this);
		setObjectTree(it);
		if(parent->inherits("QWidget"))
		{
			QString n = parent->name();
			ObjectTreeItem *parent = m_form->objectTree()->lookup(n);
			m_form->objectTree()->addChild(parent, it);
		}
		else
		{
			if(!pc)
				m_form->objectTree()->addChild(it);
			else
				m_form->objectTree()->addChild(pc->tree(), it);
		}
	}

	connect(container, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
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
			/*if(m_moving->parent()->inherits("QWidgetStack"))
			{
				m_moving = m_moving->parentWidget()->parentWidget();
				kdDebug() << "composed widget  " << m_moving->name() << endl; 
			}*/

			setSelectedWidget(m_moving);

			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			m_grab = QPoint(mev->x(), mev->y());

			if(s == m_container && m_form->manager()->inserting())
			{
				int tmpx,tmpy;
				int gridX = Form::gridX();
				int gridY = Form::gridY();
				tmpx = int((float)mev->x()/((float)gridX)+0.5);
				tmpx*=gridX;
				tmpy = int((float)mev->y()/((float)gridY)+0.5);
				tmpy*=gridX;

				m_insertBegin = QPoint(tmpx, tmpy);
				return true;
			}

			return true;
		}
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(m_form->manager()->inserting())
			{
				QString name = m_form->objectTree()->genName(m_form->manager()->insertClass());
				QWidget *w = m_form->manager()->lib()->createWidget(m_form->manager()->insertClass(), m_container, name.latin1(), this);

				if(!w)
					return true;

				w->move(m_insertRect.x(), m_insertRect.y());
				w->resize(m_insertRect.width()-1, m_insertRect.height()-1);
				w->show();
				m_container->repaint();

				m_insertRect = QRect();
				m_form->manager()->stopInsert();

				if (!m_form->objectTree()->lookup(name))
					m_form->objectTree()->addChild(m_tree, new ObjectTreeItem(m_form->manager()->insertClass(), name, w));
				kdDebug() << "Container::eventFilter(): widget added " << this << endl;
			}
			else if(mev->button() == RightButton)
			{
				kdDebug() << "Container::eventFilter(): context menu" << endl;
				KPopupMenu *parent = m_form->manager()->popupMenu();
				
				QWidget *w = (QWidget*)s;
				QString n = w->className();
				KPopupMenu *p = new KPopupMenu();

				m_form->manager()->lib()->createMenuActions(n,w,p,this);
				int id = parent->insertItem(n,p);

				m_form->manager()->setInsertPoint(m_container->mapFromGlobal(QCursor::pos()));
				parent->exec(QCursor::pos());
				m_form->manager()->setInsertPoint(QPoint());

				parent->removeItem(id);
			}
			return true; // eat
		}
		case QEvent::MouseMove:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(s == m_container && m_form->manager()->inserting())
			{
				int tmpx,tmpy;
				int gridX = Form::gridX();
				int gridY = Form::gridY();
				tmpx = int((float)mev->x()/((float)gridX)+0.5);
				tmpx*=gridX;
				tmpy = int((float)mev->y()/((float)gridY)+0.5);
				tmpy*=gridX;

				m_insertRect = QRect(m_insertBegin, QPoint(tmpx, tmpy));
				if(m_form->manager()->inserting() && m_insertRect.isValid())
				{
					QPainter p(m_container);
					m_container->repaint(); // TODO: find a less cpu consuming solution
					p.setBrush(QBrush::NoBrush);
					p.setPen(QPen(m_container->paletteForegroundColor(), 2));
					p.drawRect(m_insertRect);
				}
				return true;
			}
			if(mev->state() & Qt::LeftButton)
			{
				if(!m_toplevel && m_moving == m_container)
					break;
				int gridX = Form::gridX();
				int gridY = Form::gridY();
				int tmpx = (((m_moving->x()+mev->x()-m_grab.x())+gridX/2)/gridX)*gridX;
				int tmpy = (((m_moving->y()+mev->y()-m_grab.y())+gridY/2)/gridY)*gridY;
				if((tmpx!=m_moving->x()) ||(tmpy!=m_moving->y()))
					m_moving->move(tmpx,tmpy);
			}
			return true; // eat
		}
		case QEvent::Paint:
		{
		
			if(s != m_container)
				return false;
			int gridX = Form::gridX();
			int gridY = Form::gridY();

			QPainter p(m_container);
			p.setPen( QPen(m_container->paletteForegroundColor(), 1) );
			int cols = m_container->width() / gridX;
			int rows = m_container->height() / gridY;

			for(int rowcursor = 1; rowcursor < rows; ++rowcursor)
			{
				for(int colcursor = 1; colcursor < cols; ++colcursor)
				{
					p.drawPoint(colcursor *gridX, rowcursor *gridY);
				}
			}
			return false;
		}

		case QEvent::MouseButtonDblClick:
		case QEvent::ContextMenu:
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

Form *
Container::form()
{
	return m_form;
}

void
Container::setForm(Form *form)
{
	m_form = form;
}

void
Container::setSelectedWidget(QWidget *w)
{
	if(w)
	kdDebug() << "slotSelectionChanged " << w->name()<< endl;

	m_selected = w;

	if(w)
		m_form->setCurrentWidget(w);
	else
		m_form->setCurrentWidget(m_container);
}

void
Container::setEditingMode(bool)
{
}

Container*
Container::toplevel()
{
	if(m_toplevel)
		return m_toplevel;
	else
		return this;
}

ObjectTreeItem*
Container::tree()
{
	return m_tree;
}

void
Container::deleteItem()
{
	if(m_selected)
	{
		kdDebug() << "deleting item : " << m_selected->name() << endl;
		form()->objectTree()->removeChild(m_selected->name());
		m_form->setCurrentWidget(m_container);
		delete m_selected;
		m_selected = 0;
	}
}

void
Container::widgetDeleted()
{
	kdDebug() << "Deleting container : " << m_tree->name() << endl;
	delete this;
}

Container::~Container()
{
}

}

#include "container.moc"
