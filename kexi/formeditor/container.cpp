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
#include "events.h"

using namespace KFormDesigner;

//// Helper class for event filtering on composed widgets

void installRecursiveEventFilter(QObject *object, QObject *container)
{
	if(!object->isWidgetType())
		return;

	object->installEventFilter(container);
	if(((QWidget*)object)->ownCursor())
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
	m_toplevel = toplevel;

	m_moving = 0;
	//m_move = false;
	//m_inlineEditing = false;
	m_tree = 0;
	m_form = toplevel ? toplevel->form() : 0;
	m_layout = 0;
	m_layType = NoLayout;
	m_state = DoingNothing;

	QString classname = container->className();
	if((classname == "HBox") || (classname == "Grid") || (classname == "VBox"))
		m_margin = 2;
	else
		m_margin = m_form ? m_form->defaultMargin() : 0;
	m_spacing = m_form ? m_form->defaultSpacing() : 0;

	if(toplevel)
	{
		ObjectTreeItem *it = new ObjectTreeItem(m_form->manager()->lib()->displayName(classname), widget()->name(), widget(), this, this);
		setObjectTree(it);

		if(parent->isWidgetType())
		{
			QString n = parent->name();
			ObjectTreeItem *parent = m_form->objectTree()->lookup(n);
			m_form->objectTree()->addChild(parent, it);
		}
		else
				m_form->objectTree()->addChild(toplevel->tree(), it);
	}

	connect(toplevel, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
	connect(container, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
}

void
Container::setForm(Form *form)
{
	m_form = form;
	m_margin = m_form ? m_form->defaultMargin() : 0;
	m_spacing = m_form ? m_form->defaultSpacing() : 0;
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

			// we are drawing a connection
			if(m_form->manager()->draggingConnection())
			{
				if(mev->button() != LeftButton)
				{
					m_form->manager()->resetCreatedConnection();
					return true;
				}
				// First click, we select the sender and display menu to choose signal
				if(m_form->manager()->createdConnection()->sender().isNull())
				{
					m_form->manager()->createdConnection()->setSender(m_moving->name());
					if(m_form->formWidget())
					{
						m_form->formWidget()->initRect();
						m_form->formWidget()->highlightWidgets(m_moving, 0/*, QPoint()*/);
					}
					m_form->manager()->createSignalMenu(m_moving);
					return true;
				}
				// the user clicked outside the menu, we cancel the connection
				if(m_form->manager()->createdConnection()->signal().isNull())
				{
					m_form->manager()->stopDraggingConnection();
					return true;
				}
				// second click to choose the receiver
				if(m_form->manager()->createdConnection()->receiver().isNull())
				{
					m_form->manager()->createdConnection()->setReceiver(m_moving->name());
					m_form->manager()->createSlotMenu(m_moving);
					m_container->repaint();
					return true;
				}
				// the user clicked outside the menu, we cancel the connection
				if(m_form->manager()->createdConnection()->slot().isNull())
				{
					m_form->manager()->stopDraggingConnection();
					return true;
				}
			}

			if(((mev->state() == ControlButton) || (mev->state() == ShiftButton)) && (!m_form->manager()->inserting())) // multiple selection mode
			{
				if(m_form->selectedWidgets()->findRef(m_moving) != -1) // widget is already selected
				{
					if(m_form->selectedWidgets()->count() > 1) // we remove it from selection
						unSelectWidget(m_moving);
					else // the widget is the only selected, so it means we want to copy it
					{
						m_copyRect = m_moving->geometry();
						m_state = CopyingWidget;
						if(m_form->formWidget())
							m_form->formWidget()->initRect();
					}
				}
				else // the widget is not yet selected, we add it
					setSelectedWidget(m_moving, true);
			}
			else if((m_form->selectedWidgets()->count() > 1))//&& (!m_form->manager()->inserting())) // more than one widget selected
			{
				if(m_form->selectedWidgets()->findRef(m_moving) == -1) // widget is not selected, it becomes the only selected widget
					setSelectedWidget(m_moving, false);
				// If the widget is already selected, we do nothing (to ease widget moving, etc.)
			}
			else// if(!m_form->manager()->inserting())
				setSelectedWidget(m_moving, false);

			// we are inserting a widget or drawing a selection rect in the form
			if((/*s == m_container &&*/ m_form->manager()->inserting()) || ((s == m_container) && !m_toplevel))
			{
				int tmpx,tmpy;
				if(!m_form->manager()->snapWidgetsToGrid() || (mev->state() == (LeftButton|ControlButton|AltButton)))
				{
					tmpx = mev->x();
					tmpy = mev->y();
				}
				else
				{
					int gridX = m_form->gridX();
					int gridY = m_form->gridY();
					tmpx = int( (float)mev->x() / ((float)gridX) + 0.5 ); // snap to grid
					tmpx *= gridX;
					tmpy = int( (float)mev->y() / ((float)gridY) + 0.5 );
					tmpy *= gridX;
				}

				m_insertBegin = ((QWidget*)s)->mapTo(m_container, QPoint(tmpx, tmpy));
				if(m_form->formWidget())
					m_form->formWidget()->initRect();

				if(!m_form->manager()->inserting())
					m_state = DrawingSelectionRect;
				return true;
			}

			m_grab = QPoint(mev->x(), mev->y());

			if(s->inherits("QTabWidget")) // to allow changing page by clicking tab
				return false;
			return true;
		}
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(m_form->manager()->inserting()) // we insert the widget at cursor pos
			{
				if(m_form->formWidget())
					m_form->formWidget()->clearRect();
				KCommand *com = new InsertWidgetCommand(this/*, mev->pos()*/);
				m_form->addCommand(com, true);
				m_insertRect = QRect();
				return true;
			}
			else if(s == m_container && !m_toplevel && (mev->button() != RightButton) && m_insertRect.isValid()) // we are drawing a rect to select widgets
			{
				//finish drawing unclipped selection rectangle: clear the surface
				if(m_form->formWidget())
					m_form->formWidget()->clearRect();
				int topx = (m_insertBegin.x() < mev->x()) ? m_insertBegin.x() :  mev->x();
				int topy = (m_insertBegin.y() < mev->y()) ? m_insertBegin.y() : mev->y();
				int botx = (m_insertBegin.x() > mev->x()) ? m_insertBegin.x() :  mev->x();
				int boty = (m_insertBegin.y() > mev->y()) ? m_insertBegin.y() : mev->y();
				QRect r = QRect(QPoint(topx, topy), QPoint(botx, boty));

				QWidget *w=0;
				setSelectedWidget(m_container, false);
				// We check which widgets are in the rect and select them
				for(ObjectTreeItem *item = m_tree->children()->first(); item; item = m_tree->children()->next())
				{
					w = item->widget();
					if(!w) continue;
					if(w->geometry().intersects(r) && w != m_container)
						setSelectedWidget(w, true);
				}

				m_insertRect = QRect();
				m_state = DoingNothing;
				m_container->repaint();
				return true;
			}
			if(mev->button() == RightButton) // Right-click -> context menu
			{
				bool enable = true;
				if(((QWidget*)s)->isA("QWidget") || ((!m_toplevel) && (s == m_container)))
					enable = false;
				m_form->manager()->createContextMenu((QWidget*)s, this, enable);
			}
			else if(mev->state() == (Qt::LeftButton|Qt::ControlButton) && (m_copyRect.isValid()))
			{
				// copying a widget by Ctrl+dragging
				m_state = DoingNothing;
				m_copyRect = QRect();
				if(m_form->formWidget())
					m_form->formWidget()->clearRect();
				if(s == m_container) // should have no effect on form
					return true;

				//if(m_container->mapFromGlobal(mev->globalPos()) == m_moving->pos())
				if(mev->pos() == m_grab)
				{
					kdDebug() << "The widget has not been moved. No copying" << endl;
					return true;
				}

				m_form->setInteractiveMode(false);
				// We simulate copy and paste
				m_form->manager()->copyWidget();
				m_form->manager()->setInsertPoint(m_container->mapFromGlobal(mev->globalPos()) - m_grab);
				m_form->manager()->pasteWidget();
				m_form->setInteractiveMode(true);

				m_initialPos = QPoint();
				//m_state = DoingNothing;
			}
			else if(/*m_move*/m_state == MovingWidget) // one widget has been moved, so we need to update the layout
			{
				reloadLayout();
				//m_move = false;
				m_initialPos = QPoint();
			}
			// cancel copying as user released Ctrl before releasing mouse button
			else if(m_state == CopyingWidget)
			{
				m_state = DoingNothing;
				m_copyRect = QRect();
				if(m_form->formWidget())
					m_form->formWidget()->clearRect();
			}

			m_copyRect = QRect();
			m_insertRect = QRect();
			m_state = DoingNothing;
			return true; // eat
		}
		case QEvent::MouseMove:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(m_form->manager()->inserting() && ((mev->state() == LeftButton) || (mev->state() == (LeftButton|ControlButton)) ||
			(mev->state() == (LeftButton|ControlButton|AltButton)) || (mev->state() == (LeftButton|ShiftButton)) ) )
			// draw the insert rect
			{
				int tmpx,tmpy;
				QPoint pos = ((QWidget*)s)->mapTo(m_container, mev->pos());
				int gridX = m_form->gridX();
				int gridY = m_form->gridY();
				if(!m_form->manager()->snapWidgetsToGrid() || (mev->state() == (LeftButton|ControlButton|AltButton)) )
				{
					tmpx = pos.x();
					tmpy = pos.y();
				}
				else
				{
					tmpx = int( (float) pos.x() / ((float)gridX) + 0.5);
					tmpx *= gridX;
					tmpy = int( (float)pos.y() / ((float)gridY) + 0.5);
					tmpy *= gridX;
				}

				int topx = (m_insertBegin.x() < tmpx) ? m_insertBegin.x() : tmpx;
				int topy = (m_insertBegin.y() < tmpy) ? m_insertBegin.y() : tmpy;
				int botx = (m_insertBegin.x() > tmpx) ? m_insertBegin.x() : tmpx;
				int boty = (m_insertBegin.y() > tmpy) ? m_insertBegin.y() : tmpy;
				m_insertRect = QRect(QPoint(topx, topy), QPoint(botx, boty));
				//m_insertRect = QRect(((QWidget*)s)->mapTo(m_container, QPoint(topx, topy)), ((QWidget*)s)->mapTo(m_container, QPoint(botx, boty)));

				if(m_insertRect.x() < 0)
					m_insertRect.setX(0);
				if(m_insertRect.y() < 0)
					m_insertRect.setY(0);
				if(m_insertRect.right() > m_container->width())
					m_insertRect.setRight(m_container->width());
				if(m_insertRect.bottom() > m_container->height())
					m_insertRect.setBottom(m_container->height());

				if(m_form->manager()->inserting() && m_insertRect.isValid())
				{
					if(m_form->formWidget())
					{
						QRect drawRect = QRect(m_container->mapTo(m_form->toplevelContainer()->widget(), m_insertRect.topLeft())
						 , m_insertRect.size());
						m_form->formWidget()->drawRect(drawRect, 2);
					}
				}
				return true;
			}
			// Creating a connection, we highlight sender and receiver, and we draw a link between them
			else if(m_form->manager()->draggingConnection() && !m_form->manager()->createdConnection()->sender().isNull())
			{
				ObjectTreeItem *tree = m_form->objectTree()->lookup(m_form->manager()->createdConnection()->sender());
				if(!tree || !tree->widget())
					return true;

				//QPoint p = m_container->mapTo(m_form->toplevelContainer()->widget(), mev->pos());
				if(m_form->formWidget() && (tree->widget() != s))
					m_form->formWidget()->highlightWidgets(tree->widget(), ((QWidget*)s)/*, p*/);
			}
			else if(s == m_container && !m_toplevel && (mev->state() != ControlButton) && !m_form->manager()->draggingConnection()) // draw the selection rect
			{
				if((mev->state() == RightButton) || /*m_inlineEditing*/ m_state == InlineEditing)
					return true;
				int topx = (m_insertBegin.x() < mev->x()) ? m_insertBegin.x() :  mev->x();
				int topy = (m_insertBegin.y() < mev->y()) ? m_insertBegin.y() : mev->y();
				int botx = (m_insertBegin.x() > mev->x()) ? m_insertBegin.x() :  mev->x();
				int boty = (m_insertBegin.y() > mev->y()) ? m_insertBegin.y() : mev->y();
				QRect r = QRect(QPoint(topx, topy), QPoint(botx, boty));
				m_insertRect = r;

				if(m_form->formWidget())
					m_form->formWidget()->drawRect(r, 1);

				m_state = DoingNothing;
				return true;
			}
			else if(mev->state() == (LeftButton|ControlButton)) // draw the insert rect for the copied widget
			{
				if((s == m_container) || (m_form->selectedWidgets()->count() > 1))
					return true;

				// We've been dragging a widget, but Ctrl was hold, so we start copy
				if(m_state == MovingWidget)
				{
					m_moving->move(m_initialPos);
					if(m_form->formWidget())
					{
						m_container->repaint();
						m_form->formWidget()->initRect();
					}
					m_state = CopyingWidget;
				}

				m_copyRect.moveTopLeft(m_container->mapFromGlobal( mev->globalPos()) - m_grab);

				if(m_form->formWidget())
				{
					QRect drawRect = QRect(m_container->mapTo(m_form->toplevelContainer()->widget(), m_copyRect.topLeft())
						 , m_copyRect.size());
					m_form->formWidget()->drawRect(drawRect, 2);
				}
				return true;
			}
			else if( ( (mev->state() == Qt::LeftButton) || (mev->state() == (LeftButton|ControlButton|AltButton)) )
			  && !m_form->manager()->inserting() && (m_state != CopyingWidget)) // we are dragging the widget(s) to move it
			{
				//QWidget *w = m_moving;
				if(!m_toplevel && m_moving == m_container) // no effect for form
					return false;
				if((!m_moving) || (!m_moving->parentWidget()))// || (m_moving->parentWidget()->inherits("QWidgetStack")))
					return true;
				int gridX = m_form->gridX();
				int gridY = m_form->gridY();

				// If we later switch to copy mode, we need to store those info
				if(m_form->selectedWidgets()->count() == 1)
				{
					if(m_initialPos.isNull())
						m_initialPos = m_form->selectedWidgets()->first()->pos();
					if(!m_insertRect.isValid())
						m_copyRect = m_form->selectedWidgets()->first()->geometry();
				}

				for(QWidget *w = m_form->selectedWidgets()->first(); w; w = m_form->selectedWidgets()->next())
				{
					if(w->parentWidget() && w->parentWidget()->isA("QWidgetStack"))
					{
						w = w->parentWidget(); // widget is WidgetStack page
						if(w->parentWidget() && w->parentWidget()->inherits("QTabWidget")) // widget is tabwidget page
							w = w->parentWidget();
					}

					int tmpx, tmpy;
					if(!m_form->manager()->snapWidgetsToGrid() || (mev->state() == (LeftButton|ControlButton|AltButton)) )
					{
						tmpx = w->x() + mev->x() - m_grab.x();
						tmpy = w->y() + mev->y() - m_grab.y();
					}
					else
					{
						tmpx = int( float( w->x() + mev->x() - m_grab.x()) / float(gridX) ) * gridX;
						tmpy = int( float( w->y() + mev->y() - m_grab.y()) / float(gridY) ) * gridY;
					}

					if((tmpx != w->x()) || (tmpy != w->y()))
						w->move(tmpx,tmpy);
				}
				/*int tmpx = (((m_moving->x()+mev->x()-m_grab.x()))/gridX)*gridX;
				int tmpy = (((m_moving->y()+mev->y()-m_grab.y()))/gridY)*gridY;
				if((tmpx!=m_moving->x()) ||(tmpy!=m_moving->y()))
					m_moving->move(tmpx,tmpy);*/
				//m_move = true;
				m_state = MovingWidget;
			}
			// cancel copying as user released Ctrl
			else if((m_state == CopyingWidget) && (mev->state() == Qt::LeftButton))
			{
				//m_state = DoingNothing;
				m_copyRect = QRect();
				if(m_form->formWidget())
					m_form->formWidget()->clearRect();
			}

			return true; // eat
		}
		case QEvent::Paint: // Draw the dotted background
		{
			if(s != m_container)
				return false;
			int gridX = m_form->gridX();
			int gridY = m_form->gridY();

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
				//m_move = true;
				m_state = MovingWidget;
			break;
		}
		case QEvent::KeyPress:
		{
			QKeyEvent *kev = static_cast<QKeyEvent*>(e);
			if(kev->key() == Key_F2) // pressing F2 == double-clicking
			{
				m_state = InlineEditing;
				QWidget *w;

				if(m_form->selectedWidgets()->count() == 1)
					w = m_form->selectedWidgets()->first();
				else if(m_form->selectedWidgets()->findRef(m_moving) != -1)
					w = m_moving;
				else
					w = m_form->selectedWidgets()->last();
				m_form->manager()->lib()->startEditing(w->className(), w, this);
			}
			else if(kev->key() == Key_Escape)
			{
				if(m_form->manager()->draggingConnection())
					m_form->manager()->stopDraggingConnection();
				else if(m_form->manager()->inserting())
					m_form->manager()->stopInsert();
				return true;

			}
			return true;
		}
		case QEvent::KeyRelease:
		{
			return true;
		}
		case QEvent::MouseButtonDblClick: // editing
		{
			kdDebug() << "Container: Mouse dbl click for widget " << s->name() << endl;
			QWidget *w = static_cast<QWidget*>(s);
			if(!w)
				return false;

			//m_inlineEditing = true;
			m_state = InlineEditing;
			m_form->manager()->lib()->startEditing(w->className(), w, this);
			return true;
		}

		case QEvent::ContextMenu:
		case QEvent::Enter:
		case QEvent::Leave:
		case QEvent::FocusIn:
		case QEvent::FocusOut:
		//case QEvent::KeyPress:
		//case QEvent::KeyRelease:
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

	if(!w)
	{
		m_form->setSelectedWidget(m_container);
		return;
	}

	m_form->setSelectedWidget(w, add);
}

void
Container::unSelectWidget(QWidget *w)
{
	if(!w)
		return;

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
Container::deleteWidget(QWidget *w)
{
	if(!w)
		return;

	kdDebug() << "Deleting a widget: " << w->name() << endl;
	m_form->objectTree()->removeChild(w->name());
	m_form->manager()->deleteWidgetLater( w );
	m_form->setSelectedWidget(m_container);
}

void
Container::widgetDeleted()
{
	m_container = 0;
	//delete this;
	deleteLater();
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

#include "container.moc"
