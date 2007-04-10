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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qpainter.h>
#include <qpixmap.h>
#include <qrect.h>
#include <qevent.h>
#include <q3valuevector.h>
#include <qlayout.h>
#include <qcursor.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3GridLayout>
#include <QKeyEvent>
#include <Q3ValueList>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QMouseEvent>

#include <kdebug.h>
#include <klocale.h>
#include <kmenu.h>

#include <cstdlib> // for abs()

#include "utils.h"
#include "container.h"
#include "widgetlibrary.h"
#include "objecttree.h"
#include "form.h"
#include "formmanager.h"
#include "commands.h"
#include "events.h"
#include <kexiutils/utils.h>

#define KEXI_NO_FLOWLAYOUT
#ifdef __GNUC__
#warning "Port Kexi flow layout!"
#endif
#ifndef KEXI_NO_FLOWLAYOUT
#include "kexiflowlayout.h"
#endif

using namespace KFormDesigner;

EventEater::EventEater(QWidget *widget, QObject *container)
 : QObject(container)
{
	m_widget = widget;
	m_container = container;

	installRecursiveEventFilter(m_widget, this);
}

bool
EventEater::eventFilter(QObject *, QEvent *ev)
{
	if(!m_container)
		return false;

	// When the user click the empty part of tab bar, only MouseReleaseEvent is sent,
	// we need to simulate the Press event
	if(ev->type() == QEvent::MouseButtonRelease && m_widget->inherits("QTabWidget"))
	{
		QMouseEvent *mev = static_cast<QMouseEvent*>(ev);
		if(mev->button() == Qt::LeftButton)
		{
			QMouseEvent *myev = new QMouseEvent(QEvent::MouseButtonPress, mev->pos(), mev->button(), mev->buttons(), mev->modifiers());
			m_container->eventFilter(m_widget, myev);
			delete myev;
			//return true;
		}
	}
//	else if(ev->type() == QEvent::ChildInserted) {
		// widget's children have changed, we need to reinstall filter
//		installRecursiveEventFilter(m_widget, this);
//	}

	return m_container->eventFilter(m_widget, ev);
}

EventEater::~EventEater()
{
	if(m_widget)
		removeRecursiveEventFilter(m_widget, this);
}

// Container itself

Container::Container(Container *toplevel, QWidget *container, QObject *parent)
: QObject(parent)
, m_insertBegin(-1,-1)
, m_mousePressEventReceived(false)
, m_mouseReleaseEvent(QEvent::None, QPoint(), Qt::NoButton, Qt::NoButton, Qt::NoModifier)
{
	m_container = container;
	m_toplevel = toplevel;

	m_moving = 0;
	m_tree = 0;
	m_form = toplevel ? toplevel->form() : 0;
	m_layout = 0;
	m_layType = NoLayout;
	m_state = DoingNothing;

	Q3CString classname = container->metaObject()->className();
	if((classname == "HBox") || (classname == "Grid") || (classname == "VBox") ||
		(classname == "HFlow")  || (classname == "VFlow"))
		m_margin = 4; // those containers don't have frames, so little margin
	else
		m_margin = m_form ? m_form->defaultMargin() : 0;
	m_spacing = m_form ? m_form->defaultSpacing() : 0;

	if(toplevel)
	{
		ObjectTreeItem *it = new ObjectTreeItem(m_form->library()->displayName(classname),
			widget()->objectName(), widget(), this, this);
		setObjectTree(it);

		if(parent->isWidgetType())
		{
			QString n = parent->objectName();
			ObjectTreeItem *parent = m_form->objectTree()->lookup(n);
			m_form->objectTree()->addItem(parent, it);
		}
		else
			m_form->objectTree()->addItem(toplevel->objectTree(), it);

		connect(toplevel, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
	}

	connect(container, SIGNAL(destroyed()), this, SLOT(widgetDeleted()));
}

Container::~Container()
{
	kDebug() << " Container being deleted this == " << objectName() << endl;
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
//	kDebug() << e->type() << endl;
	switch(e->type())
	{
		case QEvent::MouseButtonPress:
		{
			m_insertBegin = QPoint(-1, -1);
			m_mousePressEventReceived = true;

			kDebug() << "QEvent::MouseButtonPress sender object = " << s->objectName()
				<< "of type " << s->metaObject()->className() << endl;
			kDebug() << "QEvent::MouseButtonPress this          = " << this->objectName() << endl;

			m_moving = static_cast<QWidget*>(s);
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			m_grab = QPoint(mev->x(), mev->y());

			// we are drawing a connection
			if(FormManager::self()->isCreatingConnection())  {
				drawConnection(mev);
				return true;
			}

			if((mev->modifiers() == Qt::ControlModifier || mev->modifiers() == Qt::ShiftModifier) 
				&& !FormManager::self()->isInserting()) // multiple selection mode
			{
				if(m_form->selectedWidgets()->findRef(m_moving) != -1) // widget is already selected
				{
					if(m_form->selectedWidgets()->count() > 1) // we remove it from selection
						unSelectWidget(m_moving);
					else // the widget is the only selected, so it means we want to copy it
					{
						//m_copyRect = m_moving->geometry();
						m_state = CopyingWidget;
						if(m_form->formWidget())
							m_form->formWidget()->initBuffer();
					}
				}
				else // the widget is not yet selected, we add it
					setSelectedWidget(m_moving, true, (mev->button() == Qt::RightButton));
			}
			else if((m_form->selectedWidgets()->count() > 1))//&& (!m_form->manager()->isInserting())) // more than one widget selected
			{
				if(m_form->selectedWidgets()->findRef(m_moving) == -1) // widget is not selected, it becomes the only selected widget
					setSelectedWidget(m_moving, false, (mev->button() == Qt::RightButton));
				// If the widget is already selected, we do nothing (to ease widget moving, etc.)
			}
			else// if(!m_form->manager()->isInserting())
				setSelectedWidget(m_moving, false, (mev->button() == Qt::RightButton));

			// we are inserting a widget or drawing a selection rect in the form
			if((/*s == m_container &&*/ FormManager::self()->isInserting()) || ((s == m_container) && !m_toplevel))
			{
				int tmpx,tmpy;
				if(!FormManager::self()->snapWidgetsToGrid() || (mev->buttons() == Qt::LeftButton && mev->modifiers()==Qt::ControlModifier|Qt::AltModifier))
				{
					tmpx = mev->x();
					tmpy = mev->y();
				}
				else
				{
					int gridX = m_form->gridSize();
					int gridY = m_form->gridSize();
					tmpx = int( (float)mev->x() / ((float)gridX) + 0.5 ); // snap to grid
					tmpx *= gridX;
					tmpy = int( (float)mev->y() / ((float)gridY) + 0.5 );
					tmpy *= gridX;
				}

				m_insertBegin = (static_cast<QWidget*>(s))->mapTo(m_container, QPoint(tmpx, tmpy));
				if(m_form->formWidget())
					m_form->formWidget()->initBuffer();

				if(!FormManager::self()->isInserting())
					m_state = DrawingSelectionRect;
			}
			else {
				if(s->inherits("QTabWidget")) // to allow changing page by clicking tab
					return false;
			}

			if (m_objectForMouseReleaseEvent) {
				const bool res = handleMouseReleaseEvent(m_objectForMouseReleaseEvent, &m_mouseReleaseEvent);
				m_objectForMouseReleaseEvent = 0;
				return res;
			}
			return true;
		}

		case QEvent::MouseButtonRelease:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if (!m_mousePressEventReceived) {
				m_mouseReleaseEvent = *mev;
				m_objectForMouseReleaseEvent = s;
				return true;
			}
			m_mousePressEventReceived = false;
			m_objectForMouseReleaseEvent = 0;
			return handleMouseReleaseEvent(s, mev);
		}

		case QEvent::MouseMove:
		{
			QMouseEvent *mev = static_cast<QMouseEvent*>(e);
			if(m_insertBegin!=QPoint(-1,-1) && FormManager::self()->isInserting() && ((mev->buttons() == Qt::LeftButton) || (mev->buttons() == Qt::LeftButton && mev->modifiers()==Qt::ControlModifier) ||
			(mev->buttons() == Qt::LeftButton && mev->modifiers()==Qt::ControlModifier|Qt::AltModifier) || (mev->buttons() == Qt::LeftButton && mev->modifiers()==Qt::ShiftModifier) ) )
			// draw the insert rect
			{
				drawInsertRect(mev, s);
				return true;
			}
			// Creating a connection, we highlight sender and receiver, and we draw a link between them
			else if(FormManager::self()->isCreatingConnection() && !FormManager::self()->createdConnection()->sender().isNull())
			{
				ObjectTreeItem *tree = m_form->objectTree()->lookup(FormManager::self()->createdConnection()->sender());
				if(!tree || !tree->widget())
					return true;

				if(m_form->formWidget() && (tree->widget() != s))
					m_form->formWidget()->highlightWidgets(tree->widget(), static_cast<QWidget*>(s));
			}
			else if(m_insertBegin!=QPoint(-1,-1) && s == m_container && !m_toplevel && (mev->modifiers() != Qt::ControlModifier) && !FormManager::self()->isCreatingConnection()) // draw the selection rect
			{
				if((mev->buttons() != Qt::LeftButton) || /*m_inlineEditing*/ m_state == InlineEditing)
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
			else if(mev->buttons() == Qt::LeftButton && mev->modifiers() == Qt::ControlModifier)
			{
				// draw the insert rect for the copied widget
				if(s == m_container)// || (m_form->selectedWidgets()->count() > 1))
					return true;
				drawCopiedWidgetRect(mev);
				return true;
			}
			else if( 
				( mev->buttons() == Qt::LeftButton || (mev->buttons() == Qt::LeftButton && mev->modifiers() == Qt::ControlModifier|Qt::AltModifier) )
			  && !FormManager::self()->isInserting() && m_state != CopyingWidget)
			{
			  // we are dragging the widget(s) to move it
				if(!m_toplevel && m_moving == m_container) // no effect for form
					return false;
				if((!m_moving) || (!m_moving->parentWidget()))// || (m_moving->parentWidget()->inherits("QWidgetStack")))
						return true;

				moveSelectedWidgetsBy(mev->x() - m_grab.x(), mev->y() - m_grab.y());
				m_state = MovingWidget;
			}

			return true; // eat
		}

		case QEvent::Paint: // Draw the dotted background
		{
			if(s != m_container)
				return false;
			int gridX = m_form->gridSize();
			int gridY = m_form->gridSize();

			QPainter p(m_container);
			p.setPen(QPen(Qt::white, 2));
			p.setCompositionMode(QPainter::CompositionMode_Xor);
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
				m_state = MovingWidget;
			break;
		}

		//case QEvent::AccelOverride:
		case QEvent::KeyPress:
		{
			QKeyEvent *kev = static_cast<QKeyEvent*>(e);

			if(kev->key() == Qt::Key_F2) // pressing F2 == double-clicking
			{
				m_state = InlineEditing;
				QWidget *w;

				// try to find the widget which was clicked last and should be edited
				if(m_form->selectedWidgets()->count() == 1)
					w = m_form->selectedWidgets()->first();
				else if(m_form->selectedWidgets()->findRef(m_moving) != -1)
					w = m_moving;
				else
					w = m_form->selectedWidgets()->last();
				m_form->library()->startEditing(w->metaObject()->className(), w, this);
			}
			else if(kev->key() == Qt::Key_Escape)
			{
				if(FormManager::self()->isCreatingConnection())
					FormManager::self()->stopCreatingConnection();
				else if(FormManager::self()->isInserting())
					FormManager::self()->stopInsert();
				return true;
			}
			else if((kev->key() == Qt::Key_Control) && (m_state == MovingWidget))
			{
				if(!m_moving)
					return true;
				// we simulate a mouse move event to update screen
				QMouseEvent *mev = new QMouseEvent(QEvent::MouseMove, m_moving->mapFromGlobal(QCursor::pos()), Qt::NoButton,
				Qt::LeftButton|Qt::ControlModifier );
				eventFilter(m_moving, mev);
				delete mev;
			}
			else if(kev->key() == FormManager::self()->contextMenuKey())
			{
					FormManager::self()->createContextMenu(static_cast<QWidget*>(s), this, false);
					return true;
			}
			else if (kev->key() == Qt::Key_Delete)
			{
				FormManager::self()->deleteWidget();
				return true;
			}
			// directional buttons move the widget
			else if(kev->key() == Qt::Key_Left){ // move the widget of gridX to the left
				moveSelectedWidgetsBy(-form()->gridSize(), 0);
				return true;
			}
			else if(kev->key() == Qt::Key_Right){ // move the widget of gridX to the right
				moveSelectedWidgetsBy(form()->gridSize(), 0);
				return true;
			}
			else if(kev->key() == Qt::Key_Up){ // move the widget of gridY to the top
				moveSelectedWidgetsBy(0, - form()->gridSize());
				return true;
			}
			else if(kev->key() == Qt::Key_Down){ // move the widget of gridX to the bottom
				moveSelectedWidgetsBy(0, form()->gridSize());
				return true;
			}
			else if((kev->key() == Qt::Key_Tab) || (kev->key() == Qt::Key_Backtab)){
				ObjectTreeItem *item = form()->objectTree()->lookup(form()->selectedWidgets()->first()->objectName());
				if(!item || !item->parent())
					return true;
				ObjectTreeList *list = item->parent()->children();
				if(list->count() == 1)
					return true;
				int index = list->findRef(item);

				if(kev->key() == Qt::Key_Backtab){
					if(index == 0) // go back to the last item
						index = list->count() - 1;
					else
						index = index - 1;
				}
				else  {
					if(index == int(list->count() - 1)) // go back to the first item
						index = 0;
					else
						index = index + 1;
				}

				ObjectTreeItem *nextItem = list->at(index);
				if(nextItem && nextItem->widget())
					form()->setSelectedWidget(nextItem->widget(), false);
			}
			return true;
		}

		case QEvent::KeyRelease:
		{
			QKeyEvent *kev = static_cast<QKeyEvent*>(e);
			if((kev->key() == Qt::Key_Control) && (m_state == CopyingWidget)) {
				// cancel copying
				//m_copyRect = QRect();
				if(m_form->formWidget())
					m_form->formWidget()->clearForm();
			}
			return true;
		}

		case QEvent::MouseButtonDblClick: // editing
		{
			kDebug() << "Container: Mouse dbl click for widget " << s->objectName() << endl;
			QWidget *w = static_cast<QWidget*>(s);
			if(!w)
				return false;

			//m_inlineEditing = true;
			m_state = InlineEditing;
			m_form->library()->startEditing(w->metaObject()->className(), w, this);
			return true;
		}

		case QEvent::ContextMenu:
		case QEvent::Enter:
		case QEvent::Leave:
		case QEvent::FocusIn:
		case QEvent::FocusOut:
//		case QEvent::DragEnter:
//		case QEvent::DragMove:
//		case QEvent::DragLeave:
			return true; // eat them

		default:
			return false; // let the widget do the rest ...
	}
	return false;
}

bool
Container::handleMouseReleaseEvent(QObject *s, QMouseEvent *mev)
{
	if(FormManager::self()->isInserting()) // we insert the widget at cursor pos
	{
		if(m_form->formWidget())
			m_form->formWidget()->clearForm();
		K3Command *com = new InsertWidgetCommand(this/*, mev->pos()*/);
		m_form->addCommand(com, true);
		m_insertBegin = QPoint(-1,-1);
		m_insertRect = QRect();
		return true;
	}
	else if(s == m_container && !m_toplevel && (mev->button() != Qt::RightButton) && m_insertRect.isValid()) // we are drawing a rect to select widgets
	{
		drawSelectionRect(mev);
		return true;
	}
	if(mev->button() == Qt::RightButton) // Right-click -> context menu
	{
		FormManager::self()->createContextMenu(static_cast<QWidget*>(s), this);
	}
	else if(mev->buttons() == Qt::LeftButton && mev->modifiers()==Qt::ControlModifier)// && (m_copyRect.isValid()))
	{
		// copying a widget by Ctrl+dragging

		if(m_form->formWidget())
			m_form->formWidget()->clearForm();
		if(s == m_container) // should have no effect on form
			return true;

		// prevent accidental copying of widget (when moving mouse a little while selecting)
		if( ( (mev->pos().x() - m_grab.x()) < form()->gridSize() &&  (m_grab.x() - mev->pos().x()) < form()->gridSize() ) &&
			( (mev->pos().y() - m_grab.y()) < form()->gridSize() &&  (m_grab.y() - mev->pos().y()) < form()->gridSize() ) )
		{
			kDebug() << "The widget has not been moved. No copying" << endl;
			return true;
		}

		m_form->setInteractiveMode(false);
		// We simulate copy and paste
		FormManager::self()->copyWidget();
		if(m_form->selectedWidgets()->count() > 1)
			FormManager::self()->setInsertPoint( mev->pos() );
		else
			FormManager::self()->setInsertPoint( static_cast<QWidget*>(s)->mapTo(m_container, mev->pos() - m_grab) );
		FormManager::self()->pasteWidget();
		m_form->setInteractiveMode(true);

		//m_initialPos = QPoint();
	}
	else if(m_state == MovingWidget) // one widget has been moved, so we need to update the layout
		reloadLayout();

	// cancel copying as user released Ctrl before releasing mouse button
	m_insertBegin = QPoint(-1,-1);
	m_insertRect = QRect();
	m_state = DoingNothing;
	return true; // eat
}

void
Container::setSelectedWidget(QWidget *w, bool add, bool dontRaise, bool moreWillBeSelected)
{
	if (w)
		kDebug() << "slotSelectionChanged " << w->objectName()<< endl;

	if(!w)
	{
		m_form->setSelectedWidget(m_container);
		return;
	}

	m_form->setSelectedWidget(w, add, dontRaise, moreWillBeSelected);
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
//	kDebug() << "Deleting a widget: " << w->objectName() << endl;
	m_form->objectTree()->removeItem(w->objectName());
	FormManager::self()->deleteWidgetLater( w );
	m_form->setSelectedWidget(m_container);
}

void
Container::widgetDeleted()
{
	m_container = 0;
	deleteLater();
}

/// Layout functions

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
		case HBox:
		{
			m_layout = (QLayout*) new Q3HBoxLayout(m_container, m_margin, m_spacing);
			createBoxLayout(new HorWidgetList(m_form->toplevelContainer()->widget()));
			break;
		}
		case VBox:
		{
			m_layout = (QLayout*) new Q3VBoxLayout(m_container, m_margin, m_spacing);
			createBoxLayout(new VerWidgetList(m_form->toplevelContainer()->widget()));
			break;
		}
		case Grid:
		{
			createGridLayout();
			break;
		}
		case  HFlow:
		{
#ifndef KEXI_NO_FLOWLAYOUT
			KexiFlowLayout *flow = new KexiFlowLayout(m_container,m_margin, m_spacing);
			flow->setOrientation(Qt::Horizontal);
			m_layout = (QLayout*)flow;
			createFlowLayout();
#endif
			break;
		}
		case VFlow:
		{
#ifndef KEXI_NO_FLOWLAYOUT
			KexiFlowLayout *flow = new KexiFlowLayout(m_container,m_margin, m_spacing);
			flow->setOrientation(Qt::Vertical);
			m_layout = (QLayout*)flow;
			createFlowLayout();
#endif
			break;
		}
		default:
		{
			m_layType = NoLayout;
			return;
		}
	}
	m_container->setGeometry(m_container->geometry()); // just update layout
	m_layout->activate();
}

void
Container::reloadLayout()
{
	LayoutType type = m_layType;
	setLayout(NoLayout);
	setLayout(type);
}

void
Container::createBoxLayout(WidgetList *list)
{
	Q3BoxLayout *layout = static_cast<Q3BoxLayout*>(m_layout);

	for(ObjectTreeItem *tree = m_tree->children()->first(); tree; tree = m_tree->children()->next())
		list->append( tree->widget());
	list->sort();

	for(QWidget *obj = list->first(); obj; obj = list->next())
		layout->addWidget(obj);
	delete list;
}

void
Container::createFlowLayout()
{
#ifndef KEXI_NO_FLOWLAYOUT
	KexiFlowLayout *flow = dynamic_cast<KexiFlowLayout*>(m_layout);
	if(!flow || m_tree->children()->isEmpty())
		return;

	const int offset = 15;
	WidgetList *list=0, *list2=0;
	if(flow->orientation() == Qt::Horizontal) {
		list = new VerWidgetList(m_form->toplevelContainer()->widget());
		list2 = new HorWidgetList(m_form->toplevelContainer()->widget());
	}
	else {
		list = new HorWidgetList(m_form->toplevelContainer()->widget());
		list2 = new VerWidgetList(m_form->toplevelContainer()->widget());
	}

	// fill the list
	for(ObjectTreeItem *tree = m_tree->children()->first(); tree; tree = m_tree->children()->next())
		list->append( tree->widget());
	list->sort();

	if(flow->orientation() == Qt::Horizontal) {
		int y = list->first()->y();
		for(QWidget *w = list->first(); w; w = list->next()) {
			if( (w->y() > y +offset)) {
				// start a new line
				list2->sort();
				for(QWidget *obj = list2->first(); obj; obj = list2->next())
					flow->add(obj);
				list2->clear();
				y = w->y();
			}
			list2->append(w);
		}

		list2->sort(); // don't forget the last line
		for(QWidget *obj = list2->first(); obj; obj = list2->next())
			flow->add(obj);
	}
	else {
		int x = list->first()->x();
		for(QWidget *w = list->first(); w; w = list->next()) {
			if( (w->x() > x +offset)) {
				// start a new column
				list2->sort();
				for(QWidget *obj = list2->first(); obj; obj = list2->next())
					flow->add(obj);
				list2->clear();
				x = w->x();
			}
			list2->append(w);
		}

		list2->sort(); // don't forget the last column
		for(QWidget *obj = list2->first(); obj; obj = list2->next())
			flow->add(obj);
	}

	delete list;
	delete list2;
#endif
}

void
Container::createGridLayout(bool testOnly)
{
	//Those lists sort widgets by y and x
	VerWidgetList *vlist = new VerWidgetList(m_form->toplevelContainer()->widget());
	HorWidgetList *hlist = new HorWidgetList(m_form->toplevelContainer()->widget());
	// The vector are used to store the x (or y) beginning of each column (or row)
	Q3ValueVector<int> cols;
	Q3ValueVector<int> rows;
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
	if(!testOnly) {
		for(WidgetListIterator it(*vlist); it.current() != 0; ++it)
		{
			QWidget *w = it.current();
			WidgetListIterator it2 = it;

			for(; it2.current() != 0; ++it2) {
				QWidget *nextw = it2.current();
				if((w->y() >= nextw->y()) || (nextw->y() >= w->geometry().bottom()))
					break;

				if(!w->geometry().intersects(nextw->geometry()))
					break;
				// If the geometries of the two widgets intersect each other,
				// we move one of the widget to the rght or bottom of the other
				if((nextw->y() - w->y()) > abs(nextw->x() - w->x()))
					nextw->move(nextw->x(), w->geometry().bottom()+1);
				else if(nextw->x() >= w->x())
					nextw->move(w->geometry().right()+1, nextw->y());
				else
					w->move(nextw->geometry().right()+1, nextw->y());
			}
		}
	}

	// Then we count the number of rows in the layout, and set their beginnings
	for(WidgetListIterator it(*vlist); it.current() != 0; ++it)
	{
		QWidget *w = it.current();
		WidgetListIterator it2 = it;
		if(!same) { // this widget will make a new row
			end = w->geometry().bottom();
			rows.append(w->y());
		}

		// If same == true, it means we are in the same row as prev widget
		// (so no need to create a new column)
		++it2;
		if(!it2.current())
			break;

		QWidget *nextw = it2.current();
		if(nextw->y() >= end)
			same = false;
		else {
			same = !(same && (nextw->y() >= w->geometry().bottom()));
			if(!same)
				end = w->geometry().bottom();
		}
	}
	kDebug() << "the new grid will have n rows: n == " << rows.size() << endl;

	end = -10000;
	same = false;
	// We do the same thing for the columns
	for(WidgetListIterator it(*hlist); it.current() != 0; ++it)
	{
		QWidget *w = it.current();
		WidgetListIterator it2 = it;
		if(!same) {
			end = w->geometry().right();
			cols.append(w->x());
		}

		++it2;
		if(!it2.current())
			break;

		QWidget *nextw = it2.current();
		if(nextw->x() >= end)
			same = false;
		else {
			same = !(same && (nextw->x() >= w->geometry().right()));
			if(!same)
				end = w->geometry().right();
		}
	}
	kDebug() << "the new grid will have n columns: n == " << cols.size() << endl;

	// We create the layout ..
	Q3GridLayout *layout=0;
	if(!testOnly) {
		layout = new Q3GridLayout(m_container, rows.size(), cols.size(), m_margin, m_spacing);
		layout->setObjectName("grid");
		m_layout = (QLayout*)layout;
	}

	// .. and we fill it with widgets
	for(WidgetListIterator it(*vlist); it.current() != 0; ++it)
	{
		QWidget *w = it.current();
		QRect r = w->geometry();
		uint wcol=0, wrow=0, endrow=0, endcol=0;
		uint i = 0;

		// We look for widget row(s) ..
		while(r.y() >= rows[i])
		{
			if((uint)rows.size() <= (i+1)) // we are the last row
			{
				wrow = i;
				break;
			}
			if(r.y() < rows[i+1])
			{
				wrow = i; // the widget will be in this row
				uint j = i + 1;
				// Then we check if the widget needs to span multiple rows
				while((uint)rows.size() >= (j+1) && r.bottom() > rows[j])
				{
					endrow = j;
					j++;
				}

				break;
			}
			i++;
		}
		//kDebug() << "the widget " << w->objectName() << " wil be in the row " << wrow <<
		   //" and will go to the row " << endrow << endl;

		// .. and column(s)
		i = 0;
		while(r.x() >= cols[i])
		{
			if((uint)cols.size() <= (i+1)) // last column
			{
				wcol = i;
				break;
			}
			if(r.x() < cols[i+1])
			{
				wcol = i;
				uint j = i + 1;
				// Then we check if the widget needs to span multiple columns
				while((uint)cols.size() >= (j+1) && r.right() > cols[j])
				{
					endcol = j;
					j++;
				}

				break;
			}
			i++;
		}
		//kDebug() << "the widget " << w->objectName() << " wil be in the col " << wcol <<
		 // " and will go to the col " << endcol << endl;

		ObjectTreeItem *item = m_form->objectTree()->lookup(w->objectName());
		if(!endrow && !endcol) {
			if(!testOnly)
				layout->addWidget(w, wrow, wcol);
			item->setGridPos(wrow, wcol, 0, 0);
		}
		else {
			if(!endcol)  endcol = wcol;
			if(!endrow)  endrow = wrow;
			if(!testOnly)
				layout->addMultiCellWidget(w, wrow, endrow, wcol, endcol);
			item->setGridPos(wrow, wcol, endrow-wrow+1, endcol-wcol+1);
		}
	}
}

QString
Container::layoutTypeToString(int type)
{
	switch(type)
	{
		case HBox: return "HBox";
		case VBox: return "VBox";
		case Grid: return "Grid";
		case HFlow: return "HFlow";
		case VFlow: return "VFlow";
		default:   return "NoLayout";
	}
}

Container::LayoutType
Container::stringToLayoutType(const QString &name)
{
	if(name == "HBox") return HBox;
	if(name == "VBox") return VBox;
	if(name == "Grid") return Grid;
	if(name == "HFlow")  return HFlow;
	if(name == "VFlow")  return VFlow;
	return NoLayout;
}

/// Drawing functions used by eventFilter
void
Container::drawConnection(QMouseEvent *mev)
{
	if(mev->button() != Qt::LeftButton)
	{
		FormManager::self()->resetCreatedConnection();
		return;
	}
	// First click, we select the sender and display menu to choose signal
	if(FormManager::self()->createdConnection()->sender().isNull())
	{
		FormManager::self()->createdConnection()->setSender(m_moving->objectName());
		if(m_form->formWidget())
		{
			m_form->formWidget()->initBuffer();
			m_form->formWidget()->highlightWidgets(m_moving, 0/*, QPoint()*/);
		}
		FormManager::self()->createSignalMenu(m_moving);
		return;
	}
	// the user clicked outside the menu, we cancel the connection
	if(FormManager::self()->createdConnection()->signal().isNull())
	{
		FormManager::self()->stopCreatingConnection();
		return;
	}
	// second click to choose the receiver
	if(FormManager::self()->createdConnection()->receiver().isNull())
	{
		FormManager::self()->createdConnection()->setReceiver(m_moving->objectName());
		FormManager::self()->createSlotMenu(m_moving);
		m_container->repaint();
		return;
	}
	// the user clicked outside the menu, we cancel the connection
	if(FormManager::self()->createdConnection()->slot().isNull())
	{
		FormManager::self()->stopCreatingConnection();
		return;
	}
}

void
Container::drawSelectionRect(QMouseEvent *mev)
{
	//finish drawing unclipped selection rectangle: clear the surface
	if(m_form->formWidget())
		m_form->formWidget()->clearForm();
	int topx = (m_insertBegin.x() < mev->x()) ? m_insertBegin.x() :  mev->x();
	int topy = (m_insertBegin.y() < mev->y()) ? m_insertBegin.y() : mev->y();
	int botx = (m_insertBegin.x() > mev->x()) ? m_insertBegin.x() :  mev->x();
	int boty = (m_insertBegin.y() > mev->y()) ? m_insertBegin.y() : mev->y();
	QRect r = QRect(QPoint(topx, topy), QPoint(botx, boty));

	setSelectedWidget(m_container, false);
	QWidget *widgetToSelect = 0;
	// We check which widgets are in the rect and select them
	for(ObjectTreeItem *item = m_tree->children()->first(); item; item = m_tree->children()->next())
	{
		QWidget *w = item->widget();
		if(!w)
			continue;
		if(w->geometry().intersects(r) && w != m_container) {
			if (widgetToSelect)
				setSelectedWidget(widgetToSelect, true/*add*/, false/*raise*/, true/*moreWillBeSelected*/);
			widgetToSelect = w; //select later
		}
	}
	if (widgetToSelect) //the last one left
		setSelectedWidget(widgetToSelect, true/*add*/, false/*raise*/, false/*!moreWillBeSelected*/);

	m_insertRect = QRect();
	m_state = DoingNothing;
	m_container->repaint();
}

void
Container::drawInsertRect(QMouseEvent *mev, QObject *s)
{
	int tmpx, tmpy;
	QPoint pos = static_cast<QWidget*>(s)->mapTo(m_container, mev->pos());
	int gridX = m_form->gridSize();
	int gridY = m_form->gridSize();
	if(!FormManager::self()->snapWidgetsToGrid() || (mev->buttons() == Qt::LeftButton && mev->modifiers() == Qt::ControlModifier|Qt::AltModifier))
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

	if(m_insertRect.x() < 0)
		m_insertRect.setLeft(0);
	if(m_insertRect.y() < 0)
		m_insertRect.setTop(0);
	if(m_insertRect.right() > m_container->width())
		m_insertRect.setRight(m_container->width());
	if(m_insertRect.bottom() > m_container->height())
		m_insertRect.setBottom(m_container->height());

	if(FormManager::self()->isInserting() && m_insertRect.isValid())
	{
		if(m_form->formWidget())
		{
			QRect drawRect = QRect(m_container->mapTo(m_form->widget(), m_insertRect.topLeft())
				, m_insertRect.size());
			m_form->formWidget()->drawRect(drawRect, 2);
		}
	}
}

void
Container::drawCopiedWidgetRect(QMouseEvent *mev)
{
	// We've been dragging a widget, but Ctrl was hold, so we start copy
	if(m_state == MovingWidget)  {
		//FormManager::self()->undo(); // undo last moving
		//m_moving->move(m_initialPos);
		if(m_form->formWidget())  {
			m_container->repaint();
			m_form->formWidget()->initBuffer();
		}
		m_state = CopyingWidget;
	}

	//m_copyRect.moveTopLeft(m_container->mapFromGlobal( mev->globalPos()) - m_grab);

	if(m_form->formWidget())  {
		Q3ValueList<QRect> rectList;
		for(QWidget *w = m_form->selectedWidgets()->first(); w; w = m_form->selectedWidgets()->next()) {
			QRect drawRect = w->geometry();
			QPoint p = mev->pos() - m_grab;
			drawRect.moveTo(drawRect.x()+p.x(), drawRect.y()+p.y());
			p = m_container->mapTo(m_form->widget(), QPoint(0, 0));
			//drawRect = QRect( ((QWidget*)s)->mapTo(m_form->widget(), drawRect.topLeft()), drawRect.size());
			drawRect.moveTo(drawRect.x()+p.x(), drawRect.y()+p.y());
			rectList.append(drawRect);
		}

		m_form->formWidget()->drawRects(rectList, 2);
	}
}

/// Other functions used by eventFilter
void
Container::moveSelectedWidgetsBy(int realdx, int realdy, QMouseEvent *mev)
{
	if (m_form->selectedWidget() == m_form->widget())
		return; //do not move top-level widget

	const int gridX = m_form->gridSize();
	const int gridY = m_form->gridSize();
	int dx=realdx, dy=realdy;

	for(QWidget *w = m_form->selectedWidgets()->first(); w; w = m_form->selectedWidgets()->next())
	{
		if(!w || !w->parent() || w->parent()->inherits("QTabWidget") || w->parent()->inherits("QWidgetStack"))
			continue;

		if(w->parentWidget() && KexiUtils::objectIsA(w->parentWidget(), "QWidgetStack"))
		{
			w = w->parentWidget(); // widget is WidgetStack page
			if(w->parentWidget() && w->parentWidget()->inherits("QTabWidget")) // widget is tabwidget page
				w = w->parentWidget();
		}

		int tmpx = w->x() + realdx;
		int tmpy = w->y() + realdy;
		if(tmpx < 0)
			dx = qMax(0 - w->x(), dx); // because dx is <0
		else if(tmpx > w->parentWidget()->width() - gridX)
			dx = qMin(w->parentWidget()->width() - gridX - w->x(), dx);

		if(tmpy < 0)
			dy = qMax(0 - w->y(), dy); // because dy is <0
		else if(tmpy > w->parentWidget()->height() - gridY)
			dy = qMin(w->parentWidget()->height() - gridY - w->y(), dy);
	}

	for(QWidget *w = m_form->selectedWidgets()->first(); w; w = m_form->selectedWidgets()->next())
	{
		// Don't move tab widget pages (or widget stack pages)
		if(!w || !w->parent() || w->parent()->inherits("QTabWidget") || w->parent()->inherits("QWidgetStack"))
			continue;

		if(w->parentWidget() && KexiUtils::objectIsA(w->parentWidget(), "QWidgetStack"))
		{
			w = w->parentWidget(); // widget is WidgetStack page
			if(w->parentWidget() && w->parentWidget()->inherits("QTabWidget")) // widget is tabwidget page
				w = w->parentWidget();
		}

		int tmpx, tmpy;
		if(!FormManager::self()->snapWidgetsToGrid() || (mev && mev->buttons() == Qt::LeftButton && mev->modifiers()==Qt::ControlModifier|Qt::AltModifier))
		{
			tmpx = w->x() + dx;
			tmpy = w->y() + dy;
		}
		else
		{
			tmpx = int( float( w->x() + dx) / float(gridX) + 0.5) * gridX;
			tmpy = int( float( w->y() + dy) / float(gridY) + 0.5) * gridY;
		}

		if((tmpx != w->x()) || (tmpy != w->y()))
			w->move(tmpx,tmpy);
	}
}

////////////

DesignTimeDynamicChildWidgetHandler::DesignTimeDynamicChildWidgetHandler()
 : m_item(0)
{
}

DesignTimeDynamicChildWidgetHandler::~DesignTimeDynamicChildWidgetHandler()
{
}

void
DesignTimeDynamicChildWidgetHandler::childWidgetAdded(QWidget* w)
{
	if (m_item) {
		installRecursiveEventFilter(w, m_item->eventEater());
	}
}

#include "container.moc"
