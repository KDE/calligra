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
#include <qlayout.h>

#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include "container.h"
#include "widgetlibrary.h"
#include "objecttree.h"
#include "form.h"
#include "formmanager.h"

namespace KFormDesigner {

// Helper classes for sorting widgets before inserting them in the layout
class HorWidgetList : public QObjectList
{
	public:
	HorWidgetList() {;}
	virtual int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2)
	{
		QWidget *w1 = static_cast<QWidget*>(item1);
		QWidget *w2 = static_cast<QWidget*>(item2);

		if(w1->x() < w2->x())
			return -1;
		if(w1->x() > w2->x())
			return 1;
		return 0; // item1 == item2
	}
};

class VerWidgetList : public QObjectList
{
	public:
	VerWidgetList() {;}
	virtual int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2)
	{
		QWidget *w1 = static_cast<QWidget*>(item1);
		QWidget *w2 = static_cast<QWidget*>(item2);

		if(w1->y() < w2->y())
			return -10;
		if(w1->y() > w2->y())
			return 1;
		return 0; // item1 == item2
	}
};

// Container itself

Container::Container(Container *toplevel, QWidget *container, QObject *parent, const char *name)
:QObject(parent, name)
{
	m_container = container;

	m_moving = 0;
	m_move = false;
	m_tree = 0;
	m_form = 0;
	m_layout = 0;
	m_layType = NoLayout;
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
			if(!m_form->manager()->isTopLevel(m_moving) && m_moving->parent()->inherits("QWidgetStack"))
			{
				m_moving = m_moving->parentWidget()->parentWidget();
				kdDebug() << "composed widget  " << m_moving->name() << endl; 
			}

			QMouseEvent *mev = static_cast<QMouseEvent*>(e);

			if((mev->state() == ControlButton) || (mev->state() == ShiftButton))
			{
				if(m_selected.findRef(m_moving) != -1)
					unSelectWidget(m_moving);
				else
					setSelectedWidget(m_moving, true);
			}
			else
				setSelectedWidget(m_moving, false);

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

				if(!m_insertRect.isValid())
					m_insertRect = QRect(mev->x(), mev->y(), w->sizeHint().width(), w->sizeHint().height());
				w->move(m_insertRect.x(), m_insertRect.y());
				w->resize(m_insertRect.width()-1, m_insertRect.height()-1);
				w->show();
				m_container->repaint();

				m_insertRect = QRect();
				m_form->manager()->stopInsert();

				if (!m_form->objectTree()->lookup(name))
					m_form->objectTree()->addChild(m_tree, new ObjectTreeItem(m_form->manager()->insertClass(), name, w));
				kdDebug() << "Container::eventFilter(): widget added " << this << endl;

				LayoutType type = layoutType();
				setLayout(NoLayout);
				setLayout(type);
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
			else if(m_move)
			{
				LayoutType type = layoutType();
				setLayout(NoLayout);
				setLayout(type);
				m_move = false;
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
				if(m_moving->parentWidget()->inherits("QWidgetStack"))
					break;
				int gridX = Form::gridX();
				int gridY = Form::gridY();
				int tmpx = (((m_moving->x()+mev->x()-m_grab.x())+gridX/2)/gridX)*gridX;
				int tmpy = (((m_moving->y()+mev->y()-m_grab.y())+gridY/2)/gridY)*gridY;
				if((tmpx!=m_moving->x()) ||(tmpy!=m_moving->y()))
					m_moving->move(tmpx,tmpy);
				m_move = true;
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

void
Container::setSelectedWidget(QWidget *w, bool add)
{
	if(w)
	kdDebug() << "slotSelectionChanged " << w->name()<< endl;

	if(add && w)
	{
		m_selected.append(w);
		m_form->addSelectedWidget(w);
	}
	else if(w)
	{
		m_selected.clear();
		m_selected.append(w);
		m_form->setCurrentWidget(w);
	}
	else
	{
		m_selected.clear();
		m_form->setCurrentWidget(m_container);
	}
}

void
Container::unSelectWidget(QWidget *w)
{
	if(!w)
		return;

	m_selected.remove(w);
	m_form->unSelectWidget(w);
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

void
Container::deleteItem()
{
	if(!m_selected.isEmpty())
	{
		QWidget *w;
		for(w = m_selected.first(); w; w = m_selected.next())
		{
			kdDebug() << "deleting item : " << w->name() << endl;
			form()->objectTree()->removeChild(w->name());
			delete w;
		}
		m_selected.clear();
		m_form->setCurrentWidget(m_container);
	}
}

void
Container::widgetDeleted()
{
	kdDebug() << "Deleting container : " << m_tree->name() << endl;
	delete this;
}

void
Container::setLayout(LayoutType type)
{
	if(m_layType == type)
		return;

	delete m_layout;
	m_layout = 0;
	m_layType = type;

	switch(type)
	{
		case NoLayout:
		{
			return;
		}
		case HBox:
		{
			m_layout = (QLayout*) new QHBoxLayout(m_container, 12);
			createBoxLayout(new HorWidgetList());
			break;
		}
		case VBox:
		{
			m_layout = (QLayout*) new QVBoxLayout(m_container, 12);
			createBoxLayout(new VerWidgetList());
			break;
		}
		case Grid:
		{
		//	QGridLayout *grid = new QGridLayout(m_container, 10);
		//	m_layout = (QLayout*)grid;
			// TODO: Make Grid Layout work :-)
			kdDebug() << "GridLayout not implemented yet !! " << endl;
			return;
		}
		default:
		{
			kdDebug() << "WRONG LAYOUT TYPE " << endl;
			return;
		}
	}
}

void
Container::createBoxLayout(QObjectList *list)
{
	QObjectList *olist = m_container->queryList("QWidget", 0, true, false);
	QBoxLayout *layout = static_cast<QBoxLayout*>(m_layout);
	QObject *obj;

	QObjectListIt iter(*olist);
	while ((obj = iter.current()) != 0)
	{
		if(obj->isA("ResizeHandle"))
			break;
		list->append(obj);
		++iter;
	}
	delete olist;

	list->sort();

	QObjectListIt it(*list);
	while ((obj = it.current()) != 0)
	{
		QWidget *w = static_cast<QWidget*>(obj);
		layout->addWidget(w);
		++it;
	}
	delete list;

//	if(!m_container->parentWidget()->inherits("QWidgetStack"))
//		m_container->resize(layout->sizeHint());
	layout->activate();
}

Container::~Container()
{
}

}

#include "container.moc"
