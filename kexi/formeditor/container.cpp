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
#include <qevent.h>
#include <qcursor.h>
#include <qvaluevector.h>
#include <qlayout.h>
#include <qobjectlist.h>

#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include <cstdlib> // for abs()

#include "container.h"
#include "widgetlibrary.h"
#include "objecttree.h"
#include "form.h"
#include "formmanager.h"
#include "commands.h"

using namespace KFormDesigner;

//// Helper class for event filtering on composed widgets

void installRecursiveEventFilter(QObject *object, QObject *container)
{
	if(!object->isWidgetType())
		return;
	kdDebug() << "Installing recursive event filter on widget " << object->name() << " of type " << object->className() << endl;
	object->installEventFilter(container);
	if( ((QWidget*)object)->cursor().shape() != Qt::ArrowCursor )
		((QWidget*)object)->setCursor(QCursor(Qt::ArrowCursor));

	if(!object->children())
		return;

	QObjectList list = *(object->children());
	for(QObject *obj = list.first(); obj; obj = list.next())
		installRecursiveEventFilter(obj, container);
}

void removeRecursiveEventFilter(QObject *object, QObject *container)
{
	object->removeEventFilter(container);
	if(!object->isWidgetType())
		return;
	if(!object->children())
		return;

	QObjectList list = *(object->children());
	for(QObject *obj = list.first(); obj; obj = list.next())
		removeRecursiveEventFilter(obj, container);
}

EventEater::EventEater(QWidget *widget, Container *container)
 : QObject(container)
{
	m_widget = widget;
	m_container = container;

	installRecursiveEventFilter(widget, this);
}

bool
EventEater::eventFilter(QObject *, QEvent *ev)
{
	if(!m_container)
		return false;

	// When the user click the empty part of tab bar, only MouseReleaseEvent is sent, we need to simulate the Press event
	if((m_widget->inherits("QTabWidget")) && (ev->type() == QEvent::MouseButtonRelease))
	{
		QMouseEvent *mev = static_cast<QMouseEvent*>(ev);
		if(mev->button() == LeftButton)
		{
			QMouseEvent *myev = new QMouseEvent(QEvent::MouseButtonPress, mev->pos(), mev->button(), mev->state());
			m_container->eventFilter(m_widget, myev);
			delete myev;
			//return true;
		}
	}

	return m_container->eventFilter(m_widget, ev);
}

EventEater::~EventEater()
{
	if(m_widget)
		removeRecursiveEventFilter(m_widget, this);
}

// Container itself

Container::Container(Container *toplevel, QWidget *container, QObject *parent, const char *name)
: QObject(parent, name)
{
	m_container = container;

	m_moving = 0;
	m_move = false;
	m_tree = 0;
	m_form = 0;
	m_layout = 0;
	m_layType = NoLayout;
	m_toplevel = toplevel;

	QString classname = container->className();
	if((classname == "HBox") || (classname == "Grid") || (classname == "VBox"))
		m_margin = 2;
	else
		m_margin = Form::defaultMargin();
	m_spacing = Form::defaultSpacing();

	if(toplevel)
	{
		Container *pc = static_cast<Container *>(parent);

		m_form = toplevel->form();

		EventEater *eater = new EventEater(container, this);
		ObjectTreeItem *it = new ObjectTreeItem(m_form->manager()->lib()->displayName(classname), widget()->name(), widget(), eater, this);
		setObjectTree(it);
		if(parent->isWidgetType())
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

	connect(toplevel, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
	connect(container, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
}

bool
Container::eventFilter(QObject *s, QEvent *e)
{
	switch(e->type())
	{
		case QEvent::MouseButtonPress:
		{
			kdDebug() << "QEvent::MouseButtonPress sender object = " << s->name() << "of type " << s->className() << endl;
			kdDebug() << "QEvent::MouseButtonPress this          = " << this->name() << endl;

			m_moving = static_cast<QWidget*>(s);

			QMouseEvent *mev = static_cast<QMouseEvent*>(e);

			if((mev->state() == ControlButton) || (mev->state() == ShiftButton)) // multiple selection mode
			{
				if(m_selected.findRef(m_moving) != -1) // widget is already selected
				{
					if(m_form->selectedWidgets()->count() > 1) // we remove it from selection
						unSelectWidget(m_moving);
					else // the widget is the only selected, so it means we want to copy it
						m_insertRect = m_moving->geometry();
				}
				else // the widget is not yet selected, we add it
					setSelectedWidget(m_moving, true);
			}
			else if(m_selected.count() > 1) // more than one widget selected
			{
				if(m_selected.findRef(m_moving) == -1) // widget is not selected, it becomes theonly selected widget
					setSelectedWidget(m_moving, false);
				// If the widget is already selected, we do nothing (to ease widget moving, etc.)
			}
			else
				setSelectedWidget(m_moving, false);

			m_grab = QPoint(mev->x(), mev->y());

			// we are inserting a widget or drawing a selection rect in the form
			if((s == m_container && m_form->manager()->inserting()) || (!m_toplevel))
			{
				int tmpx,tmpy;
				int gridX = Form::gridX();
				int gridY = Form::gridY();
				tmpx = int((float)mev->x()/((float)gridX)+0.5); // snap to grid
				tmpx*=gridX;
				tmpy = int((float)mev->y()/((float)gridY)+0.5);
				tmpy*=gridX;

				m_insertBegin = QPoint(tmpx, tmpy);
				if (!m_form->manager()->inserting()) {
					//init drawing unclipped selection rectangle
					if (m_container->inherits("KFormDesigner::FormWidget")) {
						QWidget *cw = static_cast<QWidget*>(m_container);
						static_cast<KFormDesigner::FormWidget*>(cw)->initSelectionRect();
					}
				}
				return true;
			}

			if(s->inherits("QTabWidget")) // to allow changing page by clicking tab
				return false;
			return true;
		}
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(m_form->manager()->inserting()) // we insert the widget at cursor pos
			{
				KCommand *com = new InsertWidgetCommand(this, mev->pos());
				m_form->addCommand(com, true);
			}
			else if(s == m_container && !m_toplevel) // we are drawing a rect to select widgets
			{
				m_container->repaint();
				if (m_container->inherits("KFormDesigner::FormWidget")) {
					//finish drawing unclipped selection rectangle: clear the surface
					QWidget *cw = static_cast<QWidget*>(m_container);
					static_cast<KFormDesigner::FormWidget*>(cw)->clearSelectionRect();
				}
				int topx = (m_insertBegin.x() < mev->x()) ? m_insertBegin.x() :  mev->x();
				int topy = (m_insertBegin.y() < mev->y()) ? m_insertBegin.y() : mev->y();
				int botx = (m_insertBegin.x() > mev->x()) ? m_insertBegin.x() :  mev->x();
				int boty = (m_insertBegin.y() > mev->y()) ? m_insertBegin.y() : mev->y();
				QRect r = QRect(QPoint(topx, topy), QPoint(botx, boty));

				QWidget *w=0;
				QtWidgetList list;

				// We check which widgets are in the rect and select them
				for(ObjectTreeItem *item = m_tree->children()->first(); item; item = m_tree->children()->next())
				{
					w = item->widget();
					if(!w) continue;
					if(w->geometry().intersects(r) && w != m_container)
						list.append(w);
				}

				if(list.isEmpty())
				{
					setSelectedWidget(m_container, false);
				}
				else
				{
					setSelectedWidget(list.first(), false);
					w = list.first();
					for(w = list.next(); w; w = list.next())
						setSelectedWidget(w, true);
				}
			}
			if(mev->button() == RightButton) // Right-click -> context menu
			{
				bool enable = true;
				if(((QWidget*)s)->isA("QWidget") || ((!m_toplevel) && (s == m_container)))
					enable = false;
				m_form->manager()->createContextMenu((QWidget*)s, this, enable);
			}
			else if(mev->state() == (Qt::LeftButton|Qt::ControlButton)) // copying a widget by Ctrl+dragging
			{
				if(s == m_container) // should have no effect on form
					return true;
				m_container->repaint();
				if(m_container->mapFromGlobal(mev->globalPos()) == m_moving->pos())
				{
					kdDebug() << "The widget has not been moved. No copying" << endl;
					return true;
				}

				m_form->setInteractiveMode(false);
				// We simulate copy and paste
				m_form->manager()->copyWidget();
				m_form->manager()->setInsertPoint(m_container->mapFromGlobal(mev->globalPos()));
				m_form->manager()->pasteWidget();
				m_form->setInteractiveMode(true);
			}
			else if(m_move) // one widget has been moved, so we need to update the layout
			{
				reloadLayout();
				m_move = false;
			}
			return true; // eat
		}
		case QEvent::MouseMove:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(s == m_container && m_form->manager()->inserting() && mev->state() != ControlButton) // draw the insert rect
			{
				int tmpx,tmpy;
				int gridX = Form::gridX();
				int gridY = Form::gridY();
				tmpx = int((float)mev->x()/((float)gridX)+0.5);
				tmpx*=gridX;
				tmpy = int((float)mev->y()/((float)gridY)+0.5);
				tmpy*=gridX;

				int topx = (m_insertBegin.x() < tmpx) ? m_insertBegin.x() : tmpx;
				int topy = (m_insertBegin.y() < tmpy) ? m_insertBegin.y() : tmpy;
				int botx = (m_insertBegin.x() > tmpx) ? m_insertBegin.x() : tmpx;
				int boty = (m_insertBegin.y() > tmpy) ? m_insertBegin.y() : tmpy;
				m_insertRect = QRect(QPoint(topx, topy), QPoint(botx, boty));

				if(m_form->manager()->inserting() && m_insertRect.isValid())
				{
					QPainter p(m_container);
					m_container->repaint(); // TODO: find a less cpu consuming solution
					p.setBrush(QBrush::NoBrush);
//					p.setPen(QPen(m_container->paletteForegroundColor(), 2));
					p.setPen(QPen(white, 2));
					p.setRasterOp(XorROP);
					p.drawRect(m_insertRect);
				}
				return true;
			}
			else if(s == m_container && !m_toplevel && mev->state() != ControlButton) // draw the selection rect
			{
				int topx = (m_insertBegin.x() < mev->x()) ? m_insertBegin.x() :  mev->x();
				int topy = (m_insertBegin.y() < mev->y()) ? m_insertBegin.y() : mev->y();
				int botx = (m_insertBegin.x() > mev->x()) ? m_insertBegin.x() :  mev->x();
				int boty = (m_insertBegin.y() > mev->y()) ? m_insertBegin.y() : mev->y();
				QRect r = QRect(QPoint(topx, topy), QPoint(botx, boty));

				if (m_container->inherits("KFormDesigner::FormWidget")) {
					//draw unclipped selection rectangle
					QWidget *cw = static_cast<QWidget*>(m_container);
					static_cast<KFormDesigner::FormWidget*>(cw)->drawSelectionRect(r);
				}
//				m_container->drawSelectionRect(r);
				return true;
			}
			if(mev->state() == (Qt::LeftButton|Qt::ControlButton)) // draw the insert rect for the copied widget
			{
				if(s == m_container)
					return true;
				QPainter p(m_container);
				m_container->repaint(); // TODO: find a less cpu consuming solution
				p.setBrush(QBrush::NoBrush);
//				p.setPen(QPen(m_container->paletteForegroundColor(), 2, Qt::DotLine));
				p.setPen(QPen(white, 2, Qt::DotLine));
				p.setRasterOp(XorROP);
				m_insertRect.moveTopLeft(m_container->mapFromGlobal( mev->globalPos()));
				p.drawRect(m_insertRect);
				return true;
			}
			else if(mev->state() == Qt::LeftButton) // we are dragging the widget(s) to move it
			{
				QWidget *w = m_moving;
				if(!m_toplevel && w == m_container) // no effect for form
					return false;
				if((!m_moving) || (!m_moving->parentWidget()))// || (m_moving->parentWidget()->inherits("QWidgetStack")))
					return true;
				int gridX = Form::gridX();
				int gridY = Form::gridY();

				for(QWidget *w = m_form->selectedWidgets()->first(); w; w = m_form->selectedWidgets()->next())
				{
					if(w->parentWidget() && w->parentWidget()->isA("QWidgetStack"))
					{
						w = w->parentWidget(); // widget is WidgetStack page
						if(w->parentWidget() && w->parentWidget()->inherits("QTabWidget")) // widget is tabwidget page
							w = w->parentWidget();
					}

					int tmpx = ( ( w->x() + mev->x() - m_grab.x()) / gridX ) * gridX;
					int tmpy = ( ( w->y() + mev->y() - m_grab.y()) / gridY ) * gridY;
					if((tmpx != w->x()) ||(tmpy != w->y()))
						w->move(tmpx,tmpy);
				}
				/*int tmpx = (((m_moving->x()+mev->x()-m_grab.x()))/gridX)*gridX;
				int tmpy = (((m_moving->y()+mev->y()-m_grab.y()))/gridY)*gridY;
				if((tmpx!=m_moving->x()) ||(tmpy!=m_moving->y()))
					m_moving->move(tmpx,tmpy);*/
				m_move = true;
			}
			return true; // eat
		}
		case QEvent::Paint: // Draw the dotted background
		{
			if(s != m_container)
				return false;
			int gridX = Form::gridX();
			int gridY = Form::gridY();

			QPainter p(m_container);
//			p.setPen( QPen(m_container->paletteForegroundColor(), 1) );
			p.setPen(QPen(white, 2));
			p.setRasterOp(XorROP);
			int cols = m_container->width() / gridX;
			int rows = m_container->height() / gridY;

			for(int rowcursor = 1; rowcursor <= rows; ++rowcursor)
			{
				for(int colcursor = 1; colcursor <= cols; ++colcursor)
				{
					p.drawPoint(-1 + colcursor *gridX, -1 + rowcursor *gridY);
				}
			}
			return false;
		}
		case QEvent::Resize: // we are resizing a widget, so we set m_move to true -> the layout will be reloaded when releasing mouse
		{
			if(m_form->interactiveMode())
				m_move = true;
			break;
		}
		case QEvent::MouseButtonDblClick: // editing
		{
			kdDebug() << "Container: Mouse dbl click for widget " << s->name() << endl;
			QWidget *w = static_cast<QWidget*>(s);
			if(!w)
				return false;
			m_form->manager()->lib()->startEditing(w->className(), w, this);
			return true;
		}

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
	if (w)
		kdDebug() << "slotSelectionChanged " << w->name()<< endl;

	//raise selected widget and all possible parents
	QWidget *wtmp = w;
	wtmp->raise();
	while (wtmp && wtmp->parentWidget() && static_cast<QObject*>(wtmp)!=m_form && !wtmp->inherits("KFormDesigner::FormWidget")
		&& static_cast<QObject*>(wtmp->parentWidget())!=this && static_cast<QObject*>(wtmp->parentWidget())!=m_form) 
	{
		wtmp = wtmp->parentWidget();
		wtmp->raise();
	}

	if (wtmp)
	{
		wtmp->setFocus();
	}

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
			//delete w;
			form()->manager()->deleteWidgetLater( w );
		}
		m_selected.clear();
		m_form->setCurrentWidget(m_container);
	}
}

void
Container::widgetDeleted()
{
	m_container = 0;
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
			m_layout = (QLayout*) new QHBoxLayout(m_container, m_margin, m_spacing);
			createBoxLayout(new HorWidgetList());
			break;
		}
		case VBox:
		{
			m_layout = (QLayout*) new QVBoxLayout(m_container, m_margin, m_spacing);
			createBoxLayout(new VerWidgetList());
			break;
		}
		case Grid:
		{
			createGridLayout();
			break;
		}
		default:
		{
			kdDebug() << "WRONG LAYOUT TYPE " << endl;
			return;
		}
	}
}

void
Container::reloadLayout()
{
	LayoutType type = m_layType;
	setLayout(NoLayout);
	setLayout(type);
}

void
Container::createBoxLayout(QtWidgetList *list)
{
	QBoxLayout *layout = static_cast<QBoxLayout*>(m_layout);

	for(ObjectTreeItem *tree = m_tree->children()->first(); tree; tree = m_tree->children()->next())
		list->append( tree->widget());
	list->sort();

	for(QWidget *obj = list->first(); obj; obj = list->next())
		layout->addWidget(obj);
	delete list;

//	if(!m_container->parentWidget()->inherits("QWidgetStack"))
//		m_container->resize(layout->sizeHint());
	layout->activate();
}

void
Container::createGridLayout()
{
	//Those lists sort widgets by y and x
	VerWidgetList *vlist = new VerWidgetList();
	HorWidgetList *hlist = new HorWidgetList();
	// The vector are used to store the x (or y) beginning of each column (or row)
	QValueVector<int> cols;
	QValueVector<int> rows;
	int end=-1000;
	bool same = false;

	for(ObjectTreeItem *tree = m_tree->children()->first(); tree; tree = m_tree->children()->next())
		vlist->append( tree->widget());
	vlist->sort();

	for(ObjectTreeItem *tree = m_tree->children()->first(); tree; tree = m_tree->children()->next())
		hlist->append( tree->widget());
	hlist->sort();

	// First we need to make sure that two widgets won't be in the same row,
	// ie that no widget overlap another one
	for(QWidget *w = vlist->first(); w; w = vlist->next())
	{
		QWidget *nextw = vlist->next();
		if(!nextw)
			break;
		while((w->y() <= nextw->y()) && (nextw->y() <= w->geometry().bottom()))
		{
			// If the geometries of the two widgets intersect each other,
			// we move one of the widget to the rght or bottom of the other
			if(w->geometry().intersects(nextw->geometry()))
			{
				if((nextw->y() - w->y()) > abs(nextw->x() - w->x()))
					nextw->move(nextw->x(), w->geometry().bottom()+1);
				else if(nextw->x() >= w->x())
					nextw->move(w->geometry().right()+1, nextw->y());
				else
					w->move(nextw->geometry().right()+1, nextw->y());
			}
			else
				break;
			nextw = vlist->next();
			if(!nextw)
				break;
		}
		QWidget *widg = vlist->prev();
		if(!widg)
			break;
		// the list current item has to be w
		while(w->name() != widg->name())
			widg = vlist->prev();
	}

	// Then we count the number of rows in the layout, and set their beginnings
	for(QWidget *w = vlist->first(); w; w = vlist->next())
	{
		if(!same) // this widget will make a new row
		{
			end = w->geometry().bottom();
			rows.append(w->y());
		}
		// If same == true, it means we are in the same row as prev widget
		// (so no need to create a new column, and we use)

		QWidget *nextw = vlist->next();
		if(!nextw)
			break;

		if(nextw->y() >= end)
		{
			vlist->prev();
			same = false;
		}
		else
		{
			if(same && (nextw->y() >= w->geometry().bottom()))
				same = false;
			else
				same = true;
			vlist->prev();
			if(!same)
				end = w->geometry().bottom();
		}
	}
	kdDebug() << "the new grid will have n rows: n == " << rows.size() << endl;

	end = -10000;
	same = false;
	// We do the same thing for the columns
	for(QWidget *w = hlist->first(); w; w = hlist->next())
	{
		if(!same)
		{
			end = w->geometry().right();
			//ncol++;
			cols.append(w->x());
		}

		QWidget *nextw = hlist->next();
		if(!nextw)
			break;

		if(nextw->x() >= end)
		{
			hlist->prev();
			same = false;
		}
		else
		{
			if(same && (nextw->x() >= w->geometry().right()))
				same = false;
			else
				same = true;
			hlist->prev();
			if(!same)
				end = w->geometry().right();
		}
	}
	kdDebug() << "the new grid will have n columns: n == " << cols.size() << endl;

	// We create the layout ..
	QGridLayout *layout = new QGridLayout(m_container, rows.size(), cols.size(), m_margin, m_spacing, "grid");
	m_layout = (QLayout*)layout;

	// .. and we fill it with widgets
	for(QWidget *w = vlist->first(); w; w = vlist->next())
	{
		QRect r = w->geometry();
		uint wcol=0, wrow=0, endrow=0, endcol=0;
		uint i = 0;

		// We look for widget row(s) ..
		while(r.y() >= rows[i])
		{
			if(rows.size() <= i+1) // we are the last row
			{
				wrow = i;
				break;
			}
			if(r.y() < rows[i+1])
			{
				wrow = i; // the widget will be in this row
				uint j = i + 1;
				// Then we check if the widget needs to span multiple rows
				while(rows.size() >= j+1 && r.bottom() > rows[j])
				{
					endrow = j;
					j++;
				}

				break;
			}
			i++;
		}
		//kdDebug() << "the widget " << w->name() << " wil be in the row " << wrow <<
		   //" and will go to the row " << endrow << endl;

		// .. and column(s)
		i = 0;
		while(r.x() >= cols[i])
		{
			if(cols.size() <= i+1) // last column
			{
				wcol = i;
				break;
			}
			if(r.x() < cols[i+1])
			{
				wcol = i;
				uint j = i + 1;
				// Then we check if the widget needs to span multiple columns
				while(cols.size() >= j+1 && r.right() > cols[j])
				{
					endcol = j;
					j++;
				}

				break;
			}
			i++;
		}
		//kdDebug() << "the widget " << w->name() << " wil be in the col " << wcol <<
		 // " and will go to the col " << endcol << endl;

		ObjectTreeItem *item = m_form->objectTree()->lookup(w->name());
		if(!endrow && !endcol)
		{
			item->setGridPos(wrow, wcol, 0, 0);
			layout->addWidget(w, wrow, wcol);
		}
		else
		{
			if(!endcol)  endcol = wcol;
			if(!endrow)  endrow = wrow;
			layout->addMultiCellWidget(w, wrow, endrow, wcol, endcol);
			item->setGridPos(wrow, wcol, endrow-wrow+1, endcol-wcol+1);
		}
	}
	layout->activate();
}

QString
Container::layoutTypeToString(int type)
{
	switch(type)
	{
		case HBox: return "HBox";
		case VBox: return "VBox";
		case Grid: return "Grid";
		default:   return "NoLayout";
	}
}

Container::LayoutType
Container::stringToLayoutType(const QString &name)
{
	if(name == "HBox")        return HBox;
	if(name == "VBox")        return VBox;
	if(name == "Grid")        return Grid;
	return NoLayout;
}

Container::~Container()
{
	kdDebug() << " Container being deleted this == " << name() << endl;
}

//--------------

/*
void ContainerWidget::drawSelectionRect(const QRect& r)
{
	QPainter p(m_container);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );
	m_container->repaint(); // TODO: find a less cpu consuming solution
	p.setBrush(QBrush::NoBrush);
	p.setPen(QPen(white, 1, Qt::DotLine));
	p.setRasterOp(XorROP);
	p.drawRect(r);
	if (!unclipped)
		clearWFlags( WPaintUnclipped );
}*/

#include "container.moc"
